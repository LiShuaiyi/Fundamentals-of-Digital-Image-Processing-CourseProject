#ifdef VIA_OPENCV

#ifndef USRGAMECONTROLLER_H
#define USRGAMECONTROLLER_H

#include "qtcyberdip.h"
#include "imageturn.h"
#include <stack>
#define WIN_NAME "Frame"

//游戏控制类
class usrGameController
{
private:
	deviceCyberDip* device;
	const int order = 6;
	bool flag1 = false;
	bool flag2 = false;
	bool flag3 = false;
	bool flag4 = false;
	bool flag5 = false;
	bool flag6 = false;
	Point location, location1, location2, location3;
	int n = 0;
	int count = 0;
	int index = 0;
	int number = 0;
	Mat arr[36];
	Mat arr1, arr2, arr3;
	Mat last, roi1, roi2;
	int seq_array[36];
	Mat warpmatrix;
	stack<int> stk1;
	stack<int> stk2;
	//以下是为了实现演示效果，增加的内容
		//鼠标回调结构体
	struct MouseArgs {
		cv::Rect box;
		bool Drawing, Hit;
		// init
		MouseArgs() :Drawing(false), Hit(false)
		{
			box = cv::Rect(0, 0, -1, -1);
		}
	};
	//鼠标回调函数
	friend void  mouseCallback(int event, int x, int y, int flags, void*param);
	MouseArgs argM;
	//以上是为了实现课堂演示效果，增加的内容
public:
	//构造函数，所有变量的初始化都应在此完成
	usrGameController(void* qtCD);
	//析构函数，回收本类所有资源
	~usrGameController();
	//处理图像函数，每次收到图像时都会调用
	int usrProcessImage(cv::Mat& img);
	Mat PicDivide(Mat srcImage, int n, float proportion);
	Point Match(Mat target_pic, Mat divide_pic, float t);
	float loop(const Mat& i1, const Mat& i2);
	void refresh(int i);
};

//以下是为了实现演示效果，增加的内容
//鼠标回调函数
void  mouseCallback(int event, int x, int y, int flags, void*param);
//以上是为了实现课堂演示效果，增加的内容

#endif
#endif