/*
XL430-W250����������� 2019/1/29 	v1.0
	�� �ٷ��ĵ���
			�� ͨ��Э�飺				http://emanual.robotis.com/docs/en/dxl/protocol2/#status-checksum
			�� ����ڿ��Ʊ�ṹ��http://emanual.robotis.com/docs/en/dxl/x/xl430-w250/#registered-instruction
	
	�� �ļ�˵����
			�� server.c			�ṩ���ͨ�����ݱ���ͽ��뷽��
			�� server_inst.c	�ṩһЩ��װ�Ĺ��ܺ���
			�� queue.c				�ṩָ��ѭ�����в���
			�� crc.c					��������ṩ��crcУ�麯��
			�� stm32f4xx.c		�����жϷ�����
	
	�� ʹ��˵����
			�� �����趨�����4�����������1~4��Ӧ����ṹ��serverData[1]~serverData[4]
			�� ����uasrt2��������һ������������������㲥pingָ���ʼ�����id
			�� ������Ӹ���������main.c��ʼ�����IDʱ��Ҫ�����¶����IDInit���
			�� ���ʹ���������ڣ���Ҫ�޸�serverDataInit()�����е�����
			�� ���ͨ�ţ�����UASRT���Ͳ�����DMA�����ܿ�����DMA������ʱע��һ��
			�� �������������������жϷ���������Ҫ���Ե�ָ���debug�۲�ѭ�����е����serverData[2].rrQueue�����������errorCodeCnt_2
			
	��	��ʵ�ֵ�ָ��(δʵ�ֵ�ָ�����˽ӿ�)
			�� ping
			�� read
			�� write
			�� reg write
			�� action
	
	�� ���ܵ��Ż���
			�� ��߶��ͨ�Ų�����
			�� ��߼��ָ����Ƶ��
			�� ָ�����ָ���ʱ����������д��instBuf�ٸ��ƹ�ȥ������ֱ��д�����е�buf��
			�� ��װ��ָ������Щ����ctrlTable��飬����дĿ���ٶȵ�ʱ��Ҫ��ת�����ơ�����ÿ��ִ�ж�Ҫ��readһ��ˢ�¿��Ʊ��ٶȻ���
			�� ...
*/
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "beep.h"
#include "key.h"
#include "delay.h"
#include "server.h"
#include "server_inst.h"
#include "queue.h"
#include "timer.h"

int main(void)
{ 
	u16 times=0;  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	delay_init(168);		
	
	LED_Init();		
	KEY_Init(); 												//������ʼ��	
	//USART1_Half_Configuration(9600);	//����1����Ϊ��˫��
	USART2_Half_Configuration(9600);		//����2����Ϊ��˫��	
	
	serverDataInit();										//�Զ���ʼ�����ID
	while(serverData[2].IDInit);				//���ID��ʼ��ʧ�ܣ������￨��			
	
	TIM2_Int_Init(100-1,8400-1);				//10ms  (����ɨ��)
	TIM3_Int_Init(100-1,8400-1);				//10ms  (���ָ���,���������ڶ��ID��ʼ����)
	
	torque_Inst(&serverData[2],1,0);		//Ť��ʹ��
	readCtrlTable(&serverData[2]);			//�����Ʊ�
	
	while(1)
	{	
		if(times==200)
		{
			;//torque_Inst(0,&serverData[2]);
		}
				
		if(times==500)
			times=0;	

		times++;
		if(times%250==0)LED0=!LED0;//��˸LED,��ʾϵͳ��������.
		
		delay_ms(1);
	}
}

