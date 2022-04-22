#pragma once
#ifndef _SUCH
#define _SUCH
#include "OpenCV/include/opencv2/opencv.hpp"  
#include <string>
#include "Windows.h"
#include <stdlib.h>
#include "OpenCV/include/opencv2/highgui.hpp"
#include "OpenCV/include/opencv2/imgproc/imgproc.hpp"
#include "Sub_show.h"
extern "C"
{
#include "libavcodec/avcodec.h"  
#include "libswscale/swscale.h"  
#include "libavutil/imgutils.h"  
#include "libavformat/avformat.h"  
#include "x264.h"
#include "x264_config.h"
};
using namespace cv;
using namespace std;
int s_save=0;
cv::Mat rgbImg = cv::Mat(cv::Size(640, 480), CV_8UC3);
int video_decode(unsigned char *videoData, int len)
{
	int got_picture;
	int ret;

	avcodec_register_all();
	AVCodec *pCodec = avcodec_find_decoder(AV_CODEC_ID_H264);
	AVCodecContext* pCodecCtx = avcodec_alloc_context3(pCodec);
	avcodec_open2(pCodecCtx, pCodec, NULL);

	pCodecCtx->time_base.num = 1;
	pCodecCtx->frame_number = 1;   //每包一个视频帧  
	pCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
	pCodecCtx->bit_rate = 0;
	pCodecCtx->time_base.den = 30;  //帧率  
	pCodecCtx->width = 640;         //视频宽  
	pCodecCtx->height = 480;        //视频高  

	AVFrame* pAvFrame = av_frame_alloc();
	AVPacket *packet = new AVPacket;
	av_new_packet(packet, len);
	packet->data = videoData;

	ret = avcodec_decode_video2(pCodecCtx, pAvFrame, &got_picture, packet);
	if (ret < 0)
	{
		printf("Decode Error\n");
		return -1;
	}
	if (got_picture)
	{
		//printf("Decode Success\n");

		Mat frame;
		frame.create(pAvFrame->height * 1.5, pAvFrame->width, CV_8UC1);

		int a = 0, i;
		for (i = 0; i < pAvFrame->height; i++)
		{
			memcpy(frame.data + a, pAvFrame->data[0] + i * pAvFrame->linesize[0], pAvFrame->width);
			a += pAvFrame->width;
		}
		for (i = 0; i < pAvFrame->height / 2; i++)
		{
			memcpy(frame.data + a, pAvFrame->data[1] + i * pAvFrame->linesize[1], pAvFrame->width / 2);
			a += pAvFrame->width / 2;
		}
		for (i = 0; i < pAvFrame->height / 2; i++)
		{
			memcpy(frame.data + a, pAvFrame->data[2] + i * pAvFrame->linesize[2], pAvFrame->width / 2);
			a += pAvFrame->width / 2;
		}
		cvtColor(frame, rgbImg, CV_YUV2BGR_I420);
		static VideoWriter mp4writ;
		static bool op=true;
		if (op)
		{
			Size wh = Size(640, 480);
			//if(s_return.save)
			mp4writ.open("Receive.avi", CV_FOURCC('I', 'Y', 'U', 'V'), 25, wh, true);
			op = false;
		}
		//s_return.bre=0;
		if (s_save)
		{	
			mp4writ.write(rgbImg);
			cout << "save" << endl;
			//waitKey(30);
		}
	}
	av_free_packet(packet);
	av_free(pAvFrame);
	avcodec_close(pCodecCtx);
	return 0;
}

//int video_encode(int width, int height, unsigned char *yuv_frame, unsigned char **out_buf,int *len_out)
//{
//
//}


#endif // such