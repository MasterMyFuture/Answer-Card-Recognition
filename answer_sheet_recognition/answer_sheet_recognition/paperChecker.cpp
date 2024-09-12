#include "paperChecker.h"
#include <vector>
#include <string>

void preprocess(cv::Mat &rsrcImage, cv::Mat &dstImage){
	cv::Mat rGrayImage;
	cv::Mat rBinImage;


	cvtColor(rsrcImage,rGrayImage, cv::COLOR_BGR2GRAY );  //灰度化
	//CV_THRESH_OTSU参数自动生成阈值，跟第三个参数也就没有关系了。 
    threshold(rGrayImage, rBinImage, 0, 255,  cv::THRESH_BINARY | cv::THRESH_OTSU); //二值化
	///imshow("binary image", rBinImage);
	cv::Mat erodeImage, dilateImage;
	//定义核  
	cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));   
	//进行形态学操作 	
	morphologyEx(rBinImage, dstImage, cv::MORPH_CLOSE, element,cv::Point(-1,-1),1);
	
	
	if (!dstImage.empty()&&DEBUG)
	{
		namedWindow("MORPH_CLOSE", cv::WINDOW_NORMAL);
		imshow("MORPH_CLOSE",dstImage);
	}
	


}



/**
 * 	 图像垂直投影 得到定位标位置，并切割
	 图像水平投影  得到答题卡区域位置，并切割
	 此段处理的图像源： edImage
 * 
*/
void getYLocate(cv::Mat &edImage){

	Axis *xcoord = new Axis();

	//std::vector<int> horizon(edImage.rows);
	//std::vector<int> vertical(edImage.cols);
	//getImageShadow(edImage,horizon, vertical);
	xcoord->getImageShadow(edImage);
	
	

	if(!xcoord->verShadow.empty()){
		//getShadowIndex(vertical, headIndex,endIndex);
		xcoord->getShadowIndex(xcoord->verShadow);
	}
	cv::Mat locImage;
	if(xcoord->headIndex.empty()&&xcoord->endIndex.empty()){
		std::cout<<"locIndex is empty!"<<std::endl;

	}else{
		// 切割黑条
		edImage(cv::Rect(xcoord->headIndex.back(),0, xcoord->endIndex.back()-xcoord->headIndex.back(), edImage.rows-1) ).copyTo(locImage);
		if(DEBUG){
			imshow("LocateImage", locImage);
		}
	}

	
	//如果不要求鲁棒性，就直接水平投影定位标就OK
	Axis *ycoord = new Axis();
	ycoord->getImageShadow(locImage);

	
	if (!ycoord->horShadow.empty())
	{
		//getShadowIndex(locHorizon, locUpIndex, locDownIndex);
		ycoord->getShadowIndex(ycoord->horShadow);
	}
	
// #ifdef 0
	if(ycoord->headIndex.size() == ycoord->endIndex.size()&&DEBUG)
	{
		cv::Mat tempq;
		edImage.copyTo(tempq);
		size_t lengthq = ycoord->headIndex.size();
		for(int i = 0; i < lengthq ; i++)
		{
			cv::Point ppt1 = cv::Point(0,ycoord->headIndex[i]);
			cv::Point ppt2 = cv::Point(tempq.cols-1, ycoord->headIndex[i]);
			cv::Point ppt3 = cv::Point(0,ycoord->endIndex[i]);
			cv::Point ppt4 = cv::Point(tempq.cols-1, ycoord->endIndex[i]);
			line(tempq, ppt1, ppt2, cv::Scalar(100));
			line(tempq, ppt3, ppt4, cv::Scalar(100));
		}
		if (!tempq.empty())
		{
			imshow("srljsf", tempq);
		}
		
		
	}

	getAnswerList(edImage, ycoord);
	
}

void getAnswerList(cv::Mat &edImage, Axis *ycoord){

	cv::Mat	answer = edImage(cv::Rect(0,ycoord->endIndex[11], edImage.cols-1, ycoord->headIndex[17]-ycoord->endIndex[11]));

	Axis *anscoord = new Axis();
	anscoord->getImageShadow(answer);
	//std::vector<int> ansUpIndex, ansDownIndex;
	//getShadowIndex(ansVertical, ansUpIndex, ansDownIndex);
	
	anscoord->getShadowIndex(anscoord->verShadow);
	if(DEBUG){
		imshow("answer", answer);
	}
	std::vector<std::string> answerList(NUMS);
	Axis *sincoord;
	for(int i = 0; i < NUMS; i ++)
	{
		cv::Mat tempAns = answer(cv::Rect(anscoord->headIndex[i], 0, anscoord->endIndex[i] - anscoord->headIndex[i] , answer.rows));
		sincoord = new Axis();
		//std::vector<int> tmpHorizon(tempAns.rows);
		//std::vector<int> tmpVertical(tempAns.cols);
		//getImageShadow(tempAns,sincoord->horShadow, sincoord->verShadow);
		sincoord->getImageShadow(tempAns);
		findAnswer(sincoord->horShadow, ycoord, answerList[i]);
		std::cout << i+1 << " : "<< answerList[i] << std::endl;
	}
}



void findAnswer(const std::vector<int> & input, Axis *y, std::string & ans)
{
	size_t length = input.size();
	int m, n;
	for(int i = 1; i < length; i++)
	{
		if(input[i-1] == 0 && input[i] >0)
		{
			m = i;	
		}
		else if(input[i-1] > 0 && input[i] == 0 )
		{
			n = i;
		}
	}
	int mid = (int)(m+n)/2 + y->endIndex[11];
	if(mid >= y->headIndex[13] && mid <= y->endIndex[13])  //A
	{
		ans = 'A';
	}
	else if(mid >= y->headIndex[14] && mid <= y->endIndex[14])
	{
		ans = 'B';
	}
	else if(mid >= y->headIndex[15] && mid <= y->endIndex[15])
	{
		ans = 'C';
	}
	else if(mid >= y->headIndex[16] && mid <= y->endIndex[16]) //D
	{
		ans = 'D';
	}
	else
	{
		ans = "None";
	}
}

