//���ܣ� ͸�ӱ任���ͼ��������
//ȱ�㣺 ���ڴ��⿨��״���⣬�޷���ɽǵ��Զ�ѡȡ���ʲ�������˹�ѡ��
//���ߣ� liangzelang��liangzelang@gmail.com��
//ʱ�䣺 2017/05/18
//�汾�� 0.01

#include <opencv2\opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

vector<Point2f> srcpt(4);
vector<Point2f> dstpt(4);
int ptflag=0;
Mat pic;
Mat perImage;
Mat pdstImage;
Mat pmidImage;
Mat psrcImage;
int threshold_value = 100;
static void on_Mouse(int event, int x, int y, int flags, void *);
void on_Change(int, void*);
void process_Pic();
int main()
{
	Mat dstImage;
	Mat midImage;
	process_Pic();
	/*pic = imread("D:\\C++������ϵ�ļ��У���ѡ����ɾ����\\Answer-Card-Recognition\\pic\\1.jpg",1);
	resize(pic, pic, cv::Size(480, 640));	
	imshow("source", pic);
	namedWindow("fuck",1);
	setMouseCallback("source", on_Mouse, 0);*/
	waitKey(0);
	return 0;

}

//��һ�ַ�����ʹ����̬ѧ�˲��ķ�ʽ��ͼ��Ԥ����
void process_Pic()
{
	Mat rsrcImage = imread("D:\\C++������ϵ�ļ��У���ѡ����ɾ����\\Answer-Card-Recognition\\pic\\result1.jpg",1);
	Mat rGrayImage;
	Mat rBinImage;
	cvtColor(rsrcImage,rGrayImage, CV_BGR2GRAY);  //�ҶȻ�

	//CV_THRESH_OTSU�����Զ�������ֵ��������������Ҳ��û�й�ϵ�ˡ� 

    threshold(rGrayImage, rBinImage, 0, 255,  CV_THRESH_BINARY | CV_THRESH_OTSU); //��ֵ��
	imshow("binary image", rBinImage);

	Mat erodeImage, dilateImage, edImage;
	//�����  
	Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));   
	//������̬ѧ���� 	
	morphologyEx(rBinImage, edImage, MORPH_CLOSE, element,Point(-1,-1),1);
	imshow("�����ͺ�ʴ--������", edImage);
	
	/*Mat erodeImage, dilateImage, edImage;
	Mat element = getStructuringElement(MORPH_RECT,Size(5,5));	
	dilate(rBinImage, dilateImage,element);		
	erode(rBinImage, erodeImage, element);
	dilate(dilateImage, edImage, element);
	imshow("Ч��ͼ", edImage);
	imshow("����ͼ",dilateImage);
	imshow("��ʴͼ",erodeImage);
	*/  //����ʾЧ��ͼ
}

//��2�ַ����������������ҵķ�ʽ
void process_Pic()
{
	Mat rsrcImage = imread("D:\\C++������ϵ�ļ��У���ѡ����ɾ����\\Answer-Card-Recognition\\pic\\result1.jpg",1);
	Mat rGrayImage;
	Mat rBinImage;
	cvtColor(rsrcImage,rGrayImage, CV_BGR2GRAY);  //�ҶȻ�

	//CV_THRESH_OTSU�����Զ�������ֵ��������������Ҳ��û�й�ϵ�ˡ� 

    threshold(rGrayImage, rBinImage, 0, 255,  CV_THRESH_BINARY | CV_THRESH_OTSU); //��ֵ��
	imshow("binary image", rBinImage);

	Mat erodeImage, dilateImage, edImage;
	//�����  
	Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));   
	//������̬ѧ���� 	
	morphologyEx(rBinImage, edImage, MORPH_CLOSE, element,Point(-1,-1),1);
	imshow("�����ͺ�ʴ--������", edImage);
	

}

void on_Change(int,void *)
{	
	threshold(pmidImage, pdstImage, threshold_value, 255, 0);
	imshow("process", pdstImage);
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
