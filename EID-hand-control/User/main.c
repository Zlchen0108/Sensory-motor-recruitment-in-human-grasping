#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "uart.h"
#include "LED.h"
#include "string.h"
#include "ST.h"
#include "SC.h"
#include "SCServo.h"
#include "stdio.h"
#include "Key.h"
#include "LED.h"
#include "myFunc.h"

// 这句代码用于从串口5监听数据
// "powershell -ExecutionPolicy Bypass -File .\Tools\Log-ServoMotion.ps1 -PortName COM5"

uint8_t ID = 1;
int16_t Position1 = 0;
int16_t Position2 = 4096;
uint16_t Speed = 600;
uint8_t ACC = 20;
uint8_t KeyNum;
uint8_t KeyNum1;

uint8_t KeyNum2;
uint8_t Flag;
uint8_t RxData2;

volatile uint16_t g_pos1 = 0;
volatile uint8_t  g_tx_flag = 0;

char buf[32];


int main(void)
{
	
	 SystemInit();

	USART3_Init();
	TIM2_Init();   // 10ms 中断 (72MHz)

	Serial_Init();
	Key_Init();
	Serial_Init_uart2();
	//设置全舵机模式
	SetServoMode();
//  uint8_t write_data = 128;
//	
//	genWrite(1, 40, &write_data, 1);
 
	
	while (1)
	{   
//		int Position;

		//设置外部按钮急停
		Key_Control(Servo_ID);
//		sprintf(buf, "P1:%d\r\n", g_pos1);
//		USART3_SendString(buf);	/* 非阻塞状态机，须频繁调用 */
//		//屏幕控制
		if (Serial_GetRxFlag() == 1)
		{
			Grasptype_Control();
			Screen_Control();
		}
		//舵机状态反馈
//		Servo_PosFeedBack();
	}
}
