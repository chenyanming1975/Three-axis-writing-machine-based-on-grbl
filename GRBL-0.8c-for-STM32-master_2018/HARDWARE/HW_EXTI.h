#ifndef __HW_EXTI_H
#define __HW_EXTI_H

#define cli()      NVIC_SETPRIMASK()  			//���жϹر�
#define sei()      NVIC_RESETPRIMASK()			//���жϿ���


//##########################��API��##########################
void HW_EXTI_Init(void);

#endif
