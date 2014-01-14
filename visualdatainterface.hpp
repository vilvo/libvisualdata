/*
 * visualdatainterface for libvisualdata
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

#ifndef VISUALDATAINTERFACE_H
#define VISUALDATAINTERFACE_H

#include <exception>

#include <opencv2/opencv.hpp>
#include <boost/function.hpp>

/**
 * Generic class that manage visual data sources of the frames: camera
 * or image files.
 */
class visualdatainterface {
    public:
        visualdatainterface();
        virtual ~visualdatainterface();
        virtual cv::Size getframesize();
        virtual unsigned int getfps();

    protected:
        /**
         * callback allows visual data source to set the latest
         * image(s) when it's ready
         */
        boost::function<void(cv::Mat&)> callback_;
        double fps_;
        cv::Mat frame_;
        cv::Size framesize_;
};

#endif
