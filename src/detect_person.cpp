#include <iostream>
#include <vector>
#include "detect_person.h"
#include "alert.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

DetectPerson::DetectPerson() {}

DetectPerson::~DetectPerson() {}

int DetectPerson::detect(cv::Mat &image, cv::Point &minPoint, cv::Point &maxPoint)
{

    cv::Mat bgr = image.clone();
    int img_w = bgr.cols;
    int img_h = bgr.rows;

    detectornet.load_param("../models/person_detector.param");
    detectornet.load_model("../models/person_detector.bin");

    ncnn::Mat in = ncnn::Mat::from_pixels_resize(bgr.data, ncnn::Mat::PIXEL_BGR2RGB,
                                                 bgr.cols, bgr.rows, detector_size_width, detector_size_height);
    const float mean_vals[3] = {0.f, 0.f, 0.f};
    const float norm_vals[3] = {1 / 255.f, 1 / 255.f, 1 / 255.f};
    in.substract_mean_normalize(mean_vals, norm_vals);

    ncnn::Extractor ex = detectornet.create_extractor();
    ex.set_num_threads(4);
    ex.input("data", in);
    ncnn::Mat out;
    ex.extract("output", out);
    for (int i = 0; i < out.h; i++)
    {
        float x1, y1, x2, y2;
        float pw, ph, cx, cy;
        const float *values = out.row(i);

        x1 = values[2] * img_w;
        y1 = values[3] * img_h;
        x2 = values[4] * img_w;
        y2 = values[5] * img_h;

        pw = x2 - x1;
        ph = y2 - y1;
        cx = x1 + 0.5 * pw;
        cy = y1 + 0.5 * ph;

        x1 = cx - 0.7 * pw;
        y1 = cy - 0.6 * ph;
        x2 = cx + 0.7 * pw;
        y2 = cy + 0.6 * ph;

        if (x1 < 0)
            x1 = 0;
        if (y1 < 0)
            y1 = 0;
        if (x2 < 0)
            x2 = 0;
        if (y2 < 0)
            y2 = 0;

        if (x1 > img_w)
            x1 = img_w;
        if (y1 > img_h)
            y1 = img_h;
        if (x2 > img_w)
            x2 = img_w;
        if (y2 > img_h)
            y2 = img_h;

        // Determine a person within the ROI
        if (x1 > minPoint.x && x2 < maxPoint.x)
        {
            // A person is trespassing here
            cv::rectangle(image, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(0, 0, 255), 2, 8, 0);
        }
        else
            cv::rectangle(image, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(0, 255, 0), 2, 8, 0);
    }
    return 0;
}