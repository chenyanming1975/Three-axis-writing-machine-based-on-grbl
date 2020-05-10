#include "iic.h"
#include "delay.h"
/*********************************************
�������� ��ʼ��IIC
*********************************************/
void IIC_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );	 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	IIC_SCL=1;
	IIC_SDA=1;
}

/**********************************************
�������� SDA���
****************************************/
void SDA_Out(void)
{
	GPIO_InitTypeDef GPIO_InitStructer;
    GPIO_InitStructer.GPIO_Pin= GPIO_Pin_7;
    GPIO_InitStructer.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_InitStructer.GPIO_Mode=GPIO_Mode_Out_PP; //�������
    GPIO_Init(GPIOB, &GPIO_InitStructer);
}
/*******************************
�������� SDA����
***************************************/

void SDA_In(void)
{
	GPIO_InitTypeDef GPIO_InitStructer;
    GPIO_InitStructer.GPIO_Pin= GPIO_Pin_7;
    GPIO_InitStructer.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_InitStructer.GPIO_Mode=GPIO_Mode_IPU; //��������
    GPIO_Init(GPIOB, &GPIO_InitStructer);
}

/*******************************************
�������� ����IIC��ʼ�ź�
****************************************/
void IIC_Start(void)
{
	SDA_Out();     
	IIC_SDA=1;	//  SDA���ߣ�Ϊ�½�����׼��	  
	IIC_SCL=1; 	// SCL�ߵ�ƽ
	delay_us(4);
 	IIC_SDA=0; // ����һ���½��أ���������ʼ�ź�
	delay_us(4);
	IIC_SCL=0; //����SCL��Ϊ�����޸���׼��
}

/********************************************
�������� ����IIC�����ź�
******************************************/
void IIC_Stop(void)
{
	SDA_Out();
	IIC_SCL = 0;
	IIC_SDA = 0; // ����SDA��Ϊ��������׼��
	delay_us(5);
	IIC_SCL = 1;
	IIC_SDA = 1; // ����һ�������أ������������ź�
	delay_us(5);
}

/*****************************************
�������� ����Ӧ���ź�
**********************************************/
void IIC_Ask(void)
{
	IIC_SCL=0;
	SDA_Out();
	IIC_SDA=0;
	delay_us(2);
	IIC_SCL=1;
	delay_us(2);
	IIC_SCL=0;
}

/****************************************
�������� ������Ӧ���ź�
****************************************/
void IIC_Nask(void)
{
	IIC_SCL=0;
	SDA_Out();
	IIC_SDA=1;
	delay_us(2);
	IIC_SCL=1;
	delay_us(2);
	IIC_SCL=0;
}

/**************************************
�������� ��ȡӦ���ź�,����1��ʾӦ�𣬲����ͽ����źţ����򷵻�0
***********************************/
u8 Wait_Ask(void)
{
	u8 ucErrTime=0;
	SDA_In();       
	IIC_SDA=1;
	delay_us(1);	   
	IIC_SCL=1;
	delay_us(1);	 
	while(READ_SDA) // SDA�����������Ӧ��
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL=0;//ʱ�����0 	   
	return 0; 
}

/****************************************
�������� ����һ���ֽ�
***********************************/
void  Send_Byte(u8 ByteData)
{
	u8 i;   
	SDA_Out(); 	    
    IIC_SCL=0;//����ʱ��Ϊ���ݴ�����׼��
    for(i=0;i<8;i++)
    {              
        IIC_SDA=(ByteData&0x80)>>7; // ��λ��ǰ����λ�ں�
        ByteData<<=1; 	  
		delay_us(2);   
		IIC_SCL=1; // ����SCL����ʼ����
		delay_us(2); 
		IIC_SCL=0;	//����ʱ��Ϊ�´����ݴ�����׼��
		delay_us(2);
    }	 
}

/*****************************************
�������� ��һ���ֽ�
*************************************/
u8 Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_In();
    for(i=0;i<8;i++ )
	{
        IIC_SCL=0; 
        delay_us(2);
		IIC_SCL=1;
        receive<<=1;
        if(READ_SDA)receive++;   
		delay_us(1); 
    }					 
    if (!ack)
        IIC_Nask();//����nACK
    else
        IIC_Ask(); //����ACK   
    return receive;
}



