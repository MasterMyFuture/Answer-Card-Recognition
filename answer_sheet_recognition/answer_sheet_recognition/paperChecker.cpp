#include "paperChecker.h"
#include "axis.h"
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


//函数作用： 将图像水平和垂直投影，得到 水平和垂直投影的图
//返回： horizon_out 水平投影的图
//		 vertical_out 垂直投影的图
/**
 * 1.为什么要投影？
*/
void getImageShadow(const cv::Mat & src,std::vector<int> &horizon,std::vector<int> &vertical)
{
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

	//imshow("hor",horImage);
	//imshow("ver",verImage);

	if(verImage.empty()&&horImage.empty()){
		std::cout<<"verImage or horImage is empty!"<<std::endl;
	}else{
		imwrite("D:/Users/Dell/Documents/GitHub/Answer-Card-Recognition/pic/verImage.jpg",verImage);
		imwrite("D:/Users/Dell/Documents/GitHub/Answer-Card-Recognition/pic/horImage.jpg",horImage);
	}

	
}
/**
 * 	 图像垂直投影 得到定位标位置，并切割
	 图像水平投影  得到答题卡区域位置，并切割
	 此段处理的图像源： edImage
 * 
*/
void getYLocate(cv::Mat &edImage){

	std::vector<int> horizon(edImage.rows);
	std::vector<int> vertical(edImage.cols);
	getImageShadow(edImage,horizon, vertical);
	
	Axis *xcoord = new Axis();

	if(!vertical.empty()){
		//getShadowIndex(vertical, headIndex,endIndex);
		xcoord->getShadowIndex(vertical);
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
	std::vector<int> locHorizon(locImage.rows);
	std::vector<int> locVertical(locImage.cols);
	getImageShadow(locImage, locHorizon, locVertical);

	Axis *ycoord = new Axis();
	if (!locHorizon.empty())
	{
		//getShadowIndex(locHorizon, locUpIndex, locDownIndex);
		ycoord->getShadowIndex(locHorizon);
	}
	
// #ifdef 0
	if(ycoord->headIndex.size() == ycoord->endIndex.size())
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

	getAnswerList(edImage, ycoord->headIndex, ycoord->endIndex);
	
}

void getAnswerList(cv::Mat &edImage, const std::vector<int> locUpIndex, const std::vector<int> locDownIndex){

	cv::Mat	answer = edImage(cv::Rect(0,locDownIndex[11], edImage.cols-1, locUpIndex[17]-locDownIndex[11]));

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
		std::vector<int> tmpHorizon(tempAns.rows);
		std::vector<int> tmpVertical(tempAns.cols);
		getImageShadow(tempAns,sincoord->horShadow, sincoord->verShadow);
		findAnswer(tmpHorizon, sincoord->horShadow, sincoord->verShadow, answerList[i]);
		std::cout << i+1 << " : "<< answerList[i] << std::endl;
	}
}





void getShadowIndex(const std::vector<int> & inputArray, std::vector<int> & begin, std::vector<int> & end){
	size_t length = inputArray.size();  //
	int upNums = 0, downNums = 0;
	for(int i = 1; i < length; i++)
	{
		if(inputArray[i-1] == 0 && inputArray[i] > 0)
		{
			begin.push_back(i);
			upNums ++;
		}
		else if(inputArray[i-1] > 0 && inputArray[i] == 0)
		{
			end.push_back(i);
			downNums ++;
		}
		
	}
	if(inputArray.back()!= 0){
		end.push_back(inputArray.size());
	}



}

/**
 * 1. 为什么要locUp和locDown两个向量？
 * 		因为loc表示右边条纹
 * 2. 为什么它可以区分右边的条纹和答案？
 * 
 * 3. 它是如何判断ABCD？
 * 		求出填涂答案的上边界m和下边界n，求出水平中位线
 * 		用中位线和右边条纹的上下boder比
 * 
 * 4. 为什么要求mid？
 * 		求填涂答案的水平中位线
*/
void findAnswer(const std::vector<int> & input, const std::vector<int> locUp, const std::vector<int> locDown, std::string & ans)
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
	int mid = (int)(m+n)/2 + locDown[11];
	if(mid >= locUp[13] && mid <= locDown[13])  //A
	{
		ans = 'A';
	}
	else if(mid >= locUp[14] && mid <= locDown[14])
	{
		ans = 'B';
	}
	else if(mid >= locUp[15] && mid <= locDown[15])
	{
		ans = 'C';
	}
	else if(mid >= locUp[16] && mid <= locDown[16]) //D
	{
		ans = 'D';
	}
	else
	{
		ans = "None";
	}
}

