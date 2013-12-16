/*
 Copyright (c) 2012, Intel
 All rights reserved.

 Description:
    Screenshot capturing thread. Screenshots with syscalls over adb.
*/

#include <stdio.h>
#include "adbscreenshotthread.hpp"

using boost::thread;
using boost::bind;
using std::string;

static const string ADBSCRCAP = "adb shell screencap -p /sdcard/screen.png";
static const string ADBGETSCR = "adb pull /sdcard/screen.png /tmp/screen.png";
static const string SCRSHOT = "/tmp/screen.png";
static const unsigned int REFRESHDELAY = 500000;

adbscreenshotthread::adbscreenshotthread(
    boost::function<void(cv::Mat&)> callback) {
    visualdatainterface::callback_ = callback;
    startthread();
}

adbscreenshotthread::~adbscreenshotthread() {
    stopthread();
}

inline void adbscreenshotthread::detach() {
    thread_->detach();
}

inline void adbscreenshotthread::operator()() {
    capture();
}

void adbscreenshotthread::startthread() {
    if (!shootscreen_) {
        shootscreen_ = true;
        thread_ = new thread(bind(&adbscreenshotthread::capture, this));
    } else {
        shootscreen_ = false;
    }
    return;
}

void adbscreenshotthread::stopthread() {
    shootscreen_ = false;
    thread_->join();
    delete thread_;
    thread_ = NULL;
}

inline void adbscreenshotthread::capture() {
    while (shootscreen_) {
        if (!runcmd(ADBSCRCAP) || !runcmd(ADBGETSCR)) {
            shootscreen_ = false;
            return;
        }
        cv::Mat image = cv::imread(SCRSHOT, CV_LOAD_IMAGE_COLOR);
        if (image.empty())
	    throw std::runtime_error("adbscreenshotthread - Image empty");
        if (framesize_.width == 0)
            visualdatainterface::framesize_ = image.size();
        frame_ = image;
        callback_(frame_);
        usleep(REFRESHDELAY);
    }
}

inline bool adbscreenshotthread::runcmd(const std::string& cmd) {
    FILE* pf = popen(cmd.c_str(), "r");
    if(!pf)
        return false;
    int status = pclose(pf);
    if(status == 0) {
        return true;
    } else {
        std::cerr << "ERROR: runcmd [" << cmd << "]:" << status << std::endl;
        return false;
    }
}
