#include <cstdlib>
#include <iostream>
#include <string>
//Pour openImages
#include <vector>
#include <string>
#include <iomanip>
//
#include <opencv2/opencv.hpp>
#include "matcher.h"
#include "correction.h"
#include "stitching.h"
#include <timer.h>

using namespace std;
using namespace cv;

vector<Mat> openImages(string path, int n) { //string path : chemin de l'image, n : nb d'images
    int i;
    vector<Mat> imgs; //liste d'images dans la variable imgs
    imgs.resize(n); //pré allouer la bonne taille dès le début
    stringstream filename; //permet la concaténation
    Mat img; //une image
#pragma omp parallel for private(img, filename) firstprivate(n, path) // parallélisation
    for (i = 1; i < n + 1; i++) { // charge toutes les images une par une
        //Reset filename
        filename.clear(); //réinitialise
        filename.str("");

        filename << path << setw(3) << setfill('0') << i << ".jpg"; //setw : taille de x chiffres, setfill : complète i par des 0 jusqu'à atteindre le nb setw
#pragma omp critical
        cout << "Loading " << filename.str() << endl;
        img = imread(filename.str(), IMREAD_GRAYSCALE);
        if (!img.data) { // Check for invalid input ==> si rien n'a été chargé
#pragma omp critical
            cerr << "Impossible d'ouvrir " << filename.str() << endl;
        } else {
            imgs[i - 1] = img; // si l'image est conforme / lu, elle est stockée dans un tableau
        }
    }
    return imgs;
}

int main(int argc, char** argv) {
    string path; //chemin des images
    if (argc < 2) {
        path = "../photonique/Endoscope non lineaire_champ 250um/"; // valeur par défaut si on ne passe pas d'argument
        //path = "../photonique/images_test/";
    } else {
        path = string(argv[1]); // si on passe un arguement cela devient le chemin
    }

    //Mat swirl_data;
    //swirl_init(Size(450, 450), Point(225, 225), 0.022 * 450, -75 / 180 * M_PI, 0, swirl_data);

    VideoCapture cap(path + "%03d.jpg");
    //VideoCapture cap(0);
    cap.set(CAP_PROP_FPS, 8);

    namedWindow("Camera", WINDOW_AUTOSIZE);
    namedWindow("Panorama", WINDOW_AUTOSIZE);

    Mat img;

    //Traitement de la première image
    while (!cap.read(img));

    cvtColor(img, img, COLOR_BGR2GRAY);

    Interpolation(img, img);
    Mat img_fusion = Mat::zeros(768 * 2, 1024 * 2, CV_8U);
    Point img_center_fusion;
    img_center_fusion.x = img_fusion.cols / 2 - img.cols / 2;
    img_center_fusion.y = img_fusion.rows / 2 - img.rows / 2;
    img.copyTo(img_fusion(Rect(img_center_fusion, Size(img.cols, img.rows))));

    //Mat mask = Mat::zeros(img.size(), CV_8U);
    //circle(mask, Point(mask.cols / 2, mask.rows / 2), mask.rows / 2, 255, CV_FILLED);
    Mat mask(img.size(), CV_8U, 255);
    Point last_corner = img_center_fusion;

    matcher m(img_center_fusion, img_fusion);

    while (true) {

        if (cap.read(img)) {
            cvtColor(img, img, COLOR_BGR2GRAY);

            timer__("Correction") {
                Interpolation(img, img);
                //swirl(img, img, swirl_data);
            }

            timer__("Matching")
            m.match(img);

            timer__("Fusion")
            m.fusion(img_fusion);

            Mat img_fusion_mini;
            resize(img_fusion, img_fusion_mini, Size(), 0.5, 0.5, INTER_LINEAR);

            imshow("Panorama", img_fusion_mini);
            imshow("Camera", img);
            //waitKey();
        }
        if (waitKey(30) >= 0) break;
    }

    if (argc < 3) {
        imwrite("out.bmp", img_fusion);
    } else {
        imwrite(string(argv[2]), img_fusion); // si un troisième argument est fournit cela l'enregistre, imwrite écrit/enregistre l'image dans le chemin d'argv[2]
    }

    return EXIT_SUCCESS;
}

