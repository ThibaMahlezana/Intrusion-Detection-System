#ifndef SELECT_AREA_H
#define SELECT_AREA_H

#include <opencv2/opencv.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class SelectArea
{
public:
    SelectArea();
    void drawPointsAndShape(cv::Mat &frame, const std::vector<cv::Point> &points, bool shapeDrawn);
};

#endif