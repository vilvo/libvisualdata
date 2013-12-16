/**
  Copyright (c) 2012, Intel
  All rights reserved.
  Description:
  Implementation of VisualDataInterface common methods
*/

#include "visualdatainterface.hpp"

visualdatainterface::visualdatainterface() {
}

visualdatainterface::~visualdatainterface() {
}

cv::Size visualdatainterface::getframesize() {
    return framesize_;
}

unsigned int visualdatainterface::getfps() {
    return fps_;
}
