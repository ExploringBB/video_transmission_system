/*
 *
 *
 * Distributed under the OpenDDS License.
 * See: http://www.opendds.org/license.html
 */

#include <ace/Log_Msg.h>
#include <ace/OS_NS_stdlib.h>

#include "DataReaderListenerImpl.h"
#include "msgTypeSupportC.h"
#include "msgTypeSupportImpl.h"

#include <iostream>
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include <opencv2/core/hal/interface.h>
#include <opencv2/opencv.hpp>  
#include "x264_Encode.hpp"
using namespace cv;
using namespace msg;
using namespace std;
string text_message = "";
void
DataReaderListenerImpl::on_requested_deadline_missed(
	DDS::DataReader_ptr /*reader*/,
	const DDS::RequestedDeadlineMissedStatus& /*status*/)
{
}

void
DataReaderListenerImpl::on_requested_incompatible_qos(
	DDS::DataReader_ptr /*reader*/,
	const DDS::RequestedIncompatibleQosStatus& /*status*/)
{
}

void
DataReaderListenerImpl::on_sample_rejected(
	DDS::DataReader_ptr /*reader*/,
	const DDS::SampleRejectedStatus& /*status*/)
{
}

void
DataReaderListenerImpl::on_liveliness_changed(
	DDS::DataReader_ptr /*reader*/,
	const DDS::LivelinessChangedStatus& /*status*/)
{
}

void
DataReaderListenerImpl::on_data_available(DDS::DataReader_ptr reader)
{
	msg::MessageDataReader_var reader_i =		//����һ������read��Ϣ�ı���������ΪMessageDataReader_var��
		msg::MessageDataReader::_narrow(reader);

	if (!reader_i) {
		ACE_ERROR((LM_ERROR,
			ACE_TEXT("ERROR: %N:%l: on_data_available() -")
			ACE_TEXT(" _narrow failed!\n")));
		ACE_OS::exit(1);
	}
	//����2���ؼ���
	msg::Message message;
	DDS::SampleInfo info;

	const DDS::ReturnCode_t error = reader_i->take_next_sample(message, info);	//ȡ����Ϣ���ݴ���
	//Message�ṹ���У���idl��ʼ���壩�����Ҵ�����ֵerror���룬ΪRETCODE_OK���ʾ�޴���,ΪRETCODE_ERROR
	if(false)
	{ 
		if (error == DDS::RETCODE_OK) {
			std::cout << "SampleInfo.sample_rank = " << info.sample_rank << std::endl;//��Ϣ��Ч�ԣ�
			std::cout << "SampleInfo.instance_state = "
				<< OpenDDS::DCPS::InstanceState::instance_state_mask_string(info.instance_state) << std::endl;
			//��Ϣʵʱ״̬
			if (info.valid_data) {
				std::cout << "Message: subject    = " << message.subject.in() << std::endl
					<< "         subject_id = " << message.subject_id << std::endl
					//<< "         from       = " << message.from.in() << std::endl
					<< "         count      = " << message.count << std::endl
					<< "         text       = " << message.text.in() << std::endl;
				video_decode(message.img, message.count);	//������
			}
		}
		else {
			ACE_ERROR((LM_ERROR,
				ACE_TEXT("ERROR: %N:%l: on_data_available() -")
				ACE_TEXT(" take_next_sample failed!\n")));
		}
	}
	if (info.valid_data)
	{
	////pre_time���
	//	static bool once = true;
	//	if (once)	
	//	{
	//		pre_start_ms = message.pre_time;
	//		SYSTEMTIME pre_end;
	//		GetLocalTime(&pre_end);
	//		pre_end_ms = (pre_end.wMinute * 60 + pre_end.wSecond) * 1000 + pre_end.wMilliseconds;
	//		once = false;
	//	}
		video_decode(message.img, message.count);
	////����ʱ����
	//	start_ms = message.time_num;	
	//	static SYSTEMTIME end_now;
	//	GetLocalTime(&end_now);
	//	end_ms_now = (end_now.wMinute * 60 + end_now.wSecond) * 1000 + end_now.wMilliseconds;
		

		if (message.subject != "")//����text��Ϣ
		{
			text_message = message.subject;
		}
		

	}
}

void
DataReaderListenerImpl::on_subscription_matched(
	DDS::DataReader_ptr /*reader*/,
	const DDS::SubscriptionMatchedStatus& /*status*/)
{
}

void
DataReaderListenerImpl::on_sample_lost(
	DDS::DataReader_ptr /*reader*/,
	const DDS::SampleLostStatus& /*status*/)
{
}
