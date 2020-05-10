#include "include.h"

void HW_EXTI_Init(void)
{
//-------------------��Limit��
#ifndef LIMIT_SWITCHES_ACTIVE_HIGH					 //Ԥ���ߣ�����Ҫ�½���
	Ex_NVIC_Config(0,X_LIMIT_PIN,FTIR);		 		//GPIOA
	Ex_NVIC_Config(0,Y_LIMIT_PIN,FTIR);		 		//GPIOA
	Ex_NVIC_Config(0,Z_LIMIT_PIN,FTIR);		 		//GPIOA	
#else // LIMIT_SWITCHES_ACTIVE_HIGH					 //Ԥ���ͣ�����Ҫ�Ͻ���
	Ex_NVIC_Config(0,X_LIMIT_PIN,RTIR);		 		//GPIOA
	Ex_NVIC_Config(0,Y_LIMIT_PIN,RTIR);		 		//GPIOA
	Ex_NVIC_Config(0,Z_LIMIT_PIN,RTIR);		 		//GPIOA	
#endif // !LIMIT_SWITCHES_ACTIVE_HIGH			  
	MY_NVIC_Init(0,0,EXTI9_5_IRQChannel,2);			//��ռ0,�����ȼ�0����2

//-------------------��Other_Pin��				//����Ԥ��������Ҫ�½��ش���
	Ex_NVIC_Config(0,OTHER_RESET_PIN,FTIR);		 	//GPIOA			  
	MY_NVIC_Init(0,0,EXTI0_IRQChannel,2);			//��ռ0,�����ȼ�0����2

	Ex_NVIC_Config(0,OTHER_FEED_HOLD_PIN,FTIR);		//GPIOA			  
	MY_NVIC_Init(0,0,EXTI1_IRQChannel,2);			//��ռ0,�����ȼ�0����2

	Ex_NVIC_Config(0,OTHER_CYCLE_START_PIN,FTIR);	//GPIOA			  
	MY_NVIC_Init(0,0,EXTI2_IRQChannel,2);			//��ռ0,�����ȼ�0����2

}


//##########################�������жϡ�##########################
/*---------------------------------------------------------------

�ô����ж�ִ�к�����stm32f10x_it.c�м��й�����������Ϻ�Ҫ��ֲ��limit.c��
protocol.c��ȥ��Ȼ�󽫴˴����õ��ĺ���ע�͵���

stm32��5���������ж�ִ�к���0~4��ʣ��5~9����һ���ж�ִ�к���,10~15����һ
���жϺ�����

---------------------------------------------------------------*/


//void EXTI0_IRQHandler(void)			  	//OTHER_RESET_PIN
//{
//	
//	EXTI->PR=1<<0;  //���LINE0�ϵ��жϱ�־λ
//}
//
//void EXTI1_IRQHandler(void)			  	//OTHER_FEED_HOLD_PIN
//{
//
//	EXTI->PR=1<<1;  //���LINE1�ϵ��жϱ�־λ 	
//}
//
//void EXTI2_IRQHandler(void)			  	//OTHER_CYCLE_START_PIN
//{
//
//	EXTI->PR=1<<2;  //���LINE2�ϵ��жϱ�־λ
//}

void EXTI3_IRQHandler(void)				//δʹ��
{
	
	EXTI->PR=1<<3;  //���LINE3�ϵ��жϱ�־λ
}

void EXTI4_IRQHandler(void)				//δʹ��
{
	
	EXTI->PR=1<<4;  //���LINE4�ϵ��жϱ�־λ
}

//void EXTI9_5_IRQHandler(void)		 	//Ҫ��Ϊlimit�ڹ����ж�
//{
//
//
//	EXTI->PR=1<<5;
//	EXTI->PR=1<<6;
//	EXTI->PR=1<<7;
//	EXTI->PR=1<<8;
//	EXTI->PR=1<<9;
//}

void EXTI15_10_IRQHandler(void)		  	//δʹ��
{

	EXTI->PR=1<<10;
	EXTI->PR=1<<11;
	EXTI->PR=1<<12;
	EXTI->PR=1<<13;
	EXTI->PR=1<<14;
	EXTI->PR=1<<15;
}



