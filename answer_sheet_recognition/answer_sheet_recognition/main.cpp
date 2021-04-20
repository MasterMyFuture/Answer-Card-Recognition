//功能： 透视变换完成图像畸变矫正
//缺点： 由于答题卡形状问题，无法完成角点自动选取，故采用鼠标人工选择
//作者： liangzelang（liangzelang@gmail.com）
//时间： 2017/05/18
//版本： 0.01

#include <opencv2/opencv.hpp>
#include <iostream>
#include <sstream>
#include <algorithm>
#include "paperChecker.h"

using namespace std;
using namespace cv;
//const int NUMS = 20;
// vector<Point2f> srcpt(4);
// vector<Point2f> dstpt(4);
//vector<string> Answer(NUMS);
//int ptflag=0;
int threshold_value = 100;
Mat pic;
Mat perImage;
Mat pdstImage;
Mat pmidImage;
Mat psrcImage;




static void on_Mouse(int event, int x, int y, int flags, void *);
void on_Change(int, void*);

int main()
{
	Mat dstImage;
	Mat midImage;
	bool isPers = true;
	//
	Mat srcImage = imread("D:/Users/Dell/Documents/GitHub/Answer-Card-Recognition/pic/Perspectived.jpg",1);

	if(!srcImage.empty()){
		
		//Mat scale(640,480,CV_8UC1,Scalar(0));
		std::vector<int> horizon(srcImage.rows);
		std::vector<int> vertical(srcImage.cols);
		preprocess(srcImage,dstImage);
		//getYLocate(dstImage,horizon, vertical);
		getYLocate(dstImage);
		//getAnswerRegion(dstImage);

	}else{

		pic = imread("D:/Users/Dell/Documents/GitHub/Answer-Card-Recognition/pic/2.jpg",1);
		namedWindow("input",WINDOW_NORMAL);
		if(!pic.empty()){
			imshow("input",pic);
		}
		setMouseCallback("input",on_Mouse,0);
	
	//

	}

	//process_Pic1(srcImage);
	waitKey(0);
	return 0;

}








void on_Change(int,void *)
{	
	threshold(pmidImage, pdstImage, threshold_value, 255, 0);
	imshow("process", pdstImage);
}




/**-----------------------------------【onMouse( )函数】---------------------------------------
//		描述：鼠标消息回调函数
//-----------------------------------------------------------------------------------------------

  1. 为什么这里是640，480，包括准考证号吗？
		因为无法确定答题卡的尺寸，不能用结果比，因为只能知道透视变换后的尺寸
**/
static void on_Mouse( int event, int x, int y, int flags, void* )
{	
	int ptflag=0;
	vector<Point2f> srcpt(4);
	vector<Point2f> dstpt(4);
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

				if(ptflag == 0&& DEBUG)// 如果点击次数为0，提取图片
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






