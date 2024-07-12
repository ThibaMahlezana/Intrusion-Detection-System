#include <iostream>
#include <stdio.h>
#include <vector>
#include "alert.h"
#include "camera.h"
#include "detect_person.h"
#include "select_area.h"
#include "main_window.h"
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#define TEST_GUI
// #define TEST_DISPLAY
// #define TEST_DETECT

int main()
{
    std::cout << "[INFO] Starting Security System ..." << std::endl;
    // Alert alert;
    // Testing Alarm
    // alert.soundBuzzer();

    // Testing Internet connection
    // alert.checkInternetConnection();

    // Testing Camera Capture
    // Camera camera;
    // camera.captureWebcam();

#ifdef TEST_DISPLAY
    SelectArea select_area;
    select_area.displayArea();
#endif

#ifdef TEST_DETECT
    std::cout << "[INFO] Detecting people..." << std::endl;
    cv::Mat frame;
    // cv::VideoCapture cap("people.mp4");
    cv::VideoCapture cap(0);
    if (!cap.isOpened())
    {
        std::cout << "[ERROR] Unable to open the camera" << std::endl;
    }

    std::vector<cv::Point> points;
    points.push_back(cv::Point(600, 10));
    points.push_back(cv::Point(400, 50));
    points.push_back(cv::Point(450, 300));
    points.push_back(cv::Point(400, 420));
    points.push_back(cv::Point(620, 420));

    cv::Point minPoint;
    cv::Point maxPoint;

    // Determining the maximum point and the minimum point
    if (!points.empty())
    {
        minPoint = points[0];
        maxPoint = points[0];

        for (const auto &point : points)
        {
            if (point.x < minPoint.x || (point.x == minPoint.x && point.y < minPoint.y))
                minPoint = point;

            if (point.x > maxPoint.x || (point.x == maxPoint.x && point.y > maxPoint.y))
                maxPoint = point;
        }

        std::cout << "Minimum Point: (" << minPoint.x << ", " << minPoint.y << ")" << std::endl;
        std::cout << "Maximum Point: (" << maxPoint.x << ", " << maxPoint.y << ")" << std::endl;
    }
    else
    {
        std::cout << "No points in the vector." << std::endl;
    }

    DetectPerson detect_person;

    while (true)
    {
        cap >> frame;
        if (frame.empty())
        {
            std::cout << "[ERROR] Cannot read a frame." << std::endl;
            break;
        }
        // Region of interest
        for (size_t i = 0; i < points.size(); i++)
        {
            cv::circle(frame, points[i], 5, cv::Scalar(0, 0, 255), -1);
        }

        detect_person.detect(frame, minPoint, maxPoint);

        cv::imshow("Detect Person", frame);

        if (cv::waitKey(5) == 27)
            break;
    }
    cap.release();
    cv::destroyAllWindows();

#endif

#ifdef TEST_GUI
    MainWindow main_window;
    main_window.displayMainWindow();
#endif

    return 0;
}