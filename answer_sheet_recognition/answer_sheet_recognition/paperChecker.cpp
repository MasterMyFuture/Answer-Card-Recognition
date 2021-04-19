#include "paperChecker.h"

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
	
	namedWindow("MORPH_CLOSE", cv::WINDOW_NORMAL);
	if (!dstImage.empty())
	{
		imshow("MORPH_CLOSE",dstImage);
	}
	


}


//函数作用： 将图像水平和垂直投影，得到 水平和垂直投影的图
//返回： horizon_out 水平投影的图
//		 vertical_out 垂直投影的图
/**
 * 1.为什么要投影？
*/
void getImageShadow(const cv::Mat & src,std::vector<int> &horizon,std::vector<int> &vertical)
{
	if(src.empty()){
		std::cout<<"img is empty!"<<std::endl;	
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
		horizon[i] = pixelValue;
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
		vertical[i] = pixelValue;
		pixelValue = 0;
	}
	// show it
	/**
	 * 如何把图像投影画出来看？
	 * 2. 为什么没有生成Mat
	*/

	cv::Mat horImage(src.rows, src.cols, CV_8UC1,cv::Scalar(255));
	cv::Mat verImage(src.rows, src.cols, CV_8UC1,cv::Scalar(255));


	// horizon
	for(int i = 0; i < horImage.rows; i++)
	{
		for(int j = 0; j < horizon[i]; j++)
			horImage.at<uchar>(i,j) = 0;  //假设初始化为0；
	}
	for(int i = 0; i < verImage.cols; i ++)
	{
		for(int j = 0; j < vertical[i]; j++)
			verImage.at<uchar>(j,i) = 0;
	}

	imshow("hor",horImage);
	imshow("ver",verImage);
	//imwrite("4.jpg",verImage);
	//imwrite("3.jpg",horImage);

	
}

void getRegion(cv::Mat &edImage){
	// 图像垂直投影 得到定位标位置，并切割
	// 图像水平投影  得到答题卡区域位置，并切割
	// 此段处理的图像源： edImage
	std::vector<int> horizon(edImage.rows);
	std::vector<int> vertical(edImage.cols);
	getImageShadow(edImage,horizon, vertical);
	
	//查找定位标
	std::vector<int> locIndex;
	findVerticalMax(vertical, locIndex);
	cv::Mat locImage;
	edImage(cv::Rect(locIndex[0],0, locIndex[1] - locIndex[0] + 1, edImage.rows-1) ).copyTo(locImage);
	if(!locImage.empty()){
		imshow("LocateImage", locImage);

	}
	
	//如果不要求鲁棒性，就直接水平投影定位标就OK
	std::vector<int> locHorizon(locImage.rows);
	std::vector<int> locVertical(locImage.cols);
	getImageShadow(locImage, locHorizon, locVertical);

	std::vector<int> locUpIndex, locDownIndex;
	findLocHorizon(locHorizon, locUpIndex, locDownIndex);
// #ifdef 0
	if(locUpIndex.size() == locDownIndex.size())
	{
		cv::Mat tempq;
		edImage.copyTo(tempq);
		size_t lengthq = locUpIndex.size();
		for(int i = 0; i < lengthq ; i++)
		{
			cv::Point ppt1 = cv::Point(0,locUpIndex[i]);
			cv::Point ppt2 = cv::Point(tempq.cols-1, locUpIndex[i]);
			cv::Point ppt3 = cv::Point(0,locDownIndex[i]);
			cv::Point ppt4 = cv::Point(tempq.cols-1, locDownIndex[i]);
			line(tempq, ppt1, ppt2, cv::Scalar(0, 0, 100));
			line(tempq, ppt3, ppt4, cv::Scalar(0, 0, 100));
		}
		if (!tempq.empty())
		{
			imshow("srljsf", tempq);
		}
		
	}
}