#ifndef __XIEYI_H
#define __XIEYI_H	

#include <Arduino.h>
//#include "usart.h"
//2020年9月13日 12:29:24 改写和优化了指令解释，去除了返回参数，使指令增加了一位有效参数位。

//#define tab 7  //不定义会函数表溢出
#define DEBUG 0//调试信息输出

#define CMD_long 11 //指令总长度（更改长度后要增删EXE函数的函数形式）
#define CMD_data CMD_long-3//参数个数 去除0xaa cmd 0xbb

#define num 20//接收缓冲区长度



// uint8_t init_xieyi(void);
void init_xieyi(uint8_t *p);
//函数表
typedef struct 
{
    void* pf;			   //函数指针
 	uint8_t cmd[CMD_long]; //通讯指令格式
}NAME_TAB;
extern NAME_TAB name_tab[];
//函数数据表
typedef struct 
{
    NAME_TAB *pfune;
	 uint8_t(*exe)(void);
	 uint8_t ID;//函数表中的位置uint8_t
	 uint8_t CMD;//指令
	 uint8_t GS;//参数个数
	 uint8_t pra[CMD_data];//函数参数分布
	 uint32_t cs[CMD_data];//函数实际参数
}NAME_par;
extern NAME_par M_name_par;




void xieyi_scan(void);
extern uint8_t exe(void);//外部不调用，只提供给xieyi_config.c使用

#endif

















