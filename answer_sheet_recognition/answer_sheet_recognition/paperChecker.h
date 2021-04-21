#include <opencv2/opencv.hpp>
#include <vector>
#include <string>
#include "axis.h"

#define DEBUG 1
#define NUMS 20  // 一栏题目数量



void preprocess(cv::Mat &rsrcImage, cv::Mat &dstImage);






void getYLocate(cv::Mat &edImage);



void findAnswer(const std::vector<int> & horShadow,Axis *coord, std::string & ans);


void getAnswerList(cv::Mat &edImage, Axis *coord);

     