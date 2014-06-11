#ifndef MATCHER_H
#define	MATCHER_H

#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <vector>

//#define DEBUG_MATCH

class matcher {
public:
    matcher(cv::Mat initialImg);
    matcher(const matcher& orig);
    virtual ~matcher();

    cv::Matx33f match(cv::Mat img);

    std::vector<cv::DMatch> matches, goodMatches;
private:
    //static const int threshold = 10;

    cv::Mat last_img;
    std::vector<cv::KeyPoint> last_keypoints, keypoints;

    cv::Mat last_descriptor, descriptor;

    cv::GoodFeaturesToTrackDetector detector;
    cv::OrbDescriptorExtractor extractor;
    cv::BFMatcher descMatcher;
};

#endif	/* MATCHER_H */

