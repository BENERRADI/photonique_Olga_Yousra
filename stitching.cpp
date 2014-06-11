#include "stitching.h"
#include "opencv2/stitching/detail/blenders.hpp"
#include <iostream>
#include <timer.h>

using namespace cv;
using namespace std;

vector<Point> calcCorners_dep(vector<Mat> imgs, int dep, bool reverse = true) { //
    unsigned int nb_imgs = imgs.size(); //nb_imgs ==> nombre d'images, imgs.sizes() ==> taille du vecteur entier, exemple : imgs[0].sizes().x = taille de la première image
    vector<Point> corners(nb_imgs); //type point stocke les coordonnées d'un point, liste de point qui s'appel corners de taille du nb_imgs : thg
#pragma omp parallel for firstprivate(dep, nb_imgs)
    for (unsigned int i = 0; i < nb_imgs; i++) { // boucle sur chacune des images
        if (reverse) { //selon le sens où on va
            corners[i].x = (nb_imgs - i) * dep; // sens invere
        } else {
            corners[i].x = i*dep; // sens normal, de lcoin de la ième image : n° de l'image*dep
        }
    }
    return corners;
}

vector<Point> calcCorners_match(vector<Mat> imgs, int template_size, vector<Mat>& matched_imgs) {
    unsigned int nb_imgs = imgs.size();
    vector<Point> corners;

    Rect selection; //structure rectangle qui est définie par x , y ==> largeur / hauteur
    selection.x = (imgs[0].cols / 2)-((template_size / 2) - 1);
    selection.y = (imgs[0].rows / 2)-((template_size / 2) - 1);
    selection.height = template_size;
    selection.width = template_size;

    corners.push_back(Point(0, 0)); //push back ==> rajoute à la fin : comme enfiler
    matched_imgs.push_back(imgs[0]); //rajoute l'image 0 dans l'img fusion

    //#pragma omp parallel for ordered firstprivate(selection, nb_imgs)
    for (unsigned int i = 1; i < nb_imgs; i++) { //boucle sur les autres images (donc toutes sauf la première)
        Mat template_img, result; //définition des variables
        double minVal, maxVal;
        Point minLoc, maxLoc;
        template_img = imgs[i - 1](selection); //sous matrice de l'image précédente
        matchTemplate(imgs[i], template_img, result, TM_SQDIFF_NORMED); //fonction d'open cv
        minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);
        if (minVal < 0.7) {
            Point corn; //définit le coin calculé, c'est le coin de l'image actuelle
            //#pragma omp ordered
            {
                corn.x = corners.back().x + (selection.x - minLoc.x); //calcul le coin en fonction d el'image matchée précédemment
                corn.y = corners.back().y + (selection.y - minLoc.y); //corners.back() ==> dénier coin super gauche d'une image rajouté précédemment
                corners.push_back(corn); //rajoute le coin
                matched_imgs.push_back(imgs[i]); // rajoute l'image dans les image qui ont matchées
            }
        } else {
            return corners; //si minval > 0,7 retourne la liste de coins
        }
    }
    return corners;
}

/*void expo(vector<Mat>& imgs) {
    vector<Mat> masks(imgs.size());
    for (unsigned int i = 0; i < imgs.size(); i++) {
        masks[i].create(imgs[i].size(), CV_8U);
        masks[i].setTo(Scalar::all(255));
    }

    vector<Point> corners = calcCorners_dep(imgs, 25);
    Ptr<ExposureCompensator> compensator = ExposureCompensator::createDefault(ExposureCompensator::GAIN);
    compensator->feed(corners, imgs, masks);
    for (unsigned int i = 0; i < imgs.size(); i++) {
        compensator->apply(i, corners[i], imgs[i], masks[i]);
    }

}*/

/*Mat fusion_dep(vector<Mat>& imgs, int dep, bool reverse) { //on lui passe la liste des images, fais la fusion
    unsigned int nb_imgs = imgs.size();
    Size img_size = imgs[0].size();

    Mat img_fusion;

    //vector<Point> corners = calcCorners_dep(imgs, dep, reverse);
    vector<Mat> matched; // sera la liste des images qui auront matchées
    vector<Point> corners = calcCorners_match(imgs, 100, matched); //met dans matched les images qui ont matchées
    vector<Size> sizes(matched.size(), img_size); //tailles des images, y'a autant d'éléments qui d'images matchées et elles font toutes la même taille

    //Mat mask = Mat(img_size, CV_8U, Scalar(0));
    //mask = imgs[0] > 0;
    //cvtColor(mask, mask, CV_BGR2GRAY);
    //circle(mask, Point(mask.cols / 2, mask.rows / 2), mask.cols / 2, 255, CV_FILLED);
    //namedWindow("mask", CV_WINDOW_AUTOSIZE);
    //imshow("mask", mask);

    Ptr<Blender> bl = Blender::createDefault(Blender::MULTI_BAND);
    bl->prepare(corners, sizes);
    //#pragma omp parallel for firstprivate(mask, corners)
    for (unsigned int i = 0; i < matched.size(); i++) {
        //imgs[i].convertTo(imgs[i], CV_16S);
        Mat mask = matched[i] > 0; //construit un mask, va mettre en blanc tous les pixels de l'img qui sont supérieur, le mask c'est ce que tu gardes le traitement n'est réalisé que sur les pixels supérieurs à 0.
        cvtColor(mask, mask, CV_BGR2GRAY);
        bl->feed(matched[i], mask, corners[i]);
        imwrite("mask.bmp", mask);
    }

    Mat dst_mask;
    bl->blend(img_fusion, dst_mask);
    img_fusion.convertTo(img_fusion, CV_8U);
    return img_fusion;
}*/

bool calcCorners_match_r(Mat& img_fusion, Mat& img, int template_size, float coeff, Point& corner, Point last_corner) {
    Mat eqImg_fus, eqImg;

    Rect selection; //structure rectangle qui est définie par x , y ==> largeur / hauteur
    selection.x = (img.cols / 2)-((template_size / 2) - 1);
    selection.y = (img.rows / 2)-((template_size / 2) - 1);
    selection.height = template_size;
    selection.width = template_size;

    int searchSize = img.cols / 4;

    Point tl, br;
    tl.x = max<int>(last_corner.x - searchSize, 0);
    tl.y = max<int>(last_corner.y - searchSize, 0);
    br.x = min<int>(last_corner.x + img.cols + searchSize, img_fusion.cols - 1);
    br.y = min<int>(last_corner.y + img.rows + searchSize, img_fusion.rows - 1);
    Rect last_img(tl, br);

    equalizeHist(img, eqImg);
    equalizeHist(img_fusion(last_img), eqImg_fus);

    Mat result; //définition des variables
    double minVal, maxVal;
    Point minLoc, maxLoc;
    matchTemplate(eqImg_fus, eqImg(selection), result, TM_SQDIFF_NORMED); //fonction d'open cv
    minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);
    if (minVal < coeff) {
        corner.x = tl.x + (minLoc.x - selection.x); //calcul le coin en fonction d el'image matchée précédemment
        corner.y = tl.y + (minLoc.y - selection.y); //corners.back() ==> dénier coin super gauche d'une image rajouté précédemment
        return true;
    } else {
        return false;
    }
}

/*void fusion_reborn(Mat& img_fusion, Mat& img) {
    vector<Point> corners(2); //met dans matched les images qui ont matchées
    vector<Size> sizes(2); //tailles des images, y'a autant d'éléments qui d'images matchées et elles font toutes la même taille
    corners[0] = Point(0, 0);
    if (calcCorners_match_r(img_fusion, img, 120, 0.3, corners[1])) {
        sizes[0] = img_fusion.size();
        sizes[1] = img.size();

        Ptr<Blender> bl = Blender::createDefault(Blender::MULTI_BAND);
        bl->prepare(corners, sizes);

        Mat mask = img_fusion > 0;
        cvtColor(mask, mask, CV_BGR2GRAY);
        bl->feed(img_fusion, mask, corners[0]);


        //namedWindow("mask", CV_WINDOW_AUTOSIZE);
        //imshow("mask", mask);

        mask = img > 0;
        cvtColor(mask, mask, CV_BGR2GRAY);
        bl->feed(img, mask, corners[1]);

        Mat dst_mask;
        bl->blend(img_fusion, dst_mask);
        img_fusion.convertTo(img_fusion, CV_8U);
        //namedWindow("mask_fus", CV_WINDOW_AUTOSIZE);
        //imshow("mask_fus", dst_mask);
    }
}*/

void Img_mean(Mat& img_fusion, Mat& img, Mat& mask, Mat& dst) {
    Mat gradient = imread("gradient.png", IMREAD_GRAYSCALE);
    gradient.convertTo(gradient, CV_32F, 1.0 / 255.0);
    dst = img_fusion;
#pragma omp parallel for collapse(2) schedule(dynamic, 50)
    for (int y = 0; y < img.rows; y++) {
        for (int x = 0; x < img.cols; x++) {
            uchar fusion_pxl = img_fusion.at<uchar>(y, x);
            if (mask.at<uchar>(y, x) == 255) {
                if (fusion_pxl > 0) {
                    dst.at<uchar>(y, x) = (1.0 - gradient.at<float>(y, x)) * img_fusion.at<uchar>(y, x) +
                            gradient.at<float>(y, x) * img.at<uchar>(y, x);
                } else {
                    dst.at<uchar>(y, x) = img.at<uchar>(y, x);
                }
            }
        }
    }
}


void fusion_maison(Mat& img_fusion, Mat& img, Mat& mask, Point & last_corner) {
    Mat img_fusion_next;
    Point corner;
    bool matched = false;

    timer__("Matching") {
        matched = calcCorners_match_r(img_fusion, img, 250, 0.5, corner, last_corner);
    }
    if (matched) {

        timer__("Fusion") {
            //Calcul de la taille de la nouvelle image
            Point tl;
            Point br;
            tl.x = min<int>(corner.x, 0);
            tl.y = min<int>(corner.y, 0);
            br.x = max<int>(corner.x + img.cols, img_fusion.cols);
            br.y = max<int>(corner.y + img.rows, img_fusion.rows);
            Rect taille(tl, br);

            if (taille.size() == img_fusion.size()) {
                Mat ROI = img_fusion(Rect(corner.x, corner.y, img.cols, img.rows));

                Img_mean(ROI, img, mask, ROI);
                last_corner = corner;
            } else {
                img_fusion_next = Mat::zeros(taille.size(), CV_8U);
                img_fusion.copyTo(img_fusion_next(Rect(-taille.x, -taille.y, img_fusion.cols, img_fusion.rows)));
                Mat ROI = img_fusion_next(Rect(corner.x - taille.x, corner.y - taille.y, img.cols, img.rows));

                Img_mean(ROI, img, mask, ROI);
                img_fusion = img_fusion_next;
                last_corner = Point(corner.x - taille.x, corner.y - taille.y);
            }

        }
    }
    last_corner = corner;
}