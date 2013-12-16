/*
 Copyright (c) 2012, Intel
 All rights reserved.

 Description:
    Camera frame capturing and processing thread for V4L2 type camera
*/

#include "v4l2camthread.hpp"

using boost::thread;
using boost::bind;

const static double C910MAXFHDFPS = 15.0;

v4l2camthread::v4l2camthread(const uint width, const uint height,
		     const bool undistort,
		     const uint fps,
		     const std::string undistortionparameters,
                     boost::function<void(cv::Mat&)> callback)
  : undistort_(undistort), capture_(false),
    emptyframes_(0) {
    visualdatainterface::framesize_ = cv::Size(width, height);
    if (undistort_) {
        readundistortionparameters(undistortionparameters);
    }
    visualdatainterface::callback_ = callback;
    opencamera();  // v4l2 devices for OpenCV run with highest available fps
    // to skip frames because we can undistort only a few frames per second
    fpsmodulo_ = static_cast<int>(C910MAXFHDFPS / fps);
    visualdatainterface::fps_ = fps;
    startcapturethread();
}

void v4l2camthread::readundistortionparameters(const std::string undistparams) {
    try {
        cv::FileStorage fs(undistparams, cv::FileStorage::READ);
        fs["camera_matrix"] >> cameramatrix_;
        fs["distortion_coefficients"] >> distcoeffs_;
        fs.release();
    } catch(...) {
        throw std::runtime_error("v4l2camthread - cannot read undistort params");
    }
    cv::Mat optimalmap = cv::getOptimalNewCameraMatrix(cameramatrix_,
                                                       distcoeffs_,
                                                       framesize_, 1,
                                                       framesize_);
    cv::Mat map1, map2;
    cv::initUndistortRectifyMap(cameramatrix_, distcoeffs_, cv::Mat(),
                                optimalmap, framesize_, CV_16SC2, map1, map2);
}

v4l2camthread::~v4l2camthread() {
    stopcapturethread();
}

inline void v4l2camthread::opencamera() {
    capturedev_.open(0);
    if (capturedev_.isOpened()) {
        capturedev_.set(CV_CAP_PROP_FRAME_HEIGHT, framesize_.height);
        capturedev_.set(CV_CAP_PROP_FRAME_WIDTH, framesize_.width);
        return;
    } else {
      throw std::runtime_error("v4l2camthread - V4L2 device cannot be opened");
    }
}

inline void v4l2camthread::detach() {
    capturethread_->detach();
}

inline void v4l2camthread::operator()() {
    capture();
}

void v4l2camthread::startcapturethread() {
    if (capturedev_.isOpened()) {
        capture_ = true;
        capturethread_ = new thread(bind(&v4l2camthread::capture, this));
    } else {
        capture_ = false;
    }
    return;
}

void v4l2camthread::stopcapturethread() {
    capture_ = false;
    capturethread_->join();
    delete capturethread_;
    capturethread_ = NULL;
}

inline void v4l2camthread::capture() {
    while (capture_) {
        capturedev_ >> frame_;
        framecounter_++;
        if (!(framecounter_ % fpsmodulo_) && !frame_.empty()) {
            processimage();
            callback_(processframe_);
        } else {
           emptyframes_++;
        }
        // else we do not sleep frame interval because processing every nth
        // frame takes enough time
    }
}

void v4l2camthread::processimage() {
    if (undistort_) {
        cv::undistort(frame_, processframe_, cameramatrix_, distcoeffs_);
    } else {  // we get raw, undistorted frame
        processframe_ = frame_;
    }
}
