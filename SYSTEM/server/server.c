/*============================================================================================================================================
1.指令格式
			0					1					2					3							4						5					6						7						8					9					10				11			12
	| Header1	|	Header2	|	Header3	|	Reserved	| PacketID	|	Length1		|	Length2	|	Instruction	|	Param		|	Param		|	Param		| CRC1	| CRC2	|
	| 0xFF		|	0xFF		|	0xFD		|	0x00			| ID				|	Len_L			|	Len_H		|	Inst				|	Param1	|	…				|	ParamN	| CRC_L	| CRC_H	|
		

2.数据包长度length = 字段后的长度(指令、参数、CRC字段) = 参数数+3
============================================================================================================================================*/

#include "server.h"
#include "crc.h"
#include "usart.h"

SERVERDATA serverData[5];		//舵机基本信息,下标是串口号(0号不用)
u8 instBuf[INSTBUFLEN];			//发送帧Buf

u8 write2CtrlTable[5][100];	//写入控制表的数据buf(注意低字节在前)

/*------------------------------------------------数据处理函数-----------------------------------------------------*/
//两个8位数合成一个16位数
u16 dataAssmeble(u8 data_L,u8 data_H)
{
	u16 temp=(data_H<<8)|data_L;
	return temp;
}

//一个16位数拆两个8位
void dataSplit(u16 data,u8 *data_L,u8 *data_H)
{
	*data_L = data & 0xFF;
	*data_H = (data & 0xFF00)>>8;
}

//一个32位数据拆四个8位（buf首存最低位）
void dataSplit_32(u32 data,u8 *buf)
{
	u32 temp=0xFF;
	for(int i=0;i<4;i++)
		buf[i]=(data & temp<<8*i)>>8*i;
}

//四个8位数据拼一个32位（buf首存最低位）
u32 dataAssmeble_32(u8 *buf)
{
	u32 temp=buf[3];
	for(int i=2;i>=0;i--)
		temp=(temp<<8)|buf[i];
	
	return temp;
}

//清inst数组
void clearInstBuf()
{
	for(int i=0;i<INSTBUFLEN;i++)
		instBuf[i]=0;
}	
/*------------------------------------------------辅助函数-----------------------------------------------------*/
//舵机结构体初始化
void serverDataInit()
{
	initQueue();							//初始化指令缓冲队列
	
	for(int i=1;i<=4;i++)
	{
		serverData[i].IDInit=1;	//准备进行id设置
		serverData[i].writeBuf=write2CtrlTable[i];	
		serverData[i].busy=0;
		serverData[i].rrQueue = &queue[i];
	}
		
	serverData[1].USARTx=USART1;
	serverData[2].USARTx=USART2;
	serverData[3].USARTx=USART3;
	serverData[4].USARTx=UART4;
	
	//发送广播指令，这里的串口2必须接上舵机
	serverData[2].ID=BroadcastID;
	sendOnly(USART2);
	PingCode(&serverData[2]);
	sendBuf(USART2,instBuf,10);
	readOnly(USART2);	
}

//设置帧头和保留位
void setHead()
{
	//帧头
	instBuf[0]=0xFF;
	instBuf[1]=0xFF;
	instBuf[2]=0xFD;
	instBuf[3]=0x00;
}

//帧头和保留位检查
u8 cheakHead(u8 *buf)
{
	if(buf[0]!=0xFF || buf[1]!=0xFF || buf[2]!=0xFD || buf[3]!=0)
		return 0;
	return 1;
}

//帧长度检查
u8 cheakLen(u8 *buf,u8 len)
{
	u16 L=dataAssmeble(buf[5],buf[6]);
	if(len!=L)
		return 0;
	return 1;
}

//CRC检查
u8 cheakCRC(u8 *buf,u8 size) 
{
	u16 crc=update_crc(0,buf,size);
	if(buf[size]!=(crc&0xFF)	|| buf[size+1]!=((crc>>8)&0xFF))
			return 0;
	return 1;
}

/*------------------------------------------------参数准备-----------------------------------------------------*/
//设置读控制表的参数
void readPara(SERVERDATA *sd,u16 addr,u16 len)
{
	sd->RWInstPara.RWAddr=addr;
	sd->RWInstPara.RWLen=len;
}

//设置写控制表参数
void writePara(SERVERDATA *sd,u16 addr,u16 len,u8 *paraBuf)
{
	sd->RWInstPara.RWAddr=addr;
	sd->RWInstPara.RWLen=len;
	copyBuf(paraBuf,sd->writeBuf,len);
}

/*-----------------------------------------------编码发送帧-----------------------------------------------------*/

//ping指令：要求指定ID的设备发回数据包，如果ID设为BroadcastID（0xFE），所有链接的设备根据其排列顺序发送状态包
void PingCode(SERVERDATA *sd)
{
	setHead();						//帧头
	instBuf[4]=sd->ID;		//ID
	instBuf[5]=0x03;			//LEN_L
	instBuf[6]=0x00;			//LEN_H
	instBuf[7]=Inst_Ping;	//Inst	
	setCrc(instBuf,8);		//CRC
	
	sd->RWInstPara.frameLen=Inst_Ping_Len;	//发送帧长度
}

//Read指令：从控制表RWAddr地址开始读RWLen长字节（地址<64是EEPROM区，否则是RAM区）
void ReadCode(SERVERDATA *sd)
{
	setHead();						//帧头
	instBuf[4]=sd->ID;		//ID
	instBuf[5]=0x07;			//LEN_L
	instBuf[6]=0x00;			//LEN_H
	instBuf[7]=Inst_Read;	//Inst
	dataSplit(sd->RWInstPara.RWAddr,&instBuf[8],&instBuf[9]);		//para_addr
	dataSplit(sd->RWInstPara.RWLen,&instBuf[10],&instBuf[11]);	//para_len
	setCrc(instBuf,12);		//CRC
	
	sd->RWInstPara.frameLen=Inst_Read_Len;	//发送帧长度
}

//write/reg write指令：立即向控制表RWAddr写入writeBuf中前RWLen长字节
void WriteCode(SERVERDATA *sd,u8 regFlag)
{
	setHead();												//帧头
	instBuf[4]=sd->ID;								//ID
	u16 len=sd->RWInstPara.RWLen+2+3;	//LEN
	dataSplit(len,&instBuf[5],&instBuf[6]);	
	
	if(regFlag)
		instBuf[7]=Inst_RegWrite;				//Inst
	else
		instBuf[7]=Inst_Write;						
	
	dataSplit(sd->RWInstPara.RWAddr,&instBuf[8],&instBuf[9]);	//para_addr
			
	copyBuf(sd->writeBuf,&instBuf[10],sd->RWInstPara.RWLen);	//写入控制表的数据	
	setCrc(instBuf,len+5);						//CRC
	
	sd->RWInstPara.frameLen=len+7;		//发送帧长度
}



//action指令，执行regWrite注册的指令
void ActionCode(SERVERDATA *sd)
{
	setHead();												//帧头
	instBuf[4]=sd->ID;								//ID
	instBuf[5]=0x03;									//len
	instBuf[6]=0x00;
	instBuf[7]=Inst_Action;						//inst
	setCrc(instBuf,8);								//CRC
	
	sd->RWInstPara.frameLen=Inst_Action_Len;	//发送帧长度
}


//发送处理函数
void Inst_Sending_Handler(u8 inst,SERVERDATA *sd)
{
		sd->inst=inst;
		
		switch(inst)
		{
			case 	Inst_Ping:			PingCode(sd);					break;	
														
			case 	Inst_Read:			ReadCode(sd);					break;

			case 	Inst_Write:			WriteCode(sd,0);			break;

			case 	Inst_RegWrite:	WriteCode(sd,1);			break;
			
			case 	Inst_Action:		ActionCode(sd);				break;
			case 	Inst_Reset:					break;
			case 	Inst_Reboot:				break;
			case 	Inst_Clear:					break;
			case 	Inst_Status:				break;
			case 	Inst_SyncRead:			break;
			case 	Inst_SyncWrite:			break;
			case 	Inst_BulkRead:			break;
			case 	Inst_BulkWrite:			break;
		}
	
		enQueue(sd->rrQueue,instBuf,sd->RWInstPara.frameLen);
}

/*-----------------------------------------------解码接受帧-----------------------------------------------------*/
//ping
void PindDecode(u8 *buf,SERVERDATA *sd)
{
	sd->PingInfo.ModelNum=dataAssmeble(buf[9],buf[10]);
	sd->PingInfo.FirmwareVersion=buf[11];
}

//read
void ReadDecode(u8 *buf,SERVERDATA *sd)
{
	u16 len=dataAssmeble(buf[5],buf[6]);
	u8 *p=(u8 *)(&sd->ctrlTable)+sd->RWInstPara.RWAddr;
	
	for(int i=0;i<len-4;i++)
		p[i]=buf[9+i];
}

//write
void WriteDecode()
{
	;	//write返回的指令包没有数据，留个接口
}

//reg write
void RegWriteDecode()
{
	;	//red write返回的指令包没有数据，留个接口
}

//action
void ActionDecode()
{
	;	//action返回的指令包没有数据，留个接口
}

//解码处理函数
u8 Inst_Decoding_Handler(u8 *buf,u8 reclen,SERVERDATA *sd)
{
	if(!cheakHead(buf))					//检测帧头
		return DC_MyHeadError;
		
	if(!cheakLen(buf,reclen-7))	//检查帧长度
		return DC_MyLenError;

	if(!cheakCRC(buf,reclen-2))	//检查CRC
		return DC_MyCRCError;	
	
	if(buf[8]!=0)								//检查发送数据错误
		return buf[8];
	
	//ID初始化
	if(sd->IDInit)
	{
		sd->ID=buf[4];
		sd->IDInit=0;
		return DC_Success;
	}
	//正常通信
	else
	{
		if(buf[4]!=sd->ID)				//检查ID
			return DC_MyIDError;
			
		switch(sd->inst)
		{
			case 	Inst_Ping:			PindDecode(buf,sd);		break;	
			case 	Inst_Read:			ReadDecode(buf,sd);		break;
			case 	Inst_Write:			WriteDecode();				break;
			case 	Inst_RegWrite:	RegWriteDecode();			break;
			case 	Inst_Action:		ActionDecode();				break;
			case 	Inst_Reset:					break;
			case 	Inst_Reboot:				break;
			case 	Inst_Clear:					break;
			case 	Inst_Status:				break;
			case 	Inst_SyncRead:			break;
			case 	Inst_SyncWrite:			break;
			case 	Inst_BulkRead:			break;
			case 	Inst_BulkWrite:			break;
		}
	
		return DC_Success;
	}
}
