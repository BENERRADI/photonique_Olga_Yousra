#include "matcher.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/video/tracking.hpp"
#include <algorithm>
#include <functional>
#include <iostream>
#include <timer.h>

using namespace std;
using namespace cv;

matcher::matcher(Point initialPosition, Mat img_fusion) {
    last_img_fusion = img_fusion;
    position = initialPosition;
}

matcher::matcher(const matcher& orig) {
}

matcher::~matcher() {
}

void matcher::match(Mat img) {
    this->img = img;
    last_position = position;

    Mat transfo = estimateRigidTransform(img, last_img_fusion(Rect(position, img.size())), false);
    if (transfo.rows != 2 && transfo.cols != 3) {
        transfo = Mat(Matx23f(
                1, 0, 0,
                0, 1, 0));
    }

    position.x = last_position.x + transfo.at<double>(0, 2);
    position.y = last_position.y + transfo.at<double>(1, 2);

    transfo.at<double>(0, 2) = 0;
    transfo.at<double>(1, 2) = 0;

    transformation = transfo;
}

void matcher::fusion(Mat& img_fusion) {
    Mat imgW;
    warpAffine(img, imgW, transformation, img.size());

    Point tl;
    Point br;
    tl.x = min<int>(position.x, 0);
    tl.y = min<int>(position.y, 0);
    br.x = max<int>(position.x + img.cols, img_fusion.cols);
    br.y = max<int>(position.y + img.rows, img_fusion.rows);
    Rect taille(tl, br);

    if (taille.size() == img_fusion.size()) {

    } else {
        Mat img_fusion_next = Mat::zeros(taille.size(), CV_8U);
        img_fusion.copyTo(img_fusion_next(Rect(-taille.x, -taille.y, img_fusion.cols, img_fusion.rows)));
        position -= taille.tl();
        img_fusion = img_fusion_next;
    }

    Rect selection = Rect(position, img.size());
    Mat dst = img_fusion(selection);
    Mat gradient = imread("gradient.png", IMREAD_GRAYSCALE);
    resize(gradient, gradient, img.size());
    warpAffine(gradient, gradient, transformation, img.size());

    gradient.convertTo(gradient, CV_32F, 1.0 / 255.0);
#pragma omp parallel for collapse(2) firstprivate(gradient) schedule(dynamic, 50)
    for (int y = 0; y < imgW.rows; y++) {
        for (int x = 0; x < imgW.cols; x++) {
            uchar* fusion_pxl = dst.ptr<uchar>(y, x);
            if (imgW.at<uchar>(y, x) > 1) {
                if (*fusion_pxl > 0) {
                    *fusion_pxl = (1.0 - gradient.at<float>(y, x)) * (*fusion_pxl) +
                            gradient.at<float>(y, x) * imgW.at<uchar>(y, x);
                } else {
                    *fusion_pxl = imgW.at<uchar>(y, x);
                }
            }
        }
    }
    last_img_fusion = img_fusion;
}