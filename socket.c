#include "socket.h"
#pragma warning(disable:4996) // 屏蔽inet_addr()函数错误

TRAIN_NET_TO_PROGRAM g_train_to_program;
SIGNAL_TO_PROGRAM g_singal_to_program;
TRAIN_INFO g_train_info;

SOCKET clientSocket;

//SOCKET初始化
void SocketInit()
{
	int result;
	WSADATA wsadata;
	if (0 == WSAStartup(MAKEWORD(2, 2), &wsadata))
	{
		printf("客户端嵌套字已打开\n");
	}
	else
	{
		printf("客户端嵌套字打开失败\n");
	}
	clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN send_addr;
	send_addr.sin_family = AF_INET;
	send_addr.sin_port = htons(8888);
	send_addr.sin_addr.s_addr = inet_addr("192.168.187.10");

	result=connect(clientSocket, (SOCKADDR*)&send_addr, sizeof(SOCKADDR));
	if (result == 0)
	{
		printf("SOCKET连接服务器成功\n");
	}
	else
	{
		printf("SOCKET连接服务器失败\n");
	}
}

//SOCKET发送
void SocketSend(UINT8 send_buff[],UINT16 length)
{
	send(clientSocket, send_buff, length, 0);
}

//SOCKET接收
int SocketReceive()
{
    int receive_length = 0;
    UINT8 receive_buff[1024];
    receive_length = recv(clientSocket, (char *)receive_buff, 100, 0);
    if (receive_length>0)
    {
        UnpackMessageFromProgram(receive_buff);
    }
    return receive_length;
}

//SOCKET关闭
void SocketClose()
{
	closesocket(clientSocket);
	WSACleanup();
	printf("SOCKET关闭\n");
}

/*************************************************************************
  * 功能描述: 将2字节数据流变为UINT16
  * 输入参数: input 输入
  * 输出参数: 无
  * 返回值:   UINT16数据
  *************************************************************************/
UINT16 ShortFromChar(const UINT8 *input)
{
    UINT16 temp;
    temp = (*input);
    temp = (temp << 8) + (*(input + 1));
    return temp;
}

/*************************************************************************
 * 功能描述: 将4字节数据流变为UINT32
 * 输入参数: input 输入
 * 输出参数: 无
 * 返回值:   UINT32数据
 *************************************************************************/
UINT32 LongFromChar(const UINT8 *input)
{
    UINT32 temp;
    temp = (*input);
    temp = (temp << 8) + (*(input + 1));
    temp = (temp << 8) + (*(input + 2));
    temp = (temp << 8) + (*(input + 3));
    return temp;
}

/*************************************************************************
 * 功能描述: 将UINT16变为2字节数据流
 * 输入参数: input  UINT16数据
 * 输出参数: output 2字节数组
 * 返回值:   无
 *************************************************************************/
void ShortToChar(UINT16 input, UINT8 *output)
{
    *output = (UINT8)((input >> 8) & 0xFF);
    *(output + 1) = (UINT8)(input & 0xFF);
}

/*************************************************************************
 * 功能描述: 将UINT32变为4字节数据流
 * 输入参数: input  UINT32数据
 * 输出参数: output 4字节数组
 * 返回值:   无
 *************************************************************************/
void LongToChar(UINT32 input, UINT8 *output)
{
    *output = (UINT8)((input >> 24) & 0xFF);
    *(output + 1) = (UINT8)((input >> 16) & 0xFF);
    *(output + 2) = (UINT8)((input >> 8) & 0xFF);
    *(output + 3) = (UINT8)(input & 0xFF);
}

/*************************************************************************
 * 功能描述:更新车辆数据
 * 输入参数:无
 * 输出参数: 无
 * 返回值: 无 
 *************************************************************************/
void RefreshTrainData()
{
    g_train_to_program.weight = 338;
    g_train_to_program.formation_num = 6;
    g_train_to_program.train_length = 170;
    g_train_to_program.traction_voltage =g_train_info.traction_voltage;
    g_train_to_program.traction_current = g_train_info.traction_current;
    g_train_to_program.current_sign = g_train_info.current_sign;
    g_train_to_program.traction_fault_flag = 2;
    g_train_to_program.brake_fault_flag = 2;
    g_train_to_program.other_fault_flag = 2;

}


/*************************************************************************
 * 功能描述: 打包车辆发送给主程序的周期消息
 * 输入参数: UINT8    *send_buffer      消息存储指针
 * 输出参数: 无
 * 返回值:   UINT16   send_buffer_length    消息长度
 *************************************************************************/
UINT16 PackTrainPeriodMsgToProgram(UINT8 *send_buffer)
{
    RefreshTrainData();
    /*消息打包*/
    UINT16 index=0;
    ShortToChar(101, send_buffer + index);//打包消息类型
    index += 2;
    UINT16 message_length=21;
    ShortToChar(message_length,send_buffer+index);//打包消息长度
    index+=2;
    ShortToChar(g_train_to_program.weight, send_buffer + index);//打包载荷
    index += 2;
    send_buffer[index++] = g_train_to_program.formation_num;//打包编组数量
    ShortToChar(g_train_to_program.train_length, send_buffer + index);//打包车长
    index += 2;
    LongToChar(g_train_to_program.traction_voltage, send_buffer + index);//打包牵引机组输入电压
    index += 4;
    LongToChar(g_train_to_program.traction_current, send_buffer + index);//打包牵引机组输入电流
    index += 4;
    send_buffer[index++] = g_train_to_program.current_sign;//打包电流方向
    send_buffer[index++] = g_train_to_program.traction_fault_flag;//打包牵引故障标志
    send_buffer[index++] = g_train_to_program.brake_fault_flag;//打包制动故障标志
    send_buffer[index++] = g_train_to_program.other_fault_flag;//打包其他故障标志
    return index;
}

/*************************************************************************
 * 功能描述:更新信号系统数据
 * 输入参数:无
 * 输出参数: 无
 * 返回值: 无
 *************************************************************************/
void RefreshSingalData()
{
    g_singal_to_program.traction_energy = (UINT32)(100.0 * g_train_info.traction_energy);
    g_singal_to_program.regeneration_energy = (UINT32)(100.0 * g_train_info.regeneration_energy);
    g_singal_to_program.direction = g_train_info.direction;
    g_singal_to_program.train_id = 1001;
    g_singal_to_program.train_number = 68;
    g_singal_to_program.arrive_flag = g_train_info.arrive_flag;
    g_singal_to_program.leave_flag = g_train_info.leave_flag;
    g_singal_to_program.plan_change_flag = g_train_info.plan_change_flag;
    g_singal_to_program.ebi = (UINT16)(g_train_info.ebi*3.6);
    g_singal_to_program.speed = (UINT16)(g_train_info.speed*3.6);
    memcpy(g_singal_to_program.next_station_name, g_train_info.next_station_name, 20);//
    memcpy(g_singal_to_program.next_arrive_time, g_train_info.next_arrive_time, 20);//
    memcpy(g_singal_to_program.next_leave_time, g_train_info.next_leave_time, 20);//
    g_singal_to_program.level_flag = g_train_info.level_flag;
    g_singal_to_program.level_output = g_train_info.level_output;
    g_singal_to_program.distance = (UINT32)g_train_info.distance;
    memcpy(g_singal_to_program.current_time, g_train_info.current_time, 20);//
    g_singal_to_program.temporary_num = 0;
}

/*************************************************************************
 * 功能描述: 打包信号系统发送给主程序的周期消息
 * 输入参数: UINT8    *send_buffer      消息存储指针
 * 输出参数: 无
 * 返回值:   UINT16   send_buffer_length    消息长度
 *************************************************************************/
UINT16 PackSingalPeriodMsgToProgram(UINT8* send_buffer)
{
    RefreshSingalData();
    /*消息打包*/
    UINT16 index = 0;
    ShortToChar(102, send_buffer + index);//打包打包消息类型
    index += 2;
    UINT16 message_length = 116+10*g_singal_to_program.temporary_num;
    ShortToChar(message_length, send_buffer + index);//打包消息长度
    index += 2;
    LongToChar(g_singal_to_program.traction_energy, send_buffer + index);//打包牵引累积能耗
    index += 4;
    LongToChar(g_singal_to_program.regeneration_energy, send_buffer + index);//打包再生累积再生能量
    index += 4;
    send_buffer[index++] = g_singal_to_program.direction;//打包运行方向
    LongToChar(g_singal_to_program.train_id, send_buffer + index);//打包车次号
    index += 4;
    LongToChar(g_singal_to_program.train_number, send_buffer + index);//打包车组号
    index += 4;
    send_buffer[index++] = g_singal_to_program.arrive_flag;//打包停准停稳标识
    send_buffer[index++] = g_singal_to_program.leave_flag;//打包允许出发标识
    send_buffer[index++] = g_singal_to_program.plan_change_flag;//打包运行计划变更标识
    ShortToChar(g_singal_to_program.ebi, send_buffer + index);//打包ATP防护速度
    index += 2;
    ShortToChar(g_singal_to_program.speed, send_buffer + index);//打包列车速度
    index += 2;
    memcpy(send_buffer + index, g_singal_to_program.next_station_name, 20);//打包下一站名称
    index += 20;
    memcpy(send_buffer + index, g_singal_to_program.next_arrive_time, 20);//打包下一站到达时间
    index += 20;
    memcpy(send_buffer + index, g_singal_to_program.next_leave_time, 20);//打包下一站出发时间
    index += 20;
    send_buffer[index++] = g_singal_to_program.level_flag;//打包列车工况
    send_buffer[index++] = g_singal_to_program.level_output;//打包列车工况
    LongToChar(g_singal_to_program.distance, send_buffer + index);//打包列车公里标
    index += 4;
    memcpy(send_buffer + index, g_singal_to_program.current_time, 20);//打包列车时间
    index += 20;
    ShortToChar(g_singal_to_program.temporary_num, send_buffer + index);//打包临时限速数量
    index += 2;
    for (int i = 0; i < g_singal_to_program.temporary_num; i++)
    {
        LongToChar(g_singal_to_program.temporary_begin_distance[i], send_buffer + index);//打包临时限速起始公里标
        index += 4;
        LongToChar(g_singal_to_program.temporary_end_distance[i], send_buffer + index);//打包临时限速结束公里标
        index += 4;
        ShortToChar(g_singal_to_program.temporary_value[i], send_buffer + index);//打包临时限速
        index += 2;
    }
    return index;
}

void UnpackMessageFromProgram(UINT8* receive_buffer)
{
    UINT8* index = receive_buffer;
    UINT16 message_id = ShortFromChar(index);
    index += 2;
    UINT16 message_length = ShortFromChar(index);
    index += 2;
    if (message_id == 202)
    {
        g_train_info.target_speed = 1.0*ShortFromChar(index)/100;
        index += 2;
    }
  
}

UINT8 station_num = 9;
UINT8 station_id[9] = { 1,2,3,4,5,6,7,8,9 };
CHAR station_name[9][20] = {"金星站","黄石站","市五医院站","凤溪河站","温泉大道站","明光站","九江北站","白佛桥站","机投桥站" };
INT32 station_distance[9] = {0,4280,7243,9094,11132,13106,19747,23926,25394};

time_t now, arrive, leave;
UINT16 stop_count=0;

//折返管理
void ReverseManage()
{
	//如果列车运行方向处于下行，且到达最后一站，改变列车运行方向
	if (g_train_info.current_station_id == station_id[8] && g_train_info.direction == 0)
	{
		g_train_info.direction = 1;
		printf("列车折返，运行方向为上行\n");
	}
	//如果列车运行方向处于上行，且到达第一站，改变列车运行方向
	if (g_train_info.current_station_id == station_id[0] && g_train_info.direction == 1)
	{
		g_train_info.direction = 0;
		printf("列车折返，运行方向为下行\n");
	}
}

//更新下一站
void UpdateNextStation()
{
	UINT8 current_index = 0;
	//停车
	if (g_train_info.speed==0)
	{
		stop_count += 1;
		//如果列车停稳，更新运行计划,(执行3个周期)
		if (stop_count >= MAX_STOP_PERIOD&&stop_count< MAX_STOP_PERIOD+3)
		{
			//列车停稳，更新当前站、下一站
			//查询当前站编号
			for (int i = 0; i < station_num; i++)
			{
				if (g_train_info.distance >= station_distance[i] - MAX_PARKING_ACCURACY && g_train_info.distance <= station_distance[i] + MAX_PARKING_ACCURACY)
				{
					g_train_info.current_station_id = station_id[i];
					current_index = i;
					break;
				}
			}
			//根据方向查询下一站编号
			if (g_train_info.direction == 0)
			{
				//如果是下行方向，按照向上索引查找下一站
				g_train_info.next_station_id = (current_index != station_num) ? station_id[current_index + 1] : station_id[current_index];
			}
			else
			{
				//如果是上行方向，按照向下索引查找下一站
				g_train_info.next_station_id = (current_index != 0) ? station_id[current_index - 1] : station_id[current_index];
			}
			//折返管理
			ReverseManage();
			//更新列车运行计划
			g_train_info.plan_change_flag = 1;
			g_train_info.arrive_flag = 1;
			g_train_info.leave_flag = 1;
			printf("列车停稳，运行计划更新，当前站：%d,下一站：%d\n", g_train_info.current_station_id, g_train_info.next_station_id);

		}
	}
	//运行过程中
	else
	{
		stop_count = 0;
		g_train_info.plan_change_flag = 0;
		g_train_info.arrive_flag = 0;
		g_train_info.leave_flag = 0;
	}


}
//速度控制
void SpeedControl()
{
	FLOAT32 acc_actual;//实际输出加速度
	if (g_train_info.speed > g_train_info.target_speed + MAX_THRESHOLD)
	{
		g_train_info.level_flag = 3;//制动
		acc_actual = g_train_info.speed - g_train_info.target_speed;
		if (acc_actual > MAX_BARKE)
		{
			acc_actual = MAX_BARKE;
		}
		g_train_info.level_output = 100 * acc_actual / MAX_BARKE;
		g_train_info.speed = g_train_info.speed - acc_actual * PERIOD;
	}
	else if (g_train_info.speed < g_train_info.target_speed - MAX_THRESHOLD)
	{
		g_train_info.level_flag = 1;//牵引
		acc_actual = g_train_info.target_speed - g_train_info.speed;
		if (acc_actual > MAX_ACCELERATE)
		{
			acc_actual = MAX_ACCELERATE;
		}
		g_train_info.level_output = 100 * acc_actual / MAX_ACCELERATE;
		g_train_info.speed = g_train_info.speed + acc_actual * PERIOD;

	}
	else
	{
		g_train_info.level_flag = 2;//惰行
		g_train_info.level_output = 0;
		g_train_info.speed = g_train_info.speed - RESISTANCE_ACC * PERIOD;
	}
	if (g_train_info.speed < 0)
	{
		g_train_info.speed = 0;
	}
	//简单处理，让列车在制动阶段及时停下来
	if (g_train_info.target_speed == 0 && g_train_info.speed < MAX_THRESHOLD)
	{
		g_train_info.speed = 0;
	}
}


//列车周期主函数
void TrainPeriod()
{
	UpdateNextStation();//更新当前站、下一站
	SpeedControl();//根据推荐速度完成速度控制

	//如果列车运行方向是下行，则列车公里标逐渐累加
	if (g_train_info.direction == 0)
	{
		g_train_info.distance += g_train_info.speed * PERIOD;//计算累计走行
	}
	//如果列车运行方向是上行，则列车公里标逐渐减少
	else
	{
		g_train_info.distance -= g_train_info.speed * PERIOD;//计算累计走行
		if (g_train_info.distance < 0)
		{
			g_train_info.distance = 0;
		}
	}


    //根据工况，随机填充牵引电压电流
    g_train_info.traction_voltage = 1500 + rand() % 101;
    if (g_train_info.level_flag == 1)
    {
        g_train_info.traction_current = 100 + rand() % 101;
        g_train_info.current_sign = 1;
    }
    else if (g_train_info.level_flag == 3)
    {
        g_train_info.traction_current = 80 + rand() % 101;
        g_train_info.current_sign = 2;
    }
    else
    {
        g_train_info.traction_current = 0;
        g_train_info.current_sign = 2;
    }

    time(&now);
    if (g_train_info.leave_flag == 1)
    {
        arrive = now + 120;
        leave = now + 150;
    }

    //CHAR* strdata = "黄石站";
    //memcpy(g_train_info.next_station_name, strdata, sizeof(strdata));

    strftime(g_train_info.next_arrive_time, 20, "%Y-%m-%d %H:%M:%S", localtime(&arrive));
    strftime(g_train_info.next_leave_time, 20, "%Y-%m-%d %H:%M:%S", localtime(&leave));
    strftime(g_train_info.current_time, 20, "%Y-%m-%d %H:%M:%S", localtime(&now));
    printf("target_speed:%f,speed:%f,distance:%f,level_flag:%d,level_output:%d\n", g_train_info.target_speed, g_train_info.speed, g_train_info.distance, g_train_info.level_flag, g_train_info.level_output);
}