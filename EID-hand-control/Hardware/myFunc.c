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

extern volatile uint16_t g_pos1;
extern volatile uint8_t  g_tx_flag;
static volatile uint8_t g_motion_stop_flag = 0;

//void Telemetry_Task(void)
//{
//    if (g_tx_flag)
//    {
//        g_tx_flag = 0;

//        char buf1[32];
//        sprintf(buf1, "P1:%d\r\n", ReadPos(8));
//        USART3_SendString(buf1);
//			
////				char buf2[32];
////        sprintf(buf2, "P2:%d\r\n", ReadPos(3));
////        USART3_SendString(buf2);
////			
////				char buf3[32];
////        sprintf(buf3, "P3:%d\r\n", ReadPos(5));
////        USART3_SendString(buf3);
////			
////				char buf4[32];
////        sprintf(buf4, "P4:%d\r\n", ReadPos(1));
////        USART3_SendString(buf4);
////			
////				char buf5[32];
////        sprintf(buf5, "P5:%d\r\n", ReadPos(11));
////        USART3_SendString(buf5);
////			
////				char buf6[32];
////        sprintf(buf6, "P6:%d\r\n", ReadPos(14));
////        USART3_SendString(buf6);
////				
////				char buf7[32];
////        sprintf(buf7, "P7:%d\r\n", ReadPos(17));
////        USART3_SendString(buf7);
////			
////				char buf8[32];
////        sprintf(buf8, "P8:%d\r\n", ReadPos(18));
////        USART3_SendString(buf8);
////				
////				char buf9[32];
////        sprintf(buf9, "P9:%d\r\n", ReadPos(21));
////        USART3_SendString(buf9);
////				
////				char buf10[32];
////        sprintf(buf10, "P10:%d\r\n", ReadPos(24));
////        USART3_SendString(buf10);
////			
////				char buf11[32];
////        sprintf(buf11, "P11:%d\r\n", ReadPos(33));
////        USART3_SendString(buf11);
////				
////				char buf12[32];
////        sprintf(buf12, "P12:%d\r\n", ReadPos(30));
////        USART3_SendString(buf12);
////			
////				char buf13[32];
////        sprintf(buf13, "P13:%d\r\n", ReadPos(27));
////        USART3_SendString(buf13);
////			
////				char buf14[32];
////        sprintf(buf14, "P14:%d\r\n", ReadPos(42));
////        USART3_SendString(buf14);
////			
////				char buf15[32];
////        sprintf(buf15, "P15:%d\r\n", ReadPos(39));
////        USART3_SendString(buf15);
////				
////				char buf16[32];
////        sprintf(buf16, "P16:%d\r\n", ReadPos(36));
////        USART3_SendString(buf16);
//			
//    }
//}

void Telemetry_Task(void)
{
    if (g_tx_flag)
    {
        g_tx_flag = 0;

        char buf[256];

        sprintf(buf,
            "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\r\n",
            ReadPos(8),  ReadPos(3),  ReadPos(5),  ReadPos(1),
            ReadPos(11), ReadPos(14), ReadPos(17), ReadPos(18),
            ReadPos(21), ReadPos(24), ReadPos(33), ReadPos(30),
            ReadPos(27), ReadPos(42), ReadPos(39), ReadPos(36)
        );

        USART3_SendString(buf);
    }
}


void SetServoMode(void)
{
	WheelMode(1);
	WheelMode(3);
	WheelMode(5);
	WheelMode(8);
	WheelMode(11);
	WheelMode(14);
	WheelMode(17);
	WheelMode(18);
	WheelMode(21);
	WheelMode(24);
	WheelMode(27);
	WheelMode(30);
	WheelMode(33);
	WheelMode(36);
	WheelMode(39);
	WheelMode(42);
	WheelMode(45);
}

void ServoStop(uint8_t ID)
{
	EnableTorque(ID, 0);
	Delay_ms(10);
	EnableTorque(ID, 1);
}

 void Key_Control(uint16_t Servo_ID)
{
	uint8_t KeyNum;
	uint8_t KeyNum1;
	int Pos;
	uint8_t Flag;
	int All_ServoID[17] = {1, 3, 5, 8, 11, 14, 17, 18, 21, 24, 27, 30, 33, 36, 39, 42, 45};
	
	
	KeyNum = Key_GetNum_1();
	KeyNum1 = Key_GetNum_2();
	if(KeyNum == 4)
	{
		g_motion_stop_flag = 1;
		LED1_ON();
		Servo_PosFeedBack();
		for (int i=0; i<17; i++)
			{
				ServoStop(All_ServoID[i]);
				Delay_ms(5);
			}
	
	
	}
	if(KeyNum == 1)
	{

		LED1_OFF();
	}
	if(KeyNum1 == 4)
	{
		g_motion_stop_flag = 1;
//		if(FeedBack(1) != -1)
//		{
//			printf("n0.val=%d\xff\xff\xff", ReadPos(-1));
//		}

		LED1_ON();
		Servo_PosFeedBack();
		for (int i=0; i<17; i++)
			{
				ServoStop(All_ServoID[i]);
				Delay_ms(5);
			}
	}
	if(KeyNum1 == 1)
	{

		LED1_OFF();
	}
	uint8_t KeyNum2;
	KeyNum2 = Key_GetNum_Stop();
	if(KeyNum2 == 4)
	{
		g_motion_stop_flag = 1;
		LED1_ON();
//		Servo_PosFeedBack();
		for (int i=0; i<17; i++)
			{
				ServoStop(All_ServoID[i]);
				Delay_ms(5);
			}
	}
	if(KeyNum2 == 1)
	{

		LED1_OFF();
	}
}

void Key_Control_1(uint8_t ID, int16_t Position1, int16_t Position2, uint16_t Speed, uint8_t ACC)
{
	uint8_t KeyNum;
	uint8_t KeyNum1;
	int Pos;
	uint8_t Flag;
	
	
	KeyNum = Key_GetNum_1();
	KeyNum1 = Key_GetNum_2();
	if(KeyNum == 4)
	{
		g_motion_stop_flag = 1;
		LED1_ON();
		WritePosEx(ID, Position1, Speed, ACC);
	}
	if(KeyNum == 1)
	{

		LED1_OFF();
	}
	if(KeyNum1 == 4)
	{
		LED1_ON();
		WritePosEx(ID, Position2, Speed, ACC);
	}
	if(KeyNum1 == 1)
	{

		LED1_OFF();
	}
}
uint16_t RxData1;


void Serial_GetAndSend(void)
{
	if(Serial_GetRxFlag() == 1)
		{
			RxData1 = Serial_GetRxData();
			if(RxData1 == 0x33)
			{
				LED1_ON();
			}
			uart2_send_char(RxData1);
			
		}
}


uint16_t Servo_ID;

void Screen_Control(void)
{
	uint8_t Finger_Name;
	uint8_t Joint_Name;
	uint8_t Joint_Max;
	uint16_t Joint_Position[5] = {-4000, -2000, 0, 2000, 4000};
//	uint16_t Joint_Position[5] = {0, 800, 1600, 2400, 3200};
	uint16_t Joint_Speed[4] = {200, 300, 500, 1000} ;
	uint16_t Joint_ID[5];
	uint16_t Joint_ACC[4] = {20, 30, 40, 50};
	uint8_t All_ServoID[17] = {1, 3, 5, 8, 11, 14, 17, 18, 21, 24, 27, 30, 33, 36, 39, 42, 45};
	
		if(Serial_RxBasket[0] == 5 && Serial_RxBasket[1] == 5)
		{
			for (int i=0; i<17; i++)
			{
				ServoStop(All_ServoID[i]);
			}
		}
		else
		{
			if(Serial_RxBasket[0] == 4 && Serial_RxBasket[1] >= 2 && Serial_RxBasket[2] == 0 && Serial_RxBasket[3] == 0 && Serial_RxBasket[4] == 0)
			{
				return;
			}
			Finger_Name = Serial_RxBasket[0];
		if(Finger_Name > 4)
		{
			return;
		}
		if(Finger_Name == 0)
		{
			uint16_t Joint_ID_Thumb[5] = {1, 3, 5, 8, 11};
			for(int i=0; i<5; i++)
			{
				Joint_ID[i] = Joint_ID_Thumb[i];
			}
		}
		if(Finger_Name == 1)
		{
			uint16_t Joint_ID_Index[3] = {14, 17, 18};
			for(int i=0; i<3; i++)
			{
				Joint_ID[i] = Joint_ID_Index[i];
			}
		}
		if(Finger_Name == 2)
		{
			uint16_t Joint_ID_Middle[3] = {21, 24, 27};
			for(int i=0; i<3; i++)
			{
				Joint_ID[i] = Joint_ID_Middle[i];
			}
		}
		if(Finger_Name == 3)
		{
			uint16_t Joint_ID_Ring[3] = {30, 33, 36};
			for(int i=0; i<3; i++)
			{
				Joint_ID[i] = Joint_ID_Ring[i];
			}
		}
		if(Finger_Name == 4)
		{
			uint16_t Joint_ID_little[3] = {39, 42, 45};
			for(int i=0; i<3; i++)
			{
				Joint_ID[i] = Joint_ID_little[i];
			}
		}
		Joint_Name = Serial_RxBasket[1];
		Joint_Max = (Finger_Name == 0) ? 5 : 3;
		if(Joint_Name >= Joint_Max || Serial_RxBasket[2] >= 5 || Serial_RxBasket[3] >= 4 || Serial_RxBasket[4] >= 4)
		{
			return;
		}
		Servo_ID = Joint_ID[Joint_Name];
		uint16_t Servo_Position = Joint_Position[Serial_RxBasket[2]];
		uint16_t Servo_Speed = Joint_Speed[Serial_RxBasket[3]];
		uint16_t Servo_ACC = Joint_ACC[Serial_RxBasket[4]];
//		printf("%d\r\n", Servo_ID);
//		printf("%d\r\n", Servo_Position);
//		printf("%d\r\n", Servo_Speed);
//		printf("%d\r\n", Servo_ACC);
		WritePosEx(Servo_ID, Servo_Position, Servo_Speed, Servo_ACC);
		
		}	
}

//void Servo_PosFeedBack(void)
//{
////	if(FeedBack(1) != -1)
////	{
////		printf("n0.val=%d", ReadPos(1));
////		printf("%c%c%c", 0xFF, 0xFF, 0xFF);
////	}
////	if(FeedBack(3) != -1)
////	{
////		printf("n1.val=%d", ReadPos(3));
////		printf("%c%c%c", 0xFF, 0xFF, 0xFF);

////	}
////	if(FeedBack(5) != -1)
////	{
////		printf("n2.val=%d", ReadPos(5));
////		printf("%c%c%c", 0xFF, 0xFF, 0xFF);

////	}
////	if(FeedBack(8) != -1)
////	{
////		printf("n3.val=%d", ReadPos(8));
////		printf("%c%c%c", 0xFF, 0xFF, 0xFF);

////	}
////	if(FeedBack(11) != -1)
////	{
////		printf("n4.val=%d", ReadPos(11));
////		printf("%c%c%c", 0xFF, 0xFF, 0xFF);

////	}
////		if(FeedBack(18) != -1)
////	{
////		printf("n5.val=%d", ReadPos(18));
////		printf("%c%c%c", 0xFF, 0xFF, 0xFF);

////	}
////		if(FeedBack(18) != -1)
////	{
////		printf("n6.val=%d", ReadPos(18));
////		printf("%c%c%c", 0xFF, 0xFF, 0xFF);

////	}
////		if(FeedBack(17) != -1)
////	{
////		printf("n7.val=%d", ReadPos(17));
////		printf("%c%c%c", 0xFF, 0xFF, 0xFF);

////	}
////		if(FeedBack(21) != -1)
////	{
////		printf("n8.val=%d", ReadPos(21));
////		printf("%c%c%c", 0xFF, 0xFF, 0xFF);

////	}
////		if(FeedBack(27) != -1)
////	{
////		printf("n9.val=%d", ReadPos(27));
////		printf("%c%c%c", 0xFF, 0xFF, 0xFF);

////	}
////		if(FeedBack(27) != -1)
////	{
////		printf("n10.val=%d", ReadPos(27));
////		printf("%c%c%c", 0xFF, 0xFF, 0xFF);

////	}
////		if(FeedBack(24) != -1)
////	{
////		printf("n11.val=%d", ReadPos(24));
////		printf("%c%c%c", 0xFF, 0xFF, 0xFF);

////	}
////		if(FeedBack(14) != -1)
////	{
////		printf("n12.val=%d", ReadPos(14));
////		printf("%c%c%c", 0xFF, 0xFF, 0xFF);

////	}
////		if(FeedBack(36) != -1)
////	{
////		printf("n13.val=%d", ReadPos(36));
////		printf("%c%c%c", 0xFF, 0xFF, 0xFF);

////	}
////		if(FeedBack(36) != -1)
////	{
////		printf("n14.val=%d", ReadPos(36));
////		printf("%c%c%c", 0xFF, 0xFF, 0xFF);

////	}
////		if(FeedBack(33) != -1)
////	{
////		printf("n15.val=%d", ReadPos(33));
////		printf("%c%c%c", 0xFF, 0xFF, 0xFF);

////	}
////		if(FeedBack(30) != -1)
////	{
////		printf("n19.val=%d", ReadPos(30));
////		printf("%c%c%c", 0xFF, 0xFF, 0xFF);

////	}
//		if(FeedBack(45) != -1)
//	{
//		printf("n0.val=%d\xff\xff\xff", ReadPos(45));
////		printf("%c%c%c", 0xFF, 0xFF, 0xFF);

//	}
////		if(FeedBack(45) != -1)
////	{
////		printf("n17.val=%d\xff\xff\xff", ReadPos(45));
////		printf("%c%c%c", 0xFF, 0xFF, 0xFF);

////	}
////		if(FeedBack(42) != -1)
////	{
////		printf("n18.val=%d", ReadPos(42));
////		printf("%c%c%c", 0xFF, 0xFF, 0xFF);

////	}
////			if(FeedBack(39) != -1)
////	{
////		printf("n20.val=%d", ReadPos(39));
////		printf("%c%c%c", 0xFF, 0xFF, 0xFF);

////	}
//}

void Servo_PosFeedBack(void)
{
	if(FeedBack(1) != -1)
	{
		printf("n0.val=%d\xff\xff\xff", ReadPos(1));
	}
	if(FeedBack(3) != -1)
	{
		printf("n1.val=%d\xff\xff\xff", ReadPos(8));

	}
	if(FeedBack(5) != -1)
	{
		printf("n2.val=%d\xff\xff\xff", ReadPos(3));

	}
	if(FeedBack(8) != -1)
	{
		printf("n3.val=%d\xff\xff\xff", ReadPos(5));

	}
	if(FeedBack(11) != -1)
	{
		printf("n4.val=%d\xff\xff\xff", ReadPos(1));

	}
		if(FeedBack(18) != -1)
	{
		printf("n5.val=%d\xff\xff\xff", ReadPos(11));

	}
		if(FeedBack(18) != -1)
	{
		printf("n6.val=%d\xff\xff\xff", ReadPos(11));

	}
		if(FeedBack(17) != -1)
	{
		printf("n7.val=%d\xff\xff\xff", ReadPos(14));

	}
		if(FeedBack(21) != -1)
	{
		printf("n8.val=%d\xff\xff\xff", ReadPos(17));
	}
		if(FeedBack(27) != -1)
	{
		printf("n9.val=%d\xff\xff\xff", ReadPos(18));

	}
		if(FeedBack(27) != -1)
	{
		printf("n10.val=%d\xff\xff\xff", ReadPos(18));

	}
		if(FeedBack(24) != -1)
	{
		printf("n11.val=%d\xff\xff\xff", ReadPos(21));

	}
		if(FeedBack(14) != -1)
	{
		printf("n12.val=%d\xff\xff\xff", ReadPos(24));

	}
		if(FeedBack(36) != -1)
	{
		printf("n13.val=%d\xff\xff\xff", ReadPos(33));

	}
		if(FeedBack(36) != -1)
	{
		printf("n14.val=%d\xff\xff\xff", ReadPos(33));

	}
		if(FeedBack(33) != -1)
	{
		printf("n15.val=%d\xff\xff\xff", ReadPos(30));

	}
		if(FeedBack(30) != -1)
	{
		printf("n19.val=%d\xff\xff\xff", ReadPos(27));

	}
		if(FeedBack(45) != -1)
	{
		printf("n0.val=%d\xff\xff\xff", ReadPos(42));

	}
		if(FeedBack(42) != -1)
	{
		printf("n17.val=%d\xff\xff\xff", ReadPos(42));

	}
		if(FeedBack(42) != -1)
	{
		printf("n18.val=%d\xff\xff\xff", ReadPos(39));

	}
			if(FeedBack(39) != -1)
	{
		printf("n20.val=%d\xff\xff\xff", ReadPos(36));

	}
}

void Stop_Allservo(void)
{
	int All_ServoID[17] = {1, 3, 5, 8, 11, 14, 17, 18, 21, 24, 27, 30, 33, 36, 39, 42, 45};
	for (int i=0; i<17; i++)
	{
		ServoStop(All_ServoID[i]);
		Delay_ms(5);
	}
}

void Grasptype_Control(void)
{
		if(Serial_RxBasket[0] == 6 && Serial_RxBasket[1] == 6)
		{

			NormalGrasp();

		}
		
		if(Serial_RxBasket[0] == 7 && Serial_RxBasket[1] == 1)
		{

			Number1();

		}
		
		if(Serial_RxBasket[0] == 7 && Serial_RxBasket[1] == 2)
		{

			Number2();

		}
		if(Serial_RxBasket[0] == 7 && Serial_RxBasket[1] == 3)
		{

			Number3();

		}
		
		if(Serial_RxBasket[0] == 7 && Serial_RxBasket[1] == 4)
		{

			Number4();

		}
		
		if(Serial_RxBasket[0] == 7 && Serial_RxBasket[1] == 5)
		{

			Number5();

		}
		
		if(Serial_RxBasket[0] == 7 && Serial_RxBasket[1] == 6)
		{

			Number6();

		}
		
		if(Serial_RxBasket[0] == 7 && Serial_RxBasket[1] == 7)
		{

			Number7();

		}
		if(Serial_RxBasket[0] == 7 && Serial_RxBasket[1] == 8)
		{
			Number8();

		}
		
		if(Serial_RxBasket[0] == 7 && Serial_RxBasket[1] == 9)
		{

			Number9();

		}
		if(Serial_RxBasket[0] == 7 && Serial_RxBasket[1] == 0)
		{
			Number10();

		}
		if(Serial_RxBasket[0] == 5 && Serial_RxBasket[1] == 1)
		{
			Kapandji_Test_1();

		}
		if(Serial_RxBasket[0] == 4 && Serial_RxBasket[1] == 2)
		{
			Kapandji_Test_2();

		}
		if(Serial_RxBasket[0] == 4 && Serial_RxBasket[1] == 3)
		{
			Kapandji_Test_3();

		}
		if(Serial_RxBasket[0] == 4 && Serial_RxBasket[1] == 4)
		{
			Kapandji_Test_4();

		}
		if(Serial_RxBasket[0] == 4 && Serial_RxBasket[1] == 5)
		{
			Kapandji_Test_5();

		}
		if(Serial_RxBasket[0] == 4 && Serial_RxBasket[1] == 6)
		{
			Kapandji_Test_6();

		}
		if(Serial_RxBasket[0] == 4 && Serial_RxBasket[1] == 7)
		{
			Kapandji_Test_7();

		}
		if(Serial_RxBasket[0] == 4 && Serial_RxBasket[1] == 8)
		{
			Kapandji_Test_8();

		}
		if(Serial_RxBasket[0] == 4 && Serial_RxBasket[1] == 9)
		{
			Kapandji_Test_9();

		}
		if(Serial_RxBasket[0] == 5 && Serial_RxBasket[1] == 2)
		{
			Kapandji_Test_10();

		}
				if(Serial_RxBasket[0] == 5 && Serial_RxBasket[1] == 0 && Serial_RxBasket[2] == 0)
		{
			Kapandji_Test_11();

		}
		
		if(Serial_RxBasket[0] == 8 && Serial_RxBasket[1] == 0 && Serial_RxBasket[2] == 0)
		{
			Hand_grasp_1();

		}
		if(Serial_RxBasket[0] == 8 && Serial_RxBasket[1] == 0 && Serial_RxBasket[2] == 1)
		{
			Hand_grasp_2();

		}
		if(Serial_RxBasket[0] == 8 && Serial_RxBasket[1] == 0 && Serial_RxBasket[2] == 2)
		{
			Hand_grasp_3();

		}
		if(Serial_RxBasket[0] == 8 && Serial_RxBasket[1] == 0 && Serial_RxBasket[2] == 3)
		{
			Hand_grasp_4();

		}
		if(Serial_RxBasket[0] == 8 && Serial_RxBasket[1] == 0 && Serial_RxBasket[2] == 4)
		{
			Hand_grasp_5();

		}
		if(Serial_RxBasket[0] == 8 && Serial_RxBasket[1] == 0 && Serial_RxBasket[2] == 5)
		{
			Hand_grasp_6();
		}
		if(Serial_RxBasket[0] == 8 && Serial_RxBasket[1] == 0 && Serial_RxBasket[2] == 6)
		{
			Hand_grasp_7();

		}
		if(Serial_RxBasket[0] == 8 && Serial_RxBasket[1] == 0 && Serial_RxBasket[2] == 7)
		{
			Hand_grasp_8();

		}
		if(Serial_RxBasket[0] == 8 && Serial_RxBasket[1] == 0 && Serial_RxBasket[2] == 8)
		{
			Hand_grasp_9();
		}
		if(Serial_RxBasket[0] == 8 && Serial_RxBasket[1] == 0 && Serial_RxBasket[2] == 9)
		{
			Hand_grasp_10();
		}
		if(Serial_RxBasket[0] == 8 && Serial_RxBasket[1] == 1 && Serial_RxBasket[2] == 0)
		{
			Hand_grasp_11();

		}
		if(Serial_RxBasket[0] == 8 && Serial_RxBasket[1] == 1 && Serial_RxBasket[2] == 1)
		{
			Hand_grasp_12();

		}
		if(Serial_RxBasket[0] == 8 && Serial_RxBasket[1] == 1 && Serial_RxBasket[2] == 2)
		{
			Hand_grasp_13();
		}
		if(Serial_RxBasket[0] == 8 && Serial_RxBasket[1] == 1 && Serial_RxBasket[2] == 3)
		{
			Hand_grasp_14();

		}
		if(Serial_RxBasket[0] == 8 && Serial_RxBasket[1] == 1 && Serial_RxBasket[2] == 4)
		{
			Hand_grasp_15();

		}
		if(Serial_RxBasket[0] == 8 && Serial_RxBasket[1] == 1 && Serial_RxBasket[2] == 5)
		{
			Hand_grasp_16();
		}
		if(Serial_RxBasket[0] == 8 && Serial_RxBasket[1] == 1 && Serial_RxBasket[2] == 6)
		{
			Hand_grasp_17();
	}
				if(Serial_RxBasket[0] == 8 && Serial_RxBasket[1] == 1 && Serial_RxBasket[2] == 7)
		{
			Hand_grasp_18();

		}
		if(Serial_RxBasket[0] == 8 && Serial_RxBasket[1] == 1 && Serial_RxBasket[2] == 8)
		{
			Hand_grasp_19();

		}
		if(Serial_RxBasket[0] == 8 && Serial_RxBasket[1] == 1 && Serial_RxBasket[2] == 9)
		{
			Hand_grasp_20();

		}
		if(Serial_RxBasket[0] == 8 && Serial_RxBasket[1] == 2 && Serial_RxBasket[2] == 0)
		{
			Hand_grasp_21();

		}
		if(Serial_RxBasket[0] == 8 && Serial_RxBasket[1] == 2 && Serial_RxBasket[2] == 1)
		{
			Hand_grasp_22();

		}
		if(Serial_RxBasket[0] == 8 && Serial_RxBasket[1] == 2 && Serial_RxBasket[2] == 2)
		{
			Hand_grasp_23();
		}
		if(Serial_RxBasket[0] == 8 && Serial_RxBasket[1] == 2 && Serial_RxBasket[2] == 3)
		{
			Hand_grasp_24();

		}
		if(Serial_RxBasket[0] == 8 && Serial_RxBasket[1] == 2 && Serial_RxBasket[2] == 4)
		{
			Hand_grasp_25();

		}
		if(Serial_RxBasket[0] == 8 && Serial_RxBasket[1] == 2 && Serial_RxBasket[2] == 5)
		{
			Hand_grasp_26();
		}
		if(Serial_RxBasket[0] == 8 && Serial_RxBasket[1] == 2 && Serial_RxBasket[2] == 6)
		{
			Hand_grasp_27();
		}
		if(Serial_RxBasket[0] == 8 && Serial_RxBasket[1] == 2 && Serial_RxBasket[2] == 7)
		{
			Hand_grasp_28();

		}
		if(Serial_RxBasket[0] == 8 && Serial_RxBasket[1] == 2 && Serial_RxBasket[2] == 8)
		{
			Hand_grasp_29();

		}
		if(Serial_RxBasket[0] == 8 && Serial_RxBasket[1] == 2 && Serial_RxBasket[2] == 9)
		{
			Hand_grasp_30();
		}
		if(Serial_RxBasket[0] == 8 && Serial_RxBasket[1] == 3 && Serial_RxBasket[2] == 0)
		{
			Hand_grasp_31();

		}
		if(Serial_RxBasket[0] == 8 && Serial_RxBasket[1] == 3 && Serial_RxBasket[2] == 1)
		{
			Hand_grasp_32();

		}
		if(Serial_RxBasket[0] == 8 && Serial_RxBasket[1] == 3 && Serial_RxBasket[2] == 2)
		{
			Hand_grasp_33();
		}
		if(Serial_RxBasket[0] == 8 && Serial_RxBasket[1] == 3 && Serial_RxBasket[2] == 3)
		{
			Hand_grasp_34();
	}
		
					if(Serial_RxBasket[0] == 8 && Serial_RxBasket[1] == 3 && Serial_RxBasket[2] == 4)
		{
			Hand_grasp_35();

		}
		if(Serial_RxBasket[0] == 8 && Serial_RxBasket[1] == 3 && Serial_RxBasket[2] == 5)
		{
			Hand_grasp_36();

		}
		if(Serial_RxBasket[0] == 8 && Serial_RxBasket[1] == 3 && Serial_RxBasket[2] == 6)
		{
			Hand_grasp_37();

		}
		if(Serial_RxBasket[0] == 8 && Serial_RxBasket[1] == 3 && Serial_RxBasket[2] == 7)
		{
			Hand_grasp_38();

		}
		if(Serial_RxBasket[0] == 8 && Serial_RxBasket[1] == 3 && Serial_RxBasket[2] == 8)
		{
			Hand_grasp_39();

		}
		if(Serial_RxBasket[0] == 8 && Serial_RxBasket[1] == 3 && Serial_RxBasket[2] == 9)
		{
			Hand_grasp_40();
		}
		if(Serial_RxBasket[0] == 8 && Serial_RxBasket[1] == 4 && Serial_RxBasket[2] == 0)
		{
			Hand_grasp_41();

		}
		if(Serial_RxBasket[0] == 8 && Serial_RxBasket[1] == 4 && Serial_RxBasket[2] == 1)
		{
			Hand_grasp_42();

		}
		if(Serial_RxBasket[0] == 8 && Serial_RxBasket[1] == 4 && Serial_RxBasket[2] == 2)
		{
			Hand_grasp_43();
		}
		if(Serial_RxBasket[0] == 8 && Serial_RxBasket[1] == 4 && Serial_RxBasket[2] == 3)
		{
			Hand_grasp_44();
		}
		if(Serial_RxBasket[0] == 8 && Serial_RxBasket[1] == 4 && Serial_RxBasket[2] == 4)
		{
			Hand_grasp_45();

		}
		if(Serial_RxBasket[0] == 8 && Serial_RxBasket[1] == 4 && Serial_RxBasket[2] == 5)
		{
			Hand_grasp_46();

		}
		if(Serial_RxBasket[0] == 8 && Serial_RxBasket[1] == 4 && Serial_RxBasket[2] == 6)
		{
			Hand_grasp_47();
		}
		if(Serial_RxBasket[0] == 8 && Serial_RxBasket[1] == 4 && Serial_RxBasket[2] == 7)
		{
			Hand_grasp_48();

		}
		if(Serial_RxBasket[0] == 8 && Serial_RxBasket[1] == 4 && Serial_RxBasket[2] == 8)
		{
			Hand_grasp_49();

		}
		if(Serial_RxBasket[0] == 8 && Serial_RxBasket[1] == 4 && Serial_RxBasket[2] == 9)
		{
			Hand_grasp_50();
		}
		if(Serial_RxBasket[0] == 8 && Serial_RxBasket[1] == 5 && Serial_RxBasket[2] == 0)
		{
			Hand_grasp_51();
	}
		if(Serial_RxBasket[0] == 8 && Serial_RxBasket[1] == 5 && Serial_RxBasket[2] == 1)
		{
			Hand_grasp_52();
	}
		
		if(Serial_RxBasket[0] == 9 && Serial_RxBasket[1] == 0 && Serial_RxBasket[2] == 0)
		{
			Stable_grasp1();
	}
		if(Serial_RxBasket[0] == 9 && Serial_RxBasket[1] == 0 && Serial_RxBasket[2] == 1)
		{
			Stable_grasp2();
	}
				if(Serial_RxBasket[0] == 9 && Serial_RxBasket[1] == 0 && Serial_RxBasket[2] == 2)
		{
			Stable_grasp3();
	}
				if(Serial_RxBasket[0] == 9 && Serial_RxBasket[1] == 0 && Serial_RxBasket[2] == 3)
		{
			Stable_grasp4();
	}
				if(Serial_RxBasket[0] == 9 && Serial_RxBasket[1] == 0 && Serial_RxBasket[2] == 4)
		{
			Stable_grasp5();
	}
		if(Serial_RxBasket[0] == 9 && Serial_RxBasket[1] == 0 && Serial_RxBasket[2] == 5)
		{
			Stable_grasp6();
	}
				if(Serial_RxBasket[0] == 9 && Serial_RxBasket[1] == 0 && Serial_RxBasket[2] == 6)
		{
			Stable_grasp7();
	}
				if(Serial_RxBasket[0] == 9 && Serial_RxBasket[1] == 0 && Serial_RxBasket[2] == 7)
		{
			Stable_grasp8();
	}
						if(Serial_RxBasket[0] == 9 && Serial_RxBasket[1] == 1 && Serial_RxBasket[2] == 0)
		{
			Stable_grasp9();
	}
						if(Serial_RxBasket[0] == 9 && Serial_RxBasket[1] == 1 && Serial_RxBasket[2] == 1)
		{
			Stable_grasp10();
	}
						if(Serial_RxBasket[0] == 9 && Serial_RxBasket[1] == 1 && Serial_RxBasket[2] == 2)
		{
			Stable_grasp11();
	}
						if(Serial_RxBasket[0] == 9 && Serial_RxBasket[1] == 1 && Serial_RxBasket[2] == 3)
		{
			Stable_grasp12();
	}
		
							if(Serial_RxBasket[0] == 9 && Serial_RxBasket[1] == 1 && Serial_RxBasket[2] == 4)
		{
			Stable_grasp13();
	}
									if(Serial_RxBasket[0] == 9 && Serial_RxBasket[1] == 1 && Serial_RxBasket[2] == 5)
		{
			Stable_grasp14();
	}
	if(Serial_RxBasket[0] == 9 && Serial_RxBasket[1] == 1 && Serial_RxBasket[2] == 6 && Serial_RxBasket[3] == 0 && Serial_RxBasket[4] == 0)
	{
		Motor11_Reciprocate_Once();
	}
	if(Serial_RxBasket[0] == 9 && Serial_RxBasket[1] == 1 && Serial_RxBasket[2] == 7 && Serial_RxBasket[3] == 0 && Serial_RxBasket[4] == 0)
	{
		Motor11_StepPause_Once();
	}
	if(Serial_RxBasket[0] == 9 && Serial_RxBasket[1] == 1 && Serial_RxBasket[2] == 8 && Serial_RxBasket[3] == 0 && Serial_RxBasket[4] == 0)
	{
		Motor24_Reach500Pause_Once();
	}
	if(Serial_RxBasket[0] == 9 && Serial_RxBasket[1] == 1 && Serial_RxBasket[2] == 9 && Serial_RxBasket[3] == 0 && Serial_RxBasket[4] == 0)
	{
		Motor14_Reset_1600();
	}
}


void NormalGrasp(void)
{	
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 2709, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 3512, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 1058, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 1808, 500, 20);
	
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 2100, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 1781, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 2778, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 3329,  500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 1073, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 2791, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 2410 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 2733, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 1900, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 1807, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 980, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 2451, 500, 20);

}



void Number1(void)
{	
		//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 256, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 3000, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 1400, 500, 20);
	Delay_ms(1500);
	
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 3253, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 3005, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 3453, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 561, 500, 20);
//	//��ָMCP�ؽ�����ҡ��
//	WritePosEx(21, 1103, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 2319, 500, 20);
	
	
//	//����ָPIP�ؽ�����ҡ��
//	WritePosEx(33, 256, 500, 20);
//	//����ָMCP�ؽ�����ҡ��
//	WritePosEx(30, 3000, 500, 20);
//	//����ָMCP�ؽ�����ҡ��
//	WritePosEx(27, 1400, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 150, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 1600, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 1800, 500, 20);
	
	Delay_ms(1700);
	
		//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 1103, 500, 20);
	
	Delay_ms(1500);
	
		//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 1419, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 2687, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 646, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 597, 500, 20);
	

	
}


void Number2(void)
{	
	
		//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 200, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 3500, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 1400, 500, 20);
	
	Delay_ms(1000);
	
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 1800, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 2687, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 1800, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1,338, 500, 20);
	
	
//	//ʳָPIP�ؽ�����ҡ��
//	WritePosEx(11, 3253, 500, 20);
//	//ʳָMCP�ؽ�����ҡ��
//	WritePosEx(14, 3005, 500, 20);
//	//ʳָMCP�ؽ�����ҡ��
//	WritePosEx(17, 3453, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 3517, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 1100, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 3098, 500, 20);

	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 150, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 1600, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 1700, 500, 20);
	
	Delay_ms(800);
	
		//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 1400, 500, 20);

	
}

void Number3(void)
{	
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 1000, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 2687, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 2108, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1,338, 500, 20);
	
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 3253, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 3005, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 3453, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 3517, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 1200, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 3098, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 2900, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 2737, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 3100, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 150, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 1600, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 1900, 500, 20);

	
}

void Number4(void)
{	
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 790, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 2687, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 2242, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1,338, 500, 20);


	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 2514, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 1600, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36,3714, 500, 20);
	
		//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 3500 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 2900, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 2700, 500, 20);
	
		//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 3700, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 1200, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 3700, 500, 20);

	
}

void Number5(void)
{	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 2514, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 990, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36,3714, 500, 20);
	
		//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 3500 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 2900, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 2700, 500, 20);
	
		//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 3700, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 1200, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 3700, 500, 20);

	
}


void Number6(void)
{	
	
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 2600, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 2126, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 646, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 1912, 500, 20);
	
	
		//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 699, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 2510, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 1756, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 894, 500, 20);
//	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 1200, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 1200, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 400, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30,3200, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 638, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 2500, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 1177, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 3656, 500, 20);
	
	Delay_ms(2500);
		//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 700, 500, 20);

	
}


void Number7(void)
{	
	
		//��ָMCP�ؽ�����ҡ��
	WritePosEx(21,1300, 500, 20);
	
	Delay_ms(500);
	
		//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 50, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30,2806, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 538, 500, 20);
	
	Delay_ms(1000);
	
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 1800, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 2520, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5,1225, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1,100, 500, 20);
	
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 1463, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 519, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 2971, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 1260, 500, 20);
//	//��ָMCP�ؽ�����ҡ��
//	WritePosEx(21,1040, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 3152, 500, 20);
	
	
//	//����ָPIP�ؽ�����ҡ��
//	WritePosEx(33, 50, 500, 20);
//	//����ָMCP�ؽ�����ҡ��
//	WritePosEx(30,2806, 500, 20);
//	//����ָMCP�ؽ�����ҡ��
//	WritePosEx(27, 538, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 35, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 1600, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 1684, 500, 20);


}

void Number8(void)
{	
	
		//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 200, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 3291, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 869, 500, 20);
	
	Delay_ms(1000);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 2600, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 2126, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 646, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 1912, 500, 20);
	
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 3255, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 3000, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 3455, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 822, 500, 20);
//	//��ָMCP�ؽ�����ҡ��
//	WritePosEx(21, 1907, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 1774, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 200, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 3291, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 869, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 149, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 1600, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 2710, 500, 20);
	
	Delay_ms(500);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 1200, 500, 20);

}


void Number9(void)
{	
//	//��ĴָMCP�ؽ�����ҡ��
//	WritePosEx(8, 645, 500, 20);
//	//��ĴָMCP�ؽ�����ҡ��
//	WritePosEx(3, 2645, 500, 20);
//	//��ĴָCMC�ؽ�����ҡ��
//	WritePosEx(5, 652, 500, 20);
//	//��ĴָCMC�ؽ�����ҡ��
//	WritePosEx(1, 1541, 500, 20);
	
		//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 100, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 3247, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 971, 500, 20);
	
	Delay_ms(1000);
	
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 1968, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 3000, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 3455, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 700, 500, 20);
//	//��ָMCP�ؽ�����ҡ��
//	WritePosEx(21, 1910, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 1804, 500, 20);
	
	
//	//����ָPIP�ؽ�����ҡ��
//	WritePosEx(33, 100, 500, 20);
//	//����ָMCP�ؽ�����ҡ��
//	WritePosEx(30, 3247, 500, 20);
//	//����ָMCP�ؽ�����ҡ��
//	WritePosEx(27, 971, 500, 20);
//	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 150, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 1600, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 2710, 500, 20);
	
	Delay_ms(500);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 1189, 500, 20);
	
	Delay_ms(2000);
	
		//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 1800, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 2645, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 790, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 300, 500, 20);
	

}

void Number10(void)
{	
//		//��ĴָMCP�ؽ�����ҡ��
//	WritePosEx(8, 645, 500, 20);
//	//��ĴָMCP�ؽ�����ҡ��
//	WritePosEx(3, 2645, 500, 20);
//	//��ĴָCMC�ؽ�����ҡ��
//	WritePosEx(5, 652, 500, 20);
//	//��ĴָCMC�ؽ�����ҡ��
//	WritePosEx(1, 1753, 500, 20);
	
		//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 100, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 3247, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 978, 500, 20);
	
	Delay_ms(1000);
	
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 490, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 2995, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 1869, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 239, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 1200, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 1804, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 100, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 3247, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 978, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 150, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 1600, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 1482, 500, 20);
	
	Delay_ms(1900);
	
//			//��ĴָMCP�ؽ�����ҡ��
//	WritePosEx(8, 1845, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 2645, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 784, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 20, 500, 20);
	
	Delay_ms(1000);
				//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 1575, 500, 20);

}

void Kapandji_Test_1(void)
{	
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 1566, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 2430, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 652, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 1158, 500, 20);
	
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 3255, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 3000, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 3455, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 3416, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 2027, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 3230, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 2714 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 3384, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 3100, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 2500, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 1600, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 3656, 500, 20);

}

void Kapandji_Test_2(void)
{	
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 1601, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 2068, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 1057, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 1162, 500, 20);
	
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 2226, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 2522, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 2778, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 3416, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 2027, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 3230, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 2714 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 3384, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 3100, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 2500, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 1600, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 3656, 500, 20);

}

void Kapandji_Test_3(void)
{	
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 1600, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 2500, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 1373, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 1162, 500, 20);
	
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 1439, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 2522, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 2778, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 3416, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 2027, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 3230, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 2714 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 3384, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 3100, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 2500, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 1600, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 3656, 500, 20);

}


void Kapandji_Test_4(void)
{	
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 1281, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 2476, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 1486, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 537, 500, 20);
	
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 3253, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 2995, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 3453, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 1172, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 2131, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 3182, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 2714 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 3384, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 3100, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 2500, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 1600, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 3656, 500, 20);

}

void Kapandji_Test_5(void)
{		
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 158, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 1189, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 2476, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 1389, 500, 20);

	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 3253, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 2995, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 3453, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 3416, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 2027, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 3230, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 601 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 3382, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 1870, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 2500, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 1600, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 3656, 500, 20);

}


void Kapandji_Test_6(void)
{	
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 900, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 2935, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 1927, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 350, 500, 20);
	
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 3253, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 2995, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 3453, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 3416, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 2027, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 3230, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 2714 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 3384, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 3100, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 1000, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 2100, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 2508, 500, 20);

}

void Kapandji_Test_7(void)
{	
		//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 400, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 900, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 2700, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 1817, 500, 20);

	
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 3253, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 2995, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 3453, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 3416, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 2027, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 3230, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 2714 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 3384, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 3100, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 691, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 1600, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 1846, 500, 20);

}

void Kapandji_Test_8(void)
{	
		//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 400, 500, 20);
//	//��ĴָMCP�ؽ�����ҡ��
//	WritePosEx(8, 870, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 2628, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 1880, 500, 20);
		//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 750, 500, 20);

	
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 3253, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 2995, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 3453, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 3416, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 2027, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 3230, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 2714 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 3384, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 3100, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 174, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 1600, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 1846, 500, 20);
}

void Kapandji_Test_9(void)
{	
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 666, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 2744, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 1682, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 275, 500, 20);
	
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 3253, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 2995, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 3453, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 3416, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 2027, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 3230, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 2714 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 3384, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 3100, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 2500, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 1600, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 3656, 500, 20);

}

void Kapandji_Test_10(void)
{	
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 1555, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 2500, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 1373, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 747, 500, 20);
	
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 1919, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 2522, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 3354, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 3416, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 2027, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 3230, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 2714 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 3384, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 3100, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 2500, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 1600, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 3656, 500, 20);

}

void Kapandji_Test_11(void)
{	
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 160, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 3048, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 2510, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 2000, 500, 20);
	
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 3600, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 1300, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 3455, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 3700,  500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 1400, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 3700, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 3500 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 2800, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 2700, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 2121, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 1600, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 3656, 500, 20);

}

void Hand_grasp_1(void)
{
	
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 2500, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 3058, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 652, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 1200, 500, 20);
	
	Delay_ms(2000);
	
		//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 600 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 3381, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 1482, 500, 20);
	
	Delay_ms(1500);
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 1716, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 3163, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 2669, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 1629, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 1595, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 2491, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 600 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 3381, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 1482, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 658, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 1600, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 1590, 500, 20);
	
	Delay_ms(3500);
	
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 1500, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 3058, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 652, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 1200, 500, 20);
	
}

void Hand_grasp_2(void)
{
	
		//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 2000, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 3000, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 1000, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 1500, 500, 20);
	
	Delay_ms(2000);
	
		//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 10 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 3288, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 545, 500, 20);
	
	Delay_ms(1500);
	
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 1000, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 672, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17,1875, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 634, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 1200, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 1509, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 10 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 3288, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 545, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 10, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 1600, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 1170, 500, 20);
	
	Delay_ms(5000);
	
		//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 1000, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 3000, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 2200, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 1500, 500, 20);
	
	
}
void Hand_grasp_3(void)
{
	
		//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 2975, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 2892, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 652, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 162, 500, 20);
	
	Delay_ms(2000);	
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 1475, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 2350, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 2617, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 868, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 1350, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 1980, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 781 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 3032, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 965, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 586, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 1600, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 2288, 500, 20);
	
	Delay_ms(3000);
	
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 1200, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 2892, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 652, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 162, 500, 20);
	
}
void Hand_grasp_4(void)
{
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 2600, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 2126, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 646, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 1912, 500, 20);
	
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 1580, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 2607, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 2940, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 169, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 1869, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 2541, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 600 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 3387, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 1034, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 639, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 1600, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 2398, 500, 20);
	
}
void Hand_grasp_5(void)
{
	
		//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 10 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 2800, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 1900, 500, 20);
	
	Delay_ms(1000);
	
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 2600, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 2126, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 646, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 2500, 500, 20);
	
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 864, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 1300, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 2583, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 425, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 1200, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 3230, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 10 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 2800, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 1900, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 10, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 1528, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 3656, 500, 20);
}
void Hand_grasp_6(void)
{
	

	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 1900, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 2600, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 2100, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 2000, 500, 20);
	
	Delay_ms(3000);
	
		//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 1100 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 3384, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 3100, 500, 20);
	
	Delay_ms(1500);
	
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 2100, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 2606, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 3455, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 2100, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 1200, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 3230, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 1100 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 3384, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 3100, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 1176, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 1700, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 2739, 500, 20);
}
void Hand_grasp_7(void)
{	
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 1290, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 2314, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 1570, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 550, 500, 20);
	
	Delay_ms(3000);
	
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 1940, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 2606, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 3453, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 1406, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 1865, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 3230, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 1141 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 3387, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 3100, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 2500, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 1528, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 3656, 500, 20);
	
}
void Hand_grasp_8(void)
{
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 1400, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 2314, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 1570, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 550, 500, 20);
	
	Delay_ms(3000);
	
		
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 2133, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 2606, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 3453, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 1700, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 1569, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 3230, 500, 20);
	
	
	
}
void Hand_grasp_9(void)
{
		//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 1500, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 2430, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 1410, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 743, 500, 20);
	
	Delay_ms(2000);
	
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 1872, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 2602, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 3453, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 3416, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 1800, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 3230, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 2714 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 3384, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 3100, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 2500, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 1700, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 3656, 500, 20);
}
void Hand_grasp_10(void)
{
	
			//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 1873, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 2391, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 652, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 1065, 500, 20);
	
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 1600, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 2606, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 3453, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 1200, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 1841, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 3230, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 1106 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 3384, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 3100, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 534, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 2246, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 2849, 500, 20);

}
void Hand_grasp_11(void)
{
		//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 1282, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 2636, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 646, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 297, 500, 20);
	
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 1342, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 3000, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 2498, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 933, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 1767, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 2237, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 890 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 3384, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 1111, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 91, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 1705, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 2300, 500, 20);
}
void Hand_grasp_12(void)
{
		//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 1100, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 2550, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 537, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 171, 500, 20);
	
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 1875, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 2595, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 2752, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 2000, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 1725, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 2775, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 1800 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 3000, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 1661, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 990, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 1801, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 2527, 500, 20);
}
void Hand_grasp_13(void)
{
	
		//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 1072 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 2733, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 1736, 500, 20);
	
	Delay_ms(1500);
	
		//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 1900, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 2804, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 1456, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 1685, 500, 20);
	
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 1928, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 2239, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 2945, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 2200, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 1225, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 3000, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 1072 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 2733, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 1736, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 1008, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 1062, 500, 20);
	
	Delay_ms(1500);
	
		//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 2000, 500, 20);
}
void Hand_grasp_14(void)
{
		//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 1705, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 2405, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 1346, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 481, 500, 20);
	
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 2300, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 2492, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 2392, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 1107, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 1782, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 2581, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 355 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 3384, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 673, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 100, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 1706, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 1800, 500, 20);
}
void Hand_grasp_15(void)
{
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 2600, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 2500, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 646, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 1000, 500, 20);
	
	Delay_ms(1500);
	
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 524, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 2996, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 2681, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 200, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 1710, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 2408, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 50 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 3387, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 852, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 100, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 1706, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 2008, 500, 20);
}
void Hand_grasp_16(void)
{
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 1200, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 2126, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 646, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 1223, 500, 20);
	
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 693, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 2489, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 2615, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 100, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 1900, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 2451, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 138 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 3387, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 1388, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 306, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 1706, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 3043, 500, 20);
}
void Hand_grasp_17(void)
{
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 2600, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 2126, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 646, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 1912, 500, 20);
	
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 3654, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 3000, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 3455, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 167, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 1869, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 2541, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 156 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 3387, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 684, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 75, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 1697, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 1698, 500, 20);
}

void Hand_grasp_18(void)
{	
	
		//��ָMCP�ؽ�����ҡ��
//	WritePosEx(21, 1698, 500, 20);
//	
//	Delay_ms(1500);
	
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 1100, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 2601, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 1399, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 304, 500, 20);
	
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 1828, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 2277, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 1999, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 1432, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 1322, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 2107, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 1322, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 3387, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 1121, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 1139, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 1699, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 2649, 500, 20);

}

void Hand_grasp_19(void)
{	
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 1740, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 2777, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 1352, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 926, 500, 20);
	
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 2249, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 2491, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 3453, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 1428, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 1909, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 3230, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 2714 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 3384, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 3100, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 2500, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 1700, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 3656, 500, 20);

}

void Hand_grasp_20(void)
{	
		//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 1500, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 2405, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��		//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 1500, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 2405, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 1346, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 982, 500, 20);
	
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 2300, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 2492, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 2392, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 1610, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 2251, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 2576, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 355 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 3384, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 1806, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 100, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 1706, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 1800, 500, 20);
	WritePosEx(5, 1346, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 982, 500, 20);
	
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 2300, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 2492, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 2392, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 1610, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 2251, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 2576, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 355 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 3384, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 1806, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 100, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 1706, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 1800, 500, 20);

}

void Hand_grasp_21(void)
{	
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 712, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 2206, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 1528, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 100, 500, 20);
	
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 1555, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 1900, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 3455, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 643, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 1831, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 2615, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 602 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 3384, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 1414, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 350, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 1700, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 1990, 500, 20);
	
	Delay_ms(2500);
		//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 300, 500, 20);
	
		Delay_ms(5000);
		//ʳָMCP�ؽ�����ҡ��
			WritePosEx(11, 1350, 500, 20);
	WritePosEx(18, 350, 500, 20);
	
//			Delay_ms(2500);
//		//ʳָMCP�ؽ�����ҡ��
//	WritePosEx(18, 643, 500, 20);
//	
//			Delay_ms(2500);
//		//ʳָMCP�ؽ�����ҡ��
//	WritePosEx(18, 400, 500, 20);
//	
//				Delay_ms(2500);
//		//ʳָMCP�ؽ�����ҡ��
//	WritePosEx(18, 643, 500, 20);

}

void Hand_grasp_22(void)
{	
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 970, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 2600, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 1490, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 286, 500, 20);
	
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 3654, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 3000, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 3455, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 3416, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 1831, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 3230, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 2714 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 3384, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 3100, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 2500, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 1700, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 3656, 500, 20);
	
	
	Delay_ms(1500);
		//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 1900, 500, 20);

}

void Hand_grasp_23(void)
{	
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 2600, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 2126, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 646, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 1912, 500, 20);
	
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 3654, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 2000, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 3455, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 3416, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 1818, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 3230, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 2714 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 3384, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 3100, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 2500, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 1700, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 3656, 500, 20);

}

void Hand_grasp_24(void)
{	
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 1485, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 2468, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 1584, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 744, 500, 20);
	
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 2000, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 2482, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 3000, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 3416, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 1781, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 3230, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 2714 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 3384, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 3100, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 2500, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 1700, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 3656, 500, 20);

}

void Hand_grasp_25(void)
{	
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 1320, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 2234, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 1351, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 584, 500, 20);
	
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 1900, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 2569, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 3455, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 1000, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 1910, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 2713, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 397 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 3384, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 1599, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 37, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 1700, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 1663, 500, 20);

}

void Hand_grasp_26(void)
{	
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 1500, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 2700, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 1367, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 149, 500, 20);
	
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 1300, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 3000, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 2604, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 1118, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 1700, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 2658, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 700 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 2918, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 1420, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 186, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 1347, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 2062, 500, 20);

}

void Hand_grasp_27(void)
{	
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 1300, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 2700, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 1648, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 178, 500, 20);
	
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 2212, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 2343, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 2024, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 3000, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 1511, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 2111, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 1798 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 3387, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 1043, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 186, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 1347, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 1759, 500, 20);

}

void Hand_grasp_28(void)
{	
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 1709, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 2641, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 1083, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 536, 500, 20);
	
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 1744, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 2996, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 3453, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 921, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 1572, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 3326, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 2712 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 3385, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 3095, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 2495, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 1697, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 3653, 500, 20);

}

void Hand_grasp_29(void)
{	
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 2600, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 2126, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 646, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 1912, 500, 20);
	
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 1400, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 2996, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 3455, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 307, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 1329, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 2516, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 61 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 3384, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 327, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 130, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 1697, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 2120, 500, 20);

}

void Hand_grasp_30(void)
{	
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 1495, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 2126, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 646, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 1200, 500, 20);
	
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 1032, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 3000, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 2740, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 370, 500, 20);
//	//��ָMCP�ؽ�����ҡ��
//	WritePosEx(21, 1600, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 2527, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 498 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 3385, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 1970, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 748, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 1697, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 2867, 500, 20);
	
	Delay_ms(800);
	
		//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 1700, 500, 20);

}

void Hand_grasp_31(void)
{	
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 1900, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 2936, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 646, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 300, 500, 20);
	
	Delay_ms(1500);
	
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 1387, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 2380, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 2743, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 3416, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 1814, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 3230, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 2714 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 3384, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 3100, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 2500, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 1700, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 3656, 500, 20);
	
	Delay_ms(3000);
		//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 1500, 500, 20);

}

void Hand_grasp_32(void)
{	
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 2600, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 2126, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 646, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 1912, 500, 20);
	
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 3398, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 3000, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 3455, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 127, 500, 20);
//	//��ָMCP�ؽ�����ҡ��
//	WritePosEx(21, 1670, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 1337, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 102 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 3384, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 378, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 71, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 1700, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 1825, 500, 20);
	
	Delay_ms(1000);
		//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 1237, 500, 20);

}

void Hand_grasp_33(void)
{	
	
		//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 1583, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 363, 500, 20);
	
	Delay_ms(1500);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 2596, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 2517, 500, 20);
//	//��ĴָCMC�ؽ�����ҡ��
//	WritePosEx(5, 1583, 500, 20);
//	//��ĴָCMC�ؽ�����ҡ��
//	WritePosEx(1, 363, 500, 20);
//	
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 2735, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 3000, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 2423, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 3416, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 1951, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 3230, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 2714 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 3384, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 3100, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 2500, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 1700, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 3656, 500, 20);

}

void Hand_grasp_34(void)
{	
	
		//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 1600, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 2414, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 646, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 736, 500, 20);
	
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 2305, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 3000, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 3455, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 3416, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 1951, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 3230, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 2714 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 3384, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 3100, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 2500, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 1700, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 3656, 500, 20);

}

void Hand_grasp_35(void)
{	
	
		//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 1500, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 2414, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 646, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 215, 500, 20);
	
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 1913, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 2541, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 3234, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 1556, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 1879, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 3191, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 2714 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 3384, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 3100, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 2500, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 1700, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 3656, 500, 20);

}

void Hand_grasp_36(void)
{	
		//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 1100, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 2948, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 1200, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 293, 500, 20);
	
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 1511, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 2541, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 2692, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 1556, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 1879, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 3191, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 1110 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 3384, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 1899, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 2500, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 1700, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 3656, 500, 20);

}

void Hand_grasp_37(void)
{	
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 900, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 3203, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 885, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 50, 500, 20);
	
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 1753, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 2113, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 2773, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 1538, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 1853, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 3230, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 1381 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 2856, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 2170, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 779, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 2096, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 2210, 500, 20);

}

void Hand_grasp_38(void)
{	
		//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 1100, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 2405, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 1346, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 982, 500, 20);
	
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 2300, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 2492, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 2392, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 1610, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 2251, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 2576, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 355 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 3384, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 1806, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 100, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 1706, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 1800, 500, 20);
	
	Delay_ms(5000);
	
		//ʳָMCP�ؽ�����ҡ��
		WritePosEx(11, 2100, 500, 20);
	WritePosEx(18, 1200, 500, 20);
	
	Delay_ms(1000);
	
		//ʳָMCP�ؽ�����ҡ��
		WritePosEx(11, 2300, 500, 20);
	WritePosEx(18, 1610, 500, 20);
	
		Delay_ms(1000);
	
		//ʳָMCP�ؽ�����ҡ��
		WritePosEx(11, 2100, 500, 20);
	WritePosEx(18, 1200, 500, 20);
	
			Delay_ms(1000);
	
		//ʳָMCP�ؽ�����ҡ��
		WritePosEx(11, 2300, 500, 20);
	WritePosEx(18, 1610, 500, 20);
	
	
	

}

void Hand_grasp_39(void)
{	
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 2600, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 1894, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 646, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 1912, 500, 20);
	
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 3400, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 3000, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 3455, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 3416, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 2300, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 3230, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 2714 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 3384, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 3100, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 2500, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 1700, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 3656, 500, 20);

}

void Hand_grasp_40(void)
{	
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 1600, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 2534, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 1392, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 343, 500, 20);
	
	Delay_ms(2000);
	
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 2200, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 2426, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 2684, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 3416, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 1847, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 3230, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 2714 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 3384, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 3100, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 2500, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 1700, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 3656, 500, 20);

}

void Hand_grasp_41(void)
{	
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 1380, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 2534, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 1392, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 16, 500, 20);
	
	Delay_ms(2000);
	
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 2100, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 2426, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 2684, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 1900, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 1600, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 2442, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 2714 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 3384, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 3100, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 2500, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 1700, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 3656, 500, 20);

}

void Hand_grasp_42(void)
{	
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 1100, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 2884, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 1392, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 10, 500, 20);
	
	Delay_ms(2000);
	
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 2200, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 2426, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 2684, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 1900, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 1600, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 2442, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 1700 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 3384, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 1520, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 2500, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 1700, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 3656, 500, 20);

}

void Hand_grasp_43(void)
{	
	
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 1000, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 3022, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 1713, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 777, 500, 20);
	
	Delay_ms(2000);
	
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 2200, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 2426, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 2684, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 1894, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 1600, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 2442, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 1700 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 3384, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 1520, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 1702, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 1700, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 3061, 500, 20);

}

void Hand_grasp_44(void)
{	

	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 530, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 3000, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 2549, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 100, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 1746, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 2440, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 20 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 3384, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 1398, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 298, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 1700, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 3035, 500, 20);
	
	Delay_ms(5000);
	
		//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 2170, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 1647, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 646, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 1307, 500, 20);
	
	Delay_ms(1500);
	
			//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 1000, 500, 20);
	

}

void Hand_grasp_45(void)
{	
	
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 973, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 2372, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 1990, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 550, 500, 20);
//	//��ָMCP�ؽ�����ҡ��
//	WritePosEx(21, 1483, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 1815, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 651 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 3381, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 705, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 641, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 1700, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 2856, 500, 20);
	
	Delay_ms(500);
	
		//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 1483, 500, 20);
	
	Delay_ms(3000);
	
		//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 1100, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 2123, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 646, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 525, 500, 20);

}

void Hand_grasp_46(void)
{	
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 2169, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 2479, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 646, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 654, 500, 20);
	
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 1558, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 2435, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 2944, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 3416, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 1825, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 3230, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 2714 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 3384, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 3100, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 2500, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 1700, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 3656, 500, 20);

}

void Hand_grasp_47(void)
{	
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 2000, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 2479, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 646, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 654, 500, 20);
	
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 1558, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 2435, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 2944, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 868, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 1253, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 2656, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 2714 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 3384, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 3100, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 2500, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 1700, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 3656, 500, 20);

}

void Hand_grasp_48(void)
{	
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 1300, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 2474, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 646, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 659, 500, 20);
	
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 2150, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 2980, 500, 20);
	//ʳָMCP�ؽ�����ҡ��0
	WritePosEx(17, 2946, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 1762, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 1897, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 3095, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 838 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 3384, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 1183, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 379, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 1700, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 2776, 500, 20);

}

void Hand_grasp_49(void)
{	
		//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 10, 500, 20);
		//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 3587, 500, 20);
	
	Delay_ms(1500);
	
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 1400, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 3781, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 1890, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 10, 500, 20);
	
	Delay_ms(3000);
	
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 2237, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 2400, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 3305, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 2322, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 1894, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 3175, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 2221 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 3382, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 1821, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 2500, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 1987, 500, 20);
	
	Delay_ms(2000);
	
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 2247, 500, 20);

}

void Hand_grasp_50(void)
{	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 973, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 2372, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 1990, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 500, 500, 20);
//	//��ָMCP�ؽ�����ҡ��
//	WritePosEx(21, 1483, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 1815, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 651 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 3381, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 705, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 641, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 1700, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 2856, 500, 20);
	
	Delay_ms(500);
	
		//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 1483, 500, 20);
	
	Delay_ms(3000);
	
		//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 1200, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 2123, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 646, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 370, 500, 20);

}

void Hand_grasp_51(void)
{	
	
		//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 20, 500, 20);
	
		//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 3995, 500, 20);
	
	Delay_ms(1500);
	
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 1600, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 3995, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 1752, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 20, 500, 20);
	
	Delay_ms(2000);
	
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 1736, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 3000, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 2413, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 3416, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 1835, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 3230, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 2714 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 3384, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 3100, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 2500, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 1700, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 3656, 500, 20);

}

void Hand_grasp_52(void)
{	
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 3300, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 3100, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 2200, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 800, 500, 20);
	
	Delay_ms(1500);
	
		//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 666 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 2798, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 2696, 500, 20);
	
	Delay_ms(1500);
	
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 1571, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 1304, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 3453, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 1956, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 1019, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 3698, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 666 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 2798, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 2696, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 617, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 1605, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 3072, 500, 20);

}


void Stable_grasp1(void)
{	
	
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 1400, 500, 20);
	
	Delay_ms(1500);
	

	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 1930, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 2455, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 1328, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 1400, 500, 20);
	
	Delay_ms(1500);
	
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 2006, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 1804, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 2333, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 1639, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 692, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 2258, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 3500 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 2800, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 2700, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 2121, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 1600, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 3656, 500, 20);

}

void Stable_grasp2(void)
{
	
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 450, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 2800, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 1334, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 1000, 500, 20);
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 700, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 1804, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 2333, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 400, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 1124, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 1200, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 2712 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 3385, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 2707, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 2496, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 1604, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 3654, 500, 20);

}

void Stable_grasp3(void)
{	

	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 200, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 2800, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 1334, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 1000, 500, 20);
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 500, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 1804, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 2333, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 250, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 1124, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 1200, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 2712 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 3385, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 2707, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 2496, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 1604, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 3654, 500, 20);

}

void Stable_grasp4(void)
{	
	
	

	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8,50 , 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 2800, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 1334, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 1000, 500, 20);
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 50, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 1804, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 2333, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 50, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 1124, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 1200, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 2712 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 3385, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 2707, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 2496, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 1604, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 3654, 500, 20);

}

void Stable_grasp5(void)
{	
	
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 627, 500, 20);
	
	Delay_ms(1500);
	

	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 1200, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 2604, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 652, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 627, 500, 20);
	Delay_ms(1500);
	
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 950, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 1303, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 1884, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 600, 500, 20);
//	//��ָMCP�ؽ�����ҡ��
//	WritePosEx(21, 1105, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 1900, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 1100 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 3385, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 1202, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 1100, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 1604, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 2647, 500, 20);
	
	Delay_ms(1500);
		//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 1105, 500, 20);

}

void Stable_grasp6(void)
{	
	
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 600, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 2604, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 652, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 627, 500, 20);
	
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 300, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 1303, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 1884, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 300, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 1105, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 1900, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 600 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 3385, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 1202, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 600, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 1604, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 2647, 500, 20);

}

void Stable_grasp7(void)
{	
	
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 10, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 2604, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 652, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 627, 500, 20);
	
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 10, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 1303, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 1884, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 10, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 1105, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 1900, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 10 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 3385, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 1202, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 10, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 1604, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 2647, 500, 20);

}

void Stable_grasp8(void)
{	
	
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 1000, 500, 20);
	
	Delay_ms(1500);
	

	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 800, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 2800, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 1334, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 1000, 500, 20);
	
	Delay_ms(1500);
	
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 1000, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 1804, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 2333, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 800, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 1124, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 1200, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 2712 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 3385, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 2707, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 2496, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 1604, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 3654, 500, 20);

}


void Stable_grasp9(void)
{	
	
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 1300, 500, 20);
	
	Delay_ms(1000);
	
	WritePosEx(3, 3900, 500, 20);
}


void Stable_grasp10(void)
{	
	
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 1600, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 3900, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 652, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 1300, 500, 20);
	
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 584, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 1303, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 980, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 875, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 1224, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 1398, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 616 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 3385, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 823, 500, 20);
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 400, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 1604, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 2000, 500, 20);
}

void Stable_grasp11(void)
{	
	
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 1300, 500, 20);
	
	WritePosEx(3, 3900, 500, 20);
}



void Stable_grasp12(void)
{	
	
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 2000, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 4000, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 257, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 1078, 500, 20);
	
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 1700, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 1300, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 1783, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 1700, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 1000, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 1964, 500, 20);
	

}


void Stable_grasp13(void)
{	
		//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 21, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 986, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 641, 500, 20);
	
	
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 2500, 500, 20);
	
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 4000, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 1050, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 1200, 500, 20);
	
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 600, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 1700, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 1570, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 1600, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 1300, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 2000, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 300 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 2700, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 300, 500, 20);
	
}


void Stable_grasp14(void)
{	
//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(8, 1600, 500, 20);
	//��ĴָMCP�ؽ�����ҡ��
	WritePosEx(3, 3900, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(5, 652, 500, 20);
	//��ĴָCMC�ؽ�����ҡ��
	WritePosEx(1, 1300, 500, 20);
	
	
	//ʳָPIP�ؽ�����ҡ��
	WritePosEx(11, 584, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(14, 1303, 500, 20);
	//ʳָMCP�ؽ�����ҡ��
	WritePosEx(17, 980, 500, 20);
	
	
	//��ָPIP�ؽ�����ҡ��
	WritePosEx(18, 875, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(21, 1224, 500, 20);
	//��ָMCP�ؽ�����ҡ��
	WritePosEx(24, 1398, 500, 20);
	
	
	//����ָPIP�ؽ�����ҡ��
	WritePosEx(33, 400 , 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(30, 3385, 500, 20);
	//����ָMCP�ؽ�����ҡ��
	WritePosEx(27, 823, 500, 20);
	
	
	//СĴָPIP�ؽ�����ҡ��
	WritePosEx(42, 600, 500, 20);
	//СĴָMCP�ؽ�����ҡ��
	WritePosEx(39, 1604, 500, 20);
    //СĴָMCP�ؽ�����ҡ��
	WritePosEx(36, 1800, 500, 20);
	
}

static uint8_t Motor11_CheckStop(void)
{
	Key_Control(Servo_ID);
	if(g_motion_stop_flag || Key_StopRequested())
	{
		ServoStop(11);
		Serial_ClearRxFlag();
		USART3_SendString("M11,STOP,0,0,0\r\n");
		return 1;
	}
	return 0;
}

static uint8_t Motor11_WaitTarget(uint16_t target)
{
	char buf[64];
	uint8_t stable_count = 0;
	int pos;
	int diff;

	WritePosEx(14, target, 600, 20);
	for(uint16_t wait = 0; wait < 200; wait++)
	{
		for(uint8_t d = 0; d < 10; d++)
		{
			Delay_ms(5);
			if(Motor11_CheckStop())
			{
				return 1;
			}
		}

		pos = ReadPos(14);
		sprintf(buf, "M11,%d,%d\r\n", target, pos);
		USART3_SendString(buf);

		if(pos != -1)
		{
			diff = pos - target;
			if(diff < 0)
			{
				diff = -diff;
			}
			if(diff <= 40)
			{
				stable_count++;
				if(stable_count >= 3)
				{
					return 0;
				}
			}
			else
			{
				stable_count = 0;
			}
		}
	}
	return 0;
}

void Motor11_Reciprocate_Once(void)
{
	g_motion_stop_flag = 0;
	Key_ClearStopRequest();
	Serial_ClearRxFlag();
	USART3_SendString("M11,START\r\n");

	for(uint8_t cycle = 1; cycle <= 3; cycle++)
	{
		if(Motor11_WaitTarget(790))
		{
			return;
		}
		if(Motor11_WaitTarget(2500))
		{
			return;
		}
		if(Motor11_WaitTarget(790))
		{
			return;
		}
	}

	Serial_ClearRxFlag();
	USART3_SendString("M11,END\r\n");
}
static uint8_t Motor11_PauseWithStop(uint16_t ms)
{
	char buf[64];
	int pos;

	for(uint16_t t = 0; t < ms; t += 5)
	{
		Delay_ms(5);
		if(Motor11_CheckStop())
		{
			return 1;
		}
		if((t % 50) == 45)
		{
			pos = ReadPos(11);
			sprintf(buf, "M11S,PAUSE,%d\r\n", pos);
			USART3_SendString(buf);
		}
	}
	return 0;
}

static uint8_t Motor11_WaitTargetFast(uint16_t target)
{
	char buf[64];
	uint8_t stable_count = 0;
	int pos;
	int diff;

	WritePosEx(11, target, 600, 20);
	for(uint16_t wait = 0; wait < 200; wait++)
	{
		for(uint8_t d = 0; d < 10; d++)
		{
			Delay_ms(5);
			if(Motor11_CheckStop())
			{
				return 1;
			}
		}

		pos = ReadPos(11);
		sprintf(buf, "M11S,%d,%d\r\n", target, pos);
		USART3_SendString(buf);

		if(pos != -1)
		{
			diff = pos - target;
			if(diff < 0)
			{
				diff = -diff;
			}
			if(diff <= 40)
			{
				stable_count++;
				if(stable_count >= 3)
				{
					return 0;
				}
			}
			else
			{
				stable_count = 0;
			}
		}
	}
	return 0;
}

void Motor11_StepPause_Once(void)
{
	uint16_t targets[] = {2800, 2000, 1500, 1000, 500, 000, 700, 1200, 1700, 2200, 2800};

	g_motion_stop_flag = 0;
	Key_ClearStopRequest();
	Serial_ClearRxFlag();
	USART3_SendString("M11S,START\r\n");

	if(Motor11_WaitTargetFast(targets[0]))
	{
		return;
	}

	for(uint8_t i = 1; i < sizeof(targets) / sizeof(targets[0]); i++)
	{
		if(Motor11_WaitTargetFast(targets[i]))
		{
			return;
		}
		if(Motor11_PauseWithStop(2000))
		{
			return;
		}
	}

	Serial_ClearRxFlag();
	USART3_SendString("M11S,END\r\n");
}

static uint8_t Motor24_CheckStop(void)
{
	Key_Control(Servo_ID);
	if(g_motion_stop_flag || Key_StopRequested())
	{
		ServoStop(24);
		Serial_ClearRxFlag();
		USART3_SendString("M24,STOP,0,0,0\r\n");
		return 1;
	}
	return 0;
}

static uint8_t Motor24_PauseWithStop(uint16_t ms)
{
	char buf[64];
	int pos;

	for(uint16_t t = 0; t < ms; t += 5)
	{
		Delay_ms(5);
		if(Motor24_CheckStop())
		{
			return 1;
		}
		if((t % 50) == 45)
		{
			pos = ReadPos(24);
			sprintf(buf, "M24,PAUSE,%d\r\n", pos);
			USART3_SendString(buf);
		}
	}
	return 0;
}

static uint8_t Motor24_WaitTargetFast(uint16_t target)
{
	char buf[64];
	uint8_t stable_count = 0;
	int pos;
	int diff;

	WritePosEx(24, target, 800, 20);
	for(uint16_t wait = 0; wait < 220; wait++)
	{
		for(uint8_t d = 0; d < 10; d++)
		{
			Delay_ms(5);
			if(Motor24_CheckStop())
			{
				return 1;
			}
		}

		pos = ReadPos(24);
		sprintf(buf, "M24,%d,%d\r\n", target, pos);
		USART3_SendString(buf);

		if(pos != -1)
		{
			diff = pos - target;
			if(diff < 0)
			{
				diff = -diff;
			}
			if(diff <= 40)
			{
				stable_count++;
				if(stable_count >= 3)
				{
					return 0;
				}
			}
			else
			{
				stable_count = 0;
			}
		}
	}
	return 0;
}

void Motor24_Reach500Pause_Once(void)
{
	g_motion_stop_flag = 0;
	Key_ClearStopRequest();
	Serial_ClearRxFlag();
	USART3_SendString("M24,START\r\n");

	for(uint8_t cycle = 1; cycle <= 5; cycle++)
	{
		if(Motor24_WaitTargetFast(3300))
		{
			return;
		}
		if(Motor24_WaitTargetFast(800))
		{
			return;
		}
		if(Motor24_PauseWithStop(5000))
		{
			return;
		}
		if(Motor24_WaitTargetFast(3300))
		{
			return;
		}
		if(cycle < 5)
		{
			if(Motor24_PauseWithStop(5000))
			{
				return;
			}
		}
	}

	Serial_ClearRxFlag();
	USART3_SendString("M24,END\r\n");
}

void Motor14_Reset_1600(void)
{
	g_motion_stop_flag = 0;
	Key_ClearStopRequest();
	Serial_ClearRxFlag();
	WritePosEx(14, 1500, 600, 20);
	USART3_SendString("M14,RESET,1600\r\n");
}
