#include "socket.h"

int main()
{
    int result;
	SocketInit();//socket初始化
	UINT8 send_buff[1024];
	UINT16 send_length;
    //列车初始位置
	g_train_info.direction = 0;//下行
	g_train_info.distance = 0;
	g_train_info.speed = 0;
	g_train_info.current_station_id = 1;
	g_train_info.plan_change_flag = 1;

	send_length = PackTrainPeriodMsgToProgram(send_buff);//打包车辆网络发送给主程序的消息
	SocketSend(send_buff, send_length);
	Sleep(50);//两次发送需要间隔一定时间
	send_length = PackSingalPeriodMsgToProgram(send_buff);//打包信号系统发送给主程序的消息
	SocketSend(send_buff, send_length);

	while (1)
	{
		result=SocketReceive();//如果未收到消息保持阻塞
		if (result < 0)
		{
			printf("通信异常断开连接\n");
			break;
		}

		TrainPeriod();//列车周期处理
		send_length = PackTrainPeriodMsgToProgram(send_buff);//打包车辆网络发送给主程序的消息
		SocketSend(send_buff, send_length);
		Sleep(50);//两次发送需要间隔一定时间
		send_length = PackSingalPeriodMsgToProgram(send_buff);//打包信号系统发送给主程序的消息
		SocketSend(send_buff, send_length);
	}

	SocketClose();

    return 1;
}