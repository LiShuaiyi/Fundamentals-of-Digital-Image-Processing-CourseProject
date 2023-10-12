#include "usrGameController.h"
#include <cmath>

#ifdef VIA_OPENCV
//构造与初始化
usrGameController::usrGameController(void* qtCD)
{
	qDebug() << "usrGameController online.";
	device = new deviceCyberDip(qtCD);//设备代理类
	cv::namedWindow(WIN_NAME);
	cv::setMouseCallback(WIN_NAME, mouseCallback, (void*)&(argM));
	int count = 0;
	for (int i = 0; i < order * order; i++)
	{
		seq_array[i] = 0;
	}
}

//析构
usrGameController::~usrGameController()
{
	cv::destroyAllWindows();
	if (device != nullptr)
	{
		delete device;
	}
	qDebug() << "usrGameController offline.";
}

//处理图像 

int usrGameController::usrProcessImage(cv::Mat& img)
{
	if (count % 150 == 0)
	{
		if (!flag1)
		{
			device->comMoveTo(-30, 50);
			refresh(order);
			Sleep(1500);

			flag1 = true;

		}
		else if (!flag4)
		{
			warpmatrix = warpmatrix_calculate(img);
			warpPerspective(img, img, warpmatrix, img.size(), CV_INTER_LINEAR);
			imshow("第一帧", img);
			flag4 = true;
		}
		else if (!flag2)
		{
			device->comMoveTo(8, -26);
			Sleep(1500);
			device->comHitOnce();
			Sleep(1000);
			device->comMoveTo(-30, 50);
			Sleep(1500);
			imwrite("last.jpg", img);
			flag2 = true;

		}

		else if (!flag3)
		{

			warpPerspective(img, img, warpmatrix, img.size(), CV_INTER_LINEAR);
			last = imread("last.jpg");
			roi1 = last(Rect(80, 75, 60, 60));
			roi2 = img(Rect(80, 75, 60, 60));
			if (loop(roi1, roi2) > 0.8)
			{
				device->comMoveTo(8, -26);
				Sleep(1500);
				device->comHitOnce();
				Sleep(500);
				device->comMoveTo(-30, 50);
				Sleep(1500);
				count = -75;
				return 0;
			}
			else
			{
				imwrite("last2.jpg", img);

				imshow("变换图", img);
				//imwrite("private.jpg", img);
				Mat ROImage = img(Rect(270, 63, 466, 471));
				//Mat ROImage = img(Rect(320, 63, 475, 481));
				imshow("原图", ROImage);
				imwrite("private.jpg", ROImage);
				//imshow("切分图", ROImage);
				device->comMoveTo(6, -30);
				Sleep(1500);
				device->comHitOnce();
				Sleep(1000);
				device->comMoveTo(-30, 50);
				Sleep(1500);
				flag3 = true;
				return 0;
			}
		}
		//warpPerspective(img,img, warpmatrix, img.size(), CV_INTER_LINEAR);
		else if(!flag6){
			if (number < 2)
			{
				number++;
			}
			else
			{
				warpPerspective(img, img, warpmatrix, img.size(), CV_INTER_LINEAR);
				last = imread("last2.jpg");
				// imwrite("last3.jpg", img);
				roi1 = last(Rect(80, 75, 60, 60));
				roi2 = img(Rect(80, 75, 60, 60));
				// qDebug() << loop(roi1, roi2) << endl;
				if (loop(roi1, roi2) > 0.8)
				{
					device->comMoveTo(8, -26);
					Sleep(2000);
					device->comHitOnce();
					Sleep(1000);
					device->comMoveTo(-30, 50);
					Sleep(3000);
					number = 0;
				}
				else
				{
					flag6 = true;
				}
			}
		}
		else
		{
			if (n < 3)
			{
				n = n + 1;
			}
			else if (n == 3 && index < order * order * 3)
			{
				while (seq_array[index % (order*order)] == 1)
				{
					index += 1;
				}
				warpPerspective(img, img, warpmatrix, img.size(), CV_INTER_LINEAR);
				imwrite("scatter.jpg", img);
				Mat srcImage = imread("private.jpg");
				Mat arr = PicDivide(srcImage, index % (order*order), 0.5);

				location = Match(img, arr, 0.3);

				if (location.x == -1 && location.y == -1)
				{
					qDebug() << "skip " << (index % (order*order)) << " low confidence" << endl;
				}
				else
				{
					qDebug() << "processing " << (index % (order*order)) << endl;
					int pointx = int((float(location.y) / img.rows) * 160);
					int pointy = int((-1)*(float(location.x) / img.cols) * 213);
					device->comMoveTo(pointx, pointy);
					Sleep(2000);
					device->comHitDown();
					Sleep(250);
					//左上角机械坐标（23，-60），右下角（139，-175）
					int move_x = 23 + (index % (order*order)) / order * (116.0 / order) + 8;
					int move_y = -60 - ((index % (order*order)) % order) * (116.0 / order) - 8;

					device->comMoveTo(move_x, move_y);
					Sleep(2000);
					device->comHitUp();
					Sleep(250);
					device->comHitDown();
					Sleep(250);
					device->comMoveTo(move_x + 3, move_y);
					Sleep(500);
					device->comHitUp();
					Sleep(250);
					device->comHitDown();
					Sleep(250);
					device->comMoveTo(move_x - 3, move_y);
					Sleep(500);
					device->comHitUp();
					Sleep(250);
					device->comHitDown();
					Sleep(250);
					device->comMoveTo(move_x, move_y + 3);
					Sleep(500);
					device->comHitUp();
					Sleep(250);
					device->comHitDown();
					Sleep(250);
					device->comMoveTo(move_x, move_y - 3);
					Sleep(500);
					device->comHitUp();
					Sleep(250);
					device->comHitDown();
					Sleep(250);
					device->comMoveTo(move_x, move_y);
					Sleep(500);
					device->comHitUp();
					Sleep(250);
					device->comMoveTo(0, 0);
					Sleep(2000);
					n = 0;
					seq_array[index % (order*order)] = 1;
				}
				index += 1;
			}
			else if (n == 3 && index < order * order * 6)
			{
				warpPerspective(img, img, warpmatrix, img.size(), CV_INTER_LINEAR);
				imwrite("scatter.jpg", img);
				Mat srcImage = imread("private.jpg");

				//左上角机械坐标（25，-59），右下角（145，-167）
				int move_x = 23 + (index % (order*order)) / order * (116.0 / order) + 8;
				int move_y = -60 - ((index % (order*order)) % order) * (116.0 / order) - 8;

				int pointx, pointy;
				int xx = 0;
				arr1 = PicDivide(srcImage, index % (order*order), 0.5);
				location1 = Match(img, arr1, 0.2);
				if (!(location1.x == -1 && location1.y == -1) && index < order * order * 5)
				{
					pointx = int((float(location1.y) / img.rows) * 160);
					pointy = int((-1)*(float(location1.x) / img.cols) * 213);

					if (abs(pointx - move_x) >= 2 || abs(pointy - move_y) >= 2)
					{
						device->comMoveTo(pointx, pointy);
						Sleep(2000);
						device->comHitDown();
						Sleep(250);

						device->comMoveTo(move_x, move_y);
						Sleep(2000);
						device->comHitUp();
						Sleep(250);
					}
					else
					{
						xx += 1;
					}


				}
				arr2 = PicDivide(srcImage, index % (order*order), 0.4);
				location2 = Match(img, arr2, 0.2);
				if (!(location2.x == -1 && location2.y == -1) && index < order * order * 5)
				{
					pointx = int((float(location2.y) / img.rows) * 160);
					pointy = int((-1)*(float(location2.x) / img.cols) * 213);

					if (abs(pointx - move_x) >= 2 || abs(pointy - move_y) >= 2)
					{
						device->comMoveTo(pointx, pointy);
						Sleep(2000);
						device->comHitDown();
						Sleep(250);

						device->comMoveTo(move_x, move_y);
						Sleep(2000);
						device->comHitUp();
						Sleep(250);
					}
					else
					{
						xx += 1;
					}
				}
				arr3 = PicDivide(srcImage, index % (order*order), 0.3);
				location3 = Match(img, arr3, 0.2);
				if (!(location3.x == -1 && location3.y == -1))
				{
					pointx = int((float(location3.y) / img.rows) * 160);
					pointy = int((-1)*(float(location3.x) / img.cols) * 213);

					if (abs(pointx - move_x) >= 2 || abs(pointy - move_y) >= 2)
					{
						device->comMoveTo(pointx, pointy);
						Sleep(2000);
						device->comHitDown();
						Sleep(250);

						device->comMoveTo(move_x, move_y);
						Sleep(2000);
						device->comHitUp();
						Sleep(250);
					}
					else
					{
						xx += 1;
					}
				}

				if (location1.x == -1 && location1.y == -1)
				{
					if (location2.x == -1 && location2.y == -1)
					{
						if (location3.x == -1 && location3.y == -1)
						{
							count = 0;
							return 0;
						}
					}
				}

				if (xx == 3)
				{
					count = 0;
					return 0;
				}

				if (index >= order * order * 5)
				{
					device->comHitUp();
					Sleep(250);
					device->comHitDown();
					Sleep(250);
					device->comMoveTo(move_x + 3, move_y);
					Sleep(2000);
					device->comHitDown();
					Sleep(250);
					device->comMoveTo(move_x - 3, move_y);
					Sleep(2000);
					device->comHitUp();
					Sleep(250);
					device->comHitDown();
					Sleep(250);
					device->comMoveTo(move_x, move_y + 3);
					Sleep(2000);
					device->comHitUp();
					Sleep(250);
					device->comHitDown();
					Sleep(250);
					device->comMoveTo(move_x, move_y - 3);
					Sleep(2000);
					device->comHitUp();
					Sleep(250);
					device->comHitDown();
					Sleep(250);
					device->comMoveTo(move_x, move_y);
					Sleep(2000);
					device->comHitUp();
					Sleep(250);
				}
				device->comMoveTo(0, 0);
				n = 0;
				index += 1;
			}
			else
			{
				device->comMoveTo(-30, 50);
				Sleep(2000);
			}
		}
		count = 1;
	}
	else
	{
		count++;
		qDebug() << count << endl;
	}
	/*
	cv::Size imgSize(img.cols, img.rows - UP_CUT);
	if (imgSize.height <= 0 || imgSize.width <= 0)
	{
		qDebug() << "Invalid image. Size:" << imgSize.width <<"x"<<imgSize.height;
		return -1;
	}

	//截取图像边缘
	cv::Mat pt = img(cv::Rect(0, UP_CUT, imgSize.width,imgSize.height));
	cv::imshow(WIN_NAME, pt);

	//判断鼠标点击尺寸
	if (argM.box.x >= 0 && argM.box.x < imgSize.width&&
		argM.box.y >= 0 && argM.box.y < imgSize.height
		)
	{
		qDebug() << "X:" << argM.box.x << " Y:" << argM.box.y;
		if (argM.Hit)
		{
			device->comHitDown();
		}
		device->comMoveToScale(((double)argM.box.x + argM.box.width) / pt.cols, ((double)argM.box.y + argM.box.height) / pt.rows);
		argM.box.x = -1; argM.box.y = -1;
		if (argM.Hit)
		{
			device->comHitUp();
		}
		else
		{
			device->comHitOnce();
		}
	}

	*/
	return 0;
}

//鼠标回调函数
void mouseCallback(int event, int x, int y, int flags, void*param)
{
	usrGameController::MouseArgs* m_arg = (usrGameController::MouseArgs*)param;
	switch (event)
	{
	case CV_EVENT_MOUSEMOVE: // 鼠标移动时
	{
		if (m_arg->Drawing)
		{
			m_arg->box.width = x - m_arg->box.x;
			m_arg->box.height = y - m_arg->box.y;
		}
	}
	break;
	case CV_EVENT_LBUTTONDOWN:case CV_EVENT_RBUTTONDOWN: // 左/右键按下
	{
		m_arg->Hit = event == CV_EVENT_RBUTTONDOWN;
		m_arg->Drawing = true;
		m_arg->box = cvRect(x, y, 0, 0);
	}
	break;
	case CV_EVENT_LBUTTONUP:case CV_EVENT_RBUTTONUP: // 左/右键弹起
	{
		m_arg->Hit = false;
		m_arg->Drawing = false;
		if (m_arg->box.width < 0)
		{
			m_arg->box.x += m_arg->box.width;
			m_arg->box.width *= -1;
		}
		if (m_arg->box.height < 0)
		{
			m_arg->box.y += m_arg->box.height;
			m_arg->box.height *= -1;
		}
	}
	break;
	}
}
Mat usrGameController::PicDivide(Mat srcImage, int n, float proportion) {
	Mat arr;
	int i = n / order;
	int j = n % order;

	Size size_srcImage = srcImage.size();
	Size new_size = Size(size_srcImage.width / order + 1, size_srcImage.height / order + 1);
	arr = srcImage(Rect(j * new_size.width + new_size.width * (1 - proportion) * 0.5, i * new_size.height + new_size.height * (1 - proportion) * 0.5, new_size.width * proportion, new_size.height * proportion));
	//arr = srcImage(Rect(j * new_size.width, i * new_size.height, new_size.width, new_size.height));
	return arr;
};

Point usrGameController::Match(Mat target_pic, Mat divide_pic, float t) {
	Mat img, templ, result, piece;
	img = target_pic;
	templ = divide_pic;

	resize(templ, templ, Size(), 0.906, 0.906);

	int result_cols = img.cols - templ.cols + 1;
	int result_rows = img.rows - templ.rows + 1;
	result.create(result_cols, result_rows, CV_32FC1);

	//matchTemplate(img, templ, result, CV_TM_SQDIFF_NORMED);//使用的匹配算法是标准平方差匹配 method=CV_TM_SQDIFF_NORMED，数值越小越准确
	matchTemplate(img, templ, result, 5);
	//normalize(result, result, 0, 1, NORM_MINMAX, -1, Mat());

	double minVal = -1;
	double maxVal;
	Point minLoc;
	Point maxLoc;
	Point matchLoc;
	Point midLoc;
	minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());

	//cout << "匹配度：" << minVal << endl;

	matchLoc = maxLoc;

	rectangle(img, matchLoc, Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows), Scalar(0, 255, 0), 2, 8, 0);
	piece = img(Rect(matchLoc.x, matchLoc.y, templ.cols, templ.rows));
	float similar2 = loop(templ, piece);

	imshow("img", img);
	imshow("templ", templ);
	midLoc.x = matchLoc.x + templ.cols / 2;
	midLoc.y = matchLoc.y + templ.rows / 2;

	//return midLoc;
	qDebug() << maxVal << ' ';
	qDebug() << similar2;

	if (similar2 < t) {
		midLoc.x = -1;
		midLoc.y = -1;
		return midLoc;
	}
	else
		return midLoc;

};

void usrGameController::refresh(int i)
{
	for (int j = 0; j < stk1.size(); ++j)
	{
		stk1.pop();
	}
	for (int j = 0; j < stk2.size(); ++j)
	{
		stk2.pop();
	}
	for (int j = 0; j <= (i - 1) / 2; ++j)
	{
		int x1 = j;
		int x2 = i - 1 - j;
		if (x1 == x2)
		{
			stk1.push(x1*i + x1);
		}
		else
		{
			stk1.push(x1*i + x1);
			stk1.push(x1*i + x2);
			stk1.push(x2*i + x1);
			stk1.push(x2*i + x2);
			for (int a = x1 + 1; a <= x2 - 1; ++a)
			{
				stk1.push(x1*i + a);
				stk1.push(x2*i + a);

				stk1.push(a*i + x1);
				stk1.push(a*i + x2);

			}

		}
	}
	for (int j = 0; j < i*i; ++j)
	{
		qDebug() << stk1.top() << ' ';
		stk2.push(stk1.top());
		stk1.pop();
	}


}

float usrGameController::loop(const Mat& i1, const Mat& i2)
{
	const double C1 = 6.5025, C2 = 58.5225;
	/***************************** INITS **********************************/
	int d = CV_32F;

	Mat I1, I2;
	i1.convertTo(I1, d);           // cannot calculate on one byte large values
	i2.convertTo(I2, d);

	Mat I2_2 = I2.mul(I2);        // I2^2
	Mat I1_2 = I1.mul(I1);        // I1^2
	Mat I1_I2 = I1.mul(I2);        // I1 * I2

	/*************************** END INITS **********************************/

	Mat mu1, mu2;   // PRELIMINARY COMPUTING
	GaussianBlur(I1, mu1, Size(11, 11), 1.5);
	GaussianBlur(I2, mu2, Size(11, 11), 1.5);

	Mat mu1_2 = mu1.mul(mu1);
	Mat mu2_2 = mu2.mul(mu2);
	Mat mu1_mu2 = mu1.mul(mu2);

	Mat sigma1_2, sigma2_2, sigma12;

	GaussianBlur(I1_2, sigma1_2, Size(11, 11), 1.5);
	sigma1_2 -= mu1_2;

	GaussianBlur(I2_2, sigma2_2, Size(11, 11), 1.5);
	sigma2_2 -= mu2_2;

	GaussianBlur(I1_I2, sigma12, Size(11, 11), 1.5);
	sigma12 -= mu1_mu2;

	///////////////////////////////// FORMULA ////////////////////////////////
	Mat t1, t2, t3;

	t1 = 2 * mu1_mu2 + C1;
	t2 = 2 * sigma12 + C2;
	t3 = t1.mul(t2);              // t3 = ((2*mu1_mu2 + C1).*(2*sigma12 + C2))

	t1 = mu1_2 + mu2_2 + C1;
	t2 = sigma1_2 + sigma2_2 + C2;
	t1 = t1.mul(t2);               // t1 =((mu1_2 + mu2_2 + C1).*(sigma1_2 + sigma2_2 + C2))

	Mat ssim_map;
	divide(t3, t1, ssim_map);      // ssim_map =  t3./t1;

	Scalar mssim = mean(ssim_map); // mssim = average of ssim map
	return (mssim[0] + mssim[1] + mssim[2]) / 3;
}


#endif
