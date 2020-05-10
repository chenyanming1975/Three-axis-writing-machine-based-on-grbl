/*
  report.h - reporting and messaging methods
  Part of Grbl

  The MIT License (MIT)

  GRBL(tm) - Embedded CNC g-code interpreter and motion-controller
  Copyright (c) 2012 Sungeun K. Jeon

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#ifndef report_h
#define report_h


// Define Grbl status codes.
#define STATUS_OK 0
#define STATUS_BAD_NUMBER_FORMAT 1//�쳣�����ָ�ʽ
#define STATUS_EXPECTED_COMMAND_LETTER 2//Ԥ�ڵ�ָ���ַ�״̬
#define STATUS_UNSUPPORTED_STATEMENT 3//��֧�ֵ�����
#define STATUS_ARC_RADIUS_ERROR 4//Բ���뾶����
#define STATUS_MODAL_GROUP_VIOLATION 5//ģ̬��Υ��
#define STATUS_INVALID_STATEMENT 6//��Ч����
#define STATUS_SETTING_DISABLED 7//������Ч
#define STATUS_SETTING_VALUE_NEG 8//�񶨵��趨ֵ
#define STATUS_SETTING_STEP_PULSE_MIN 9//�趨��С�����岽��
#define STATUS_SETTING_READ_FAIL 10//�鿴ʧ��
#define STATUS_IDLE_ERROR 11
#define STATUS_ALARM_LOCK 12//ʱ������
#define STATUS_OVERFLOW 13

// ����Grbl�������롣С��0�����ֱ��������״̬����
#define ALARM_HARD_LIMIT -1//������
#define ALARM_ABORT_CYCLE -2

// ����Grbl������Ϣ���롣
#define MESSAGE_CRITICAL_EVENT 1//�����¼�
#define MESSAGE_ALARM_LOCK 2//��������
#define MESSAGE_ALARM_UNLOCK 3
#define MESSAGE_ENABLED 4
#define MESSAGE_DISABLED 5

// ��ӡϵͳ״̬��Ϣ��
void report_status_message(uint8_t status_code);

//��ӡϵͳ������Ϣ��
void report_alarm_message(int8_t alarm_code);

//��ӡ���ַ�����Ϣ��
void report_feedback_message(uint8_t message_code);

//��ӡ��ʼ��Ϣ
void report_init_message(void);

// ��ӡGrbl�����͵�ǰȫ������
void report_grbl_help(void);

//��ӡGrblȫ������
void report_grbl_settings(void);

//��ӡʵʱ״̬����
void report_realtime_status(void);

// ��ӡGrbl�����������
void report_gcode_parameters(void);

// ��ӡ��ǰ��g-code������ģʽ״̬
void report_gcode_modes(void);

// ��ӡ������
void report_startup_line(uint8_t n, char *line);

#endif
