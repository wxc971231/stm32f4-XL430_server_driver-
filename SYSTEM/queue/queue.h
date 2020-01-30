#ifndef __QUEUE_H
#define __QUEUE_H

#include "sys.h"

#define queueLen 10 		//队列长度
#define INSTBUFLEN 30		//发送帧缓Buf长度	

//一条指令（队列元素）
typedef struct QINST 
{
	u8 buf[INSTBUFLEN];		//存储整个指令帧
	u16 readAddr;					//读指令的附加信息
	u16 readLen;
	u8 instLen;						//指令长度
}QINST;

//一个循环队列
typedef struct RRQUEUE
{
	u8 head;
	u8 tail;
	QINST qinst[queueLen];
}RRQUEUE;
	
extern RRQUEUE queue[5];

void copyBuf(u8 *from,u8 *to,u8 len);
void initQueue(void);
void enQueue(RRQUEUE *q,u8 *buf,u8 len);

u8 queueEmpty(RRQUEUE *q);

#endif 
