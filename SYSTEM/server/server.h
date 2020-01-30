#ifndef __SERVER_H
#define __SERVER_H
#include "sys.h"
#include "queue.h"

//广播ID
#define BroadcastID 0xFE

//read指令的缓存长度
#define readBufMax	100

//运行模式（Operating Mode），控制表地址11
#define Velocity_Control_Mode 					1
#define Position_Control_Mode 					3
#define Extended_Position_Control_Mode	4
#define PWM_Control_Mode								16

//指令参数
#define Inst_Ping 			0x01		//Instruction that checks whether the Packet has arrived to a device with the same ID as Packet ID
#define Inst_Read 			0x02		//Instruction to read data from the Device
#define Inst_Write 			0x03		//Instruction to write data on the Device
#define Inst_RegWrite 	0x04		//Instruction that registers the Instruction Packet to a standby status; Packet is later executed through the Action command
#define Inst_Action			0x05		//Instruction that executes the Packet that was registered beforehand using Reg Write
#define Inst_Reset 			0x06 		//Instruction that resets the Control Table to its initial factory default settings
#define Inst_Reboot 		0x08		//Instruction to reboot the Device
#define Inst_Clear 			0x10		//Instruction to reset certain information
#define Inst_Status 		0x55		//Return Instruction for the Instruction Packet
#define Inst_SyncRead 	0x82		//For multiple devices, Instruction to read data from the same Address with the same length at once
#define Inst_SyncWrite 	0x83		//For multiple devices, Instruction to write data on the same Address with the same length at once
#define Inst_BulkRead 	0x92		//For multiple devices, Instruction to read data from different Addresses with different lengths at once
#define Inst_BulkWrite	0x93		//For multiple devices, Instruction to write data on different Addresses with different lengths at once

//指令长度
#define Inst_Ping_Len 			10		
#define Inst_Read_Len 			14	
//#define Inst_Write_Len;		//write指令长度不定，相关变量在serverData结构体里
//#define Inst_RegWrite_Len 	
#define Inst_Action_Len			10			
//#define Inst_Reset_Len 			
//#define Inst_Reboot_Len 		
//#define Inst_Clear_Len 			
//#define Inst_Status_Len 		
//#define Inst_SyncRead_Len 	
//#define Inst_SyncWrite_Len 	
//#define Inst_BulkRead_Len 	
//#define Inst_BulkWrite_Len	

//解码成功标志
#define DC_Success 						0x00	
//发送帧错误标志（舵机进行判断，写入返回状态帧的Err位）
#define DC_ResultFail					0x01	//Failed to process the sent Instruction Packet
#define DC_InstructionError		0x02	//Undefined Instruction has been used || Action has been used without Reg Write
#define DC_CRCError						0x03	//发送数据的帧的CRC校验出错
#define DC_DataRangeError			0x04	//要写入相应地址的数据超出了最小/最大值的范围
#define DC_DataLenError				0x05	//尝试写入比相应地址的数据长度短的数据
#define DC_DataLimitError			0x06	//要写入相应地址的数据超出了限值
#define DC_AccessError				0x07	//尝试在只读或未定义的地址中写入值 || 尝试读取仅写入或未定义的地址中的值 || 在转矩启用状态(ROM锁)时，尝试在ROM域中写入值
//接受帧错误标志（解码时进行判断）
#define DC_MyHeadError 				0x08	//接受数据帧的数据头出错
#define DC_MyLenError					0x09	//接受数据帧的长度出错
#define DC_MyIDError					0x0A	//接受数据帧的ID出错
#define DC_MyCRCError					0x0B	//接受数据帧的CRC校验出错			
		
typedef struct EEPROM
{
	u8 Model_Number[2];				//0
	u8 Model_Information[4];	//2
	u8 Firmware_Version;			//6
	u8 ID;										//7
	u8 Baud_Rate;							//8
	u8 Return_Delay_Time;			//9
	u8 Drive_Mode;						//10
	u8 Operating_Mode;				//11
	u8 Secondary_ID;					//12
	u8 Protocol_Type;					//13
	u8 RSRV1[6];							//14	保留段1
	u8 Homing_Offset[4];			//20
	u8 Moving_Threshold[4];		//24
	u8 RSRV2[3];							//28	保留段2
	u8 Temperature_Limit;			//31
	u8 Max_Voltage_Limit[2];	//32
	u8 Min_Voltage_Limit[2];	//34
	u8 PWM_Limit[2];					//36
	u8 RSRV3[6];							//38	保留段3
	u8 Velocity_Limit[4];			//44
	u8 Max_Position_Limit[4];	//48
	u8 Min_Position_Limit[4];	//52
	u8 RSRV4[7];							//56	保留段4
	u8 Shutdown;							//63
}EEPROM;

typedef struct RAM
{
	u8 Torque_Enable;						//64
	u8 LED;											//65
	u8 RSRV1[2];								//66 保留段1
	u8 Status_Return_Level;			//68
	u8 Registered_Instruction;	//69
	u8 Hardware_Error_Status;		//70
	u8 RSRV2[5];								//71 保留段2
	u8 Velocity_I_Gain[2];			//76
	u8 Velocity_P_Gain[2];			//78
	u8 Position_D_Gain[2];			//80
	u8 Position_I_Gain[2];			//82
	u8 Position_P_Gain[2];			//84
	u8 RSRV3[2];								//86 保留段3
	u8 Feedforward_2nd_Gain[2];	//88
	u8 Feedforward_1st_Gain[2];	//90
	u8 RSRV4[7];								//91 保留段4
	u8 Bus_Watchdog;						//98
	u8 Goal_PWM[2];							//100
	u8 RSRV5[2];								//102 保留段5
	u8 Goal_Velocity[4];				//104
	u8 Profile_Acceleration[4];	//108
	u8 Profile_Velocity[4];			//112
	u8 Goal_Position[4];				//116
	u8 Realtime_Tick[2];				//120		
	u8 Moving;									//122
	u8 Moving_Status;						//123
	u8 Present_PWM[2];					//124
	u8 Present_Load[2];					//126
	u8 Present_Velocity[4];			//128
	u8 Present_Position[4];			//132
	u8 Velocity_Trajectory[4];	//136
	u8 Position_Trajectory[4];	//140
	u8 Present_Input_Voltage[2];//144
	u8 Present_Temperature;			//146
}RAM;

//舵机控制表
typedef struct CONTROL_TABLE
{
	EEPROM rom;
	RAM ram;
}CONTROL_TABLE;

//read指令参数包
typedef struct RWINST
{
	u16 RWAddr;						//最近一次读写控制表的起始地址
	u16 RWLen;						//最近一次读写控制表的长度
	u16 frameLen;					//数据帧的长度
}RWINST;


//ping指令接受数据包
typedef struct PINGINFO
{
	u16 ModelNum;
	u8 FirmwareVersion;
}PINGINFO;


//舵机数据
typedef struct SERVERDATA
{
	u8 inst;								//最近一次发送的指令
	u8 ID;									//串口连接舵机的ID
	u8 recLen;							//最近一次接受的指令长度
	u8 IDInit;							//初始化ID标志
	u8 *writeBuf;						//写入数据buf的指针
	u8 busy;								//正在收发数据
	u8 lock;								//锁，lock=1时不能从缓冲队列取指令
	
	USART_TypeDef* USARTx;	//串口号
	RRQUEUE *rrQueue;				//舵机对应的指令缓冲队列
	
	RWINST RWInstPara;			//读写指令参数包
	PINGINFO PingInfo;			//ping接受数据包

	CONTROL_TABLE ctrlTable;//控制表
}SERVERDATA;


extern u8 instBuf[INSTBUFLEN];
extern SERVERDATA serverData[5];
extern u8 write2CtrlTable[5][100];	//写入控制表的数据缓冲(注意低字节在前)

u16 dataAssmeble(u8 data_L,u8 data_H);
void dataSplit(u16 data,u8 *data_L,u8 *data_H);
u32 dataAssmeble_32(u8 *buf);
void dataSplit_32(u32 data,u8 *buf);

void serverDataInit(void);


void readPara(SERVERDATA *sd,u16 addr,u16 len);
void writePara(SERVERDATA *sd,u16 addr,u16 len,u8 *paraBuf);

void PingCode(SERVERDATA *sd);
u8 Inst_Decoding_Handler(u8 *buf,u8 reclen,SERVERDATA *sd);
void Inst_Sending_Handler(u8 inst,SERVERDATA *sd);
void deQueue(SERVERDATA *sd,u8 *buf,u8 len);

#endif
