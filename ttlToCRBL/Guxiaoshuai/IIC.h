#ifndef IIC_H
#define IIC_H
#include "sys.h"

// IO����
#define IIC_SCL PBout(6) // �궨��ʱ��������PB6
#define IIC_SDA PBout(7) // �궨������������PB7
#define READ_SDA PBin(7) //�궨����������������PB7


// ��������
void IIC_Init(void); // ��ʼ��IIC
void IIC_Start(void); // ��ʼ�źź���
void IIC_Stop(void); // �����źź���
void IIC_Ask(void); // ����Ӧ���ź�
void IIC_Nask(void); // ������Ӧ���ź�
void Send_Byte(u8 ByteData); //����һ���ֽ�
u8 Read_Byte(unsigned char ack); // ���ֽ�
u8 Wait_Ask(void); //��Ӧ���ź�
void SDA_Out(void); // SDA���
void SDA_In(void); // SDA����
#endif

