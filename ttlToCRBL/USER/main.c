#include "stm32f10x.h"
#include "led.h"
#include	"sys.h"
#include "delay.h"
#include "iic.h"
#include "oled.h"
#include "key.h"
#include "usart.h"
#include "OLEDout.h"
#include "writer.h"
//**********************************����������
int main(void)
{ 
	u8 key=0;	//���ڽ��հ���ֵ
  
	delay_init();	//��ʱ������ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	led_init(); // LED��ʼ����PA2��LED��������ʹ�ã�������޹�
//	M_Usart1_Init();
	//uart_init(9600);
	IIC_Init();   		//IIC��ʼ��
	OLED_Init();		//OLED��ʼ�� 
	KEY_Init();			// ������ʼ��
	uart_init(9600);
	Start_out(); // ������OLED��ʾ������
  //X_axis_add(5);
	
	while(1) //����ɨ�谴��
	{	
		key=KEY_Scan(0);	//�õ���ֵ
	   	if(key)
		{						   
			switch(key)
			{				 
				case Xu:	
					led_shansuo();
					//PA2��LED��˸��������ʹ�ã�ע��
				  X_axis_add(3);  //X������ת��һ���
				  //delay_ms(300);
					Key_Xu(); //OLED��ʾ��ʱ�Ķ����������Key_xx() �������Ǵ�����
					break;
				case Xd:	
					led_shansuo();
				  X_axis_add(-3);  //X�ᷴ��ת��һ��
					//delay_ms(300);
				  Key_Xd();
					break;
				case Yu:	 
					led_shansuo();
				  Y_axis_add(3);   //Y������
					//delay_ms(300);
				  Key_Yu();	
					break;
				case Yd:	
					led_shansuo();
				  Y_axis_add(-3);   //Y�ᷴ��
					//delay_ms(300);
				  Key_Xu();;	
					break;
				case Zu:	
					led_shansuo();
				  Z_axis_add(3);     //Z������
					//delay_ms(300);
				  Key_Zu();	
					break;
				case Zd:	
					led_shansuo();
				  Z_axis_add(-3);      //Z�ᷴ��
				  //delay_ms(300);
					Key_Zd();	
					break;
				case Ci:	
					led_shansuo();
					//delay_ms(30);
				  round_draw(4);      //����һ���Ĳ���Ϊ�뾶��Բ
					//delay_ms(300);
				  Key_Ci();
					
					
					break;
			}
		}else delay_ms(10); 
	}
}
 
