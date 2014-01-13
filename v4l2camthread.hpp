/*
 Copyright (c) 2012, Intel
 All rights reserved.

 Description:
    Camera frame capturing and processing thread for V4L2 type camera
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
