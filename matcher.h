#ifndef MATCHER_H
#define	MATCHER_H

#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <vector>

class matcher {
public:
    matcher(cv::Point initialPosition, cv::Mat img_fusion);
    matcher(const matcher& orig);
    virtual ~matcher();

    bool match(cv::Mat img);
    void fusion(cv::Mat& img_fusion);

    cv::Mat transformation;
private:
    //static const int threshold = 10;

    cv::Mat img, last_img_fusion;
    cv::Point position, last_position;
};

#endif	/* MATCHER_H */

