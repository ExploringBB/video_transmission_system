#include <dds/DdsDcpsInfrastructureC.h>
#include <dds/DdsDcpsPublicationC.h>
#include <dds/DCPS/Marked_Default_Qos.h>
#include <dds/DCPS/Service_Participant.h>
#include <dds/DCPS/WaitSet.h>

#include <dds/DCPS/StaticIncludes.h>
#ifdef ACE_AS_STATIC_LIBS
#  include <dds/DCPS/RTPS/RtpsDiscovery.h>
#  include <dds/DCPS/transport/rtps_udp/RtpsUdp.h>
#endif

#include "msgTypeSupportImpl.h"
# include <fstream>
# include <ios>
# include <iostream>
#include<process.h>
#include<windows.h>
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>  
#include "x264_Encode.hpp"
//#define CVUI_IMPLEMENTATION
#include "pub_show.hpp"
#include "Sub_show.h"
#pragma comment(lib, "User32.lib") //改为在静态库中使用，添加windows依赖库
using namespace cv;
using namespace msg;
using namespace std;
cv::Mat frame_capture = cv::Mat(cv::Size(640, 480), CV_8UC3);
extern struct ret_p p_return;
extern bool Writer_Reliable,Writer_Best;
x264_param_t param;
int Options();

int main(int argc, char *argv[]) {
	try {		
		cout << "Publisher" << endl;
		DDS::DomainParticipantFactory_var dpf = TheParticipantFactoryWithArgs(argc, argv);
		DDS::DomainParticipant_var participant = dpf->create_participant(42, PARTICIPANT_QOS_DEFAULT, 0, OpenDDS::DCPS::DEFAULT_STATUS_MASK);
		if (!participant) {
			std::cerr << "creat_particpant fialed. code:line 19" << std::endl;
			//return 1;
		}
		msg::MessageTypeSupport_var mts = new msg::MessageTypeSupportImpl();
		if (DDS::RETCODE_OK != mts->register_type(participant, "")) {
			std::cerr << "register_type failed in code line 26" << std::endl;
			//return 1;
		}
		//从类型支持对象中获取注册类型的名称并创建主题
		CORBA::String_var type_name = mts->get_type_name();
		DDS::Topic_var topic = participant->create_topic("Video_Transmission_System", type_name, TOPIC_QOS_DEFAULT, 0, OpenDDS::DCPS::DEFAULT_STATUS_MASK);
		if (!topic) {
			std::cerr << "creat_topic error ,in code line 34" << std::endl;
			//return 1;
		}
		//创建发布者（变量）	
		DDS::Publisher_var pub = participant->create_publisher(PUBLISHER_QOS_DEFAULT, 0, OpenDDS::DCPS::DEFAULT_STATUS_MASK);
		if (!pub) {
			std::cerr << "creat_publisher error ,in code line 41" << std::endl;
			//return 1;
		}
/*==========================UI_PORCESS==============================================*/
		_beginthread(time_show, 0, NULL);
		_beginthread(Capture_show, 0, NULL);
		_beginthread(gui_pub, 0, NULL);
		
/*==========================UI_PORCESS==============================================*/
		DDS::DataWriterQos Dr;
		pub->get_default_datawriter_qos(Dr);
		p_return.Pub_text = "Wait for Choosing Qos...";
		while (p_return.qos_conf == false);
		if (Writer_Reliable)
		{
			Dr.reliability.kind = DDS::RELIABLE_RELIABILITY_QOS;	//可靠
		}
		else if(Writer_Best)
		{
			Dr.reliability.kind = DDS::BEST_EFFORT_RELIABILITY_QOS;//最大努力
		}
		Dr.reliability.max_blocking_time.sec = 10;				//每个可靠类型的阻塞时间为10s;
		Dr.reliability.max_blocking_time.nanosec = 0;			//纳秒=0？
		//Dr.resource_limits.max_samples_per_instance = 100;		//设置每个实例样本数为100


	/*以上策略的组合意味着:当100个样本等待传送时，写者在返回错误码前可以阻塞多达10秒。
	如果相同的策略应用在数据读者上，置意味着 : 直至有100个未读的样本排队之后，
	允许有样本被拒收。被拒绝的样本会被丢弃掉，并更
	新SampleRejectStatus(样本拒收状态)*/

		DDS::DataWriter_var writer = pub->create_datawriter(topic, Dr, 0, OpenDDS::DCPS::DEFAULT_STATUS_MASK);
		if (!writer) {
			std::cerr << "creat_data_writer error ,in code line 41" << std::endl;
			//return 1;
		} 
		//将数据写入器引用的范围缩小到messageDataWriter对象引用，以便我们可以使用特定的类型的发布操作。
		msg::MessageDataWriter_var message_writer = msg::MessageDataWriter::_narrow(writer);
		p_return.Pub_text = "Waiting Subscriber...";
		DDS::StatusCondition_var conditon = writer->get_statuscondition();
		conditon->set_enabled_statuses(DDS::PUBLICATION_MATCHED_STATUS);
		DDS::WaitSetInterf_var ws = new DDS::WaitSet;
		ws->attach_condition(conditon);
		p_return.Pub_text= "Waiting Subscriber...1";
		while (true) 
		{
			DDS::PublicationMatchedStatus matches;
			if (writer->get_publication_matched_status(matches) != DDS::RETCODE_OK) {
				//std::cerr << "get_publication_match_status error,in code line 73" << std::endl;
				p_return.Pub_text = "get_publication_match_status error";
				//return 1;
			}
			if (matches.current_count >= 1) break;
			p_return.Pub_text = "Waiting Subscriber...2";
			DDS::ConditionSeq conditions;
			DDS::Duration_t timeout = { 60,0 };
			if (ws->wait(conditions, timeout) != DDS::RETCODE_OK) {
				//std::cerr << "wait failed,in code line 81" << std::endl;
				p_return.Pub_text = "wait failed";
				//return 1;
			}
		}

		ws->detach_condition(conditon);
		p_return.Pub_text = "Connetcted";
		//消息样本发布：(填充样本信息)
		msg::Message TheMessage;
		TheMessage.subject_id = 99;
		//TheMessage.subject = "worse movie";
		//TheMessage.from = "comic book guy";
		TheMessage.count = 0;
		//get time

		int length = 0;
		string content = "";
		int width = 640, height = 480;
		int yuv_bufLen = width * height * 3 / 2;		
		x264_t *encoder;
		x264_picture_t pic_in, pic_out;
		
		x264_param_default_preset(&param, "veryfast", "zerolatency");
		param.i_threads = 1;
		param.i_width = width;
		param.i_height = height;
		param.i_fps_num = 30;
		param.i_fps_den = 1;
		param.b_intra_refresh = 1;
		param.b_annexb = 1;
		param.b_repeat_headers = 1;
		param.rc.b_mb_tree = 0;
		param.i_level_idc = 30;//编码复杂度 
		param.rc.f_rf_constant = 25; //图像质量 
		param.rc.f_rf_constant_max = 50; //图像质量 
		param.i_keyint_max = 10;
		param.i_bframe_adaptive = X264_B_ADAPT_TRELLIS;
		param.i_threads = 1;
		param.b_sliced_threads = 0;
		x264_param_apply_profile(&param, "baseline");
		x264_picture_alloc(&pic_in, X264_CSP_I420, width, height);
		
		
		//unsigned char* pYuvBuf = new unsigned char[yuv_bufLen];//malloc
		unsigned char *pYuvBuf = (unsigned char *)malloc(yuv_bufLen+1);
		unsigned char *buf = (unsigned char *)malloc(35000);
		Mat yuv_mat;
		while (1)
		{	
			encoder = x264_encoder_open(&param);
			cvtColor(frame_capture, yuv_mat, CV_BGR2YUV_I420);
			memcpy(pYuvBuf, yuv_mat.data, yuv_bufLen);

			pic_in.img.plane[0] = pYuvBuf;
			pic_in.img.plane[1] = pic_in.img.plane[0] + width * height;
			pic_in.img.plane[2] = pic_in.img.plane[1] + width * height / 4;
			
			int64_t i_pts = 0;
			x264_nal_t *nals = NULL;
			x264_nal_t *nal;
			int nnal;
			pic_in.i_pts = i_pts++;
			x264_encoder_encode(encoder, &nals, &nnal, &pic_in, &pic_out);
			int i = 0;
			for (nal = nals; nal < nals + nnal; nal++)
			{
				memcpy(buf + i, nal->p_payload, nal->i_payload);
				i = i + nal->i_payload;
			}
			//----------------------------------------------------------------------
			TheMessage.count = i;
			memcpy(TheMessage.img, buf, 35000);						
			if (p_return.send)
			{
				TheMessage.subject = CORBA::string_dup(p_return.str.data());//string
				p_return.send = false;
			}
			message_writer->write(TheMessage, DDS::HANDLE_NIL);				//send
			p_return.str = "";												//clear string
			if (p_return.bre == -1)											//exit
			{
				p_return.bre = 0;
				destroyAllWindows();
				break;
			}
			x264_encoder_close(encoder);
			Options();
		}
		free(buf);
		free(pYuvBuf);
/*=========================================================================================*/

		participant->delete_contained_entities();
		dpf->delete_participant(participant);

		TheServiceParticipant->shutdown();
	}
	catch (const CORBA::Exception& e) {
		e._tao_print_exception("！！！	Exception caught in main():");
		return 1;
	}
}

int Options(void)
{
	if (p_return.confirm)
	{
		p_return.confirm = false;
		if (p_return.video_quality == 1)
		{
			param.rc.f_rf_constant = 45;
		}
		else if (p_return.video_quality == 2)
		{
			param.rc.f_rf_constant = 40;
		}
		else if (p_return.video_quality == 3)
		{
			param.rc.f_rf_constant = 35;
		}
		else if (p_return.video_quality == 4)
		{
			param.rc.f_rf_constant = 30;
		}
		else if (p_return.video_quality == 5)
		{
			param.rc.f_rf_constant = 25;
		}
		x264_param_apply_profile(&param, "baseline");
		
	}return 0;
}