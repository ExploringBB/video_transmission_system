#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "EnhancedWindow.h"
#include "inputbox.hpp"
#include "x264_Encode.hpp"
#pragma comment(lib, "User32.lib") //改为在静态库中使用，添加windows依赖库
#define CVUI_IMPLEMENTATION
#define WINDOW_PUB	"Publisher Controller"
using namespace cv;
using namespace std;
extern Mat frame_capture;
bool Writer_Reliable = true, Writer_Best = false;
struct ret_p {
	int bre;
	string str;
	int w=0, h=0;
	string qos;
	int video_quality=3;
	bool confirm = false,send=false,close_time=false;
	string Pub_text = "";
	bool qos_conf = false;//
};
struct ret_p p_return;
double val = 3;
SYSTEMTIME timestamp;
char time_stamp_d[20];
char time_stamp_t[20];
char time_stamp[50];
void Capture_show(void *cap)
{
	VideoCapture capture(0);
	p_return.w = capture.get(CV_CAP_PROP_FRAME_WIDTH);
	p_return.h = capture.get(CV_CAP_PROP_FRAME_HEIGHT);
	namedWindow("Video Capture", CV_WINDOW_AUTOSIZE);
	while (true)
	{
		capture >> frame_capture;
		GetLocalTime(&timestamp);
		sprintf(time_stamp_d, "%d-%d-%d", timestamp.wYear, timestamp.wMonth, timestamp.wDay);
		sprintf(time_stamp_t, "%d:%d:%d ( %d )", timestamp.wHour, timestamp.wMinute,
			timestamp.wSecond,timestamp.wMilliseconds);
		cv::putText(frame_capture, time_stamp_d, Point(20, 20), FONT_HERSHEY_PLAIN,1.5,Scalar(10, 10, 10),2);
		cv::putText(frame_capture, time_stamp_t, Point(20, 40), FONT_HERSHEY_PLAIN, 1.5, Scalar(10, 10, 10), 2);
;		cv::imshow("Video Capture", frame_capture);
		if (waitKey(1) == 27)break;	//	"esc"=exit
		if (p_return.bre == -1)
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
		if (p_return.close_time)
		{
			p_return.close_time = false;
			_endthread();		//end process;
		}
		cv::imshow("PUB_TIMT_STAMP", time_f);
		waitKey(1);
	}
}
//========================================================================================
void gui_pub(void *p)
{
	char video_qulaity[25] = "Video Quality: 3";
	cv::Mat frame = cv::Mat(cv::Size(500, 300), CV_8UC3);
	//EnhancedWindow Qos(340, 100, 130, 100, "Qos Choose");
	cvui::init(WINDOW_PUB, 1);
	
	while (true)
	{
		frame = cv::Scalar(223, 223, 223);	//background :RGB
		//Qos
		cvui::text(frame, 10, 10, "Reliablity Qos:", 1.2* cvui::DEFAULT_FONT_SCALE, 0x0f0f0f);
		if(Writer_Reliable && p_return.qos_conf)
			cvui::text(frame, 130, 10, "Reliable", 1.2* cvui::DEFAULT_FONT_SCALE, 0x0f0f0f);
		else if(Writer_Best && p_return.qos_conf)
			cvui::text(frame,130, 10, "Best Effort", 1.2* cvui::DEFAULT_FONT_SCALE, 0x0f0f0f);
		else
			cvui::text(frame, 130, 10, "Waiting Choose", 1.2* cvui::DEFAULT_FONT_SCALE, 0x0f0f0f);
		cv::rectangle(frame, cv::Rect(10, 40 - 5, 150, 85), Scalar(170, 170, 170));
		if (Writer_Best)Writer_Reliable = false;
		cvui::checkbox(frame, 20, 50 - 5, "Reliablity", &Writer_Reliable, 0x0f0f0f, 0.4*1.5);
		//MessageBox(NULL, TEXT("yes"), TEXT("asd"),NULL);
		if (Writer_Reliable)Writer_Best = false;
		cvui::checkbox(frame, 20,100 - 20, "Best Effort", &Writer_Best, 0x0f0f0f, 0.4*1.5);
		//
		//Button
		if (cvui::button(frame, 200, 40 - 5, "Qos Conf", 1.3* cvui::DEFAULT_FONT_SCALE, 0xe6e6e6))
		{
			if (Writer_Reliable == false && Writer_Best == false)
				MessageBox(NULL, LPCSTR(("至少选择一个")), LPCSTR("Error"), NULL);
			else
				p_return.qos_conf = true;
		}
		if (cvui::button(frame, 340, 40 - 5, "   &Quit  ", 1.2* cvui::DEFAULT_FONT_SCALE, 0xe6e6e6))
		{
			p_return.bre = -1;//break;
			p_return.close_time = true;
			destroyAllWindows();
			_endthread();		//end process;
		}
		if (cvui::button(frame, 200, 90 - 5, "Send Msg ", 1.2* cvui::DEFAULT_FONT_SCALE, 0xe6e6e6))
		{
			p_return.str = input_command();
			p_return.send = true;
		}
		if (cvui::button(frame, 200, 160 - 5, "Confirm", 1.2* cvui::DEFAULT_FONT_SCALE, 0xe6e6e6))
		{
			p_return.confirm = true;
			p_return.video_quality = val;
			sprintf(video_qulaity, "Video Quality: %1.0f", val);
		}

		//Publish Status
		//cv::rectangle(frame, cv::Rect(10, 260 - 5, 480 - 20, 20), Scalar(170, 170, 170));
		cv::line(frame, Point(10, 225), Point(480, 225), Scalar(100, 100, 100));
		cvui::text(frame, 20, 260 - 5, "Status: ", 1.5* cvui::DEFAULT_FONT_SCALE, 0x0f0f0f);
		cvui::text(frame, 90, 260 - 5, p_return.Pub_text, 1.5* cvui::DEFAULT_FONT_SCALE, 0x0f0f0f);

		//Publish Time Stamp
		GetLocalTime(&timestamp);
		sprintf(time_stamp, "Time_Stamp: %d-%d-%d | %d:%d:%d( %d )",
			timestamp.wYear, timestamp.wMonth, timestamp.wDay,
			timestamp.wHour, timestamp.wMinute, timestamp.wSecond, timestamp.wMilliseconds);
		cvui::text(frame, 20, 240 - 5, time_stamp, 1.5* cvui::DEFAULT_FONT_SCALE, 0x0f0f0f);

		//Video Quality
		cvui::text(frame, 10, 136 - 5, video_qulaity, 1.2* cvui::DEFAULT_FONT_SCALE, 0x0f0f0f);
		cvui::rect(frame, 10, 155 - 5, 150, 60, 0xbebebe, 0xa5a5a5);
		cvui::trackbar(frame, 10, 150, 150, &val, 1., 5., 1, "%.0f", 4, 1., 0.5);
			//cv::createTrackbar("")
		cvui::update();
		if (!cvGetWindowHandle(WINDOW_PUB))
		{
			p_return.bre = -1;//break;
			destroyAllWindows();
			_endthread();		//end process;
		}
		cv::imshow(WINDOW_PUB, frame);
	}
}