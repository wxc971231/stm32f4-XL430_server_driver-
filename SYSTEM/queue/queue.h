#ifndef __QUEUE_H
#define __QUEUE_H

#include "sys.h"

#define queueLen 10 		//���г���
#define INSTBUFLEN 30		//����֡��Buf����	

//һ��ָ�����Ԫ�أ�
typedef struct QINST 
{
	u8 buf[INSTBUFLEN];		//�洢����ָ��֡
	u16 readAddr;					//��ָ��ĸ�����Ϣ
	u16 readLen;
	u8 instLen;						//ָ���
}QINST;

//һ��ѭ������
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
