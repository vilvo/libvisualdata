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

#ifndef IMAGESOURCE_HPP
#define IMAGESOURCE_HPP

#include <opencv2/opencv.hpp>
#include <string>

#include "visualdatainterface.hpp"

class imagesource : public visualdatainterface {
    public:
        explicit imagesource(const std::string& imagefilename,
                             boost::function<void(cv::Mat&)> callback);
        virtual cv::Size getframesize();
        ~imagesource();
    private:
        void loadImage();
        std::string imagefilename_;
};

#endif
