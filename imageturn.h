#ifndef IMAGETURN_H
#define IMAGETURN_H

#include <opencv2/opencv.hpp>
#include <iostream> 
#include <cmath>
using namespace cv;
using namespace std;




//�˺�����������ͷ�����ĵ�һ֡ͼƬ�����͸�ӱ任��ת������
Mat warpmatrix_calculate(Mat srcImage);
void compute_point(int i, Point& point1, Point& point2, vector<Vec2f>& lines);
//�˺�����ԭͼ����Ϊ64��
void seperate(Mat srcImage, Mat* image);


#endif // IMAGETURN_H
