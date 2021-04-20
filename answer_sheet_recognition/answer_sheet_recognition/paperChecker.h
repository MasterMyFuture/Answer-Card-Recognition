#include <opencv2/opencv.hpp>
#include <vector>
#include <string>
#define DEBUG 0
#define NUMS 20  // 一栏题目数量



void preprocess(cv::Mat &rsrcImage, cv::Mat &dstImage);

//int getImageShadow(const cv::Mat & src);

void getImageShadow(const cv::Mat & src,std::vector<int> &horizon,std::vector<int> &vertical);



void getAnswerRegion(cv::Mat &edImage, std::vector<int> & locUpIndex, std::vector<int> & locDownIndex);

void getYLocate(cv::Mat &edImage,std::vector<int> & headIndex, std::vector<int> & endIndex);
void getYLocate(cv::Mat &edImage);

void getShadowIndex(const std::vector<int> & inputArray, std::vector<int> & begin, std::vector<int> & end);

void findAnswer(const std::vector<int> & horShadow,
     const std::vector<int> locUp, const std::vector<int> locDown, std::string & ans);

void getAnswerList(cv::Mat &edImage, const std::vector<int> locUpIndex, const std::vector<int> locDownIndex);

     