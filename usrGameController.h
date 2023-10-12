#ifdef VIA_OPENCV

#ifndef USRGAMECONTROLLER_H
#define USRGAMECONTROLLER_H

#include "qtcyberdip.h"
#include "imageturn.h"
#include <stack>
#define WIN_NAME "Frame"

//��Ϸ������
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
	//������Ϊ��ʵ����ʾЧ�������ӵ�����
		//���ص��ṹ��
	struct MouseArgs {
		cv::Rect box;
		bool Drawing, Hit;
		// init
		MouseArgs() :Drawing(false), Hit(false)
		{
			box = cv::Rect(0, 0, -1, -1);
		}
	};
	//���ص�����
	friend void  mouseCallback(int event, int x, int y, int flags, void*param);
	MouseArgs argM;
	//������Ϊ��ʵ�ֿ�����ʾЧ�������ӵ�����
public:
	//���캯�������б����ĳ�ʼ����Ӧ�ڴ����
	usrGameController(void* qtCD);
	//�������������ձ���������Դ
	~usrGameController();
	//����ͼ������ÿ���յ�ͼ��ʱ�������
	int usrProcessImage(cv::Mat& img);
	Mat PicDivide(Mat srcImage, int n, float proportion);
	Point Match(Mat target_pic, Mat divide_pic, float t);
	float loop(const Mat& i1, const Mat& i2);
	void refresh(int i);
};

//������Ϊ��ʵ����ʾЧ�������ӵ�����
//���ص�����
void  mouseCallback(int event, int x, int y, int flags, void*param);
//������Ϊ��ʵ�ֿ�����ʾЧ�������ӵ�����

#endif
#endif