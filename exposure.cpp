#include <cmath>
#include "exposure.h"
#include <iostream>

#define COUNT(x)  (sizeof(x) / sizeof(x[0]))

using namespace cv;

const float coeff[16] = {
    1.17282794e+02, 1.86158803e+00, -1.10957450e-02, 1.44598186e-05,
    9.23658877e-01, -1.23909653e-02, 1.26146176e-04, -3.71264252e-07,
    -8.63965319e-03, 7.09884372e-05, -8.29506717e-07, 2.77218067e-09,
    2.10482548e-05, -1.30565698e-07, 1.77961643e-09, -6.53321841e-12
};

float polyVal2D(float x, float y) {
    int order = sqrt(COUNT(coeff)) - 1;
    int i, j;
    float z = 0.0;
    for (j = 0; j < (order + 1); j++)
        for (i = 0; (i < order + 1); i++) {
            z += coeff[j * (order + 1) + i] * pow(x, i) * pow(y, j);
        }
    return MIN(z, 255.0);
}

Mat calcPoly() {
    Mat out(250, 250, CV_32F);

    int x, y;
    float *ptr;
    for (y = 0; y < out.rows; y++)
        for (x = 0; x < out.cols; x++) {
            ptr = out.ptr<float>(y, x);
            *ptr = polyVal2D(x, y);
        }

    double poly_min, poly_max;
    minMaxLoc(out, &poly_min, &poly_max, NULL, NULL, Mat());
    out.convertTo(out, CV_32F, 1.0 / poly_max);
    return out;
}
#define USE_THRES 0

void exposure(Mat& img, Mat poly) {
#if USE_THRES
    Mat aux;
    divide(img, poly, aux, 1, CV_8U);
    aux.copyTo(img, img > 70);
#else
    divide(img, poly, img, 1, CV_8U);
#endif
}

