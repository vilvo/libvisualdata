/*
 Copyright (c) 2012, Intel
 All rights reserved.

 Description:
    Screenshot capturing thread. Screenshots with syscalls over adb from DUT.
*/

#ifndef ADBSCREENSHOTTHREAD_HPP
#define ADBSCREENSHOTTHREAD_HPP

#include <opencv2/opencv.hpp>
#include <boost/thread.hpp>
#include <string>

#include "visualdatainterface.hpp"

class adbscreenshotthread : public visualdatainterface {
    public:
        adbscreenshotthread(boost::function<void(cv::Mat&)> callback);
        ~adbscreenshotthread();

    private:
        void startthread();
        void stopthread();
        void capture();
        void detach();
        void operator()();
        bool runcmd(const std::string& cmd);

        bool shootscreen_;
        boost::thread* thread_;
        cv::Mat frame_;
};

#endif
