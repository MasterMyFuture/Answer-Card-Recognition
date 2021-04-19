#include <opencv2\opencv.hpp>


void preprocess(cv::Mat &rsrcImage, cv::Mat &dstImage);

//int getImageShadow(const cv::Mat & src);

void getImageShadow(const cv::Mat & src,std::vector<int> &horizon,std::vector<int> &vertical);

void findVerticalMax(const std::vector<int> & inputArray, std::vector<int> & maxIndex);

int findVerMax(const std::vector<int> & inputArray, std::vector<int> & upIndex, std::vector<int> & downIndex);

void findLocHorizon(const std::vector<int> & inputArray, std::vector<int> & locUp, std::vector<int> & locDown);

void getRegion(cv::Mat &edImage);