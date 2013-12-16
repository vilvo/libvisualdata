#include <iostream>
#include <string>

#include "gtest/gtest.h"

#include "visualdatainterface.hpp"
#include "imagesource.hpp"
#include "v4l2camthread.hpp"
#include "idscamthread.hpp"
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
