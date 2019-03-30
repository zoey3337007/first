#include<opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <vector>
#include <iostream>
#include <opencv2/imgproc/imgproc.hpp>
#include<stdio.h>
#include <windows.h>   //串口添加相关头文件
#include "SerialPort.h"  
#include "StdAfx.h" 
#include <string.h>
#include <tchar.h>
#include <stdlib.h>
//int main(int argc, _TCHAR* argv[])   


using namespace std;
using namespace cv;
//unsigned char *temp = new unsigned char[8];//动态创建一个数组
float oriX;float oriY;
float X; float Y; float dis=0;
int center_idx = 0; int center_idy = 0;
float mindis = 10; int cir_r; 
//char e[100];
//int reseivex, int reseivey;
Mat gray; Mat frame;Mat edge;
IplImage gray2 = IplImage(gray); CvArr* arr = (CvArr*)&gray2;
class box
{
	int num;
public:
	void send(void);
	void contour(const Mat&src);
};
void box::contour(const Mat&src)
{
	src.copyTo(frame);
	cvtColor(frame, gray, CV_BGR2GRAY);
	threshold(gray, gray, 120, 255, CV_THRESH_BINARY);//阈值设60-70，实验室光亮下效果较好;55暗光近距离效果好，85亮光条件较好
	bilateralFilter(gray, edge, 5, 5, 5, BORDER_DEFAULT);
	imshow("vedio", edge);
	vector<Vec3f> circles;
	vector<Vec3f> miss;
	Point abc;
	HoughCircles(edge,circles, CV_HOUGH_GRADIENT, 1, 10,100, 100, 0,0);
	num=circles.size();
		for (size_t i = 0; i < circles.size(); i++)
		{

			Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
			int radius = cvRound(circles[i][2]);
			X = center.x - oriX; Y = center.y - oriY;
			float d = X * X + Y * Y;
			dis += d;
			Vec3f missing(X, Y, d);
			miss.push_back(missing);
			abc.x = int(circles[0][0]);
			abc.y = int(circles[0][1]);
			center_idx = int(circles[0][0] - oriX);
			center_idy = int(circles[0][1] - oriY);
			cir_r = cvRound(circles[0][2]);
			circle(frame, abc, 3, Scalar(0, 255, 0), -1, 8, 0);
			circle(frame, abc, cir_r, Scalar(155, 50, 255), 3, 8, 0);
			Point a(50, 50);
			Point b(100, 100);
			//Point c(150, 150);
			String text1 = "X:" + format("%f", center_idx);
			String text2 = "Y:" + format("%f", center_idy);
			//String text3 = "d:" + format("%f",radius);
			putText(frame, text1, a, FONT_HERSHEY_SIMPLEX, 0.4, Scalar(0, 0, 255), 1, 8);
			putText(frame, text2, b, FONT_HERSHEY_SIMPLEX, 0.4, Scalar(0, 0, 255), 1, 8);
			//putText(frame, text3,c, FONT_HERSHEY_SIMPLEX, 0.4, Scalar(0, 0, 255), 1, 8)
			//_itoa_s(center_idx, e, 16);

			////绘制圆心  
			//circle(frame, center, 3,Scalar(0, 255, 0), -1, 8, 0);
			////绘制圆轮廓  
			//circle(frame, center, radius,Scalar(155, 50, 255), 3, 8, 0);
	
	}
	//for (size_t i = 0; i <miss.size(); i++)
	//{
	//	
	//	dis /= dis;
	//	miss[i][2] = (miss[i][2] - dis)*(miss[i][2] - dis);
	//	if (miss[i][2]<mindis)
	//	{
	//		mindis = miss[i][2];
	//		center_idx = i;
	//	}
	//	else break;
	//}


	imshow("frame",frame);
	//
	//vector<Vec4i> hierarchy;
	//vector<RotatedRect> box_rects;
	//cout << box_rects.size() << endl;
	//for (int i = 0; i < box_rects.size(); ++i)
	//{
	//	Point2f vertices[4];
	//	box_rects[i].points(vertices);
	//	for (int i = 0; i < 4; i++)
	//	{
	//		line(gray, vertices[i], vertices[(i + 1) % 4], Scalar(50, 255, 50), 2);
	//		int m = (vertices[3].x + vertices[2].x + vertices[1].x + vertices[0].x) / 4;
	//		int n = (vertices[3].y + vertices[2].y + vertices[1].y + vertices[0].y) / 4;
	//	}
	//}
	
 }
void box::send(void)
{
	CSerialPort mySerialPort;//首先将之前定义的类实例化
	int length = 8;
	unsigned char *temp = new unsigned char[8];//动态创建一个数组
	if (!mySerialPort.InitPort(1, CBR_115200, 'N', 8, 1, EV_RXCHAR))
	{
		std::cout << "initPort fail !" << std::endl;
	}
	else
	{
		std::cout << "initPort success !" << std::endl;
	}
	unsigned char ef[8] = { 0xAD,0,0,0,0,0,0,0xAE };
	if (num != 0)
	{
		ef[2] = center_idy / 2;
		ef[3] = center_idx / 2;
		
		//char ef[8] = {0,0,0,0,0,0, ,center_idy / 50 };
	}	
	else
	{
		ef[1] = num;
		ef[2] = 0;
		ef[3] = 0;
	}
		cout << mySerialPort.WriteData(ef, 8) << endl;
	
		
	//cout << mySerialPort.GetBytesInCOM() << endl;
	waitKey(30);
}

	//void CRectangle::set_values(int a, int b) 
    //{
	//	x = a;
	//	y = b;
	//}

	int _tmain(int argc, _TCHAR* argv[])
	{

		
		box reality;
		///Mat abc = reality.camera();
		///Mat efg=reality.contour(abc);
		//VideoCapture capture(0);
		VideoCapture capture(1);
		
		while (true)
		{
			capture >> frame;
			oriX = frame.cols / 2;
			oriY = frame.rows / 2;
			circle(frame, Point2f(oriX, oriY), 2, Scalar(0, 0, 255), 3);
			if (frame.empty()) 
			{
				std::cerr << "error! blank frame grabbed\n";
				break;
			}
			reality.contour(frame);
			//cout << mySerialPort.WriteData(try1, 8) << endl;
			reality.send();
		}
		

    return 0;
	}
