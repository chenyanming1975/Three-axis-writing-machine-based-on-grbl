#ifndef __HW_GPIO_H
#define __HW_GPIO_H

//#############################��API��#############################
void HW_GPIO_Init(void);

#define HW_GPIO_OUT(GPIO,PIN,val) 	if(val==0) GPIO->ODR&=~(1<<PIN); else GPIO->ODR|=1<<PIN; //д����   
#define HW_GPIO_IN(GPIO,PIN) 		((GPIO->IDR&(1<<PIN))?1:0)								//������



#endif
