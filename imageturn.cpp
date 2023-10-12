#include "imageturn.h"





Mat warpmatrix_calculate(Mat srcImage)
{
	Mat warpmatrix;
	int Blue_threshold = 86;
	int Green_threshold = 83;
	int Red_threshold = 53;
	int houph_parameter = 133;

	//三个颜色的像素分别与阈值进行比较，根据比较结果求原图二值化图
	Mat binaryImage;
	binaryImage = Mat::zeros(srcImage.size(), CV_8UC1);
	for (int x = 0; x < srcImage.rows; ++x)
	{
		for (int y = 0; y < srcImage.cols; ++y)
		{
			if (srcImage.at<Vec3b>(x, y)[0] > Blue_threshold&& srcImage.at<Vec3b>(x, y)[1] > Green_threshold&& srcImage.at<Vec3b>(x, y)[2] > Red_threshold)
			{
				binaryImage.at<uchar>(x, y) = 255;
			}
			else
			{
				binaryImage.at<uchar>(x, y) = 0;
			}
		}
	}

	//形态学开运算
	Mat filterImage;
	Mat element = getStructuringElement(MORPH_RECT, Size(4,4));
	morphologyEx(binaryImage, filterImage, MORPH_OPEN, element, Point(-1, -1));

	//边缘检测
	Mat cannyImage;
	Canny(filterImage, cannyImage, 3, 9, 3);

	//霍夫变换
	vector<Vec2f> lines;
	HoughLines(cannyImage, lines, 1, CV_PI / 180, houph_parameter, 0, 0);

	//求出所有交点
	vector<Point> cross_point;
	for (size_t i = 0; i < lines.size(); ++i)
	{
		for (size_t j = i + 1; j < lines.size(); ++j)
		{
			if (abs(lines[i][1] - lines[j][1]) < 0.8)
			{
				continue;
			}
			else
			{
				Point crosspoint, point0, point1, point2, point3;
				compute_point(i, point0, point1, lines);
				compute_point(j, point2, point3, lines);
				float a1 = (float)(point1.y - point0.y) / (float)(point1.x - point0.x);
				float a2 = (float)(point3.y - point2.y) / (float)(point3.x - point2.x);
				float b1 = (float)(point0.y * point1.x - point1.y * point0.x) / (float)(point1.x - point0.x);
				float b2 = (float)(point2.y * point3.x - point3.y * point2.x) / (float)(point3.x - point2.x);
				crosspoint.x = (int)((b2 - b1) / (a1 - a2));
				crosspoint.y = (int)(a1 * crosspoint.x + b1);
				if (crosspoint.x > 0 && crosspoint.y > 0 && crosspoint.x < 3000 && crosspoint.y < 3000)
				{
					cross_point.push_back(crosspoint);

				}


			}
		}
	}
	//求出交点的平均值
	vector<Point>::iterator ite = cross_point.begin();
	int sum_x = 0, sum_y = 0;
	for (ite = cross_point.begin(); ite != cross_point.end(); ++ite)
	{
		sum_x += (*ite).x;
		sum_y += (*ite).y;
	}
	sum_x = sum_x / cross_point.size();
	sum_y = sum_y / cross_point.size();

	//根据交点的平均值，找出平板的四个角
	Mat dstImage;
	dstImage.create(srcImage.size(), CV_8UC3);
	vector<Point2f> src_corners(4);
	vector<Point2f> dst_corners(4);
	float alpha[5];
	int Min = 1000;
	Point corner;
	for (ite = cross_point.begin(); ite != cross_point.end(); ++ite)
	{
		if (sqrt(((*ite).x - sum_x) * ((*ite).x - sum_x) + ((*ite).y - sum_y) * ((*ite).y - sum_y)) < Min)
		{
			corner.x = (*ite).x;
			corner.y = (*ite).y;
			Min = sqrt(((*ite).x - sum_x) * ((*ite).x - sum_x) + ((*ite).y - sum_y) * ((*ite).y - sum_y));
		}
	}

	if (corner.x <= sum_x && corner.y <= sum_y)
	{
		alpha[1] = 3.14 - atan2(sum_y - corner.y, sum_x - corner.x);
	}
	else if (corner.x <= sum_x && corner.y >= sum_y)
	{
		alpha[1] = 3.14 + atan2(corner.y - sum_y, sum_x - corner.x);
	}
	else if (corner.x >= sum_x && corner.y >= sum_y)
	{
		alpha[1] = 6.28 - atan2(corner.y - sum_y, corner.x - sum_x);
	}
	else if (corner.x >= sum_x && corner.y <= sum_y)
	{
		alpha[1] = atan2(sum_y - corner.y, corner.x - sum_x);
	}
	for (ite = cross_point.begin(); ite != cross_point.end(); )
	{
		if ((*ite) == corner)
		{
			ite = cross_point.erase(ite);
		}
		else
		{
			if ((*ite).x <= sum_x && (*ite).y <= sum_y)
			{
				alpha[0] = 3.14 - atan2(sum_y - (*ite).y, sum_x - (*ite).x);
			}
			else if ((*ite).x <= sum_x && (*ite).y >= sum_y)
			{
				alpha[0] = 3.14 + atan2((*ite).y - sum_y, sum_x - (*ite).x);
			}
			else if ((*ite).x >= sum_x && (*ite).y >= sum_y)
			{
				alpha[0] = 6.28 - atan2((*ite).y - sum_y, (*ite).x - sum_x);
			}
			else if ((*ite).x >= sum_x && (*ite).y <= sum_y)
			{
				alpha[0] = atan2(sum_y - (*ite).y, (*ite).x - sum_x);
			}
			if (abs(alpha[0] - alpha[1]) < 0.5 || abs(alpha[0] - alpha[1]) > 5.7)
			{
				ite = cross_point.erase(ite);
			}
			else
			{
				++ite;
			}
		}
	}
	src_corners[0] = corner;
	Min = 1000;
	for (ite = cross_point.begin(); ite != cross_point.end(); ++ite)
	{
		if (sqrt(((*ite).x - sum_x) * ((*ite).x - sum_x) + ((*ite).y - sum_y) * ((*ite).y - sum_y)) < Min)
		{
			corner.x = (*ite).x;
			corner.y = (*ite).y;
			Min = sqrt(((*ite).x - sum_x) * ((*ite).x - sum_x) + ((*ite).y - sum_y) * ((*ite).y - sum_y));
		}
	}
	if (corner.x <= sum_x && corner.y <= sum_y)
	{
		alpha[2] = 3.14 - atan2(sum_y - corner.y, sum_x - corner.x);
	}
	else if (corner.x <= sum_x && corner.y >= sum_y)
	{
		alpha[2] = 3.14 + atan2(corner.y - sum_y, sum_x - corner.x);
	}
	else if (corner.x >= sum_x && corner.y >= sum_y)
	{
		alpha[2] = 6.28 - atan2(corner.y - sum_y, corner.x - sum_x);
	}
	else if (corner.x >= sum_x && corner.y <= sum_y)
	{
		alpha[2] = atan2(sum_y - corner.y, corner.x - sum_x);
	}
	for (ite = cross_point.begin(); ite != cross_point.end(); )
	{
		if ((*ite) == corner)
		{
			ite = cross_point.erase(ite);
		}
		else
		{
			if ((*ite).x <= sum_x && (*ite).y <= sum_y)
			{
				alpha[0] = 3.14 - atan2(sum_y - (*ite).y, sum_x - (*ite).x);
			}
			else if ((*ite).x <= sum_x && (*ite).y >= sum_y)
			{
				alpha[0] = 3.14 + atan2((*ite).y - sum_y, sum_x - (*ite).x);
			}
			else if ((*ite).x >= sum_x && (*ite).y >= sum_y)
			{
				alpha[0] = 6.28 - atan2((*ite).y - sum_y, (*ite).x - sum_x);
			}
			else if ((*ite).x >= sum_x && (*ite).y <= sum_y)
			{
				alpha[0] = atan2(sum_y - (*ite).y, (*ite).x - sum_x);
			}
			if (abs(alpha[0] - alpha[2]) < 0.5 || abs(alpha[0] - alpha[2]) > 5.7)
			{
				ite = cross_point.erase(ite);
			}
			else
			{
				++ite;
			}
		}
	}

	src_corners[1] = corner;
	Min = 1000;
	for (ite = cross_point.begin(); ite != cross_point.end(); ++ite)
	{
		if (sqrt(((*ite).x - sum_x) * ((*ite).x - sum_x) + ((*ite).y - sum_y) * ((*ite).y - sum_y)) < Min)
		{
			corner.x = (*ite).x;
			corner.y = (*ite).y;
			Min = sqrt(((*ite).x - sum_x) * ((*ite).x - sum_x) + ((*ite).y - sum_y) * ((*ite).y - sum_y));
		}
	}
	if (corner.x <= sum_x && corner.y <= sum_y)
	{
		alpha[3] = 3.14 - atan2(sum_y - corner.y, sum_x - corner.x);
	}
	else if (corner.x <= sum_x && corner.y >= sum_y)
	{
		alpha[3] = 3.14 + atan2(corner.y - sum_y, sum_x - corner.x);
	}
	else if (corner.x >= sum_x && corner.y >= sum_y)
	{
		alpha[3] = 6.28 - atan2(corner.y - sum_y, corner.x - sum_x);
	}
	else if (corner.x >= sum_x && corner.y <= sum_y)
	{
		alpha[3] = atan2(sum_y - corner.y, corner.x - sum_x);
	}
	for (ite = cross_point.begin(); ite != cross_point.end(); )
	{
		if ((*ite) == corner)
		{
			ite = cross_point.erase(ite);
		}
		else
		{
			if ((*ite).x <= sum_x && (*ite).y <= sum_y)
			{
				alpha[0] = 3.14 - atan2(sum_y - (*ite).y, sum_x - (*ite).x);
			}
			else if ((*ite).x <= sum_x && (*ite).y >= sum_y)
			{
				alpha[0] = 3.14 + atan2((*ite).y - sum_y, sum_x - (*ite).x);
			}
			else if ((*ite).x >= sum_x && (*ite).y >= sum_y)
			{
				alpha[0] = 6.28 - atan2((*ite).y - sum_y, (*ite).x - sum_x);
			}
			else if ((*ite).x >= sum_x && (*ite).y <= sum_y)
			{
				alpha[0] = atan2(sum_y - (*ite).y, (*ite).x - sum_x);
			}
			if (abs(alpha[0] - alpha[3]) < 0.5 || abs(alpha[0] - alpha[3]) > 5.7)
			{
				ite = cross_point.erase(ite);
			}
			else
			{
				++ite;
			}
		}
	}
	src_corners[2] = corner;
	Min = 1000;
	for (ite = cross_point.begin(); ite != cross_point.end(); ++ite)
	{
		if (sqrt(((*ite).x - sum_x) * ((*ite).x - sum_x) + ((*ite).y - sum_y) * ((*ite).y - sum_y)) < Min)
		{
			corner.x = (*ite).x;
			corner.y = (*ite).y;
			Min = sqrt(((*ite).x - sum_x) * ((*ite).x - sum_x) + ((*ite).y - sum_y) * ((*ite).y - sum_y));
		}
	}
	if (corner.x <= sum_x && corner.y <= sum_y)
	{
		alpha[4] = 3.14 - atan2(sum_y - corner.y, sum_x - corner.x);
	}
	else if (corner.x <= sum_x && corner.y >= sum_y)
	{
		alpha[4] = 3.14 + atan2(corner.y - sum_y, sum_x - corner.x);
	}
	else if (corner.x >= sum_x && corner.y >= sum_y)
	{
		alpha[4] = 6.28 - atan2(corner.y - sum_y, corner.x - sum_x);
	}
	else if (corner.x >= sum_x && corner.y <= sum_y)
	{
		alpha[4] = atan2(sum_y - corner.y, corner.x - sum_x);
	}
	src_corners[3] = corner;
	for (int i = 1; i < 5; ++i)
	{
		for (int j = i + 1; j < 5; ++j)
		{
			if (alpha[j] < alpha[i])
			{
				float tmp = alpha[j];
				alpha[j] = alpha[i];
				alpha[i] = tmp;
				Point Tmp = src_corners[i - 1];
				src_corners[i - 1] = src_corners[j - 1];
				src_corners[j - 1] = Tmp;
			}
		}
	}
	Point tmp = src_corners[0];
	src_corners[0] = src_corners[1];
	src_corners[1] = tmp;
	tmp = src_corners[2];
	src_corners[2] = src_corners[3];
	src_corners[3] = tmp;
	dst_corners[0] = Point(0, 0);
	dst_corners[1] = Point(dstImage.cols, 0);
	dst_corners[2] = Point(dstImage.cols, dstImage.rows);
	dst_corners[3] = Point(0, dstImage.rows);
	warpmatrix = getPerspectiveTransform(src_corners, dst_corners);
	return warpmatrix;
}

void compute_point(int i, Point& point1, Point& point2, vector<Vec2f> &lines)
{
	float rho = lines[i][0], theta = lines[i][1];
	double a = cos(theta), b = sin(theta);
	double x0 = rho * a, y0 = rho * b;
	point1.x = cvRound(x0 + 1000 * (-b));
	point1.y = cvRound(y0 + 1000 * (a));
	point2.x = cvRound(x0 - 1000 * (-b));
	point2.y = cvRound(y0 - 1000 * (a));
}

/*
int main()
{
	
	Mat srcImage;
	VideoCapture capture(1);
	capture >> srcImage;
	warpmatrix_calculate(srcImage);
	Mat dstImage;
	dstImage.create(srcImage.size(), CV_8UC3);
	warpPerspective(srcImage, dstImage, warpmatrix, dstImage.size(), CV_INTER_LINEAR);
	imshow("效果图", dstImage);
	waitKey(0);


Mat srcImage;
VideoCapture capture(1);
while(1){
capture >> srcImage;
imshow("效果图", srcImage);
waitKey(30);}

	Mat srcImage;
	srcImage = imread("D://1.png");
	Mat image[64];
	seperate(srcImage, image);
	imshow("原图", srcImage);
	imshow("1", image[0]);
	imshow("64", image[63]);

	waitKey(0);
	return 0;
}
*/
void seperate(Mat srcImage, Mat* image)
{
	int height = srcImage.rows;
	int width = srcImage.cols;
	height /= 8;
	width /= 8;
	for (int i = 0; i < 8; ++i)
	{
		for (int j = 0; j < 8; ++j)
		{
			image[j * 8 + i] = srcImage(Rect(i * width, j * height, width, height));
		}
	}
}