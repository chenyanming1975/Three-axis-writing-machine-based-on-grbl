#include "include.h"

//####################################����ʼ����####################################

void HW_USART_Init(u32 bound)
{  	 
	USART_InitTypeDef USART_InitStructure;
	RCC->APB2ENR|=1<<2;   //ʹ��PORTA��ʱ��  
	RCC->APB2ENR|=1<<14;  //ʹ�ܴ���ʱ�� 
	GPIOA->CRH&=0XFFFFF00F; 
	GPIOA->CRH|=0X000008B0;//IO״̬����

	USART_InitStructure.USART_BaudRate = bound;//����������;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ 8 λ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No; //����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx |USART_Mode_Tx;//�շ�ģʽ
	USART_Init(USART1, &USART_InitStructure); //��ʼ������
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//���������ж�
	MY_NVIC_Init(3,3,USART1_IRQChannel,2);//������ȼ�		

	USART_Cmd(USART1, ENABLE); //ʹ�ܴ���
}


//####################################���жϡ�####################################


/*-------------------------------------------------------------------
USART1�Ľ����жϺ���USART1_IRQHandler()��stm32f10x_it.c���Ƶ����ļ�
���ݴ棬��Ҫ��ֲ��serial.c��
Ȼ������Ҫע�͵�!!!!!

-------------------------------------------------------------------*/
//-------------------[�����жϺ���]


//void USART1_IRQHandler(void)
//{
//	if(USART_GetFlagStatus(USART1 , USART_IT_RXNE)!=RESET)//���յ�����
//	{
//		//-----------------�����ա�
////		rx_data=USART1->DR;				   //�������Զ����ж�
//
//		//--------------------------
//	}
//	if (USART_GetITStatus(USART1, USART_IT_TXE) != RESET) 	//д���ݼĴ����գ�����д����
//	{
//		//-----------------�����͡�
////		USART1->DR=tx_data;
//
//		//--------------------------
//		USART_ITConfig(USART1, USART_IT_TXE, DISABLE);//TXE�жϱ����ֶ��أ�����ֻҪ���ˣ��ͻ���ж�
//	}
//}




