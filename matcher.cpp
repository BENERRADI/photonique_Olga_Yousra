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

matcher::matcher(Mat initialImg) {
    Mat eqImg;
    equalizeHist(initialImg, eqImg);
    detector = GoodFeaturesToTrackDetector(1000, 0.3, 1.2, 7, false, 0.04);
    last_img = initialImg;
    detector.detect(last_img, last_keypoints);
    extractor.compute(last_img, last_keypoints, last_descriptor);
    descMatcher = BFMatcher(NORM_HAMMING);
}

matcher::matcher(const matcher& orig) {
}

matcher::~matcher() {
}

Matx33f matcher::match(Mat img) {
    detector.detect(img, keypoints);
    extractor.compute(img, keypoints, descriptor);
    descMatcher.match(last_descriptor, descriptor, matches);

    float min_dist = min_element(matches.begin(), matches.end())->distance;

    goodMatches.clear();

    cout << min_dist << endl;
    for (int i = 0; i < last_descriptor.rows; i++) {
        if (matches[i].distance <= max(2 * min_dist, 10.0f))
            goodMatches.push_back(matches[i]);
    }

#ifdef DEBUG_MATCH
    Mat img_matches;
    drawMatches(last_img, last_keypoints, img, keypoints, goodMatches, img_matches,
            Scalar::all(-1), Scalar::all(-1), vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
    namedWindow("Matches", CV_WINDOW_AUTOSIZE);
    imshow("Matches", img_matches);
    waitKey();
#endif

    vector<Point2f> src(goodMatches.size()), dst(goodMatches.size());
    for (int i = 0; i < goodMatches.size(); i++) {
        dst[i] = last_keypoints[goodMatches[i].queryIdx].pt;
        src[i] = keypoints[goodMatches[i].trainIdx].pt;
    }

    Matx33f transfo;
    if (src.size() >= 4) {
        transfo = findHomography(src, dst, CV_LMEDS);
    } else {
        transfo = Matx33f(
                1, 0, 0,
                0, 1, 0,
                0, 0, 1);
    }

    last_img = img;
    swap(last_keypoints, keypoints);
    last_descriptor = descriptor;
    return transfo;
}