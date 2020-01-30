/*
	这里提供一些封装的功能函数，基本都是向控制表读写数据
*/
#include "server_inst.h"
#include "server.h"
#include "delay.h"
#include "queue.h"
u8 paraBuf[20];		//写入控制表的数据缓冲


//在指定位置读指定长数据
void read_Inst(SERVERDATA *sd,u8 addr,u8 len)
{
	readPara(sd,addr,len);
	Inst_Sending_Handler(Inst_Read,sd);
}

//Write 在指定位置写指定长数据。
void write_Inst(SERVERDATA *sd,u16 addr,u16 len,u8 *paraBuf,u8 regFlag)
{
	writePara(sd,addr,len,paraBuf);
	
	if(regFlag)
		Inst_Sending_Handler(Inst_RegWrite,sd);
	else
		Inst_Sending_Handler(Inst_Write,sd);	
}

//Reg Write在指定位置写指定长数据。
void regWrite_Inst(SERVERDATA *sd,u16 addr,u16 len,u8 *paraBuf)
{
	writePara(sd,addr,len,paraBuf);
  Inst_Sending_Handler(Inst_RegWrite,sd);	
}

//Action 执行Reg Write注册的最新一条指令（注册多条指令的话，只会执行最后注册的那条）
void action_Inst(SERVERDATA *sd)
{
  Inst_Sending_Handler(Inst_Action,sd);	
}

//ping指令
void ping_Inst(SERVERDATA *sd)
{
	Inst_Sending_Handler(Inst_Ping,sd);
}

//读RAM区数据
void readRAM_Inst(SERVERDATA *sd)
{
	read_Inst(sd,64,146-64);
}

//读ROM区数据
void readROM_Inst(SERVERDATA *sd)
{
	read_Inst(sd,0,64);			
}

//读控制表（RAM+ROM）
void readCtrlTable(SERVERDATA *sd)
{
	read_Inst(sd,0,146);			
}

//LED使能/失能
void LED_Inst(SERVERDATA *sd,u8 EN,u8 regFlag)
{
	paraBuf[0]=EN;
	write_Inst(sd,65,1,paraBuf,regFlag);
}

//扭矩使能/失能
void torque_Inst(SERVERDATA *sd,u8 EN,u8 regFlag)
{
	paraBuf[0]=EN;
	write_Inst(sd,64,1,paraBuf,regFlag);
}

//设置运行模式
void setOperatingMode_Inst(SERVERDATA *sd,u8 mode,u8 regFlag)
{
	sd->lock=1;					//lock=1时禁止指令缓冲出队（这里似乎也不用lock）
	
	torque_Inst(sd,0,0);
	paraBuf[0]=mode;
	write_Inst(sd,11,1,paraBuf,regFlag);
	torque_Inst(sd,1,0);
	read_Inst(sd,11,1);		//read一下才能在debug观察到变化	
	
	sd->lock=0;
}

//在位置控制模式下，设置目标位置(检查运行模式，没检查角度限制)
u8 setGoalPos_Inst(SERVERDATA *sd,u32 pos)
{	
	read_Inst(sd,11,1);		//读运行模式
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

//在速度控制模式下，设置目标速度(检查运行模式，检查速度限制)
s32 vLim;
u8 setGoalVel_Inst(SERVERDATA *sd,s32 vel)
{	
	readROM_Inst(sd);			//读运行模式、速度限制
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




