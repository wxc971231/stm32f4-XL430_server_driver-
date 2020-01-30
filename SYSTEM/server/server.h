#ifndef __SERVER_H
#define __SERVER_H
#include "sys.h"
#include "queue.h"

//�㲥ID
#define BroadcastID 0xFE

//readָ��Ļ��泤��
#define readBufMax	100

//����ģʽ��Operating Mode�������Ʊ��ַ11
#define Velocity_Control_Mode 					1
#define Position_Control_Mode 					3
#define Extended_Position_Control_Mode	4
#define PWM_Control_Mode								16

//ָ�����
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

//ָ���
#define Inst_Ping_Len 			10		
#define Inst_Read_Len 			14	
//#define Inst_Write_Len;		//writeָ��Ȳ�������ر�����serverData�ṹ����
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

//����ɹ���־
#define DC_Success 						0x00	
//����֡�����־����������жϣ�д�뷵��״̬֡��Errλ��
#define DC_ResultFail					0x01	//Failed to process the sent Instruction Packet
#define DC_InstructionError		0x02	//Undefined Instruction has been used || Action has been used without Reg Write
#define DC_CRCError						0x03	//�������ݵ�֡��CRCУ�����
#define DC_DataRangeError			0x04	//Ҫд����Ӧ��ַ�����ݳ�������С/���ֵ�ķ�Χ
#define DC_DataLenError				0x05	//����д�����Ӧ��ַ�����ݳ��ȶ̵�����
#define DC_DataLimitError			0x06	//Ҫд����Ӧ��ַ�����ݳ�������ֵ
#define DC_AccessError				0x07	//������ֻ����δ����ĵ�ַ��д��ֵ || ���Զ�ȡ��д���δ����ĵ�ַ�е�ֵ || ��ת������״̬(ROM��)ʱ��������ROM����д��ֵ
//����֡�����־������ʱ�����жϣ�
#define DC_MyHeadError 				0x08	//��������֡������ͷ����
#define DC_MyLenError					0x09	//��������֡�ĳ��ȳ���
#define DC_MyIDError					0x0A	//��������֡��ID����
#define DC_MyCRCError					0x0B	//��������֡��CRCУ�����			
		
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
	u8 RSRV1[6];							//14	������1
	u8 Homing_Offset[4];			//20
	u8 Moving_Threshold[4];		//24
	u8 RSRV2[3];							//28	������2
	u8 Temperature_Limit;			//31
	u8 Max_Voltage_Limit[2];	//32
	u8 Min_Voltage_Limit[2];	//34
	u8 PWM_Limit[2];					//36
	u8 RSRV3[6];							//38	������3
	u8 Velocity_Limit[4];			//44
	u8 Max_Position_Limit[4];	//48
	u8 Min_Position_Limit[4];	//52
	u8 RSRV4[7];							//56	������4
	u8 Shutdown;							//63
}EEPROM;

typedef struct RAM
{
	u8 Torque_Enable;						//64
	u8 LED;											//65
	u8 RSRV1[2];								//66 ������1
	u8 Status_Return_Level;			//68
	u8 Registered_Instruction;	//69
	u8 Hardware_Error_Status;		//70
	u8 RSRV2[5];								//71 ������2
	u8 Velocity_I_Gain[2];			//76
	u8 Velocity_P_Gain[2];			//78
	u8 Position_D_Gain[2];			//80
	u8 Position_I_Gain[2];			//82
	u8 Position_P_Gain[2];			//84
	u8 RSRV3[2];								//86 ������3
	u8 Feedforward_2nd_Gain[2];	//88
	u8 Feedforward_1st_Gain[2];	//90
	u8 RSRV4[7];								//91 ������4
	u8 Bus_Watchdog;						//98
	u8 Goal_PWM[2];							//100
	u8 RSRV5[2];								//102 ������5
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

//������Ʊ�
typedef struct CONTROL_TABLE
{
	EEPROM rom;
	RAM ram;
}CONTROL_TABLE;

//readָ�������
typedef struct RWINST
{
	u16 RWAddr;						//���һ�ζ�д���Ʊ����ʼ��ַ
	u16 RWLen;						//���һ�ζ�д���Ʊ�ĳ���
	u16 frameLen;					//����֡�ĳ���
}RWINST;


//pingָ��������ݰ�
typedef struct PINGINFO
{
	u16 ModelNum;
	u8 FirmwareVersion;
}PINGINFO;


//�������
typedef struct SERVERDATA
{
	u8 inst;								//���һ�η��͵�ָ��
	u8 ID;									//�������Ӷ����ID
	u8 recLen;							//���һ�ν��ܵ�ָ���
	u8 IDInit;							//��ʼ��ID��־
	u8 *writeBuf;						//д������buf��ָ��
	u8 busy;								//�����շ�����
	u8 lock;								//����lock=1ʱ���ܴӻ������ȡָ��
	
	USART_TypeDef* USARTx;	//���ں�
	RRQUEUE *rrQueue;				//�����Ӧ��ָ������
	
	RWINST RWInstPara;			//��дָ�������
	PINGINFO PingInfo;			//ping�������ݰ�

	CONTROL_TABLE ctrlTable;//���Ʊ�
}SERVERDATA;


extern u8 instBuf[INSTBUFLEN];
extern SERVERDATA serverData[5];
extern u8 write2CtrlTable[5][100];	//д����Ʊ�����ݻ���(ע����ֽ���ǰ)

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
