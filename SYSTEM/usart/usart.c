#include "sys.h"
#include "usart.h"	
#include "delay.h"	

u8 idleTemp;
uint8_t		USART1_Tx_DMA_Buffer[USART1_TX_BUFFER_SIZE] = {0};
uint8_t		USART1_Rx_DMA_Buffer[USART1_RX_BUFFER_SIZE] = {0};
uint8_t 	USART2_Rx_DMA_Buffer[USART2_RX_BUFFER_SIZE] = {0};
uint8_t 	USART2_Tx_DMA_Buffer[USART2_TX_BUFFER_SIZE] = {0};

//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{ 	
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
	USART1->DR = (u8) ch;      
	return ch;
}
#endif
 


//����1����Ϊ��˫��
void USART1_Half_Configuration(u32 buad)  
{  
  GPIO_InitTypeDef GPIO_InitStructure;  
  USART_InitTypeDef USART_InitStructure;   
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //ʹ��GPIOAʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//ʹ��USART1ʱ��
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); //GPIOA9����ΪUSART1
	//GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); //GPIOA9����ΪUSART1
	
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;  
	//GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
  GPIO_Init(GPIOA, &GPIO_InitStructure);  

//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;  
//	GPIO_Init(GPIOA, &GPIO_InitStructure); 
	
  USART_InitStructure.USART_BaudRate = buad;  
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;  
  USART_InitStructure.USART_StopBits = USART_StopBits_1;  
  USART_InitStructure.USART_Parity = USART_Parity_No;  
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;  
  USART_Init(USART1,&USART_InitStructure);  

  USART_HalfDuplexCmd(USART1, ENABLE);  

	//Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//����1�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����

	USART_ClearFlag(USART1, USART_FLAG_TC);
	
#ifndef USART1_Rec_DMA
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//��������ж�
#else	
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);//���������ж�
	USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);//����DMA����
	USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);//����DMA����
	
	//DMA for rx
	DMA_Config(DMA2_Stream2,DMA_Channel_4, 
						 (uint32_t)&(USART1->DR),
						 (uint32_t)USART1_Rx_DMA_Buffer,
						 DMA_DIR_PeripheralToMemory,
						 USART1_RX_BUFFER_SIZE/2);
#endif						 

#ifdef USART1_Send_DMA
//DMA for tx					 
	DMA_Config(DMA2_Stream7,DMA_Channel_4, 
						 (uint32_t)&(USART1->DR),
						 (uint32_t)USART1_Tx_DMA_Buffer,
						 DMA_DIR_MemoryToPeripheral,
						 USART1_TX_BUFFER_SIZE);
#endif
	USART_Cmd(USART1,ENABLE); 
}  



//����2����Ϊ��˫��
void USART2_Half_Configuration(u32 buad)  
{  
  GPIO_InitTypeDef GPIO_InitStructure;  
  USART_InitTypeDef USART_InitStructure;   
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE); //ʹ��GPIOAʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//ʹ��USART1ʱ��
	
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource5,GPIO_AF_USART2); //GPIOA9����ΪUSART1
	
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	//GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
  GPIO_Init(GPIOD, &GPIO_InitStructure);  

	
  USART_InitStructure.USART_BaudRate = buad;  
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;  
  USART_InitStructure.USART_StopBits = USART_StopBits_1;  
  USART_InitStructure.USART_Parity = USART_Parity_No;  
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;  
  USART_Init(USART2,&USART_InitStructure);  

  USART_HalfDuplexCmd(USART2, ENABLE);  	//����Ϊ��˫��ģʽ

	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;//����1�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	

	USART_ClearFlag(USART2, USART_FLAG_TC);
#ifndef USART2_Rec_DMA
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//��������ж�
#else  
	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);//���������ж�
	USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE);//����DMA����
	USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE);//����DMA����
	
	//DMA for rx
	DMA_Config(DMA1_Stream5,DMA_Channel_4, 
						 (uint32_t)&(USART2->DR),
						 (uint32_t)USART2_Rx_DMA_Buffer,
						 DMA_DIR_PeripheralToMemory,  
						 USART2_RX_BUFFER_SIZE);
						 
	
#endif

#ifdef USART2_Send_DMA
	//DMA for tx					 
	DMA_Config(DMA1_Stream6,DMA_Channel_4, 
						 (uint32_t)&(USART2->DR),
						 (uint32_t)USART2_Tx_DMA_Buffer,
						 DMA_DIR_MemoryToPeripheral,
						 USART2_RX_BUFFER_SIZE);
#endif	

  USART_Cmd(USART2,ENABLE);  
}  


//����2����Ϊȫ˫��
void USART2_Configuration(void)  
{  
  GPIO_InitTypeDef GPIO_InitStructure;  
  USART_InitTypeDef USART_InitStructure;   
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE); //ʹ��GPIOAʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//ʹ��USART1ʱ��
	
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource5,GPIO_AF_USART2); //GPIOA9����ΪUSART1
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource6,GPIO_AF_USART2); //GPIOA9����ΪUSART1
	
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;  
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
  //GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
  GPIO_Init(GPIOD, &GPIO_InitStructure);  

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; 
	GPIO_Init(GPIOD, &GPIO_InitStructure); 
	
  USART_InitStructure.USART_BaudRate = 115200;  
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;  
  USART_InitStructure.USART_StopBits = USART_StopBits_1;  
  USART_InitStructure.USART_Parity = USART_Parity_No;  
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;  
	//USART_InitStructure.USART_Mode = USART_Mode_Tx ;
  USART_Init(USART2,&USART_InitStructure);  

  //USART_HalfDuplexCmd(USART2, ENABLE);  

  USART_Cmd(USART2,ENABLE);  
  USART_ClearFlag(USART2, USART_FLAG_TC);

	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//��������ж�

  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;//����1�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	
}  



//ֱ�ӷ���һ��buf
void sendBuf(USART_TypeDef* USARTx, u8 *buf, u16 len)
{
	for(int i=0;i<len;i++)
	{
		USART_SendData(USARTx,buf[i]);
		while((USARTx->SR & 0x40)==0);	//�ȷ���
	}
}

void ByteArrayCopy(unsigned char *buf1,unsigned char *buf2,uint16_t i_1,uint16_t i_2,uint16_t length)
{
	uint16_t i;
	for(i=0;i<length;i++)
		buf2[i_1+i]=buf1[i_2+i];
}

//DMA����һ��buf
void USARTSendDatasByDMA(USART_TypeDef* USARTX,u8 *buf,uint16_t length)
{
	DMA_Stream_TypeDef *DMA_StreamX;
	uint32_t FLAG;
	if(USARTX==USART1)
	{
		DMA_StreamX=DMA2_Stream7;
		FLAG=DMA_FLAG_TCIF7;
		ByteArrayCopy(buf,USART1_Tx_DMA_Buffer,0,0,length);
	}
	else if(USARTX==USART2)
	{
		DMA_StreamX=DMA1_Stream6;
		FLAG=DMA_FLAG_TCIF6;
		ByteArrayCopy(buf,USART2_Tx_DMA_Buffer,0,0,length);
	}
//	else if(USARTX==USART3)
//	{
//		DMA_StreamX=DMA1_Stream3;
//		FLAG=DMA_FLAG_TCIF3;
//		ByteArrayCopy(buf,USART3_Tx_DMA_Buffer,0,0,length);
//	}
//	else if(USARTX==UART4)
//	{
//		DMA_StreamX=DMA1_Stream4;
//		FLAG=DMA_FLAG_TCIF4;
//		ByteArrayCopy(buf,UART4_Tx_DMA_Buffer,0,0,length);
//	}
//	else if(USARTX==UART5)
//	{	
//		DMA_StreamX=DMA1_Stream7;
//		FLAG=DMA_FLAG_TCIF7;
//		ByteArrayCopy(buf,UART5_Tx_DMA_Buffer,0,0,length);
//	}
	
	USART_DMACmd(USARTX,USART_DMAReq_Tx,ENABLE);
	if(DMA_GetFlagStatus(DMA_StreamX,FLAG)!=RESET) //if send finish
	{ 
		DMA_ClearFlag(DMA_StreamX,FLAG);
	}		
	else
		return;
	
	DMA_Enable(DMA_StreamX,length);
}


int GetUsartNum(USART_TypeDef* USARTX)
{
	if(USARTX==USART1)
		return 1;
	else if(USARTX==USART2)
		return 2;
	else if(USARTX==USART3)
		return 3;
	else if(USARTX==UART4)
		return 4;
	else if(USARTX==UART5)
		return 5;
	else if(USARTX==USART6)
		return 6;
	else if(USARTX==UART7)
		return 7;
	else if(USARTX==UART8)
		return 8;
	
	return 0;
}





 



