#ifndef _OLED_H
#define _OLED_H
#define	Brightness	0xCF 
#define X_WIDTH 	128
#define Y_WIDTH 	64

void OLED_WrDat(unsigned char dat);//	��OLED��д����
void OLED_WrCmd(unsigned char cmd);// ��OLED��д����
void OLED_Init(void);// -- OLED����ʼ�����򣬴˺���Ӧ�ڲ�����Ļ֮ǰ���ȵ���

void OLED_Fill(unsigned char x1,unsigned char y1,unsigned char x2,unsigned char y2,unsigned char dot);
void OLED_Clear(void);

void OLED_Refresh_Gram(void);		   
void OLED_DrawPoint(unsigned char x,unsigned char y,unsigned char t);
void OLED_ShowHz(unsigned char x,unsigned char y,unsigned char chr,unsigned char mode,unsigned char q);
void OLED_ShowChar(unsigned char x,unsigned char y,unsigned char chr,unsigned char size,unsigned char mode);
void OLED_ShowNum(unsigned char x,unsigned char y,unsigned int num,unsigned char len,unsigned char size);
void OLED_ShowString(unsigned char x,unsigned char y,const unsigned char *p,unsigned char size);	


#endif

