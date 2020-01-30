/*============================================================================================================================================
1.ָ���ʽ
			0					1					2					3							4						5					6						7						8					9					10				11			12
	| Header1	|	Header2	|	Header3	|	Reserved	| PacketID	|	Length1		|	Length2	|	Instruction	|	Param		|	Param		|	Param		| CRC1	| CRC2	|
	| 0xFF		|	0xFF		|	0xFD		|	0x00			| ID				|	Len_L			|	Len_H		|	Inst				|	Param1	|	��				|	ParamN	| CRC_L	| CRC_H	|
		

2.���ݰ�����length = �ֶκ�ĳ���(ָ�������CRC�ֶ�) = ������+3
============================================================================================================================================*/

#include "server.h"
#include "crc.h"
#include "usart.h"

SERVERDATA serverData[5];		//���������Ϣ,�±��Ǵ��ں�(0�Ų���)
u8 instBuf[INSTBUFLEN];			//����֡Buf

u8 write2CtrlTable[5][100];	//д����Ʊ������buf(ע����ֽ���ǰ)

/*------------------------------------------------���ݴ�����-----------------------------------------------------*/
//����8λ���ϳ�һ��16λ��
u16 dataAssmeble(u8 data_L,u8 data_H)
{
	u16 temp=(data_H<<8)|data_L;
	return temp;
}

//һ��16λ��������8λ
void dataSplit(u16 data,u8 *data_L,u8 *data_H)
{
	*data_L = data & 0xFF;
	*data_H = (data & 0xFF00)>>8;
}

//һ��32λ���ݲ��ĸ�8λ��buf�״����λ��
void dataSplit_32(u32 data,u8 *buf)
{
	u32 temp=0xFF;
	for(int i=0;i<4;i++)
		buf[i]=(data & temp<<8*i)>>8*i;
}

//�ĸ�8λ����ƴһ��32λ��buf�״����λ��
u32 dataAssmeble_32(u8 *buf)
{
	u32 temp=buf[3];
	for(int i=2;i>=0;i--)
		temp=(temp<<8)|buf[i];
	
	return temp;
}

//��inst����
void clearInstBuf()
{
	for(int i=0;i<INSTBUFLEN;i++)
		instBuf[i]=0;
}	
/*------------------------------------------------��������-----------------------------------------------------*/
//����ṹ���ʼ��
void serverDataInit()
{
	initQueue();							//��ʼ��ָ������
	
	for(int i=1;i<=4;i++)
	{
		serverData[i].IDInit=1;	//׼������id����
		serverData[i].writeBuf=write2CtrlTable[i];	
		serverData[i].busy=0;
		serverData[i].rrQueue = &queue[i];
	}
		
	serverData[1].USARTx=USART1;
	serverData[2].USARTx=USART2;
	serverData[3].USARTx=USART3;
	serverData[4].USARTx=UART4;
	
	//���͹㲥ָ�����Ĵ���2������϶��
	serverData[2].ID=BroadcastID;
	sendOnly(USART2);
	PingCode(&serverData[2]);
	sendBuf(USART2,instBuf,10);
	readOnly(USART2);	
}

//����֡ͷ�ͱ���λ
void setHead()
{
	//֡ͷ
	instBuf[0]=0xFF;
	instBuf[1]=0xFF;
	instBuf[2]=0xFD;
	instBuf[3]=0x00;
}

//֡ͷ�ͱ���λ���
u8 cheakHead(u8 *buf)
{
	if(buf[0]!=0xFF || buf[1]!=0xFF || buf[2]!=0xFD || buf[3]!=0)
		return 0;
	return 1;
}

//֡���ȼ��
u8 cheakLen(u8 *buf,u8 len)
{
	u16 L=dataAssmeble(buf[5],buf[6]);
	if(len!=L)
		return 0;
	return 1;
}

//CRC���
u8 cheakCRC(u8 *buf,u8 size) 
{
	u16 crc=update_crc(0,buf,size);
	if(buf[size]!=(crc&0xFF)	|| buf[size+1]!=((crc>>8)&0xFF))
			return 0;
	return 1;
}

/*------------------------------------------------����׼��-----------------------------------------------------*/
//���ö����Ʊ�Ĳ���
void readPara(SERVERDATA *sd,u16 addr,u16 len)
{
	sd->RWInstPara.RWAddr=addr;
	sd->RWInstPara.RWLen=len;
}

//����д���Ʊ����
void writePara(SERVERDATA *sd,u16 addr,u16 len,u8 *paraBuf)
{
	sd->RWInstPara.RWAddr=addr;
	sd->RWInstPara.RWLen=len;
	copyBuf(paraBuf,sd->writeBuf,len);
}

/*-----------------------------------------------���뷢��֡-----------------------------------------------------*/

//pingָ�Ҫ��ָ��ID���豸�������ݰ������ID��ΪBroadcastID��0xFE�����������ӵ��豸����������˳����״̬��
void PingCode(SERVERDATA *sd)
{
	setHead();						//֡ͷ
	instBuf[4]=sd->ID;		//ID
	instBuf[5]=0x03;			//LEN_L
	instBuf[6]=0x00;			//LEN_H
	instBuf[7]=Inst_Ping;	//Inst	
	setCrc(instBuf,8);		//CRC
	
	sd->RWInstPara.frameLen=Inst_Ping_Len;	//����֡����
}

//Readָ��ӿ��Ʊ�RWAddr��ַ��ʼ��RWLen���ֽڣ���ַ<64��EEPROM����������RAM����
void ReadCode(SERVERDATA *sd)
{
	setHead();						//֡ͷ
	instBuf[4]=sd->ID;		//ID
	instBuf[5]=0x07;			//LEN_L
	instBuf[6]=0x00;			//LEN_H
	instBuf[7]=Inst_Read;	//Inst
	dataSplit(sd->RWInstPara.RWAddr,&instBuf[8],&instBuf[9]);		//para_addr
	dataSplit(sd->RWInstPara.RWLen,&instBuf[10],&instBuf[11]);	//para_len
	setCrc(instBuf,12);		//CRC
	
	sd->RWInstPara.frameLen=Inst_Read_Len;	//����֡����
}

//write/reg writeָ���������Ʊ�RWAddrд��writeBuf��ǰRWLen���ֽ�
void WriteCode(SERVERDATA *sd,u8 regFlag)
{
	setHead();												//֡ͷ
	instBuf[4]=sd->ID;								//ID
	u16 len=sd->RWInstPara.RWLen+2+3;	//LEN
	dataSplit(len,&instBuf[5],&instBuf[6]);	
	
	if(regFlag)
		instBuf[7]=Inst_RegWrite;				//Inst
	else
		instBuf[7]=Inst_Write;						
	
	dataSplit(sd->RWInstPara.RWAddr,&instBuf[8],&instBuf[9]);	//para_addr
			
	copyBuf(sd->writeBuf,&instBuf[10],sd->RWInstPara.RWLen);	//д����Ʊ������	
	setCrc(instBuf,len+5);						//CRC
	
	sd->RWInstPara.frameLen=len+7;		//����֡����
}



//actionָ�ִ��regWriteע���ָ��
void ActionCode(SERVERDATA *sd)
{
	setHead();												//֡ͷ
	instBuf[4]=sd->ID;								//ID
	instBuf[5]=0x03;									//len
	instBuf[6]=0x00;
	instBuf[7]=Inst_Action;						//inst
	setCrc(instBuf,8);								//CRC
	
	sd->RWInstPara.frameLen=Inst_Action_Len;	//����֡����
}


//���ʹ�����
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

/*-----------------------------------------------�������֡-----------------------------------------------------*/
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
	;	//write���ص�ָ���û�����ݣ������ӿ�
}

//reg write
void RegWriteDecode()
{
	;	//red write���ص�ָ���û�����ݣ������ӿ�
}

//action
void ActionDecode()
{
	;	//action���ص�ָ���û�����ݣ������ӿ�
}

//���봦����
u8 Inst_Decoding_Handler(u8 *buf,u8 reclen,SERVERDATA *sd)
{
	if(!cheakHead(buf))					//���֡ͷ
		return DC_MyHeadError;
		
	if(!cheakLen(buf,reclen-7))	//���֡����
		return DC_MyLenError;

	if(!cheakCRC(buf,reclen-2))	//���CRC
		return DC_MyCRCError;	
	
	if(buf[8]!=0)								//��鷢�����ݴ���
		return buf[8];
	
	//ID��ʼ��
	if(sd->IDInit)
	{
		sd->ID=buf[4];
		sd->IDInit=0;
		return DC_Success;
	}
	//����ͨ��
	else
	{
		if(buf[4]!=sd->ID)				//���ID
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
