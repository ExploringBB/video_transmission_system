#include<string>
#include<windows.h> 
#include<iostream>    
#include<Opencv2\opencv.hpp>
using namespace cv;
using namespace std;
string input_command()
{
	//��֧��ascll���ַ����룬��֧������
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
		int key_vaule = waitKeyEx(1000);//waitKey()�ļ�ֵ����ascll���룬δ�ȵ�����ʱ�ķ���ֵ��-1
		if (key_vaule == 13)//�س����������
			break;
		else if (key_vaule == -1)//������
			continue;
		else if (key_vaule == 8)//���˼���ɾ��һλ����
		{
			if (!command.empty())	//��ֹ������ʱ��λ��ɴ���
				command.pop_back();	//ɾ�����һλԪ��
		}
		else if (key_vaule == 2424832)//��
		{
			
		}
		else if (key_vaule == 2490368)//��
		{

		}
		else if (key_vaule == 2555904)//��
		{

		}
		else if (key_vaule == 2621440)//��
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