#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "define.h"
#include <WinSock2.h>
#include <Windows.h>
#pragma comment(lib, "ws2_32.lib")  //加载 ws2_32.dll

extern TRAIN_NET_TO_PROGRAM g_train_to_program;
extern SIGNAL_TO_PROGRAM g_singal_to_program;
extern TRAIN_INFO g_train_info;
/*************************************************************************
  * 功能描述: 将2字节数据流变为UINT16
  * 输入参数: input 输入
  * 输出参数: 无
  * 返回值:   UINT16数据
  *************************************************************************/
UINT16 ShortFromChar(const UINT8 *input);

/*************************************************************************
 * 功能描述: 将4字节数据流变为UINT32
 * 输入参数: input 输入
 * 输出参数: 无
 * 返回值:   UINT32数据
 *************************************************************************/
UINT32 LongFromChar(const UINT8 *input);

/*************************************************************************
 * 功能描述: 将UINT16变为2字节数据流
 * 输入参数: input  UINT16数据
 * 输出参数: output 2字节数组
 * 返回值:   无
 *************************************************************************/
void ShortToChar(UINT16 input, UINT8 *output);

/*************************************************************************
 * 功能描述: 将UINT32变为4字节数据流
 * 输入参数: input  UINT32数据
 * 输出参数: output 4字节数组
 * 返回值:   无
 *************************************************************************/
void LongToChar(UINT32 input, UINT8 *output);


void SocketInit();

void SocketSend(UINT8 send_buff[], UINT16 length);

int SocketReceive();


void SocketClose();

void UnpackMessageFromProgram(UINT8* receive_buffer);

void TrainPeriod();