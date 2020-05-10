#ifndef __HW_EEPROM_h
#define __HW_EEPROM_h


/*---------------------------------------------------------------
��Ҫʵ�������ײ㺯��������ʹ��STM32�Դ���flash��EEPROM����GRBL��ʹ�õ�����EEPROM��СΪ1k��
��������1279B����ַ������ż����
����Ҫ�ȵ���̽�����ȷ����������EEPROM�Ŀ�ʼ��ַ��������ʽu32�����0x08000000�����0x0800FFFF��
����Ĭ����0x0800FF00~0x0800FFFF����ΪEPPROM��


����дʱҪ�ر������ж�!!!!!�����ȱ�ݲ��ã����ܺ��ڻ��Ϊ�ⲿEEPROM
---------------------------------------------------------------*/

//##############################���ڲ�ʹ�á�##############################
//FLASH��ʼ��ַ
#define STM32_FLASH_BASE 0x08000000 	//STM32 FLASH����ʼ��ַ
//FLASH������ֵ
#define FLASH_KEY1 0X45670123
#define FLASH_KEY2 0XCDEF89AB

//##############################��API��##############################
void HW_EEPROM_Init(void);			//EEPROM��ʼ��

unsigned char HW_EEPROM_getChar(unsigned int addr);				   //EEPROM��ȡһ��u8����
void HW_EEPROM_putChar(unsigned int addr,unsigned char new_value); //EEPROMд��һ��u8����

#endif
