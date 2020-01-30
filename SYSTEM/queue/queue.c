/*
	指令缓冲循环队列
		keil对多维数组指针支持好像不太好，这里用结构体
		
	阻塞式通信说明：
		在舵机结构体SERVERDATA中有一个busy元素，在发送命令帧时置1，在解码状态帧时清0
		发送指令时，指令编码为指令帧后存入队列（如果是read指令，注意addr和len也要存）
		tim3定时器中断以10ms周期检查所有舵机的busy标志和指令循环队列，如果队列非空且busy=0，出队一个指令发送
*/
#include "queue.h"
#include "server.h"

RRQUEUE queue[5];	//指令缓冲队列

//复制数组
void copyBuf(u8 *from,u8 *to,u8 len)
{
	for(int i=0;i<len;i++)
		to[i]=from[i];
}

//初始化队列
void initQueue()
{	
	for(int i=1;i<=4;i++)
		queue[i].head=queue[i].tail=0;
}

//入队一条指令
void enQueue(RRQUEUE *q,u8 *buf,u8 len)
{
	u8 tailTemp=q->tail;
	
	tailTemp++;
	if(tailTemp == queueLen)
		tailTemp=0;
	
	if(tailTemp == q->head)	//满了,不能入队
		return;
	
	copyBuf(buf,q->qinst[q->tail].buf,len);	//复制指令帧
	
	switch(buf[7])
	{
		case Inst_Read:	
				q->qinst[q->tail].readAddr = dataAssmeble(buf[8],buf[9]);
				q->qinst[q->tail].readLen = dataAssmeble(buf[10],buf[11]);	break;
	}
	
	q->qinst[q->tail].instLen = len;				//记录帧长度						
	q->tail=tailTemp;
}

//出队一条指令
void deQueue(SERVERDATA *sd,u8 *buf,u8 len)
{
	RRQUEUE *q = sd->rrQueue;
	copyBuf(q->qinst[q->head].buf,buf,len);
	
	u8 headTemp=q->head;
	headTemp++;
	if(headTemp==queueLen)
		headTemp=0;
	
	q->head=headTemp;
	
	switch(buf[7])
	{
		case Inst_Read:		//如果是read指令，更新RWAddr和RWLen(否则解码时会出错)
				sd->RWInstPara.RWAddr = dataAssmeble(buf[8],buf[9]);
				sd->RWInstPara.RWLen = dataAssmeble(buf[10],buf[11]);	break;
	}
}

//判空
u8 queueEmpty(RRQUEUE *q)
{
	u8 res = q->head == q->tail ? 1:0;
	return res;
}

