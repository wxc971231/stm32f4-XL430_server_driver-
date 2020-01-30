#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "stm32f4xx_conf.h"
#include "sys.h" 
#include "dma.h"

//dma发送舵机收不到，dma接收可以
//#define USART1_Send_DMA	
#define USART1_Rec_DMA
//#define USART2_Send_DMA	
#define USART2_Rec_DMA



#define readOnly(x)	x->CR1 |= 4;	x->CR1 &= 0xFFFFFFF7;		//串口x配置为只读，CR1->RE=1, CR1->TE=0
#define sendOnly(x)	x->CR1 |= 8;	x->CR1 &= 0xFFFFFFFB;		//串口x配置为只写，CR1->RE=0, CR1->TE=1

#define readSend(x)	x->CR1 |= 4;	x->CR1 &= 0xFFFFFFFB;		//串口x配置为读写

#define clearIDLE(x) {idleTemp=x->SR,idleTemp=x->DR;}			//清IDLE中断标志


#define USART1_TX_BUFFER_SIZE     10
#define USART1_RX_BUFFER_SIZE     10
#define	USART2_TX_BUFFER_SIZE    	100
#define	USART2_RX_BUFFER_SIZE    	200

extern u8 idleTemp;
extern uint8_t	USART1_Tx_DMA_buffer[USART1_TX_BUFFER_SIZE] ;
extern uint8_t	USART1_Rx_DMA_Buffer[USART1_RX_BUFFER_SIZE] ;
extern uint8_t 	USART2_Rx_DMA_Buffer[USART2_RX_BUFFER_SIZE] ;
extern uint8_t 	USART2_Tx_DMA_Buffer[USART2_TX_BUFFER_SIZE];


void USART1_Half_Configuration(u32 buad);
void USART2_Half_Configuration(u32 buad);
void USART2_Configuration(void);
void sendBuf(USART_TypeDef* USARTx, u8 *buf, u16 len);
void USARTSendDatasByDMA(USART_TypeDef* USARTX,u8 *buf,uint16_t length);
int GetUsartNum(USART_TypeDef* USARTX);
#endif


