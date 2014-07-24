#include "optimization.h"
#include "matcher.h"
#include <iostream>
#include <timer.h>

using namespace std;
using namespace cv;

matcher::matcher(Rect initialPosition, InputArray img_fusion, Matching_Mode mode) {
    last_img_fusion = img_fusion.getMat();
    last_position = initialPosition.tl();
    last_img = last_img_fusion(initialPosition);

    //Premier canal : somme
    //Deuxième canal : nombre de valeurs
    redondance.create(last_img_fusion.size(), CV_32FC2);
    update_redondance(last_img, initialPosition.tl());

    this->mode = mode;

    gradient = imread("gradient.png", IMREAD_GRAYSCALE);
    resize(gradient, gradient, initialPosition.size());
    gradient.convertTo(gradient, CV_32F, 1.0 / 255.0);
}

matcher::~matcher() {
}

void matcher::update_redondance(InputArray _img, Point position, InputArray _gradient) {
    Mat gradient = _gradient.getMat(), img = _img.getMat();
#pragma omp parallel for collapse(2) schedule(dynamic, 50)
    for (int y = 0; y < img.rows; y++) {
        for (int x = 0; x < img.cols; x++) {
            if (img.at<uchar>(y, x) > 0) {
                if (gradient.data && redondance.at<Vec2f>(y + position.y, x + position.x)[1] > 0) {
                    redondance.at<Vec2f>(y + position.y, x + position.x)[0] += (1.0 - gradient.at<float>(y, x)) * img.at<uchar>(y, x);
                } else {
                    redondance.at<Vec2f>(y + position.y, x + position.x)[0] += img.at<uchar>(y, x);
                }
                redondance.at<Vec2f>(y + position.y, x + position.x)[1] += 1;
            }
        }
    }
#ifdef SHOW_REDONDANCE
    namedWindow("Redondance Somme", WINDOW_AUTOSIZE);
    namedWindow("Redondance Nombre", WINDOW_AUTOSIZE);
    Mat somme, nombre;
    vector<Mat> channels(2);
    split(redondance, channels);
    normalize(channels[0], somme, 0, 255, NORM_MINMAX, CV_8U);
    normalize(channels[1], nombre, 0, 255, NORM_MINMAX, CV_8U);
    imshow("Redondance Somme", somme);
    imshow("Redondance Nombre", nombre);
    imwrite("red_somme.png", somme);
    imwrite("red_nombre.png", nombre);
    //waitKey();
#endif
}

void matcher::next_img() {
    last_position = position;
    last_img = img;
}

void matcher::doBlend(InputOutputArray _img_fusion, InputArray _gradient, InputArray _img) {
    Mat img = _img.getMat();
    Mat img_fusion = _img_fusion.getMat();

    Point tl;
    Point br;
    tl.x = min<int>(cvRound(position.x), 0);
    tl.y = min<int>(cvRound(position.y), 0);
    br.x = max<int>(cvRound(position.x) + img.cols, img_fusion.cols);
    br.y = max<int>(cvRound(position.y) + img.rows, img_fusion.rows);
    Rect taille(tl, br);

    if (unlikely(taille.size() != img_fusion.size())) {
        Mat img_fusion_next = Mat::zeros(taille.size(), CV_8U);
        Mat redondance_next = Mat::zeros(taille.size(), CV_32FC2);
        img_fusion.copyTo(img_fusion_next(Rect(-taille.x, -taille.y, img_fusion.cols, img_fusion.rows)));
        redondance.copyTo(redondance_next(Rect(-taille.x, -taille.y, redondance.cols, redondance.rows)));
        position.x -= taille.tl().x;
        position.y -= taille.tl().y;
        last_position.x -= taille.tl().x;
        last_position.y -= taille.tl().y;
        img_fusion = img_fusion_next;
        redondance = redondance_next;
        _img_fusion.assign(img_fusion_next);
    }

    Rect selection = Rect(position, img.size());
    Mat dst = img_fusion(selection);

    update_redondance(img, selection.tl(), _gradient);


#pragma omp parallel for collapse(2) schedule(dynamic, 50)
    for (int y = 0; y < dst.rows; y++) {
        for (int x = 0; x < dst.cols; x++) {
            Vec2f red_pix = redondance.at<Vec2f>(position.y + y, position.x + x);
            if (red_pix[1] > 0)
                dst.at<uchar>(y, x) = red_pix[0] / red_pix[1];
        }
    }

    last_img_fusion = img_fusion;
}