#ifndef __SERVER_INST_H
#define __SERVER_INST_H

#include "server.h"
#include "crc.h"
#include "usart.h"

void action_Inst(SERVERDATA *sd);

void readRAM_Inst(SERVERDATA *sd);
void readROM_Inst(SERVERDATA *sd);
void readCtrlTable(SERVERDATA *sd);

void LED_Inst(SERVERDATA *sd,u8 EN,u8 regFlag);
void torque_Inst(SERVERDATA *sd,u8 EN,u8 regFlag);
void setOperatingMode_Inst(SERVERDATA *sd,u8 mode,u8 regFlag);

u8 setGoalVel_Inst(SERVERDATA *sd,s32 vel);
u8 setGoalPos_Inst(SERVERDATA *sd,u32 pos);

#endif
