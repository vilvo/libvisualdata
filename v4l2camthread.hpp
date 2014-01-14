/*
 * v4l2 camera implementation of visualdatainterface for libvisualdata
 * Copyright (c) 2012-2014, Intel Corporation.
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

#ifndef V4L2CAMTHREAD_HPP
#define V4L2CAMTHREAD_HPP

#include <opencv2/opencv.hpp>
#include <boost/thread.hpp>

#include "visualdatainterface.hpp"

class v4l2camthread : public visualdatainterface {
    public:
        v4l2camthread(const uint width, const uint height,
                      const bool undistort, const uint fps,
                      const std::string undistortionparameters,
                      boost::function<void(cv::Mat&)> callback);
        ~v4l2camthread();

    private:
        /** Reads undistortion parameters
        @except std::runtime_error with invalid configuration
        */
        void readundistortionparameters(const std::string undistparams);
        /** Opens camera HW to read frames using OpenCV capv4l interface
        @except std::runtime_error if camera cannot be opened
        */
        void opencamera();
        void startcapturethread();
        void stopcapturethread();
        void capture();
        void detach();
        void operator()();
        void processimage();

        bool undistort_;
        cv::VideoCapture capturedev_;
        bool capture_;
        cv::Mat processframe_;
        boost::thread* capturethread_;
        cv::Mat cameramatrix_;
        cv::Mat distcoeffs_;
        uint64_t framecounter_;
        double camerafps_;
        int fpsmodulo_;
        unsigned int emptyframes_;
};

#endif
