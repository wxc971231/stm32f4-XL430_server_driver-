/*
	ָ���ѭ������
		keil�Զ�ά����ָ��֧�ֺ���̫�ã������ýṹ��
		
	����ʽͨ��˵����
		�ڶ���ṹ��SERVERDATA����һ��busyԪ�أ��ڷ�������֡ʱ��1���ڽ���״̬֡ʱ��0
		����ָ��ʱ��ָ�����Ϊָ��֡�������У������readָ�ע��addr��lenҲҪ�棩
		tim3��ʱ���ж���10ms���ڼ�����ж����busy��־��ָ��ѭ�����У�������зǿ���busy=0������һ��ָ���
*/
#include "queue.h"
#include "server.h"

RRQUEUE queue[5];	//ָ������

//��������
void copyBuf(u8 *from,u8 *to,u8 len)
{
	for(int i=0;i<len;i++)
		to[i]=from[i];
}

//��ʼ������
void initQueue()
{	
	for(int i=1;i<=4;i++)
		queue[i].head=queue[i].tail=0;
}

//���һ��ָ��
void enQueue(RRQUEUE *q,u8 *buf,u8 len)
{
	u8 tailTemp=q->tail;
	
	tailTemp++;
	if(tailTemp == queueLen)
		tailTemp=0;
	
	if(tailTemp == q->head)	//����,�������
		return;
	
	copyBuf(buf,q->qinst[q->tail].buf,len);	//����ָ��֡
	
	switch(buf[7])
	{
		case Inst_Read:	
				q->qinst[q->tail].readAddr = dataAssmeble(buf[8],buf[9]);
				q->qinst[q->tail].readLen = dataAssmeble(buf[10],buf[11]);	break;
	}
	
	q->qinst[q->tail].instLen = len;				//��¼֡����						
	q->tail=tailTemp;
}

//����һ��ָ��
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
		case Inst_Read:		//�����readָ�����RWAddr��RWLen(�������ʱ�����)
				sd->RWInstPara.RWAddr = dataAssmeble(buf[8],buf[9]);
				sd->RWInstPara.RWLen = dataAssmeble(buf[10],buf[11]);	break;
	}
}

//�п�
u8 queueEmpty(RRQUEUE *q)
{
	u8 res = q->head == q->tail ? 1:0;
	return res;
}

