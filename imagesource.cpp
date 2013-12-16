/*
 Copyright (c) 2012, Intel
 All rights reserved.

 Description:
    Implements image file data source
*/


#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdlib.h>

#include "imagesource.hpp"

imagesource::imagesource(const std::string& imagefilename,
                         boost::function<void(cv::Mat&)> callback) :
    imagefilename_(imagefilename) {
    visualdatainterface::fps_ = 0;
    visualdatainterface::callback_ = callback;
    loadImage();
}

imagesource::~imagesource() {}

cv::Size imagesource::getframesize() {
    return frame_.size();
}

void imagesource::loadImage() {
    cv::Mat image = cv::imread(imagefilename_.c_str(), CV_32FC1);
    if (image.empty())
        return;
    frame_ = image;
    callback_(image);
}
