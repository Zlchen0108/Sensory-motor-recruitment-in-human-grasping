#ifndef		_UART_H
#define		_UART_H

#include <stdint.h>
#include <stdio.h>

extern uint8_t Serial_TxBasket[];
extern uint8_t Serial_RxBasket[];

extern char Serial_RxPacket[];
extern uint8_t Serial_RxFlag;

void Serial_Init(void);
void Serial_SendByte(uint8_t Byte);
void Serial_SendArray(uint8_t *Array, uint16_t Length);
void Serial_SendString(char *String);
void Serial_SendNumber(uint32_t Number, uint8_t Length);
void Serial_Printf(char *format, ...);
void uart2_send_char(uint8_t temp);  
void Serial_Init_uart2(void);
uint8_t Serial_RxGetFlag(void);
uint16_t Serial_GetRxData(void);
void Serial_SendBasket(uint8_t Serial_Basket[4]);
uint8_t Serial_GetRxFlag(void);
void Serial_ClearRxFlag(void);
void uart2_send_int(int temp);
void Serial_SendInt_USART2(int data);
int16_t Uart_Read(void);
int return_tail(void);
void Serial_SendArray_usart1(uint8_t *Array, uint16_t Length);
void USART3_Init(void);
void USART3_SendString(char *str);
#endif
