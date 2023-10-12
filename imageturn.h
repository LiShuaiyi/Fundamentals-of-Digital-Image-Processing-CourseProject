#ifndef IMAGETURN_H
#define IMAGETURN_H

#include <opencv2/opencv.hpp>
#include <iostream> 
#include <cmath>
using namespace cv;
using namespace std;




//此函数根据摄像头读到的第一帧图片计算出透视变换的转化矩阵
Mat warpmatrix_calculate(Mat srcImage);
void compute_point(int i, Point& point1, Point& point2, vector<Vec2f>& lines);
//此函数将原图划分为64份
void seperate(Mat srcImage, Mat* image);


#endif // IMAGETURN_H
