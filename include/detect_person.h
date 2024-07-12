#ifndef DETECT_PERSON_H
#define DETECT_PERSON_H

#include <opencv2/core/core.hpp>
#include <net.h>

// struct KeyPoint
// {
//     cv::Point2f p;
//     float prob;
// };

class DetectPerson
{

public:
    DetectPerson();
    ~DetectPerson();

    int detect(cv::Mat &image, cv::Point &minPoint, cv::Point &maxPoint);

private:
    ncnn::Net detectornet;
    int detector_size_width = 320;
    int detector_size_height = 320;
};

#endif