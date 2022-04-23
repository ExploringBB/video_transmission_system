#include "Sub_show.h"
#include"cvui.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include<Windows.h>
#include <process.h>
#include"inputbox.hpp"
#define CVUI_IMPLEMENTATION
#define WINDOW_SUB	"Subscirber Controller"
using namespace cv;
using namespace std;
extern Mat rgbImg;
extern int s_save;
int s_bre = 0;
string Sub_text = "";
bool Reader_Reliable = true, Reader_Best = false, Reader_Qos_conf=false, close_time=false;
extern string text_message;
char time_stamp[50];
SYSTEMTIME timestamp;
void Image_show(void *p)
{
	while (true)
	{
		imshow("Decode Image", rgbImg);//显示decode的图像
		waitKey(1);
		if (s_bre == -1)
		{
			_endthread();
			break;
		}
	}
}

void time_show(void *time)
{
	cv::Mat time_f = cv::Mat(cv::Size(340, 40), CV_8UC3);
	while (true)
	{
		time_f = cv::Scalar(223, 223, 223);	//background :RGB
		GetLocalTime(&timestamp);
		sprintf(time_stamp, "%d-%d-%d | %d:%d:%d( %d )",
			timestamp.wYear, timestamp.wMonth, timestamp.wDay,
			timestamp.wHour, timestamp.wMinute, timestamp.wSecond, timestamp.wMilliseconds);
		cv::putText(time_f, time_stamp, Point(10, 20), FONT_HERSHEY_PLAIN, 1.3, Scalar(10, 10, 10), 2);
		if (close_time)
		{
			close_time = false;
			_endthread();		//end process;
		}
		cv::imshow("SUB_TIMT_STAMP", time_f);
		waitKey(1);
	}
}

void gui_sub(void *st)
{
	cv::Mat frame = cv::Mat(cv::Size(500, 300-80), CV_8UC3);
	cvui::init(WINDOW_SUB, 1);
	while (true)
	{
		frame = cv::Scalar(223, 223, 223);	//background :RGB
		//Qos  
		cvui::text(frame, 10, 10, "Reliablity Qos:", 1.2* cvui::DEFAULT_FONT_SCALE, 0x0f0f0f);
		if (Reader_Reliable && Reader_Qos_conf)
			cvui::text(frame, 130, 10, "Reliable", 1.2* cvui::DEFAULT_FONT_SCALE, 0x0f0f0f);
		else if (Reader_Best && Reader_Qos_conf)
			cvui::text(frame, 130, 10, "Best Effort", 1.2* cvui::DEFAULT_FONT_SCALE, 0x0f0f0f);
		else
			cvui::text(frame, 130, 10, "Waiting Choose", 1.2* cvui::DEFAULT_FONT_SCALE, 0x0f0f0f);
		cv::rectangle(frame, cv::Rect(10, 40 - 5, 150, 85), Scalar(170, 170, 170));
		if (Reader_Best)Reader_Reliable = false;
		cvui::checkbox(frame, 20, 50 - 5, "Reliable", &Reader_Reliable, 0x0f0f0f, 0.4*1.5);
		if (Reader_Reliable)Reader_Best = false;
		cvui::checkbox(frame, 20, 80 , "Best Effort", &Reader_Best, 0x0f0f0f, 0.4*1.5);

		//Button
		if (cvui::button(frame, 200, 40 - 5, "Qos Conf", 1.3* cvui::DEFAULT_FONT_SCALE, 0xe6e6e6))
		{
			if (Reader_Reliable == false && Reader_Best == false)
				MessageBox(NULL, LPCSTR(("至少选择一个")), LPCSTR("Error"), NULL);
			else
				Reader_Qos_conf = true;
		}
		if (cvui::button(frame, 340, 40 - 5, "   &Quit  ", 1.2* cvui::DEFAULT_FONT_SCALE, 0xe6e6e6))
		{
			s_bre = -1;//break;
			close_time = true;
			destroyAllWindows();
			_endthread();		//end process;
		}
		if (s_save == 1) {
			cv::rectangle(frame, cv::Rect(200 - 3, 85 - 4, 120 + 3, 40 - 1), Scalar(150, 150, 150), 2);
		}
		if (cvui::button(frame, 200, 90 - 5, "Save Video", 1.2* cvui::DEFAULT_FONT_SCALE, 0xe6e6e6))
		{
			s_save = 1;
		}
		if (cvui::button(frame, 340, 90 - 5, "Stop Save", 1.2* cvui::DEFAULT_FONT_SCALE, 0xe6e6e6))
		{
			s_save = 0;
		}
		//Publish Status
		//cv::rectangle(frame, cv::Rect(10, 260 - 5, 480 - 20, 20), Scalar(170, 170, 170));
		cv::line(frame, Point(10, 200-50), Point(480, 200-50), Scalar(100, 100, 100));
		cvui::text(frame, 20, 270 - 55, Sub_text, 1.5* cvui::DEFAULT_FONT_SCALE, 0x0f0f0f);
		GetLocalTime(&timestamp);
		sprintf(time_stamp, "Time_Stamp: %d-%d-%d | %d:%d:%d( %d )",
			timestamp.wYear, timestamp.wMonth, timestamp.wDay,
			timestamp.wHour, timestamp.wMinute, timestamp.wSecond, timestamp.wMilliseconds);
		cvui::text(frame, 20, 210-50, time_stamp, 1.5* cvui::DEFAULT_FONT_SCALE, 0x0f0f0f);
		cvui::text(frame, 20, 200-15, "Text : ", 1.5* cvui::DEFAULT_FONT_SCALE, 0x0f0f0f);
		cvui::text(frame, 85, 200-15, text_message, 1.5* cvui::DEFAULT_FONT_SCALE, 0x0f0f0f);
		//Video Quality
		//cvui::text(frame, 10, 166 - 5, video_qulaity, 1.2* cvui::DEFAULT_FONT_SCALE, 0x0f0f0f);
		//cvui::rect(frame, 10, 185 - 5, 150, 60, 0xbebebe, 0xa5a5a5);
		//cvui::trackbar(frame, 10, 180, 150, &val, 1., 5., 1, "%.0f", 4, 1., 0.5);
		cvui::update();
		if (!cvGetWindowHandle(WINDOW_SUB))
		{
			s_bre = -1;//break;
			destroyAllWindows();
			_endthread();		//end gui process;
		}
		cv::imshow(WINDOW_SUB, frame);
	}
}