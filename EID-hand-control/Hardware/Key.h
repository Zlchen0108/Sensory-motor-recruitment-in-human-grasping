#ifndef __KEY_H
#define __KEY_H

void Key_Init(void);
uint8_t Key_GetNum_1(void);
uint8_t Key_GetNum_2(void);
uint8_t Key_GetNum_Stop(void);
uint8_t Key_StopRequested(void);
void Key_ClearStopRequest(void);

#endif
