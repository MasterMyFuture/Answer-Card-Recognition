//���ܣ� ͸�ӱ任���ͼ��������
//ȱ�㣺 ���ڴ��⿨��״���⣬�޷���ɽǵ��Զ�ѡȡ���ʲ�������˹�ѡ��
//���ߣ� liangzelang��liangzelang@gmail.com��
//ʱ�䣺 2017/05/18
//�汾�� 0.01

#include <opencv2\opencv.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;
using namespace cv;
const int NUMS = 20;
vector<Point2f> srcpt(4);
vector<Point2f> dstpt(4);
vector<string> Answer(NUMS);
int ptflag=0;
Mat pic;
Mat perImage;
Mat pdstImage;
Mat pmidImage;
Mat psrcImage;

int threshold_value = 100;
static void on_Mouse(int event, int x, int y, int flags, void *);
void on_Change(int, void*);
void process_Pic1();
void project_Pic(const Mat & src, vector<int> & horizon_out, vector<int> & vertical_out);
void findMax(const vector<int> & inputArray, vector<int> & maxIndex);
int findVerMax(const vector<int> & inputArray, vector<int> & upIndex, vector<int> & downIndex);
void findVerticalMax(const vector<int> & inputArray, vector<int> & maxIndex);
void findLocHorizon(const vector<int> & inputArray, vector<int> & locUp, vector<int> & locDown);
void picCut(const Mat & inputArray, Mat & outputArray, const Rect & rectRoi);
void findAnswer(const vector<int> & input, const vector<int> locUp, const vector<int> locDown, string & ans);
int main()
{
	Mat dstImage;
	Mat midImage;
	process_Pic1();
	waitKey(0);
	return 0;

}

int findVerMax(const vector<int> & inputArray, vector<int> & upIndex, vector<int> & downIndex)
{
	int length = inputArray.size();  //0 ��ɫ  255��ɫ
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

void findVerticalMax(const vector<int> & inputArray, vector<int> & maxIndex)
{
	int length = inputArray.size();
	int maxVal = 0;
	int maxInd = 0;
	//�ҵ�����index
	for(int i = 0; i < length; i++)
	{
		if(maxVal < inputArray[i])
		{
			maxVal = inputArray[i];
			maxInd = i;
		}
	}
	//Ѱ�Ҷ�λ����߽�
	for(int i = maxInd; i > 0; i--)
	{
		if(inputArray[i] == 0 && inputArray[i+1] > 0)
		{
			maxIndex.push_back(i);
			break;
		}
			
	}
	//Ѱ�Ҷ�λ���ұ߽�
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
	int length = inputArray.size();
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

//��һ�ַ�����ʹ����̬ѧ�˲��ķ�ʽ��ͼ��Ԥ����
void process_Pic1()
{

// ͼ��Ԥ����
// �õ���ֵ��ͼ�񣬻����ܹ������Ϳ��λ�ã�Ϊ���洦���ṩԴ
// ����׶����ѵ㣺 1��͸�ӱ任��׼ȷ��   2����ֵ����ֵ��ѡȡ������ 3����̬ѧ�˲�
	Mat rsrcImage = imread("D:\\C++������ϵ�ļ��У���ѡ����ɾ����\\Answer-Card-Recognition\\pic\\result1.jpg",1);
	Mat rGrayImage;
	Mat rBinImage;
	cvtColor(rsrcImage,rGrayImage, CV_BGR2GRAY);  //�ҶȻ�
	//CV_THRESH_OTSU�����Զ�������ֵ��������������Ҳ��û�й�ϵ�ˡ� 
    threshold(rGrayImage, rBinImage, 0, 255,  CV_THRESH_BINARY | CV_THRESH_OTSU); //��ֵ��
	///imshow("binary image", rBinImage);
	Mat erodeImage, dilateImage, edImage;
	//�����  
	Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));   
	//������̬ѧ���� 	
	morphologyEx(rBinImage, edImage, MORPH_CLOSE, element,Point(-1,-1),1);
	imshow("�����ͺ�ʴ--������", edImage);

	
// ͼ��ֱͶӰ �õ���λ��λ�ã����и�
// ͼ��ˮƽͶӰ  �õ����⿨����λ�ã����и�
// �˶δ����ͼ��Դ�� edImage
	Mat psrcImage(edImage);  //����Դͼ��
	vector<int> horizon(psrcImage.rows);
	vector<int> vertical(psrcImage.cols);
	project_Pic(psrcImage,horizon, vertical);
	
	//���Ҷ�λ��
	vector<int> locIndex;
	findVerticalMax(vertical, locIndex);
	Mat locImage;
	psrcImage(Rect(locIndex[0],0, locIndex[1] - locIndex[0] + 1, psrcImage.rows-1) ).copyTo(locImage);
	imshow("LocateImage", locImage);
	//�����Ҫ��³���ԣ���ֱ��ˮƽͶӰ��λ���OK
	vector<int> locHorizon(locImage.rows);
	vector<int> locVertical(locImage.cols);
	project_Pic(locImage, locHorizon, locVertical);

	vector<int> locUpIndex, locDownIndex;
	findLocHorizon(locHorizon, locUpIndex, locDownIndex);
// #ifdef 0
	if(locUpIndex.size() == locDownIndex.size())
	{
		Mat tempq;
		psrcImage.copyTo(tempq);
		int lengthq = locUpIndex.size();
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
	
	Mat answer;
	picCut(psrcImage,answer, Rect(0,locDownIndex[11], psrcImage.cols-1, locUpIndex[17]-locDownIndex[11]));

	vector<int> ansHorizon(answer.rows);
	vector<int> ansVertical(answer.cols);
	project_Pic(answer, ansHorizon, ansVertical);
	vector<int> ansUpIndex, ansDownIndex;
	findLocHorizon(ansVertical, ansUpIndex, ansDownIndex);
	//imshow("answer", answer);


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

void findAnswer(const vector<int> & input, const vector<int> locUp, const vector<int> locDown, string & ans)
{
	int length = input.size();
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

void findLocHorizon(const vector<int> & inputArray, vector<int> & locUp, vector<int> & locDown)
{
	int length = inputArray.size();  //
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


//�������ã� ��ͼ��ˮƽ�ʹ�ֱͶӰ���õ� ˮƽ�ʹ�ֱͶӰ��ͼ
//���أ� horizon_out ˮƽͶӰ��ͼ
//		 vertical_out ��ֱͶӰ��ͼ
void project_Pic(const Mat & src, vector<int> & horizon_out, vector<int> & vertical_out)
{
	// ��src���ж�ֵ��ֵͳ��
	//horizontal ˮƽ
	
	int pixelValue = 0;
	for(int i = 0; i < src.rows; i++)   //�ж�����
	{
		for(int j = 0; j < src.cols; j++)
		{
			if(src.at<uchar>(i,j) == 0)
				pixelValue++;
		}
		horizon_out[i] = pixelValue;
		pixelValue = 0;
	}

	//vertical ��ֱ
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
			horImage.at<uchar>(i,j) = 0;  //�����ʼ��Ϊ0��
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


//-----------------------------------��onMouse( )������---------------------------------------
//		�����������Ϣ�ص�����
//-----------------------------------------------------------------------------------------------
static void on_Mouse( int event, int x, int y, int flags, void* )
{	
	if( x < 0 || x >= pic.cols || y < 0 || y >= pic.rows )
	{
		cout << "Mouse is out \n" << endl;
		return;
	}		
	switch(event)
	{
		case CV_EVENT_LBUTTONUP :
			srcpt[ptflag] = Point(x,y);
			//srcpt.push_back(Point(x,y));  //����ѡȡ�ĵ�
			cout << "The chosen point is : " << srcpt[ptflag].x << " , " << srcpt[ptflag].y << endl;
			ptflag++;
			if(ptflag == 4)
			{
				ptflag = 0;
				cout << "Work has done\n" << endl;
				dstpt[0] = Point2f(0,0);
				dstpt[3] = Point2f(0,640);
				dstpt[2] = Point2f(480,640);
				dstpt[1] = Point2f(480,0);
			
				//��ȡӳ�����
				perImage = Mat::zeros(640,480, CV_8UC3);   //�����zeros ��һ��������rows ��  �ڶ���cols ��  ��width�����������������෴
				Mat transMat = getPerspectiveTransform(srcpt, dstpt);
				warpPerspective(pic, perImage, transMat, perImage.size());	
				imshow("fuck", perImage);

				imwrite("D:\\C++������ϵ�ļ��У���ѡ����ɾ����\\Answer-Card-Recognition\\pic\\result.jpg",perImage);	

				if(ptflag == 0)
				{
					psrcImage = imread("D:\\C++������ϵ�ļ��У���ѡ����ɾ����\\Answer-Card-Recognition\\pic\\result1.jpg",1);
					cvtColor(psrcImage, pmidImage, CV_RGB2GRAY);
					namedWindow("process", 1);
					createTrackbar("value", "process", &threshold_value, 255, on_Change);
				}
			}
				
			break;
		default:
			break;
	}
}

