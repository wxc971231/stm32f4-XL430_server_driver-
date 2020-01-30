/*
XL430-W250舵机驱动程序 2019/1/29 	v1.0
	· 官方文档：
			· 通信协议：				http://emanual.robotis.com/docs/en/dxl/protocol2/#status-checksum
			· 舵机内控制表结构：http://emanual.robotis.com/docs/en/dxl/x/xl430-w250/#registered-instruction
	
	· 文件说明：
			· server.c			提供舵机通信数据编码和解码方法
			· server_inst.c	提供一些封装的功能函数
			· queue.c				提供指令循环队列操作
			· crc.c					舵机厂家提供的crc校验函数
			· stm32f4xx.c		所有中断服务函数
	
	· 使用说明：
			· 现在设定最多连4个舵机，串口1~4对应舵机结构体serverData[1]~serverData[4]
			· 现在uasrt2必须连接一个舵机，利用这个舵机广播ping指令初始化舵机id
			· 如果连接更多舵机，在main.c初始化舵机ID时，要加上新舵机的IDInit检查
			· 如果使用其他串口，需要修改serverDataInit()函数中的设置
			· 舵机通信，主机UASRT发送不能用DMA，接受可以用DMA，配置时注意一下
			· 配置了三个按键，在中断服务函数发送要测试的指令，用debug观察循环队列的情况serverData[2].rrQueue及解码错误码errorCodeCnt_2
			
	·	已实现的指令(未实现的指令留了接口)
			· ping
			· read
			· write
			· reg write
			· action
	
	· 可能的优化点
			· 提高舵机通信波特率
			· 提高检查指令缓冲的频率
			· 指令进入指令缓冲时，现在是先写到instBuf再复制过去，可以直接写到队列的buf里
			· 封装的指令中有些做了ctrlTable检查，比如写目标速度的时候要读转速限制。这样每次执行都要先read一下刷新控制表，速度会慢
			· ...
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
	KEY_Init(); 												//按键初始化	
	//USART1_Half_Configuration(9600);	//串口1配置为半双工
	USART2_Half_Configuration(9600);		//串口2配置为半双工	
	
	serverDataInit();										//自动初始化舵机ID
	while(serverData[2].IDInit);				//如果ID初始化失败，在这里卡死			
	
	TIM2_Int_Init(100-1,8400-1);				//10ms  (按键扫描)
	TIM3_Int_Init(100-1,8400-1);				//10ms  (检查指令缓冲,这个必须放在舵机ID初始化后)
	
	torque_Inst(&serverData[2],1,0);		//扭矩使能
	readCtrlTable(&serverData[2]);			//读控制表
	
	while(1)
	{	
		if(times==200)
		{
			;//torque_Inst(0,&serverData[2]);
		}
				
		if(times==500)
			times=0;	

		times++;
		if(times%250==0)LED0=!LED0;//闪烁LED,提示系统正在运行.
		
		delay_ms(1);
	}
}

