//功能： 透视变换完成图像畸变矫正
//缺点： 由于答题卡形状问题，无法完成角点自动选取，故采用鼠标人工选择
//作者： liangzelang（liangzelang@gmail.com）
//时间： 2017/05/18
//版本： 0.01

#include <opencv2\opencv.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include "paperChecker.h"

using namespace std;
using namespace cv;
const int NUMS = 20;
vector<Point2f> srcpt(4);
vector<Point2f> dstpt(4);
vector<string> Answer(NUMS);
int ptflag=0;
int threshold_value = 100;
Mat pic;
Mat perImage;
Mat pdstImage;
Mat pmidImage;
Mat psrcImage;


static void on_Mouse(int event, int x, int y, int flags, void *);
void on_Change(int, void*);
void process_Pic1(Mat srcImage);
void project_Pic(const Mat & src, vector<int> & horizon_out, vector<int> & vertical_out);
void findMax(const vector<int> & inputArray, vector<int> & maxIndex);
void picCut(const Mat & inputArray, Mat & outputArray, const Rect & rectRoi);
void findAnswer(const vector<int> & input, const vector<int> locUp, const vector<int> locDown, string & ans);
int main()
{
	Mat dstImage;
	Mat midImage;
	bool isPers = true;
	//pic = imread("D:/Users/Dell/Documents/GitHub/Answer-Card-Recognition/pic/1.jpg",1);

	if(isPers){
		Mat srcImage = imread("D:/Users/Dell/Documents/GitHub/Answer-Card-Recognition/pic/Perspectived.jpg",1);
		//Mat scale(640,480,CV_8UC1,Scalar(0));
		std::vector<int> horizon(srcImage.rows);
		std::vector<int> vertical(srcImage.cols);
		preprocess(srcImage,dstImage);
		getAnswerRegion(dstImage);

	}else{
	//getImageShadow(dstImage,horizon,vertical);
	//namedWindow("input",WINDOW_NORMAL);
	//imshow("input",dstImage);
	//setMouseCallback("input",on_Mouse,0);
	//imshow("scale",scale);
	//getRegion(dstImage);
	//namedWindow("src", WINDOW_NORMAL);
	//imshow("src",srcImage);

	}

	//process_Pic1(srcImage);
	waitKey(0);
	return 0;

}

int findVerMax(const vector<int> & inputArray, vector<int> & upIndex, vector<int> & downIndex)
{
	size_t length = inputArray.size();  //0 黑色  255白色
	int maxValue = inputArray[0];
	int m = 0,n= 0;
	int maxIndex = 0;
	for(int i = 1; i < length-1; i++)
	{
		if(inputArray[i-1] ==0 && inputArray[i] > 0)
		{
			upIndex[m] = i; 
			m++;
		}
		else if(inputArray[i-1] > 0 && inputArray[i] == 0)
		{
			downIndex[n] = i;
			n++;
		}
		if(maxValue < inputArray[i])
		{
			maxValue = inputArray[i];
			maxIndex = i;
		}
	}
	return maxIndex;
}

/**
 * 1. 为什么要写这个函数？
 * 
*/

void findVerticalMax(const vector<int> & inputArray, vector<int> & maxIndex)
{
	size_t length = inputArray.size();
	int maxVal = 0;
	int maxInd = 0;
	//找到最大的index
	for(int i = 0; i < length; i++)
	{
		if(maxVal < inputArray[i])
		{
			maxVal = inputArray[i];
			maxInd = i;
		}
	}
	//寻找定位标左边界
	for(int i = maxInd; i > 0; i--)
	{
		if(inputArray[i] == 0 && inputArray[i+1] > 0)
		{
			maxIndex.push_back(i);
			break;
		}
			
	}
	//寻找定位标右边界
	for(int i = maxInd; i < length; i++)
	{
		if(inputArray[i] == 0 && inputArray[i-1] > 0)
		{
			maxIndex.push_back(i);
			break;
		}
	}
}

void findMax(const vector<int> & inputArray, vector<int> & maxIndex)
{
	//vector<int> maxIndex(2);
	size_t length = inputArray.size();
	int maxValue1 = inputArray[0];
	maxIndex[0] = 0;
	for(int i = 1; i < length/2; i++)
	{
		if(maxValue1 < inputArray[i])
		{
			maxValue1 = inputArray[i];
			maxIndex[0] = i;
		}
	}
	int m = maxIndex[0];
	while(inputArray[m]>3)
		m++;
	maxIndex[2] = m;
	int maxValue2 = inputArray[length/2];
	maxIndex[1] = length/2;
	for(int i = length/2; i < length; i++)
	{
		if(maxValue2 < inputArray[i])
		{
			maxValue2 = inputArray[i];
			maxIndex[1] = i;
		}
	}
	int n = maxIndex[1];
	while(inputArray[n]>3)
		n--;
	maxIndex[3] = n;
}

//第一种方法，使用形态学滤波的方式对图像预处理

void process_Pic1(Mat rsrcImage)
{

// 图像预处理
// 得到二值化图像，基本能够分离出涂卡位置，为后面处理提供源
// 这个阶段重难点： 1、透视变换的准确性   2、二值化阈值的选取合理性 3、形态学滤
	Mat rGrayImage;
	Mat rBinImage;


	cvtColor(rsrcImage,rGrayImage, COLOR_BGR2GRAY );  //灰度化
	//CV_THRESH_OTSU参数自动生成阈值，跟第三个参数也就没有关系了。 
    threshold(rGrayImage, rBinImage, 0, 255,  THRESH_BINARY | THRESH_OTSU); //二值化
	///imshow("binary image", rBinImage);
	Mat erodeImage, dilateImage, edImage;
	//定义核  
	Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));   
	//进行形态学操作 	
	morphologyEx(rBinImage, edImage, MORPH_CLOSE, element,Point(-1,-1),1);
	imshow("MORPH_CLOSE",edImage);

	
// 图像垂直投影 得到定位标位置，并切割
// 图像水平投影  得到答题卡区域位置，并切割
// 此段处理的图像源： edImage
/**
 * 1. 为什么这里是locIndex[0]和locIndex[1]
 * 		因为这个vector只放两个值，且是x
 * **/
	Mat psrcImage(edImage);  //处理源图像
	vector<int> horizon(psrcImage.rows);
	vector<int> vertical(psrcImage.cols);
	project_Pic(psrcImage,horizon, vertical);
	
	//查找定位标
	vector<int> locIndex;
	findVerticalMax(vertical, locIndex);
	
	Mat locImage;
	psrcImage(Rect(locIndex[0],0, locIndex[1] - locIndex[0] + 1, psrcImage.rows-1) ).copyTo(locImage);
	imshow("LocateImage", locImage);
	//如果不要求鲁棒性，就直接水平投影定位标就OK
	vector<int> locHorizon(locImage.rows);
	vector<int> locVertical(locImage.cols);
	project_Pic(locImage, locHorizon, locVertical);

	vector<int> locUpIndex, locDownIndex;
	findLocHorizon(locHorizon, locUpIndex, locDownIndex);
// #ifdef 0
	/**
	 * 1.为什么要写这个？为什么要判断上条纹数等于下条纹数？
	 * 
	 * 2.为什么要画线？
	 * 
	 * 3. 为什么我在图中没看到线？
	 * 4. 它们是什么形状？
	 * 
	*/
	if(locUpIndex.size() == locDownIndex.size())
	{
		Mat tempq;
		psrcImage.copyTo(tempq);
		size_t lengthq = locUpIndex.size();
		for(int i = 0; i < lengthq ; i++)
		{
			Point ppt1 = Point(0,locUpIndex[i]);
			Point ppt2 = Point(tempq.cols-1, locUpIndex[i]);
			Point ppt3 = Point(0,locDownIndex[i]);
			Point ppt4 = Point(tempq.cols-1, locDownIndex[i]);
			line(tempq, ppt1, ppt2, Scalar(0, 0, 100));
			line(tempq, ppt3, ppt4, Scalar(0, 0, 100));
		}
		imshow("srljsf", tempq);
	}
// #endif
	
	
	//切割
	/**
	 * 1.为什么是11和17-11？
	 * 	因为准考证刚好占11个黑色条，第一栏答案刚好是到第17条结束
	 * 2. 为什么picCut的宽度用原图宽度？
	 * 
	 * 3. NUMS有什么用？为什么是20？
	 * 		因为第一栏有20道题
	 * 4. 为什么切掉条纹后不会发生一行全A，然后？
	 * 		使用原来的Index vector去定位
	 * 5. 为什么这里的是findLocHorizon(ansVertical, ansUpIndex, ansDownIndex)？
	 * 		因为它想用纵向的方法来区别横向，即第几题
	 * 		
	*/
	Mat answer;
	picCut(psrcImage,answer, Rect(0,locDownIndex[11], psrcImage.cols-1, locUpIndex[17]-locDownIndex[11]));

	vector<int> ansHorizon(answer.rows);
	vector<int> ansVertical(answer.cols);
	project_Pic(answer, ansHorizon, ansVertical);
	vector<int> ansUpIndex, ansDownIndex;
	findLocHorizon(ansVertical, ansUpIndex, ansDownIndex);
	//imshow("answer", answer);

	/*** 1.没有横向的条纹，怎么判断是第几题？
	 * 		作者假设每一题都有答案，然后用垂直投影找出每一题的index
	 * 		这个假设只影响题目迭代
	 * 
	 * */
	for(int i = 0; i < NUMS; i ++)
	{
		Mat tempAns = answer(Rect(ansUpIndex[i], 0, ansDownIndex[i] - ansUpIndex[i] , answer.rows));
		vector<int> tmpHorizon(tempAns.rows);
		vector<int> tmpVertical(tempAns.cols);
		project_Pic(tempAns,tmpHorizon, tmpVertical);
		findAnswer(tmpHorizon, locUpIndex, locDownIndex, Answer[i]);
		cout << i+1 << " : "<< Answer[i] << endl;
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
void findAnswer(const vector<int> & input, const vector<int> locUp, const vector<int> locDown, string & ans)
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

void picCut(const Mat & inputArray, Mat & outputArray, const Rect & rectRoi)
{
	outputArray = inputArray(rectRoi);
}
/**
 * 1. 为什么要写这个函数？
 * 		因为要找到上下边界，然后切割
 * 2. 为什么不和垂直方向上的一样，从中间开始往两边走，找到边缘？
 * 
 * 3. 为什么Horizon用的是Down和Up，而不是Left和right？
 * 		因为水平投影就是统计x轴，保留y轴
 * 4. 为什么没有设置阈值，求出的vector那么长，而不是length=2？为什么“注意事项”这些字符没有影响条纹扫描？
 * 		因为经过了形态学处理，把字符都干掉了
 * 
 * 5.为什么这里需要两个vector来保存结果？
 * 		Up和Down表示每个条纹的上边界和下边界，vector的index表示条纹数
*/
void findLocHorizon(const vector<int> & inputArray, vector<int> & locUp, vector<int> & locDown)
{
	size_t length = inputArray.size();  //
	int upNums = 0, downNums = 0;
	for(int i = 1; i < length; i++)
	{
		if(inputArray[i-1] == 0 && inputArray[i] > 0)
		{
			locUp.push_back(i);
			upNums ++;
		}
		else if(inputArray[i-1] > 0 && inputArray[i] == 0)
		{
			locDown.push_back(i);
			downNums ++;
		}
	}
}

void on_Change(int,void *)
{	
	threshold(pmidImage, pdstImage, threshold_value, 255, 0);
	imshow("process", pdstImage);
}


//函数作用： 将图像水平和垂直投影，得到 水平和垂直投影的图
//返回： horizon_out 水平投影的图
//		 vertical_out 垂直投影的图
void project_Pic(const Mat & src, vector<int> & horizon_out, vector<int> & vertical_out)
{
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
		horizon_out[i] = pixelValue;
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
		vertical_out[i] = pixelValue;
		pixelValue = 0;
	}
	// show it
	Mat horImage(src.rows, src.cols, CV_8UC1);
	Mat verImage(src.rows, src.cols, CV_8UC1);
	// horizon
	for(int i = 0; i < horImage.rows; i++)
	{
		for(int j = 0; j < horizon_out[i]; j++)
			horImage.at<uchar>(i,j) = 0;  //假设初始化为0；
	}
	for(int i = 0; i < verImage.cols; i ++)
	{
		for(int j = 0; j < vertical_out[i]; j++)
			verImage.at<uchar>(j,i) = 0;
	}
	imshow("hor",horImage);
	imshow("ver",verImage);
	imwrite("3.jpg",horImage);
	imwrite("4.jpg",verImage);
}


/**-----------------------------------【onMouse( )函数】---------------------------------------
//		描述：鼠标消息回调函数
//-----------------------------------------------------------------------------------------------

  1. 为什么这里是640，480，包括准考证号吗？
		以边框和黑色条纹的边角
**/
static void on_Mouse( int event, int x, int y, int flags, void* )
{	
	if( x < 0 || x >= pic.cols || y < 0 || y >= pic.rows )
	{
		cout << "Mouse is out \n" << endl;
		return;
	}		
	switch(event)
	{
		case EVENT_LBUTTONUP:
			srcpt[ptflag] = Point(x,y);
			//srcpt.push_back(Point(x,y));  //保存选取的点
			cout << "The chosen point is : " << srcpt[ptflag].x << " , " << srcpt[ptflag].y << endl;
			ptflag++;
			
			
			  		
			
			if(ptflag == 4)
			{
				ptflag = 0;
				cout << "Work has done\n" << endl;
				dstpt[0] = Point2f(0,0);
				dstpt[1] = Point2f(0,640);
				dstpt[2] = Point2f(480,640);
				dstpt[3] = Point2f(480,0);
				
				
			
				//求取映射矩阵
				perImage = Mat::zeros(640,480, CV_8UC3);   //这里的zeros 第一个参数是rows 行  第二是cols 列  和width（）函数定义正好相反
				Mat transMat = getPerspectiveTransform(srcpt, dstpt);
				warpPerspective(pic, perImage, transMat, perImage.size());	
				imshow("perspectived", perImage);

				imwrite("D:/Users/Dell/Documents/GitHub/Answer-Card-Recognition/pic/Perspectived.jpg",perImage);	

				if(ptflag == 0)// 如果点击次数为0，提取图片
				{
					psrcImage = imread("D:/Users/Dell/Documents/GitHub/Answer-Card-Recognition/pic/Perspectived.jpg",1);
					cvtColor(psrcImage, pmidImage, COLOR_RGB2GRAY);
					namedWindow("process", WINDOW_NORMAL);
					createTrackbar("value", "process", &threshold_value, 255, on_Change);
				}
			}
				
			break;
		default:
			break;
	}
}






