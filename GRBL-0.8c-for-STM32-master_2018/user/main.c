#include "include.h"


system_t sys; 

int main()
{

	Stm32_Clock_Init();		//ϵͳʱ�ӳ�ʼ��
	delay_init(72);			//��ʱ������ʼ��
	HW_GPIO_Init();			//GPIO��ʼ��
	HW_EEPROM_Init();		//flash��EEPROM��ʼ��
	HW_USART_Init(BAUD_RATE);	//���ڳ�ʼ��
	HW_EXTI_Init();			//�жϳ�ʼ��
	HW_TIM_Init();			//��ʱ����ʼ��


	serial_init(); // ���ô��в����ʺ��ж�
	settings_init(); //��EEPROM����grbl����
	st_init(); // ���ò������ź��жϼ�ʱ��
	sei(); // ʹ���ж�
	
	memset(&sys, 0, sizeof(sys));  // Clear all system variables
	sys.abort = true;   // Set abort to complete initialization
	sys.state = STATE_INIT;  // Set alarm state to indicate unknown initial position
	
	for(;;) {
	
	// Execute system reset upon a system abort, where the main program will return to this loop.
	// Once here, it is safe to re-initialize the system. At startup, the system will automatically
	// reset to finish the initialization process.
		if (sys.abort) {
		// Reset system.
			serial_reset_read_buffer(); // Clear serial read buffer
			plan_init(); // Clear block buffer and planner variables
			gc_init(); // Set g-code parser to default state
			protocol_init(); // Clear incoming line data and execute startup lines
			spindle_init();
			coolant_init();
			limits_init();
			st_reset(); // Clear stepper subsystem variables.
			
			// Sync cleared gcode and planner positions to current system position, which is only
			// cleared upon startup, not a reset/abort. 
			sys_sync_current_position();
			
			// Reset system variables.
			sys.abort = false;
			sys.execute = 0;
			if (bit_istrue(settings.flags,BITFLAG_AUTO_START)) { sys.auto_start = true; }
			
			// Check for power-up and set system alarm if homing is enabled to force homing cycle
			// by setting Grbl's alarm state. Alarm locks out all g-code commands, including the
			// startup scripts, but allows access to settings and internal commands. Only a homing
			// cycle '$H' or kill alarm locks '$X' will disable the alarm.
			// NOTE: The startup script will run after successful completion of the homing cycle, but
			// not after disabling the alarm locks. Prevents motion startup blocks from crashing into
			// things uncontrollably. Very bad.
			#ifdef HOMING_INIT_LOCK
			if (sys.state == STATE_INIT && bit_istrue(settings.flags,BITFLAG_HOMING_ENABLE)) { sys.state = STATE_ALARM; }
			#endif
			
			// Check for and report alarm state after a reset, error, or an initial power up.
			if (sys.state == STATE_ALARM) {
				report_feedback_message(MESSAGE_ALARM_LOCK); 
			} else {
			// All systems go. Set system to ready and execute startup script.
				sys.state = STATE_IDLE;
				protocol_execute_startup(); 
			}
		}
		
		protocol_execute_runtime();
		protocol_process(); // ... process the serial protocol
		
	}
}


/*	�����ô���
		HW_GPIO_OUT(GPIOB,8,0);
		delay_ms(10);
		HW_GPIO_OUT(GPIOB,8,1);	
*/




