#include"include.h"

/*----------------------------------------------------
			���ļ�������stm32��GPIO��ʼ��
----------------------------------------------------*/

void HW_GPIO_Init(void)
{
	RCC->APB2ENR|=(1<<2|1<<3);    //ʹ��PORTA,PORTBʱ��         
	
//----------------------------�����������

#if X_STEP_PIN/8														
		STEP_GPIOx->CRH&=~((u32)0xf<<((X_STEP_PIN%8)*4));
		STEP_GPIOx->CRH|=(u32)0x3<<((X_STEP_PIN%8)*4);
#else
		STEP_GPIOx->CRL&=~((u32)0xf<<((X_STEP_PIN%8)*4));
		STEP_GPIOx->CRL|=(u32)0x3<<((X_STEP_PIN%8)*4);
#endif

#if Y_STEP_PIN/8														
		STEP_GPIOx->CRH&=~((u32)0xf<<((Y_STEP_PIN%8)*4));
		STEP_GPIOx->CRH|=(u32)0x3<<((Y_STEP_PIN%8)*4);
#else
		STEP_GPIOx->CRL&=~((u32)0xf<<((Y_STEP_PIN%8)*4));
		STEP_GPIOx->CRL|=(u32)0x3<<((Y_STEP_PIN%8)*4);
#endif

#if Z_STEP_PIN/8														
		STEP_GPIOx->CRH&=~((u32)0xf<<((Z_STEP_PIN%8)*4));
		STEP_GPIOx->CRH|=(u32)0x3<<((Z_STEP_PIN%8)*4);
#else
		STEP_GPIOx->CRL&=~((u32)0xf<<((Z_STEP_PIN%8)*4));
		STEP_GPIOx->CRL|=(u32)0x3<<((Z_STEP_PIN%8)*4);
#endif

#if X_DIRECTION_PIN/8														
		STEP_GPIOx->CRH&=~((u32)0xf<<((X_DIRECTION_PIN%8)*4));
		STEP_GPIOx->CRH|=(u32)0x3<<((X_DIRECTION_PIN%8)*4);
#else
		STEP_GPIOx->CRL&=~((u32)0xf<<((X_DIRECTION_PIN%8)*4));
		STEP_GPIOx->CRL|=(u32)0x3<<((X_DIRECTION_PIN%8)*4);
#endif

#if Y_DIRECTION_PIN/8														
		STEP_GPIOx->CRH&=~((u32)0xf<<((Y_DIRECTION_PIN%8)*4));
		STEP_GPIOx->CRH|=(u32)0x3<<((Y_DIRECTION_PIN%8)*4);
#else
		STEP_GPIOx->CRL&=~((u32)0xf<<((Y_DIRECTION_PIN%8)*4));
		STEP_GPIOx->CRL|=(u32)0x3<<((Y_DIRECTION_PIN%8)*4);
#endif

#if Z_DIRECTION_PIN/8														
		STEP_GPIOx->CRH&=~((u32)0xf<<((Z_DIRECTION_PIN%8)*4));
		STEP_GPIOx->CRH|=(u32)0x3<<((Z_DIRECTION_PIN%8)*4);
#else
		STEP_GPIOx->CRL&=~((u32)0xf<<((Z_DIRECTION_PIN%8)*4));
		STEP_GPIOx->CRL|=(u32)0x3<<((Z_DIRECTION_PIN%8)*4);
#endif

#if STEPPERS_DISABLE_PIN/8														
		STEP_GPIOx->CRH&=~((u32)0xf<<((STEPPERS_DISABLE_PIN%8)*4));
		STEP_GPIOx->CRH|=(u32)0x3<<((STEPPERS_DISABLE_PIN%8)*4);
#else
		STEP_GPIOx->CRL&=~((u32)0xf<<((STEPPERS_DISABLE_PIN%8)*4));
		STEP_GPIOx->CRL|=(u32)0x3<<((STEPPERS_DISABLE_PIN%8)*4);
#endif

//----------------------------�����᡿

#if SPINDLE_ENABLE_PIN/8														
		SPINDLE_GPIOx->CRH&=~((u32)0xf<<((SPINDLE_ENABLE_PIN%8)*4));
		SPINDLE_GPIOx->CRH|=(u32)0x3<<((SPINDLE_ENABLE_PIN%8)*4);
#else
		SPINDLE_GPIOx->CRL&=~((u32)0xf<<((SPINDLE_ENABLE_PIN%8)*4));
		SPINDLE_GPIOx->CRL|=(u32)0x3<<((SPINDLE_ENABLE_PIN%8)*4);
#endif

#if SPINDLE_DIRECTION_PIN/8														
		SPINDLE_GPIOx->CRH&=~((u32)0xf<<((SPINDLE_DIRECTION_PIN%8)*4));
		SPINDLE_GPIOx->CRH|=(u32)0x3<<((SPINDLE_DIRECTION_PIN%8)*4);
#else
		SPINDLE_GPIOx->CRL&=~((u32)0xf<<((SPINDLE_DIRECTION_PIN%8)*4));
		SPINDLE_GPIOx->CRL|=(u32)0x3<<((SPINDLE_DIRECTION_PIN%8)*4);
#endif

//----------------------------����ȴ��

#if COOLANT_FLOOD_PIN/8														
		COOLANT_GPIOx->CRH&=~((u32)0xf<<((COOLANT_FLOOD_PIN%8)*4));
		COOLANT_GPIOx->CRH|=(u32)0x3<<((COOLANT_FLOOD_PIN%8)*4);
#else
		COOLANT_GPIOx->CRL&=~((u32)0xf<<((COOLANT_FLOOD_PIN%8)*4));
		COOLANT_GPIOx->CRL|=(u32)0x3<<((COOLANT_FLOOD_PIN%8)*4);
#endif

#ifdef ENABLE_M7 
	#if COOLANT_MIST_PIN/8																	
			COOLANT_GPIOx->CRH&=~((u32)0xf<<((COOLANT_MIST_PIN%8)*4));
			COOLANT_GPIOx->CRH|=(u32)0x3<<((COOLANT_MIST_PIN%8)*4);
	#else
			COOLANT_GPIOx->CRL&=~((u32)0xf<<((COOLANT_MIST_PIN%8)*4));
			COOLANT_GPIOx->CRL|=(u32)0x3<<((COOLANT_MIST_PIN%8)*4);
	#endif
#endif

//----------------------------����λ��
#if X_LIMIT_PIN/8																	
		LIMIT_GPIOx->CRH&=~((u32)0xf<<((X_LIMIT_PIN%8)*4));
		LIMIT_GPIOx->CRH|=(u32)0x8<<((X_LIMIT_PIN%8)*4);
#else
		LIMIT_GPIOx->CRL&=~((u32)0xf<<((X_LIMIT_PIN%8)*4));
		LIMIT_GPIOx->CRL|=(u32)0x8<<((X_LIMIT_PIN%8)*4);
#endif

#if Y_LIMIT_PIN/8																	
		LIMIT_GPIOx->CRH&=~((u32)0xf<<((Y_LIMIT_PIN%8)*4));
		LIMIT_GPIOx->CRH|=(u32)0x8<<((Y_LIMIT_PIN%8)*4);
#else
		LIMIT_GPIOx->CRL&=~((u32)0xf<<((Y_LIMIT_PIN%8)*4));
		LIMIT_GPIOx->CRL|=(u32)0x8<<((Y_LIMIT_PIN%8)*4);
#endif

#if X_LIMIT_PIN/8																	
		LIMIT_GPIOx->CRH&=~((u32)0xf<<((X_LIMIT_PIN%8)*4));
		LIMIT_GPIOx->CRH|=(u32)0x8<<((X_LIMIT_PIN%8)*4);
#else
		LIMIT_GPIOx->CRL&=~((u32)0xf<<((X_LIMIT_PIN%8)*4));
		LIMIT_GPIOx->CRL|=(u32)0x8<<((X_LIMIT_PIN%8)*4);
#endif

//-----------ȷ���������ĺ�

#ifndef LIMIT_SWITCHES_ACTIVE_HIGH
//	LIMIT_PORT |= (LIMIT_MASK); // Enable internal pull-up resistors. Normal high operation.
	LIMIT_GPIOx->ODR|=((1<<X_LIMIT_PIN)|(1<<Y_LIMIT_PIN)|(1<<Z_LIMIT_PIN));	//Ԥ����
#else // LIMIT_SWITCHES_ACTIVE_HIGH
//	LIMIT_PORT &= ~(LIMIT_MASK); // Normal low operation. Requires external pull-down.
	LIMIT_GPIOx->ODR&=(~((1<<X_LIMIT_PIN)|(1<<Y_LIMIT_PIN)|(1<<Z_LIMIT_PIN)));	//Ԥ����
#endif // !LIMIT_SWITCHES_ACTIVE_HIGH

//----------------------------��������

#if OTHER_RESET_PIN/8																	
		OTHER_GPIOx->CRH&=~((u32)0xf<<((OTHER_RESET_PIN%8)*4));
		OTHER_GPIOx->CRH|=(u32)0x8<<((OTHER_RESET_PIN%8)*4);
#else
		OTHER_GPIOx->CRL&=~((u32)0xf<<((OTHER_RESET_PIN%8)*4));
		OTHER_GPIOx->CRL|=(u32)0x8<<((OTHER_RESET_PIN%8)*4);
#endif

#if OTHER_FEED_HOLD_PIN/8																	
		OTHER_GPIOx->CRH&=~((u32)0xf<<((OTHER_FEED_HOLD_PIN%8)*4));
		OTHER_GPIOx->CRH|=(u32)0x8<<((OTHER_FEED_HOLD_PIN%8)*4);
#else
		OTHER_GPIOx->CRL&=~((u32)0xf<<((OTHER_FEED_HOLD_PIN%8)*4));
		OTHER_GPIOx->CRL|=(u32)0x8<<((OTHER_FEED_HOLD_PIN%8)*4);
#endif

#if OTHER_RESET_PIN/8																	
		OTHER_GPIOx->CRH&=~((u32)0xf<<((OTHER_CYCLE_START_PIN%8)*4));
		OTHER_GPIOx->CRH|=(u32)0x8<<((OTHER_CYCLE_START_PIN%8)*4);
#else
		OTHER_GPIOx->CRL&=~((u32)0xf<<((OTHER_CYCLE_START_PIN%8)*4));
		OTHER_GPIOx->CRL|=(u32)0x8<<((OTHER_CYCLE_START_PIN%8)*4);
#endif


//-----------����
		OTHER_GPIOx->ODR|=((1<<OTHER_RESET_PIN)|(1<<OTHER_FEED_HOLD_PIN)|(1<<OTHER_CYCLE_START_PIN));	//Ԥ����

	
}								

