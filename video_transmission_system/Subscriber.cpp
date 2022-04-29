#include <ace/Log_Msg.h>

#include <dds/DdsDcpsInfrastructureC.h>
#include <dds/DdsDcpsSubscriptionC.h>

#include <dds/DCPS/Marked_Default_Qos.h>
#include <dds/DCPS/Service_Participant.h>
#include <dds/DCPS/WaitSet.h>

#include <dds/DCPS/StaticIncludes.h>
#ifdef ACE_AS_STATIC_LIBS
#  include <dds/DCPS/RTPS/RtpsDiscovery.h>
#  include <dds/DCPS/transport/rtps_udp/RtpsUdp.h>
#endif
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "DataReaderListenerImpl.h"
#include "msgTypeSupportImpl.h"
#include <process.h>
#include "OpenCV/include/opencv2/opencv.hpp"  
#define CVUI_IMPLEMENTATION
#include "Sub_show.h"
using namespace std;
extern int s_bre;
extern bool Reader_Reliable , Reader_Best , Reader_Qos_conf ;
int main(int argc, char *argv[]) {
	cout << "Publisher" << endl;
	ShowWindow(GetConsoleWindow(), SW_HIDE);
	DDS::DomainParticipantFactory_var dpf = TheParticipantFactoryWithArgs(argc, argv);
	DDS::DomainParticipant_var participant = dpf->create_participant(42, PARTICIPANT_QOS_DEFAULT, 0, OpenDDS::DCPS::DEFAULT_STATUS_MASK);
	if (!participant) {
		std::cerr << "creat_participant error" << std::endl;
		return 1;
	}

	//注册数据类型并创建主题
	msg::MessageTypeSupport_var mts = new msg::MessageTypeSupportImpl();
	if (DDS::RETCODE_OK != mts->register_type(participant, "")) {
		std::cerr << "regist error" << std::endl;
		return 1;
	}

	//cvui::init("name", 10);
	

	const char topic_tmp[20] = "Discussion List";
	//DDS::Duration_t timeout = { 4,0 };
	//DDS::Topic_var topic_find = participant->find_topic(topic_tmp, timeout);
	//cout << topic_find;

	CORBA::String_var type_name = mts->get_type_name();
	DDS::Topic_var topic1 =
		participant->create_topic("Video_Transmission_System",type_name,TOPIC_QOS_DEFAULT,0,OpenDDS::DCPS::DEFAULT_STATUS_MASK);
	if (!topic1) {
		std::cerr << "creat_topic error" << std::endl;
		return 1;
	}

	//topic_find = participant->find_topic(topic_tmp, timeout);
	//cout << topic_find;

	//创建订阅者
	DDS::Subscriber_var sub = participant->create_subscriber(SUBSCRIBER_QOS_DEFAULT, 0, OpenDDS::DCPS::DEFAULT_STATUS_MASK);
	if (!sub) {
		std::cerr << "creat_suscriber error" << std::endl;
		return 1;
	}
//======PROCESS=======================================================================
	_beginthread(gui_sub, 0, NULL);
	_beginthread(Image_show, 0, NULL);
	_beginthread(time_show, 0, NULL);
//======PROCESS=======================================================================
	//创建数据 阅读者 和 侦听者	dr=data reader
	DDS::DataReaderListener_var listener(new DataReaderListenerImpl);

	DDS::DataReaderQos Dr;
	sub->get_default_datareader_qos(Dr);
	while (Reader_Qos_conf == false) {
		if (s_bre)
			break;
	}
	if(!s_bre)
	{ 
	if (Reader_Reliable)
	{
		Dr.reliability.kind = DDS::RELIABLE_RELIABILITY_QOS;	//可靠
	}
	else if (Reader_Best)
	{
		Dr.reliability.kind = DDS::BEST_EFFORT_RELIABILITY_QOS;	//最大努力
	}
	Dr.reliability.max_blocking_time.sec = 10;				//每个可靠类型的阻塞时间为10s;
	Dr.reliability.max_blocking_time.nanosec = 0;			//纳秒=0？
	//Dr.resource_limits.max_samples_per_instance = 100;		//设置每个实例样本数为100

	DDS::DataReader_var dr = sub->create_datareader(topic1, DATAREADER_QOS_DEFAULT, listener, OpenDDS::DCPS::DEFAULT_STATUS_MASK);
	if (!dr) {
		std::cerr << "creat_DataReader error" << std::endl;
		return 1;
	}

	////阻塞15s，确认订阅者收到所有写入的数据
	DDS::StatusCondition_var condition = dr->get_statuscondition();
	condition->set_enabled_statuses(DDS::SUBSCRIPTION_MATCHED_STATUS);

	DDS::WaitSet_var ws = new DDS::WaitSet;
	ws->attach_condition(condition); 
	while (true) 
	{
		DDS::SubscriptionMatchedStatus matches;
		if (dr->get_subscription_matched_status(matches) != DDS::RETCODE_OK) {
			std::cerr << "block wait error1" << std::endl;
		}
		if (matches.current_count == 0 && matches.total_count > 0) {
			break;
		}

		DDS::ConditionSeq conditions;
		DDS::Duration_t timeout = { 60, 0 };
		if (ws->wait(conditions, timeout) != DDS::RETCODE_OK) {
			std::cerr << "block wait error2" << std::endl;
		}
		if (s_bre == -1)
		{
			s_bre = 0;
			cv::destroyAllWindows();

			break;
		}
	}ws->detach_condition(condition);
	}
	//crear all
	participant->delete_contained_entities();
	dpf->delete_participant(participant);
	TheServiceParticipant->shutdown();
}
