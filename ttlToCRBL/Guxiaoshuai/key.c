#include "key.h"
#include "stm32f10x.h"
#include "delay.h"
 							    
//������ʼ������
void KEY_Init(void) //IO��ʼ��
{ 
 	GPIO_InitTypeDef GPIO_InitStructure;
 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB,ENABLE);//ʹ��PORTA,PORTEʱ��

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_10|GPIO_Pin_14|GPIO_Pin_15|GPIO_Pin_9;//KEY0-KEY2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //���ó���������
 	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOE2,3,4
}

u8 KEY_Scan(u8 mode)
{	 
	static u8 key_up=1;//�������ɿ���־
	if(mode)key_up=1;  //֧������		  
	if(key_up&&(KEY9==0||KEY10==0||KEY11==0||KEY12==0||KEY13==0||KEY14==0||KEY15==0)) //����Ƿ��а�������
	{
		delay_ms(10);//ȥ���� 
		key_up=0;
		if(KEY10==0)return Xu;		//�а�������ʱ������Ӧ��ֵ
		else if(KEY11==0)return Xd;
		else if(KEY12==0)return Yu;
		else if(KEY13==0)return Yd;
		else if(KEY14==0)return Zu;
		else if(KEY15==0)return Zd;
		else if(KEY9==0)return Ci;
	}else if(KEY9==1&&KEY10==1&&KEY11==1&&KEY12==1&&KEY13==1&&KEY14==1&&KEY15==1)key_up=1; 	    
 	return 0;// �ް�������
}
