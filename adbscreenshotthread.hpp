/*
 * adb screenshots implementation of visualdatainterface for libvisualdata
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
