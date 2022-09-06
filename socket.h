#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "define.h"
#include <WinSock2.h>
#include <Windows.h>
#pragma comment(lib, "ws2_32.lib")  //���� ws2_32.dll

extern TRAIN_NET_TO_PROGRAM g_train_to_program;
extern SIGNAL_TO_PROGRAM g_singal_to_program;
extern TRAIN_INFO g_train_info;
/*************************************************************************
  * ��������: ��2�ֽ���������ΪUINT16
  * �������: input ����
  * �������: ��
  * ����ֵ:   UINT16����
  *************************************************************************/
UINT16 ShortFromChar(const UINT8 *input);

/*************************************************************************
 * ��������: ��4�ֽ���������ΪUINT32
 * �������: input ����
 * �������: ��
 * ����ֵ:   UINT32����
 *************************************************************************/
UINT32 LongFromChar(const UINT8 *input);

/*************************************************************************
 * ��������: ��UINT16��Ϊ2�ֽ�������
 * �������: input  UINT16����
 * �������: output 2�ֽ�����
 * ����ֵ:   ��
 *************************************************************************/
void ShortToChar(UINT16 input, UINT8 *output);

/*************************************************************************
 * ��������: ��UINT32��Ϊ4�ֽ�������
 * �������: input  UINT32����
 * �������: output 4�ֽ�����
 * ����ֵ:   ��
 *************************************************************************/
void LongToChar(UINT32 input, UINT8 *output);


void SocketInit();

void SocketSend(UINT8 send_buff[], UINT16 length);

int SocketReceive();


void SocketClose();

void UnpackMessageFromProgram(UINT8* receive_buffer);

void TrainPeriod();