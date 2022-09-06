#pragma once

#include <string.h>
#include "stdio.h"
#include <time.h>
/* �����������Ͷ��� */
typedef char                CHAR;
typedef unsigned char       UCHAR;
typedef signed char         INT8;
typedef unsigned char       UINT8;
typedef short int           INT16;
typedef unsigned short int  UINT16;
typedef int                 INT32;
typedef unsigned int        UINT32;
typedef float               FLOAT32;
typedef double              FLOAT64;

#define MAX_ACCELERATE 1.4  //�����ٶ� 1.4m/s^2
#define MAX_BARKE 1.2      //�����ٶ� 1.2m/s^2
#define RESISTANCE_ACC 0.1  //���м��ٶ�
#define MAX_THRESHOLD 1   //��ֵ 1m/s
#define PERIOD 0.2 //200ms
#define MAX_PARKING_ACCURACY 200 //���ͣ������
#define MAX_STOP_PERIOD 10 //ͣ������ʱ�� 2s
/*�г����ݽṹ��*/
typedef struct
{
	//��������
	FLOAT32 target_speed;  // m/s

	//�������
	FLOAT32 distance;      //m
	FLOAT32 speed;         // m/s
	FLOAT32 ebi;           // m/s
	UINT8 direction;
	UINT8 level_flag;
	UINT8 level_output;
	UINT32 traction_voltage;
	UINT32 traction_current;
	UINT8 current_sign;
	FLOAT32 traction_energy;
	FLOAT32 regeneration_energy;

	UINT8 arrive_flag;
	UINT8 leave_flag;
	UINT8 plan_change_flag;
	CHAR next_station_name[20];
	CHAR next_arrive_time[20];
	CHAR next_leave_time[20];
	CHAR current_time[20];
	UINT8 current_station_id;
	UINT8 next_station_id;
}TRAIN_INFO;

/*�������緢�͸���������������ݽṹ��*/
typedef struct
{
	UINT16 weight;       //t
	UINT8 formation_num;
	UINT16 train_length;  //m
	UINT32 traction_voltage; 
	UINT32 traction_current;
	UINT8 current_sign;
	UINT8 traction_fault_flag;
	UINT8 brake_fault_flag;
	UINT8 other_fault_flag;
}TRAIN_NET_TO_PROGRAM;

/*�ź�ϵͳ���͸���������������ݽṹ��*/
typedef struct
{
	UINT32 traction_energy;
	UINT32 regeneration_energy;
	UINT8 direction;
	UINT32 train_id;
	UINT32 train_number;
	UINT8 arrive_flag;
	UINT8 leave_flag;
	UINT8 plan_change_flag;
	UINT16 ebi;
	UINT16 speed;
	CHAR next_station_name[20];
	CHAR next_arrive_time[20];
	CHAR next_leave_time[20];
	UINT8 level_flag;
	UINT8 level_output;
	UINT32 distance;
	CHAR current_time[20];
	UINT16 temporary_num;
	UINT32 temporary_begin_distance[20];
	UINT32 temporary_end_distance[20];
	UINT32 temporary_value[20];
}SIGNAL_TO_PROGRAM;