#include "socket.h"
#pragma warning(disable:4996) // ����inet_addr()��������

TRAIN_NET_TO_PROGRAM g_train_to_program;
SIGNAL_TO_PROGRAM g_singal_to_program;
TRAIN_INFO g_train_info;

SOCKET clientSocket;

//SOCKET��ʼ��
void SocketInit()
{
	int result;
	WSADATA wsadata;
	if (0 == WSAStartup(MAKEWORD(2, 2), &wsadata))
	{
		printf("�ͻ���Ƕ�����Ѵ�\n");
	}
	else
	{
		printf("�ͻ���Ƕ���ִ�ʧ��\n");
	}
	clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN send_addr;
	send_addr.sin_family = AF_INET;
	send_addr.sin_port = htons(8888);
	send_addr.sin_addr.s_addr = inet_addr("192.168.187.10");

	result=connect(clientSocket, (SOCKADDR*)&send_addr, sizeof(SOCKADDR));
	if (result == 0)
	{
		printf("SOCKET���ӷ������ɹ�\n");
	}
	else
	{
		printf("SOCKET���ӷ�����ʧ��\n");
	}
}

//SOCKET����
void SocketSend(UINT8 send_buff[],UINT16 length)
{
	send(clientSocket, send_buff, length, 0);
}

//SOCKET����
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

//SOCKET�ر�
void SocketClose()
{
	closesocket(clientSocket);
	WSACleanup();
	printf("SOCKET�ر�\n");
}

/*************************************************************************
  * ��������: ��2�ֽ���������ΪUINT16
  * �������: input ����
  * �������: ��
  * ����ֵ:   UINT16����
  *************************************************************************/
UINT16 ShortFromChar(const UINT8 *input)
{
    UINT16 temp;
    temp = (*input);
    temp = (temp << 8) + (*(input + 1));
    return temp;
}

/*************************************************************************
 * ��������: ��4�ֽ���������ΪUINT32
 * �������: input ����
 * �������: ��
 * ����ֵ:   UINT32����
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
 * ��������: ��UINT16��Ϊ2�ֽ�������
 * �������: input  UINT16����
 * �������: output 2�ֽ�����
 * ����ֵ:   ��
 *************************************************************************/
void ShortToChar(UINT16 input, UINT8 *output)
{
    *output = (UINT8)((input >> 8) & 0xFF);
    *(output + 1) = (UINT8)(input & 0xFF);
}

/*************************************************************************
 * ��������: ��UINT32��Ϊ4�ֽ�������
 * �������: input  UINT32����
 * �������: output 4�ֽ�����
 * ����ֵ:   ��
 *************************************************************************/
void LongToChar(UINT32 input, UINT8 *output)
{
    *output = (UINT8)((input >> 24) & 0xFF);
    *(output + 1) = (UINT8)((input >> 16) & 0xFF);
    *(output + 2) = (UINT8)((input >> 8) & 0xFF);
    *(output + 3) = (UINT8)(input & 0xFF);
}

/*************************************************************************
 * ��������:���³�������
 * �������:��
 * �������: ��
 * ����ֵ: �� 
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
 * ��������: ����������͸��������������Ϣ
 * �������: UINT8    *send_buffer      ��Ϣ�洢ָ��
 * �������: ��
 * ����ֵ:   UINT16   send_buffer_length    ��Ϣ����
 *************************************************************************/
UINT16 PackTrainPeriodMsgToProgram(UINT8 *send_buffer)
{
    RefreshTrainData();
    /*��Ϣ���*/
    UINT16 index=0;
    ShortToChar(101, send_buffer + index);//�����Ϣ����
    index += 2;
    UINT16 message_length=21;
    ShortToChar(message_length,send_buffer+index);//�����Ϣ����
    index+=2;
    ShortToChar(g_train_to_program.weight, send_buffer + index);//����غ�
    index += 2;
    send_buffer[index++] = g_train_to_program.formation_num;//�����������
    ShortToChar(g_train_to_program.train_length, send_buffer + index);//�������
    index += 2;
    LongToChar(g_train_to_program.traction_voltage, send_buffer + index);//���ǣ�����������ѹ
    index += 4;
    LongToChar(g_train_to_program.traction_current, send_buffer + index);//���ǣ�������������
    index += 4;
    send_buffer[index++] = g_train_to_program.current_sign;//�����������
    send_buffer[index++] = g_train_to_program.traction_fault_flag;//���ǣ�����ϱ�־
    send_buffer[index++] = g_train_to_program.brake_fault_flag;//����ƶ����ϱ�־
    send_buffer[index++] = g_train_to_program.other_fault_flag;//����������ϱ�־
    return index;
}

/*************************************************************************
 * ��������:�����ź�ϵͳ����
 * �������:��
 * �������: ��
 * ����ֵ: ��
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
 * ��������: ����ź�ϵͳ���͸��������������Ϣ
 * �������: UINT8    *send_buffer      ��Ϣ�洢ָ��
 * �������: ��
 * ����ֵ:   UINT16   send_buffer_length    ��Ϣ����
 *************************************************************************/
UINT16 PackSingalPeriodMsgToProgram(UINT8* send_buffer)
{
    RefreshSingalData();
    /*��Ϣ���*/
    UINT16 index = 0;
    ShortToChar(102, send_buffer + index);//��������Ϣ����
    index += 2;
    UINT16 message_length = 116+10*g_singal_to_program.temporary_num;
    ShortToChar(message_length, send_buffer + index);//�����Ϣ����
    index += 2;
    LongToChar(g_singal_to_program.traction_energy, send_buffer + index);//���ǣ���ۻ��ܺ�
    index += 4;
    LongToChar(g_singal_to_program.regeneration_energy, send_buffer + index);//��������ۻ���������
    index += 4;
    send_buffer[index++] = g_singal_to_program.direction;//������з���
    LongToChar(g_singal_to_program.train_id, send_buffer + index);//������κ�
    index += 4;
    LongToChar(g_singal_to_program.train_number, send_buffer + index);//��������
    index += 4;
    send_buffer[index++] = g_singal_to_program.arrive_flag;//���ͣ׼ͣ�ȱ�ʶ
    send_buffer[index++] = g_singal_to_program.leave_flag;//������������ʶ
    send_buffer[index++] = g_singal_to_program.plan_change_flag;//������мƻ������ʶ
    ShortToChar(g_singal_to_program.ebi, send_buffer + index);//���ATP�����ٶ�
    index += 2;
    ShortToChar(g_singal_to_program.speed, send_buffer + index);//����г��ٶ�
    index += 2;
    memcpy(send_buffer + index, g_singal_to_program.next_station_name, 20);//�����һվ����
    index += 20;
    memcpy(send_buffer + index, g_singal_to_program.next_arrive_time, 20);//�����һվ����ʱ��
    index += 20;
    memcpy(send_buffer + index, g_singal_to_program.next_leave_time, 20);//�����һվ����ʱ��
    index += 20;
    send_buffer[index++] = g_singal_to_program.level_flag;//����г�����
    send_buffer[index++] = g_singal_to_program.level_output;//����г�����
    LongToChar(g_singal_to_program.distance, send_buffer + index);//����г������
    index += 4;
    memcpy(send_buffer + index, g_singal_to_program.current_time, 20);//����г�ʱ��
    index += 20;
    ShortToChar(g_singal_to_program.temporary_num, send_buffer + index);//�����ʱ��������
    index += 2;
    for (int i = 0; i < g_singal_to_program.temporary_num; i++)
    {
        LongToChar(g_singal_to_program.temporary_begin_distance[i], send_buffer + index);//�����ʱ������ʼ�����
        index += 4;
        LongToChar(g_singal_to_program.temporary_end_distance[i], send_buffer + index);//�����ʱ���ٽ��������
        index += 4;
        ShortToChar(g_singal_to_program.temporary_value[i], send_buffer + index);//�����ʱ����
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
CHAR station_name[9][20] = {"����վ","��ʯվ","����ҽԺվ","��Ϫ��վ","��Ȫ���վ","����վ","�Ž���վ","�׷���վ","��Ͷ��վ" };
INT32 station_distance[9] = {0,4280,7243,9094,11132,13106,19747,23926,25394};

time_t now, arrive, leave;
UINT16 stop_count=0;

//�۷�����
void ReverseManage()
{
	//����г����з��������У��ҵ������һվ���ı��г����з���
	if (g_train_info.current_station_id == station_id[8] && g_train_info.direction == 0)
	{
		g_train_info.direction = 1;
		printf("�г��۷������з���Ϊ����\n");
	}
	//����г����з��������У��ҵ����һվ���ı��г����з���
	if (g_train_info.current_station_id == station_id[0] && g_train_info.direction == 1)
	{
		g_train_info.direction = 0;
		printf("�г��۷������з���Ϊ����\n");
	}
}

//������һվ
void UpdateNextStation()
{
	UINT8 current_index = 0;
	//ͣ��
	if (g_train_info.speed==0)
	{
		stop_count += 1;
		//����г�ͣ�ȣ��������мƻ�,(ִ��3������)
		if (stop_count >= MAX_STOP_PERIOD&&stop_count< MAX_STOP_PERIOD+3)
		{
			//�г�ͣ�ȣ����µ�ǰվ����һվ
			//��ѯ��ǰվ���
			for (int i = 0; i < station_num; i++)
			{
				if (g_train_info.distance >= station_distance[i] - MAX_PARKING_ACCURACY && g_train_info.distance <= station_distance[i] + MAX_PARKING_ACCURACY)
				{
					g_train_info.current_station_id = station_id[i];
					current_index = i;
					break;
				}
			}
			//���ݷ����ѯ��һվ���
			if (g_train_info.direction == 0)
			{
				//��������з��򣬰�����������������һվ
				g_train_info.next_station_id = (current_index != station_num) ? station_id[current_index + 1] : station_id[current_index];
			}
			else
			{
				//��������з��򣬰�����������������һվ
				g_train_info.next_station_id = (current_index != 0) ? station_id[current_index - 1] : station_id[current_index];
			}
			//�۷�����
			ReverseManage();
			//�����г����мƻ�
			g_train_info.plan_change_flag = 1;
			g_train_info.arrive_flag = 1;
			g_train_info.leave_flag = 1;
			printf("�г�ͣ�ȣ����мƻ����£���ǰվ��%d,��һվ��%d\n", g_train_info.current_station_id, g_train_info.next_station_id);

		}
	}
	//���й�����
	else
	{
		stop_count = 0;
		g_train_info.plan_change_flag = 0;
		g_train_info.arrive_flag = 0;
		g_train_info.leave_flag = 0;
	}


}
//�ٶȿ���
void SpeedControl()
{
	FLOAT32 acc_actual;//ʵ��������ٶ�
	if (g_train_info.speed > g_train_info.target_speed + MAX_THRESHOLD)
	{
		g_train_info.level_flag = 3;//�ƶ�
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
		g_train_info.level_flag = 1;//ǣ��
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
		g_train_info.level_flag = 2;//����
		g_train_info.level_output = 0;
		g_train_info.speed = g_train_info.speed - RESISTANCE_ACC * PERIOD;
	}
	if (g_train_info.speed < 0)
	{
		g_train_info.speed = 0;
	}
	//�򵥴������г����ƶ��׶μ�ʱͣ����
	if (g_train_info.target_speed == 0 && g_train_info.speed < MAX_THRESHOLD)
	{
		g_train_info.speed = 0;
	}
}


//�г�����������
void TrainPeriod()
{
	UpdateNextStation();//���µ�ǰվ����һվ
	SpeedControl();//�����Ƽ��ٶ�����ٶȿ���

	//����г����з��������У����г���������ۼ�
	if (g_train_info.direction == 0)
	{
		g_train_info.distance += g_train_info.speed * PERIOD;//�����ۼ�����
	}
	//����г����з��������У����г�������𽥼���
	else
	{
		g_train_info.distance -= g_train_info.speed * PERIOD;//�����ۼ�����
		if (g_train_info.distance < 0)
		{
			g_train_info.distance = 0;
		}
	}


    //���ݹ�����������ǣ����ѹ����
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

    //CHAR* strdata = "��ʯվ";
    //memcpy(g_train_info.next_station_name, strdata, sizeof(strdata));

    strftime(g_train_info.next_arrive_time, 20, "%Y-%m-%d %H:%M:%S", localtime(&arrive));
    strftime(g_train_info.next_leave_time, 20, "%Y-%m-%d %H:%M:%S", localtime(&leave));
    strftime(g_train_info.current_time, 20, "%Y-%m-%d %H:%M:%S", localtime(&now));
    printf("target_speed:%f,speed:%f,distance:%f,level_flag:%d,level_output:%d\n", g_train_info.target_speed, g_train_info.speed, g_train_info.distance, g_train_info.level_flag, g_train_info.level_output);
}