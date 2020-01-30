/*
	�����ṩһЩ��װ�Ĺ��ܺ�����������������Ʊ��д����
*/
#include "server_inst.h"
#include "server.h"
#include "delay.h"
#include "queue.h"
u8 paraBuf[20];		//д����Ʊ�����ݻ���


//��ָ��λ�ö�ָ��������
void read_Inst(SERVERDATA *sd,u8 addr,u8 len)
{
	readPara(sd,addr,len);
	Inst_Sending_Handler(Inst_Read,sd);
}

//Write ��ָ��λ��дָ�������ݡ�
void write_Inst(SERVERDATA *sd,u16 addr,u16 len,u8 *paraBuf,u8 regFlag)
{
	writePara(sd,addr,len,paraBuf);
	
	if(regFlag)
		Inst_Sending_Handler(Inst_RegWrite,sd);
	else
		Inst_Sending_Handler(Inst_Write,sd);	
}

//Reg Write��ָ��λ��дָ�������ݡ�
void regWrite_Inst(SERVERDATA *sd,u16 addr,u16 len,u8 *paraBuf)
{
	writePara(sd,addr,len,paraBuf);
  Inst_Sending_Handler(Inst_RegWrite,sd);	
}

//Action ִ��Reg Writeע�������һ��ָ�ע�����ָ��Ļ���ֻ��ִ�����ע���������
void action_Inst(SERVERDATA *sd)
{
  Inst_Sending_Handler(Inst_Action,sd);	
}

//pingָ��
void ping_Inst(SERVERDATA *sd)
{
	Inst_Sending_Handler(Inst_Ping,sd);
}

//��RAM������
void readRAM_Inst(SERVERDATA *sd)
{
	read_Inst(sd,64,146-64);
}

//��ROM������
void readROM_Inst(SERVERDATA *sd)
{
	read_Inst(sd,0,64);			
}

//�����Ʊ�RAM+ROM��
void readCtrlTable(SERVERDATA *sd)
{
	read_Inst(sd,0,146);			
}

//LEDʹ��/ʧ��
void LED_Inst(SERVERDATA *sd,u8 EN,u8 regFlag)
{
	paraBuf[0]=EN;
	write_Inst(sd,65,1,paraBuf,regFlag);
}

//Ť��ʹ��/ʧ��
void torque_Inst(SERVERDATA *sd,u8 EN,u8 regFlag)
{
	paraBuf[0]=EN;
	write_Inst(sd,64,1,paraBuf,regFlag);
}

//��������ģʽ
void setOperatingMode_Inst(SERVERDATA *sd,u8 mode,u8 regFlag)
{
	sd->lock=1;					//lock=1ʱ��ָֹ�����ӣ������ƺ�Ҳ����lock��
	
	torque_Inst(sd,0,0);
	paraBuf[0]=mode;
	write_Inst(sd,11,1,paraBuf,regFlag);
	torque_Inst(sd,1,0);
	read_Inst(sd,11,1);		//readһ�²�����debug�۲쵽�仯	
	
	sd->lock=0;
}

//��λ�ÿ���ģʽ�£�����Ŀ��λ��(�������ģʽ��û���Ƕ�����)
u8 setGoalPos_Inst(SERVERDATA *sd,u32 pos)
{	
	read_Inst(sd,11,1);		//������ģʽ
	if(sd->ctrlTable.rom.Operating_Mode != Position_Control_Mode)
		return 0;
	else
	{
		dataSplit_32(pos,paraBuf);
		writePara(sd,116,4,paraBuf);
		Inst_Sending_Handler(Inst_Write,sd);
		return 1;
	}
}

//���ٶȿ���ģʽ�£�����Ŀ���ٶ�(�������ģʽ������ٶ�����)
s32 vLim;
u8 setGoalVel_Inst(SERVERDATA *sd,s32 vel)
{	
	readROM_Inst(sd);			//������ģʽ���ٶ�����
	if(sd->ctrlTable.rom.Operating_Mode != Velocity_Control_Mode)
		return 0;
	else
	{
		vLim = (s32)dataAssmeble_32(sd->ctrlTable.rom.Velocity_Limit);
		
		if(fabs(vel)>vLim)
			return 0;
		
		dataSplit_32(vel,paraBuf);
		writePara(sd,104,4,paraBuf);
		Inst_Sending_Handler(Inst_Write,sd);
		return 1;
	}
}




