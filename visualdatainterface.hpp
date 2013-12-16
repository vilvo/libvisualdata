/**
 Copyright (c) 2012, Intel
 All rights reserved.
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
