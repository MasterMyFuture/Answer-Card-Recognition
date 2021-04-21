#include "axis.h"

void Axis::getShadowIndex(const std::vector<int> & shadowArray){
    size_t length = shadowArray.size();  //
	int upNums = 0, downNums = 0;
	for(int i = 1; i < length; i++)
	{
		if(shadowArray[i-1] == 0 && shadowArray[i] > 0)
		{
			this->headIndex.push_back(i);
			upNums ++;
		}
		else if(shadowArray[i-1] > 0 && shadowArray[i] == 0)
		{
			this->endIndex.push_back(i);
			downNums ++;
		}
		
	}
	if(shadowArray.back()!= 0){
		this->endIndex.push_back(shadowArray.size());
	}

}

void Axis::getImageShadow(const cv::Mat & src){
    if(src.empty()){
		std::cout<<"src img is empty in getImageShadow()!"<<std::endl;	
	}
	// 对src进行二值化值统计
	//horizontal 水平

	
	int pixelValue = 0;
	for(int i = 0; i < src.rows; i++)   //有多少行
	{
		for(int j = 0; j < src.cols; j++)
		{
			if(src.at<uchar>(i,j) == 0)
				pixelValue++;
		}
		this->horShadow[i] = pixelValue;
		pixelValue = 0;
	}

	//vertical 垂直
	//vector<int> vertical_out;
	for(int i = 0; i < src.cols; i++)
	{
		for(int j = 0; j < src.rows; j ++)
		{
			if(src.at<uchar>(j,i) == 0)
				pixelValue++;
		}
		this->verShadow[i] = pixelValue;
		pixelValue = 0;
	}

}