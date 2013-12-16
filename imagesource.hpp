/*
 Copyright (c) 2012, Intel
 All rights reserved.

 Description:
    Defines image file data source interface
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
