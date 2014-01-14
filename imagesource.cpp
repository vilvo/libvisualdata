/*
 * static image implementation of visualdatainterface for libvisualdata
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
