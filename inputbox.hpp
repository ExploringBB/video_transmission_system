#include<string>
#include<windows.h> 
#include<iostream>    
#include<Opencv2\opencv.hpp>
using namespace cv;
using namespace std;
string input_command()
{
	//仅支持ascll表字符输入，不支持中文
	string command = "";

	cvNamedWindow("Input Command");
	bool flag = true;
	while (true)
	{
		Mat input_box = Mat(Size(600, 100), CV_8UC3, Scalar(0, 0, 0));
		if (flag)
			putText(input_box, command, Point(10, 30), 
				CV_FONT_HERSHEY_COMPLEX, 0.7, Scalar(255, 255, 255), 2, 8);
		else
			putText(input_box, command + "|", Point(10, 30), 
				CV_FONT_HERSHEY_COMPLEX, 0.7, Scalar(255, 255, 255), 2, 8);
		flag = !flag;
		if (!cvGetWindowHandle("Input Command"))
		{
		break;
		}
		imshow("Input Command", input_box);
		int key_vaule = waitKeyEx(1000);//waitKey()的键值等于ascll编码，未等到输入时的返回值是-1
		if (key_vaule == 13)//回车键完成输入
			break;
		else if (key_vaule == -1)//无输入
			continue;
		else if (key_vaule == 8)//回退键，删除一位输入
		{
			if (!command.empty())	//防止无输入时退位造成错误
				command.pop_back();	//删除最后一位元素
		}
		else if (key_vaule == 2424832)//←
		{
			
		}
		else if (key_vaule == 2490368)//↑
		{

		}
		else if (key_vaule == 2555904)//→
		{

		}
		else if (key_vaule == 2621440)//↓
		{

		}
		else
		{
			command += key_vaule;
			//cout << key_vaule<<endl;
		}
		//command += to_string(key_vaule);
	}

	cv::destroyWindow("Input Command");
	return command;
}