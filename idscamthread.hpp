/*
 * IDS camera implementation of visualdatainterface for libvisualdata
 * Copyright (c) 2013-2014, Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU Lesser General Public License,
 * version 2.1, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#ifndef IDSCAMTHREAD_HPP
#define IDSCAMTHREAD_HPP

#include <opencv2/opencv.hpp>
#include <boost/thread.hpp>
#include <ueye.h>
#include <string>

#include "visualdatainterface.hpp"

class idscamthread : public visualdatainterface {
    public:
        /**
         * Constructor.
         * @param width the width of the area of interest within full image.
         * @param height the height of the area of interest within full image.
         * @param fps frames per second. NOTICE: FPS depends on AOI, e.g,
         * IDS 4K camera has the value range 2 - 195. If the given value is
         * higher than with current AOI, sets the highest possible value and
         * prints it to the rataservice log.
         * @param callback callback function to pass the captured images to
         * the caller.
         * @param camindex the index of the camera to be used. With 0, the
         * fist available camera is selected.
         * @throw std::runtime_error if the camera cannot be initialized
         * @throw std::runtime_error the area of interest or the static
         * parameters cannot be set
         */
        idscamthread(const unsigned int width, const unsigned int height,
                     const unsigned int fps,
                     boost::function<void(cv::Mat&)> callback,
                     const unsigned int camindex = 0);

        ~idscamthread();

        /**
         * Sets digital gamma correction.
         * @param gamma the value range is 1-1000 (actual gamma value
         * multiplied with 100).
         * @throw std::runtime_error if the paramater cannot be set.
         */
        void setgamma(const int gamma = 180);
        /**
         * Sets the frame rate.
         * NOTE: The frame rate depends on AOI, e.g, in IDS 4K cam the value
         * range is 2-195. If we try to set above the range, the highest
         * possible value is set.
         * @param fps frames per second.
         * @throw std::runtime_error if the paramater cannot be set.
         * @return actual frame rate set.
         */
        double setfps(const double fps);
        /**
         * Sets the exposure time.
         * @param exposure The value range of exposure time depends on the
         * current AOI and FPS. Can be set dynamically with zero to the max
         * value which is 1/FPS.
         * @throw std::runtime_error if the paramater cannot be set.
         * @return actual exposure time set.
         */
        double setexposure(double exposure = 0);

        /**
         * Sets the size of the camera view (area of interest within the
         * full image).
         * @param width the width of the image
         * @param height the height of the image
         * @throw std::runtime_error if the size cannot be set.
         */
        void setviewsize(const unsigned int width,
                         const unsigned int height);

       /**
         * Set autobrightness parameters
         * @param state Enables=1/disables=0 the automatic disable for
         * automatic brightness control (gain and exposure time)
         * @param level Sets the setpoint for auto gain control / auto
         * exposure shutter. For the XS the setpoint for auto gain
         * control / auto exposure shutter is not a quantitavie setpoint
         * for the middle image brightness but a qualitative one.
         * @return True if successfull
         */
        bool setautobrightness(double state, double level);

       /**
         * Set gain parameters
         * @param  autogain Enables=1/disables=0 the auto gain control
         * function or, in case of HDR sensors, the white level
         * adjustment
         * @param boost enables=IS_SET_GAINBOOST_ON / disables=
         * IS_SET_GAINBOOST_OFF an additional analog hardware
         * gain boost feature on the sensor.
         * @param  hwgainlevel controls the sensor gain channels. These
         * can be set between 0 % and 100 % independently of of each
         * other. The actual gain factor obtained for the value 100%
         * depends on the sensor and is specified in Camera and sensor
         * data chapter.
         * @return True if successfull
         * */
        bool sethwgain(double autogain, double boost,
                       double hwgainlevel);
       /**
         * Set color filter
         * @param irfilter
         *  - IS_CCOR_ENABLE_NORMAL Enables simple color correction.
         * - IS_CCOR_ENABLE_BG40_ENHANCED Enables color correction for
         *   cameras with optical IR filter glasses of the BG40 type.
         * - IS_CCOR_ENABLE_HQ_ENHANCED Enables color correction for
         *   cameras with optical IR filter glasses of the HQ type.
         * - IS_CCOR_SET_IR_AUTOMATIC Enables color correction for
         *   cameras with optical IR filter glasses. The glass type is
         *   set automatically as specified in the camera EEPROM.
         * - IS_CCOR_DISABLE Disables color correction.
         * is_SetColorCorrection() enables color correction in
         * the uEye driver. This enhances the rendering of colors for
         * cameras with color sensors. Color correction is a digital
         * correction based on a color matrix which is adjusted
         * individually for each sensor.
         * @param factors Sets the strength of the color correction
         * between 0.0 (no correction) and 1.0 (strong correction).
         * @return True if successfull
         */
        bool setcolour(int irfilter, double factors);

        static const int PARAM_NOT_SET = -1;

    private:
        /**
         * Starts the IDS driver and creates a connection to the camera.
         * @throw std::runtime_error if the camera cannot be initialized.
         */
        void initcamera();
        /**
         * Allocates memory for the image and sets the size of AOI
         * (area of interest within the full image).
         * NOTE: when the image size is changed the value ranges of
         * exposure time and FPS are also changed. To avoid problems, call
         * setstaticparams after this function.
         * @param width the width of the image
         * @param height the height of the image
         * @throw std::runtime_error if the AOI cannot be set.
         */
        void setimg(const unsigned int width, const unsigned int height);

        /**
         * Gets images from the camera, and passes them to the caller.
         */
        void capture();

        void startcapturethread();
        void stopcapturethread();
        void detach();
        void operator()();

        bool capture_;
        boost::thread* capturethread_;
        uint64_t framecounter_;
        HIDS hCam_;
        char* imgmem_;
        INT pid_;
        INT maxwidth_;
        INT maxheight_;
};

#endif  // CamThread_HPP
