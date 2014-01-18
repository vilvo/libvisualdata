/*
 * test and usage example of visualdatainterface for libvisualdata
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

#include <iostream>
#include <string>

#include "gtest/gtest.h"

#include "visualdatainterface.hpp"
#include "imagesource.hpp"
#include "v4l2camthread.hpp"
#include "adbscreenshotthread.hpp"

#include <opencv2/opencv.hpp> // cv::Mat
#include "opencv2/highgui/highgui.hpp"

class visualdatacallback {
    public:
        void setimage(const cv::Mat& frame) {
	    frame.copyTo(frame_);
	};
    cv::Mat frame_;
};

TEST(visualdatasources, testadbscreenshots) {
    visualdatainterface *vdi;
    cv::namedWindow("preview", CV_WINDOW_AUTOSIZE);
    visualdatacallback *cb = new visualdatacallback();
    vdi = new adbscreenshotthread(bind(&visualdatacallback::setimage, cb, _1));
    usleep(2000000);
    for (int i = 0; i < 10; i++ ) {
        cv::imshow("preview", cb->frame_);
	cv::waitKey(500);
    }
    delete vdi;
    delete cb;
}
