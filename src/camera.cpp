#include <iostream>
#include "camera.h"
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

Camera::Camera()
{
    std::cout << "[INFO] Initializing the camera..." << std::endl;
}
Camera::~Camera()
{
    std::cout << "[INFO] Webcam closed." << std::endl;
}

void Camera::captureWebcam()
{
    cv::Mat frame1, frame2;
    cv::VideoCapture cap;
    cap.open(0);
    if (!cap.isOpened())
    {
        std::cout << "[ERROR] Unable to open the camera" << std::endl;
    }

    while (true)
    {
        cap >> frame1;
        cap >> frame2;
        if (frame1.empty() || frame2.empty())
        {
            std::cout << "[ERROR] Cannot read a frame." << std::endl;
            break;
        }

        cv::Mat combined_frame;
        cv::hconcat(frame1, frame2, combined_frame);

        cv::imshow("Two Frames", combined_frame);

        if (cv::waitKey(5) == 27)
            break;
    }
    cap.release();
    cv::destroyAllWindows();
}