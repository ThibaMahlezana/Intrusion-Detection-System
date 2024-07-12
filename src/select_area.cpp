#include <windows.h>
#include <vector>
#include <iostream>
#include "select_area.h"
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

SelectArea::SelectArea() {}

void SelectArea::drawPointsAndShape(cv::Mat &frame, const std::vector<cv::Point> &points, bool shapeDrawn)
{
    for (size_t i = 0; i < points.size(); i++)
    {
        // Draw a small red circle at each clicked point.
        cv::circle(frame, points[i], 5, cv::Scalar(0, 0, 255), -1);
        if (i > 0)
        {
            // Draw a red line between the current point and the previous point.
            cv::line(frame, points[i - 1], points[i], cv::Scalar(0, 0, 255), 2);
        }
    }

    if (shapeDrawn)
    {
        cv::Mat overlay;
        double alpha = 0.3;
        frame.copyTo(overlay);

        // Convert points to the format required by fillPoly
        std::vector<std::vector<cv::Point>> contours;
        contours.push_back(points);

        // Draw a filled polygon with gray color
        cv::fillPoly(overlay, contours, cv::Scalar(128, 128, 128));
        cv::addWeighted(overlay, alpha, frame, 1 - alpha, 0, frame);
    }
}

// void handleMouse(int event, int x, int y)
// {
//     if (event == cv::EVENT_LBUTTONDOWN)
//     {
//         points.push_back(cv::Point(x, y));
//         // drawPointsAndShape(frame, points, shapeDrawn);
//     }
// }

// static void onMouse(int event, int x, int y, int, void *)
// {
//     handleMouse(event, x, y);
// }

// void SelectArea::handleMouse(int event, int x, int y)
// {
//     if (event == cv::EVENT_LBUTTONDOWN)
//     {
//         points.push_back(cv::Point(x, y)); // Store the clicked point.
//         drawPointsAndShape(frame, points, shapeDrawn);
//     }
// }

/*
//-----------------------------------------------------
// cv::Rect SelectArea::getBoundingRect()
// {
//     cv::Point topLeft(INT_MAX, INT_MAX);
//     cv::Point bottomRight(INT_MIN, INT_MIN);

//     for (const cv::Point &p : points)
//     {
//         topLeft.x = std::min(topLeft.x, p.x);
//         topLeft.y = std::min(topLeft.y, p.y);
//         bottomRight.x = std::max(bottomRight.x, p.x);
//         bottomRight.y = std::max(bottomRight.y, p.y);
//     }

//     return cv::Rect(topLeft, bottomRight);
// }
*/

/*
void SelectArea::detectAndDrawPerson(cv::Mat &frame, const std::vector<cv::Point> &points)
{
    if (shapeDrawn)
    {
        // Create a region of interest (ROI) using the points defining the shape.
        if (points.size() > 2)
        {
            std::vector<std::vector<cv::Point>> contours;
            contours.push_back(points);

            // Draw a filled polygon on the ROI with transparent gray color
            cv::Mat roi = frame.clone();
            cv::fillPoly(roi, contours, cv::Scalar(128, 128, 128));

            // Call the person detection function on the ROI.
            detect_person.detect(roi); // Adapt this to your detection function.

            // Draw the detected person within the ROI (you need to adapt this part).
            // For example, you can draw a green bounding box around the detected person.
            // cv::rectangle(frame, cv::Point(10, 10), cv::Point(100, 100), cv::Scalar(0, 255, 0), 2);
        }
    }
}
*/

//---------------------------------------------------------

/**
void SelectArea::selectROI(cv::Mat &frame)
{
    cv::namedWindow("Select Area", cv::WINDOW_NORMAL);
    cv::moveWindow("Select Area", 50, 150);

    HWND cvWindowHandle = ::FindWindow(NULL, "Select Area");
    LONG style = GetWindowLong(cvWindowHandle, GWL_STYLE);
    // Remove title bar and drop shadow.
    style &= ~WS_CAPTION;
    style &= ~WS_THICKFRAME;
    SetWindowLong(cvWindowHandle, GWL_STYLE, style);

    cv::setMouseCallback("Select Area", onMouse, this);

    char key = cv::waitKey(10);

    if (key == 'r' || key == 'R')
    {
        points.clear();
        shapeDrawn = false;
    }
    if (key == 'c' || key == 'C')
    {
        if (points.size() > 2)
        {
            shapeDrawn = true;
        }
    }
    drawPointsAndShape(frame, points, shapeDrawn);
    cv::imshow("Select Area", frame);
}
**/

/**
void SelectArea::displayArea()
{
    cv::namedWindow("Select Area");

    while (true)
    {
        cap >> frame; // Capture a frame from the camera

        if (frame.empty())
        {
            std::cerr << "Error: Could not read frame from the camera." << std::endl;
            break;
        }

        // cv::setMouseCallback("Select Area", onMouse, this);

        char key = cv::waitKey(10);

        if (key == 27)
        { // Press ESC key to exit.
            break;
        }
        if (key == 'r' || key == 'R')
        { // Press 'R' to reset.
            points.clear();
            shapeDrawn = false;
        }
        if (key == 'c' || key == 'C')
        { // Press 'C' to close the polygon.
            if (points.size() > 2)
            {
                shapeDrawn = true;
            }
        }

        drawPointsAndShape(frame, points, shapeDrawn);
        // detectAndDrawPerson(frame, points);
        detect_person.detect(frame);

        cv::imshow("Select Area", frame);
    }

    cap.release();
    cv::destroyAllWindows();
}
**/