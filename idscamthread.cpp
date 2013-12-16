/*
 Copyright (c) 2012, Intel
 All rights reserved.

 Description:
    Camera frame capturing and processing thread for IDS type camera
*/

#include "idscamthread.hpp"

using boost::thread;
using boost::bind;

const static uint IDSMAXRETRIES = 5;

idscamthread::idscamthread(const unsigned int width,
                           const unsigned int height,
                           const unsigned int fps,
                           boost::function<void(cv::Mat&)> callback,
                           const unsigned int camindex)
    : capture_(false), hCam_(camindex), imgmem_(NULL), pid_(0),
      maxwidth_(0), maxheight_(0) {
    visualdatainterface::callback_ = callback;
    fps_ = fps;
    visualdatainterface::framesize_ = cv::Size(width, height);
    frame_ = cv::Mat(framesize_.height , framesize_.width, CV_8UC3);

    initcamera();
    setimg(framesize_.width, framesize_.height);
    // Set software gamma to get evenly illuminated DUT screen
    setgamma();
    setfps(fps_);
    setexposure();
    startcapturethread();
}

idscamthread::~idscamthread() {
    stopcapturethread();
    is_ExitCamera(hCam_);
}

inline void idscamthread::initcamera() {
    INT retcode = 0;
    bool tryconnect = true;
    uint retrycount = 0;

    while (tryconnect) {
        retcode = is_InitCamera(&hCam_, 0);
        if (retcode == IS_SUCCESS) {
            tryconnect = false;
        } else if (retcode == IS_CANT_OPEN_DEVICE) {
            // To handle situation when is_ExitCamera and is_InitCamera are
            // executed sequentially; it's a known issue that
            // is_InitCamera might not work immediately is_ExitCamera,
            // and some delay is needed.
            ++retrycount;
            if (retrycount >= IDSMAXRETRIES) {
                tryconnect = false;
            }
            sleep(2.0);
        } else {
            // Errors from which we don't try to recover by retrying.
            tryconnect = false;
        }
    }
    if (retcode != IS_SUCCESS) {
        throw std::runtime_error("idscamthread - is_InitCamera");
    }
    // The max image size is retrieved once instead of dynamically in setimg
    // because for some reason is_AOI did not return the absolute max size
    // after image AOI and camera params were set already once
    // (calling setviewsize several times.)
    IS_SIZE_2D maxsize;
    is_AOI(hCam_, IS_AOI_IMAGE_GET_SIZE_MAX,
           (void*)&maxsize,  // NOLINT(readability/casting)
           sizeof(maxsize));
    if (retcode != IS_SUCCESS) {
        throw std::runtime_error("idscamthread - is_AOI");
    }
    maxwidth_ = maxsize.s32Width;
    maxheight_ = maxsize.s32Height;
}

inline void idscamthread::setimg(const unsigned int width,
                                 const unsigned int height) {
    // Allocate and set image memory
    INT retcode = 0;
    // Color depth 24 bits/pixel (true color) must match with the OpenCV type
    // CV_8UC3: three channels, 8 bits per each channel.
    retcode = is_AllocImageMem(hCam_, width, height, 24, &imgmem_, &pid_);
    if (retcode != IS_SUCCESS) {
        throw std::runtime_error("idscamthread - is_AllocImageMem");
    }
    retcode = is_SetImageMem(hCam_, imgmem_, pid_);
    if (retcode != IS_SUCCESS) {
        throw std::runtime_error("idscamthread - is_setImageMem");
    }
    // AOI calculations
    // Check that width and size are valid
    IS_POINT_2D sizeinc;
    is_AOI(hCam_, IS_AOI_IMAGE_GET_SIZE_INC,
           (void*)&sizeinc,  // // NOLINT(readability/casting)
           sizeof(sizeinc));
    if (width % sizeinc.s32X != 0) {
        throw std::runtime_error("idscamthread - invalid width");
    }
    if (height % sizeinc.s32Y != 0) {
        throw std::runtime_error("idscamthread - invalid height");
    }
    // For activating the center of the sensor - the upper left corner is the
    // starting point
    INT temp_x = maxwidth_  / 2 - width / 2;
    INT temp_y = maxheight_ / 2 - height / 2;
    // Setting the sensor position has certain allowed increments, for example
    // with UI5580-CP x is 4, and y is 2
    IS_POINT_2D posinc;
    is_AOI(hCam_, IS_AOI_IMAGE_GET_POS_INC,
           (void*)&posinc,  // // NOLINT(readability/casting)
           sizeof(posinc));
    INT inc_x = posinc.s32X;
    INT inc_y = posinc.s32Y;
    temp_x -= temp_x % inc_x;
    temp_y -= temp_y % inc_y;

    IS_RECT rectAOI;
    rectAOI.s32Width = width;
    rectAOI.s32Height = height;
    rectAOI.s32X = temp_x;
    rectAOI.s32Y = temp_y;

    retcode = is_AOI(hCam_, IS_AOI_IMAGE_SET_AOI,
                     (void*)&rectAOI,  // NOLINT(readability/casting)
                     sizeof(rectAOI));
    if (retcode != IS_SUCCESS) {
        throw std::runtime_error("idscamthread - is_AOI failed");;
    }
}

inline void idscamthread::setgamma(const int gamma) {
    INT retcode = 0;
    retcode = is_SetGamma(hCam_, gamma);
    if (retcode != IS_SUCCESS)
        throw std::runtime_error("idscamthread - is_SetGamma");

}
inline double idscamthread::setfps(const double fps) {
    INT retcode = 0;
    double newFPS = 0;
    retcode = is_SetFrameRate(hCam_, fps_, &newFPS);
    if (retcode != IS_SUCCESS) {
        throw std::runtime_error("idscamthread - is_SetFrameRate");
    }
    return newFPS;
}

inline double idscamthread::setexposure(const double exposure) {
    INT retcode = 0;
    // The same variable is used to set and get the value
    double newexposure = exposure;
    retcode = is_Exposure(hCam_, IS_EXPOSURE_CMD_SET_EXPOSURE, &newexposure,
                          sizeof(newexposure));
    if (retcode != IS_SUCCESS)
        throw std::runtime_error("idscamthread - is_Exposure");
    return newexposure;
}

void idscamthread::setviewsize(const unsigned int width,
                               const unsigned int height) {
    stopcapturethread();
    INT retcode = 0;
    is_FreeImageMem(hCam_, imgmem_, pid_);
    if (retcode != IS_SUCCESS)
        throw std::runtime_error("idscamthread - is_FreeImageMem");
    imgmem_ = NULL;
    pid_ = 0;

    frame_.release();
    framesize_.width  = width;
    framesize_.height = height;
    frame_ = cv::Mat(framesize_.height, framesize_.width, CV_8UC3);

    setimg(framesize_.width, framesize_.height);
    // Set again the FPS and exposure time because changing the image AOI
    // affects to their max values. For example, when the AOI is decreased,
    // new max value is set automatically. But when changing back to larger AOI
    // the camera does not automatically use the max value.
    setfps(fps_);
    setexposure();

    startcapturethread();
}

bool idscamthread::sethwgain(double autogain = 1,
                             double boost = IS_SET_GAINBOOST_ON,
                             double hwgainlevel = PARAM_NOT_SET) {
    bool ret = false;
    int retcode = IS_INVALID_PARAMETER;

    if (autogain == 1 or autogain == 0) {
        retcode = is_SetAutoParameter(hCam_,
                                      IS_SET_ENABLE_AUTO_GAIN,
                                      &autogain, 0);
    }

    if (boost == IS_SET_GAINBOOST_ON or boost == IS_SET_GAINBOOST_OFF) {
        retcode = is_SetGainBoost(hCam_, boost);
    }


    retcode = is_SetHardwareGain(hCam_,
                                 hwgainlevel,
                                 IS_IGNORE_PARAMETER,
                                 IS_IGNORE_PARAMETER,
                                 IS_IGNORE_PARAMETER);
    if (retcode == IS_SUCCESS) {
        ret = true;
    }
    return ret;
}

bool idscamthread::setautobrightness(double state = 1,
                                     double level = 128) {
    bool ret = false;
    int retcode = IS_INVALID_PARAMETER;

    if (state == 1 or state == 0) {
        retcode = is_SetAutoParameter(hCam_,
                                      IS_SET_AUTO_BRIGHTNESS_ONCE,
                                      &state, 0);
    }

    if (level >= 0 and level <= 255) {
       retcode = is_SetAutoParameter(hCam_,
                                     IS_SET_AUTO_REFERENCE,
                                     &level, 0);
    }

    if (retcode == IS_SUCCESS) {
        ret = true;
    }
    return ret;
}

bool idscamthread::setcolour(int irfilter = IS_CCOR_ENABLE_HQ_ENHANCED,
                             double factors = 0) {
    bool ret = false;
    int retcode = IS_INVALID_PARAMETER;

    /* irfilter = is_SetColorCorrection() enables color correction in
     * the uEye driver. This enhances the rendering of colors for
     * cameras with color sensors. Color correction is a digital
     * correction based on a color matrix which is adjusted individually
     * for each sensor. */

    if (irfilter >= 0) {
        retcode = is_SetColorCorrection(hCam_, irfilter, &factors);
    }

    if (retcode == IS_SUCCESS) {
        ret = true;
    }
    return ret;
}

inline void idscamthread::detach() {
    capturethread_->detach();
}

inline void idscamthread::operator()() {
    capture();
}

void idscamthread::startcapturethread() {
    capturethread_ = new thread(bind(&idscamthread::capture, this));
    capture_ = true;
    return;
}

void idscamthread::stopcapturethread() {
    capture_ = false;
    capturethread_->join();
    delete capturethread_;
    capturethread_ = NULL;
}

inline void idscamthread::capture() {
    while (capture_) {
        // Gets the next ready frame. No need to sleep between the calls.
        INT retcode = is_FreezeVideo(hCam_, IS_WAIT);
        if (retcode != IS_SUCCESS) {
            throw std::runtime_error("idscamthread - is_FreezeVideo");
        }
        VOID* pMem;
        retcode = is_GetImageMem(hCam_, &pMem);
        if (retcode != IS_SUCCESS) {
	    throw std::runtime_error("idscamthread - is_GetImageMem");
        }
        // Three color channels (byte per each channel)
        int numbytes = framesize_.width  * framesize_.height * 3;
        memcpy(frame_.ptr(), pMem, numbytes);

        framecounter_++;
        callback_(frame_);
    }
}
