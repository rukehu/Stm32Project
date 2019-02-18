#ifndef __CAN_H__
#define __CAN_H__
#include "user.h"

void CAN_Mod_Init(uint8_t tsjw, uint8_t tbs2, uint8_t tbs1, uint16_t brp, uint8_t mode);
int8_t CAN1_Msg_Send(uint8_t *msg, uint8_t len);
uint8_t CAN1_Msg_Receive(uint8_t *buf);
void can_test_process(void);
#endif /*__CAN_H__*/
