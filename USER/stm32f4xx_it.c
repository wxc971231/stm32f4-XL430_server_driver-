/**
  ******************************************************************************
  * @file    Project/STM32F4xx_StdPeriph_Templates/stm32f4xx_it.c 
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    04-August-2014
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_it.h"
#include "usart.h"	
#include "server.h"
#include "key.h"
#include "server_inst.h"
/** @addtogroup Template_Project
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
 
}

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f4xx.s).                                               */
/******************************************************************************/
#ifdef USART1_Rec_DMA
long long errorCodeCnt_1[12];
void USART1_IRQHandler(void)                
{
	uint8_t rc_tmp;
	uint16_t rc_len;
	USART_ClearITPendingBit(USART1,USART_IT_RXNE);
	if(USART_GetITStatus(USART1,USART_IT_IDLE)!=RESET)
	{
			rc_tmp=USART1->SR;
      rc_tmp=USART1->DR;
      DMA_Cmd(DMA2_Stream2, DISABLE);
      DMA_ClearITPendingBit(DMA2_Stream2, DMA_IT_TCIF2);	// Clear Transfer Complete flag
      DMA_ClearITPendingBit(DMA2_Stream2, DMA_IT_TEIF2);	// Clear Transfer error flag	
			DMA_ClearITPendingBit(DMA2_Stream2, DMA_IT_HTIF2);	// Clear Transfer half Complete flag
      rc_len = USART1_RX_BUFFER_SIZE - DMA_GetCurrDataCounter(DMA2_Stream2);
			
			
		
			DMA_Enable(DMA2_Stream2,USART1_RX_BUFFER_SIZE); 
	}
} 
#else
u8 res;
void USART1_IRQHandler(void)
{
	if(USART_GetITStatus(USART1,USART_IT_RXNE))//串口非空标志位为1，收到数据
	{
		res = USART_ReceiveData(USART1);//读取最新一个收到的数据
//		USART_SendData(USART1,res );//发送数据
	}
}
#endif

#ifdef USART2_Rec_DMA
long long errorCodeCnt_2[12];
void USART2_IRQHandler(void)                	
{
	u8 errorCode;
	uint8_t rc_tmp;
	uint16_t rc_len;
	
	USART_ClearITPendingBit(USART2,USART_IT_RXNE);
  if(USART_GetITStatus(USART2,USART_IT_IDLE)!=RESET)
  {
			rc_tmp=USART2->SR;	
			rc_tmp=USART2->DR;
			DMA_Cmd(DMA1_Stream5, DISABLE);
			DMA_ClearITPendingBit(DMA1_Stream5, DMA_IT_TCIF5);	// Clear Transfer Complete flag
			DMA_ClearITPendingBit(DMA1_Stream5, DMA_IT_TEIF5);	// Clear Transfer error flag	
			rc_len = USART2_RX_BUFFER_SIZE - DMA_GetCurrDataCounter(DMA1_Stream5);
	
			errorCode=Inst_Decoding_Handler(USART2_Rx_DMA_Buffer,rc_len,&serverData[2]);
			errorCodeCnt_2[errorCode]++;
		
			DMA_Enable(DMA1_Stream5,USART2_RX_BUFFER_SIZE); 
	}
	serverData[2].busy=0;
} 
#else
u8 cnt=0;
u8 rec[200];
void USART2_IRQHandler(void)
{
	if(USART_GetITStatus(USART2,USART_IT_RXNE))//串口非空标志位为1，收到数据
	{
		rec[cnt++] = USART_ReceiveData(USART2);//读取最新一个收到的数据
//		USART_SendData(USART1,res );//发送数据
	}
}
#endif



//定时器中断2  按键扫描--------------------------------------------------
u8 key;
u16 Key_Count=0;
u8 debug=0;
void TIM2_IRQHandler(void)//10ms
{
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET) //溢出中断
	{		
		Key_Count++;
		if(Key_Count>2)//20ms
		{
			Key_Count=0;
			key=KEY_Scan(0);
			
			if(key==KEY0_PRES)
			{
				//setOperatingMode_Inst(&serverData[2],Velocity_Control_Mode);
				//debug=setGoalVel_Inst(&serverData[2],100);
				LED_Inst(&serverData[2],0,1);
				LED_Inst(&serverData[2],1,1);
				
				//setGoalPos_Inst(&serverData[2],4000);
				
			}
			else if(key==KEY1_PRES)
			{
				//setOperatingMode_Inst(&serverData[2],Velocity_Control_Mode);
				//debug=setGoalVel_Inst(&serverData[2],0);
				
				action_Inst(&serverData[2]);
				//LED_Inst(&serverData[2],0,1);
				//setGoalPos_Inst(&serverData[2],1000);
			}
			else if(key==WKUP_PRES)
			{
				;
			}
		}
	}
	TIM_ClearITPendingBit(TIM2,TIM_IT_Update);  //清除中断标志位
}


//Tim3中断，检查指令缓冲
u8 len;
u8 instTemp[INSTBUFLEN];	//暂存出队的指令
void TIM3_IRQHandler(void)//10ms
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //溢出中断
	{		
		for(int i=1;i<=4;i++)
		{
			//如果舵机not busy && not lock && 指令缓冲队列非空，出队一个指令发送
			if(!serverData[i].busy && !serverData[i].lock && !queueEmpty(serverData[i].rrQueue))
			{
				u8 headPos = serverData[i].rrQueue->head;
				len = serverData[i].rrQueue->qinst[headPos].instLen;
				deQueue(&serverData[i],instTemp,len);
				
				serverData[i].busy=1;
				USART_TypeDef* USARTx = serverData[i].USARTx;
				sendOnly(USARTx);	
				sendBuf(USARTx,instTemp,len);
				readOnly(USARTx);
			}
		}
	}
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //清除中断标志位
}


/**
  * @}
  */ 


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
