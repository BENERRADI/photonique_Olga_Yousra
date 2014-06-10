#ifndef STITCHING_H
#define	STITCHING_H
#include <opencv2/opencv.hpp>
#include <vector>

cv::Mat fusion_dep(std::vector<cv::Mat>& imgs, int dep, bool reverse = false);
void fusion_reborn(cv::Mat& img_fusion, cv::Mat& img);
void fusion_maison(cv::Mat& img_fusion, cv::Mat& img, cv::Mat& mask, cv::Point& last_corner);
#endif	/* STITCHING_H */

