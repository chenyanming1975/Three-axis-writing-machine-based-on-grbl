#ifndef __include_h
#define __include_h

//-----------------��GRBL�����ò��֡�
#include "config.h"	   				//������
#include "defaults.h"				//Ĭ�ϲ���
#include "pin_map.h"				//����


//############################��STM32������á�##########################

#define F_CPU 72000000
#define M_PI  3.14159265358979323846

/******************************************************************
							�����Ŷ��塿
******************************************************************/

/*--------------------------------
���ŷֲ������
***********************************
���ã�
��������飺GPIOB 5-11	   	����OUT
��ȴ��		GPIOB 0,1 		����OUT
������ƣ�	GPIOA 11,12		����OUT
��λ���أ�	GPIOA 6,7,8		IN	�������ж�5-9��
�������ţ�	GPIOA 0,1,2		IN	�������ж�0,1,2��		
USART1��	GPIOA 9,10		������HW_USART.c��

�����漰���жϣ���ò�Ҫ�Ķ�

***********************************
ʣ�ࣺ
���У�		GPIOA 3,4,5 
SPI2:		GPIOB 12-15		
����1��		GPIOA 13,14,15
����2&BOOT0	GPIOB 2,3,4

--------------------------------*/

//----------------------------�����������
#define STEP_GPIOx GPIOB

#define X_STEP_PIN 5	 		//x������
#define Y_STEP_PIN 6	 		//y������
#define Z_STEP_PIN 7	 		//z������

#define X_DIRECTION_PIN	8 		//x�᷽��
#define Y_DIRECTION_PIN	9 		//y�᷽��
#define Z_DIRECTION_PIN	10 		//z�᷽��

#define STEPPERS_DISABLE_PIN 11 //���������ʧ��

//�����

//----------------------------�����᡿
#define SPINDLE_GPIOx GPIOA

#define SPINDLE_ENABLE_PIN 11	//����ʹ��
#define SPINDLE_DIRECTION_PIN 12//���᷽��

//�����

//----------------------------����ȴ��
#define COOLANT_GPIOx GPIOB

#define COOLANT_FLOOD_PIN 0 	//������ȴʹ��

#ifdef ENABLE_M7 
#define COOLANT_MIST_PIN 1 		//������ȴʹ��
#endif

//�����

//----------------------------����λ��
#define LIMIT_GPIOx GPIOA

#define X_LIMIT_PIN 6  		   	//x����λ����
#define Y_LIMIT_PIN 7		  	//y����λ����
#define Z_LIMIT_PIN 8 		  	//z����λ����

//����ڣ�Ҫʹ�ù����жϣ�EXTI9_5_IRQHandler()��
//�ı�����������Ҫ������������˿ڵ�ֵ��������
	

//----------------------------��������
#define OTHER_GPIOx	GPIOA

#define OTHER_RESET_PIN 0		//��λ���������Ա��浱ǰ�����꣩
#define OTHER_FEED_HOLD_PIN 1 	//��ͣ����
#define OTHER_CYCLE_START_PIN 2 //��������

//����ڣ���ʹ�ö����жϣ�0~2��
//�ı�����������Ҫ������������˿ڵ�ֵ��������

/******************************************************************
							��EEPROM���塿
******************************************************************/


#define STM32_FLASH_WREN 	1              	//ʹ��FLASHд��(0��������;1��ʹ��)
#define STM32_FLASH_SIZE 	64 	 			//��ѡSTM32��FLASH������С(��λΪK)

#define STM32_EEPROM_ADDR_START	0x0800FC00	//��ΪEEPROM����ʼ��ַ����ַ������ż����
#define STM32_EEPROM_ADDR_MAX	0x400			//EEPROM����(��λB)


//###############################��ͷ�ļ���##############################
//-----------------���ⲿ���롿
#include"stm32f10x_lib.h"
#include"system.h"
#include"delay.h"
#include"HW_GPIO.h"
#include"HW_EEPROM.h"
#include"HW_TIM.h"
#include"HW_USART.h"
#include"HW_EXTI.h"								   

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <inttypes.h> 
#include <math.h>





//-----------------��GRBL�ڲ�ʹ�á�
#include "planner.h"				//Gcode����Э��
#include "nuts_bolts.h"				//��Ӳ��GRBL���ú���
#include "stepper.h"				//��Ӳ�������������
#include "spindle_control.h"		//��Ӳ����������������
#include "coolant_control.h"		//��Ӳ����ȴ��������
#include "motion_control.h" 		//�����������
#include "gcode.h"					//�����ڷ��͵�Gcode����
#include "protocol.h"				//��Ӳ�������ڷ��͵ķ�Gcode����
#include "limits.h"					//��Ӳ����λ��������
#include "report.h"					//�˻���������
#include "settings.h"				//epprom�����Ķ�д
#include "serial.h"					//��Ӳ����������
#include "print.h"					//���ڴ�ӡ�ַ�
#include "eeprom.h"				 	//��Ӳ��epprom����




#endif
