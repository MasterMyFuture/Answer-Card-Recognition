#include<iostream>
#include <vector>
#include <opencv2/opencv.hpp>



class Axis{
    private:
        


    public:
        std::vector<int> headIndex;
        std::vector<int> endIndex;

        std::vector<int> verShadow;
        std::vector<int> horShadow;

        void getShadowIndex(const std::vector<int> & shadowArray);
        void getImageShadow(const cv::Mat & src);

};
