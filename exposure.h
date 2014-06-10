#ifndef EXPOSURE_H
#define	EXPOSURE_H
#include <opencv2/opencv.hpp>

/**
 * Polynom initialization
 * @return The calculated value for each point within a matrix
 */
cv::Mat calcPoly();

void exposure(cv::Mat& img, cv::Mat poly);
#endif	/* EXPOSURE_H */

