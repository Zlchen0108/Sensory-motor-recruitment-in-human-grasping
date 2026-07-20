/*
 * uart.c
 * UART接口
 * 日期: 2019.9.7
 * 作者: 
 */

#include <stdio.h>
#include "stm32f10x.h"
#include "uart.h"
#include <stdarg.h>
#define USE_USART1_ 1
//#define USE_USART2_ 1

//UART 读数据缓冲区
__IO uint8_t uartBuf[128];
__IO int head = 0;
__IO int tail  = 0;

uint8_t RxData;
uint8_t RxFlag = 0;
unsigned short Serial_RxData;
unsigned short Serial_RxData2;
uint8_t Serial_RxFlag;
uint8_t Serial_TxBasket[5];
uint8_t Serial_RxBasket[5];



int fputc(int ch, FILE *f)
 
{
 
USART_SendData(USART2, (unsigned char) ch);// USART1 鍙互鎹㈡垚 USART2 绛?
 
while (!(USART2->SR & USART_FLAG_TXE));
 
return (ch);
}

struct __FILE
 
{
 
int handle; // Add whatever you need here
 
};
 
FILE __stdout;

void Uart_Flush(void)
{
	head = tail = 0;
}

int16_t Uart_Read(void)
{
	if(head!=tail){
		uint8_t Data = uartBuf[head];
		head =  (head+1)%128;
		return Data;
	}else{
		return -1;
	}
}

/*---------------
使用USE_USART1_ 宏定义
配置USART1，端口映射(TX)PA9/(RX)PA10
USART1作为舵机串口
------------------*/
#ifdef USE_USART1_

char Serial_RxPacket[100];				//"@MSG\r\n"
uint8_t Serial_RxFlag;

void Serial_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_USART1|RCC_APB2Periph_AFIO,ENABLE); 
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 1000000;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1, &USART_InitStructure);
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
	
	USART_Cmd(USART1, ENABLE);

}

void Serial_Init_uart2(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART2, &USART_InitStructure);
	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
	
	USART_Cmd(USART2, ENABLE);

}

void uart2_send_char(uint8_t temp)      
{       
    USART_SendData(USART2,(uint8_t)temp);        
    while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)==RESET);         
} 

void uart2_send_int(int temp)      
{       
    USART_SendData(USART2,(int)temp);        
    while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)==RESET);         
} 


void Serial_SendByte(uint8_t Byte)
{
	USART_SendData(USART1, Byte);
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

void Serial_SendByte_USART2(uint8_t Byte)
{
	USART_SendData(USART2, Byte);
	while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
}

void Serial_SendArray(uint8_t *Array, uint16_t Length)
{
	uint16_t i;
	for (i = 0; i < Length; i ++)
	{
		uart2_send_char(Array[i]);
	}
}

void Serial_SendArray_usart1(uint8_t *Array, uint16_t Length)
{
	uint16_t i;
	for (i = 0; i < Length; i ++)
	{
		Serial_SendByte(Array[i]);
	}
	
}

void Serial_SendInt_USART2(int data)
{
    Serial_SendByte_USART2((uint8_t)(data & 0xFF));        // Send lower 8 bits
    Serial_SendByte_USART2((uint8_t)((data >> 8) & 0xFF)); // Send upper 8 bits (if 16-bit int)
    // If `int` is 32-bit, add two more shifts for higher bytes
}

void Serial_SendString(char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i ++)
	{
		Serial_SendByte(String[i]);
	}
}

uint32_t Serial_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while (Y --)
	{
		Result *= X;
	}
	return Result;
}

void Serial_SendNumber(uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i ++)
	{
		Serial_SendByte_USART2(Number / Serial_Pow(10, Length - i - 1) % 10 + '0');
	}
}


void Serial_Printf(char *format, ...)
{
	char String[100];
	va_list arg;
	va_start(arg, format);
	vsprintf(String, format, arg);
	va_end(arg);
	Serial_SendString(String);
}

void USART1_IRQHandler(void)
{
	static uint8_t RxState = 0;
	static uint8_t pRxPacket = 0;
	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
	{
		uint8_t RxData = USART_ReceiveData(USART1);
		
		if (RxState == 0)
		{
			if (RxData == '@' && Serial_RxFlag == 0)
			{
				RxState = 1;
				pRxPacket = 0;
			}
		}
		else if (RxState == 1)
		{
			if (RxData == '\r')
			{
				RxState = 2;
			}
			else
			{
				Serial_RxPacket[pRxPacket] = RxData;
				pRxPacket ++;
			}
		}
		else if (RxState == 2)
		{
			if (RxData == '\n')
			{
				RxState = 0;
				Serial_RxPacket[pRxPacket] = '\0';
				Serial_RxFlag = 1;
			}
		}
		uartBuf[tail] = USART_ReceiveData(USART1);
		tail = (tail+1)%128;
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}
	
}

int return_tail(void)
{
	return tail;
}


uint8_t Serial_GetRxFlag(void)
{
	if (Serial_RxFlag == 1)
	{
		Serial_RxFlag = 0;
		return 1;
	}
	return 0;
}
void Serial_ClearRxFlag(void)
{
	Serial_RxFlag = 0;
}

void Serial_SendBasket(uint8_t Serial_Basket[4])
{
	uart2_send_char(0xFF);
	Serial_SendArray(Serial_Basket, 4);
	uart2_send_char(0xFE);
}

uint16_t Serial_GetRxData(void)
{
	return Serial_RxData;
}


void USART2_IRQHandler(void)
{
	static uint8_t RxState = 0;
	static uint8_t pRxBasket = 0;
	if (USART_GetITStatus(USART2, USART_IT_RXNE) == SET)
	{
//		Serial_RxData = USART_ReceiveData(USART2);
//		Serial_RxFlag = 1;
		uint8_t RxData1 = USART_ReceiveData(USART2);
		if (RxState ==0)
		{
			if(RxData1 == 0xFF)
			{
				RxState = 1;
				pRxBasket = 0;
			}
			else
			{
				RxState = 0;
			}
			
		}
		else if (RxState ==1)
		{
			Serial_RxBasket[pRxBasket] = RxData1;
			pRxBasket++;
			if (pRxBasket >= 5)
			{
				RxState = 2;
			}
			
		}
		else if (RxState ==2)
		{
			if(RxData1 == 0xFE)
			{
				RxState = 0;
				Serial_RxFlag = 1;
			}
			
		}
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
	}
}

#endif


/*---------------
使用USE_USART2_宏定义
配置USART2，端口映射(TX)PA2/(RX)PA3
USART2作为舵机串口
------------------*/
#ifdef USE_USART2_
void Uart_Init_2(uint32_t baudRate)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	//UART2 GPIO 配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;//PA2 UART2_TX
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
    
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA3 UART2_RX
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//UART 数据格式配置
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;
	USART_InitStructure.USART_BaudRate = baudRate;
	USART_Init(USART2, &USART_InitStructure);
	
	//中断配置
	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//打开UART使能开关
	USART_Cmd(USART2, ENABLE);
}

void USART2_IRQHandler(void)
{
	USART_ClearITPendingBit(USART2, USART_IT_ORE);
	uartBuf[tail] = USART_ReceiveData(USART2);
	tail = (tail+1)%128;
}

void Uart_Send_2(uint8_t *buf , uint8_t len)
{
	uint8_t i=0;
	for(i=0; i<len; i++){
		USART_SendData(USART2, buf[i]);
		while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
	}
	while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
}

#endif


void USART3_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

    // TX PB10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // RX PB11
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(USART3, &USART_InitStructure);

    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    USART_Cmd(USART3, ENABLE);
}



void USART3_SendString(char *str)
{
    while (*str)
    {
        while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
        USART_SendData(USART3, *str++);
    }
}




