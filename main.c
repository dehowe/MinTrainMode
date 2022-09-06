#include "socket.h"

int main()
{
    int result;
	SocketInit();//socket��ʼ��
	UINT8 send_buff[1024];
	UINT16 send_length;
    //�г���ʼλ��
	g_train_info.direction = 0;//����
	g_train_info.distance = 0;
	g_train_info.speed = 0;
	g_train_info.current_station_id = 1;
	g_train_info.plan_change_flag = 1;

	send_length = PackTrainPeriodMsgToProgram(send_buff);//����������緢�͸����������Ϣ
	SocketSend(send_buff, send_length);
	Sleep(50);//���η�����Ҫ���һ��ʱ��
	send_length = PackSingalPeriodMsgToProgram(send_buff);//����ź�ϵͳ���͸����������Ϣ
	SocketSend(send_buff, send_length);

	while (1)
	{
		result=SocketReceive();//���δ�յ���Ϣ��������
		if (result < 0)
		{
			printf("ͨ���쳣�Ͽ�����\n");
			break;
		}

		TrainPeriod();//�г����ڴ���
		send_length = PackTrainPeriodMsgToProgram(send_buff);//����������緢�͸����������Ϣ
		SocketSend(send_buff, send_length);
		Sleep(50);//���η�����Ҫ���һ��ʱ��
		send_length = PackSingalPeriodMsgToProgram(send_buff);//����ź�ϵͳ���͸����������Ϣ
		SocketSend(send_buff, send_length);
	}

	SocketClose();

    return 1;
}