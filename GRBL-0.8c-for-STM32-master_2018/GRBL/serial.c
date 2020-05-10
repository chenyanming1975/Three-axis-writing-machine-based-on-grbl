/*
ϵ�С�c -ͨ�����ڷ��ͺͽ����ֽڵĵͼ�����
Grbl��һ����
g-code���������˶�������
*/

//#include <avr/interrupt.h>
//#include "serial.h"
//#include "config.h"
//#include "motion_control.h"
//#include "protocol.h"

#include "include.h"

uint8_t rx_buffer[RX_BUFFER_SIZE];
uint8_t rx_buffer_head = 0;
volatile uint8_t rx_buffer_tail = 0;

uint8_t tx_buffer[TX_BUFFER_SIZE];
uint8_t tx_buffer_head = 0;
volatile uint8_t tx_buffer_tail = 0;

#ifdef ENABLE_XONXOFF
  volatile uint8_t flow_ctrl = XON_SENT; // ������״̬����
  
  // ����RX�������е��ֽ������⽫�滻Ҫ��ֹ�ĵ����ֽڼ�����
  // �жϺ��������д�뵽��������ͬʱ��
static uint8_t get_rx_buffer_count()
  {
    if (rx_buffer_head == rx_buffer_tail) { return(0); }
    if (rx_buffer_head < rx_buffer_tail) { return(rx_buffer_tail-rx_buffer_head); }
    return (RX_BUFFER_SIZE - (rx_buffer_head-rx_buffer_tail));
  }
#endif

void serial_init()
{
//==========================================
//  // Set baud rate
//  #if BAUD_RATE < 57600
//    uint16_t UBRR0_value = ((F_CPU / (8L * BAUD_RATE)) - 1)/2 ;
//    UCSR0A &= ~(1 << U2X0); // baud doubler off  - Only needed on Uno XXX
//  #else
//    uint16_t UBRR0_value = ((F_CPU / (4L * BAUD_RATE)) - 1)/2;
//    UCSR0A |= (1 << U2X0);  // baud doubler on for high baud rates, i.e. 115200
//  #endif
//  UBRR0H = UBRR0_value >> 8;					 //���ò�����
//  UBRR0L = UBRR0_value;
//            
//  // enable rx and tx
//  UCSR0B |= 1<<RXEN0;							 //ʹ��IO��Ϊ�շ�
//  UCSR0B |= 1<<TXEN0;
//	
//  // enable interrupt on complete reception of a byte
//  UCSR0B |= 1<<RXCIE0;						 //���ս����ж�ʹ��

/*--------���ڴ��ڳ�ʼ�������ж���---------*/
//==========================================
	  
  // defaults to 8-bit, no parity, 1 stop bit
}

//д����
void serial_write(uint8_t data) {
  // Calculate next head
  uint8_t next_head = tx_buffer_head + 1;
  if (next_head == TX_BUFFER_SIZE) { next_head = 0; }

  // Wait until there is space in the buffer
  while (next_head == tx_buffer_tail) { 
    if (sys.execute & EXEC_RESET) { return; } // Only check for abort to avoid an endless loop.
  }

  // Store data and advance head
  tx_buffer[tx_buffer_head] = data;
  tx_buffer_head = next_head;
 //========================================== 
  // Enable Data Register Empty Interrupt to make sure tx-streaming is running
//  UCSR0B |=  (1 << UDRIE0);		   //ʹ�ܴ��ڷ�������жϺ�����̽��뷢������ж�!!!!!
	USART_ITConfig(USART1,USART_IT_TXE, ENABLE);//���������жϲ����̽��뷢���ж�
  //========================================== 
}


//������
uint8_t serial_read()
{
  uint8_t tail = rx_buffer_tail; // ��ʱrx_buffer_tail
  uint8_t data ;
  if (rx_buffer_head == tail) {
    return SERIAL_NO_DATA;
  } else {
    data = rx_buffer[tail];
    tail++;
    if (tail == RX_BUFFER_SIZE) { tail = 0; }
    rx_buffer_tail = tail;
    
    #ifdef ENABLE_XONXOFF
      if ((get_rx_buffer_count() < RX_BUFFER_LOW) && flow_ctrl == XOFF_SENT) { 
        flow_ctrl = SEND_XON;
        UCSR0B |=  (1 << UDRIE0); // Force TX
      }
    #endif
    
    return data;
  }
}

//���ڻ���������
void serial_reset_read_buffer() 
{
  rx_buffer_tail = rx_buffer_head;

  #ifdef ENABLE_XONXOFF
    flow_ctrl = XON_SENT;
  #endif
}

//�ж�ʹ�ܺ���
//==========================================
void USART1_IRQHandler(void)
{
	if(USART_GetFlagStatus(USART1 , USART_IT_RXNE)!=RESET)//���յ�����
	{
		//-----------------�������жϡ�
		//==========================================
		uint8_t data=USART1->DR;				   //����
		uint8_t next_head;
		//==========================================
//ISR(SERIAL_RX)
//{
//  
//  	uint8_t data = UDR0;
//  	uint8_t next_head;
  // ֱ�ӴӴ�������ѡ������ʱ�����ַ�����Щ�ַ���
  // û�д��ݵ���������������Щ����ϵͳ��״̬��־λ��������ʱִ�С�
  switch (data) {
    case CMD_STATUS_REPORT: sys.execute |= EXEC_STATUS_REPORT; break; // Set as true
    case CMD_CYCLE_START:   sys.execute |= EXEC_CYCLE_START; break; // Set as true
    case CMD_FEED_HOLD:     sys.execute |= EXEC_FEED_HOLD; break; // Set as true
    case CMD_RESET:         mc_reset(); break; // Call motion control reset routine.
    default: // д�ַ���������			
      next_head = rx_buffer_head + 1;
      if (next_head == RX_BUFFER_SIZE) { next_head = 0; }
    
      // Write data to buffer unless it is full.
      if (next_head != rx_buffer_tail) {
        rx_buffer[rx_buffer_head] = data;
        rx_buffer_head = next_head;    
        
//        #ifdef ENABLE_XONXOFF
//          if ((get_rx_buffer_count() >= RX_BUFFER_FULL) && flow_ctrl == XON_SENT) {
//            flow_ctrl = SEND_XOFF;
//            UCSR0B |=  (1 << UDRIE0); // Force TX
//          } 
//        #endif

      }
  }
//}
//--------------------------
	}
	if (USART_GetITStatus(USART1, USART_IT_TXE) != RESET) 	//д���ݼĴ����գ�����д����
	{
		//-----------------��BUFF���жϡ�
//		// Data Register Empty Interrupt handler
//		ISR(SERIAL_UDRE)
//		{
		  // Temporary tx_buffer_tail (to optimize for volatile)
		  uint8_t tail = tx_buffer_tail;
		  
		  #ifdef ENABLE_XONXOFF
		    if (flow_ctrl == SEND_XOFF) { 
		      UDR0 = XOFF_CHAR; 
		      flow_ctrl = XOFF_SENT; 
		    } else if (flow_ctrl == SEND_XON) { 
		      UDR0 = XON_CHAR; 
		      flow_ctrl = XON_SENT; 
		    } else
		  #endif
		  { 
		    // Send a byte from the buffer
		   //==========================================

		    USART1->DR= tx_buffer[tail];  //����
		   //==========================================
		    // Update tail position
		    tail++;
		    if (tail == TX_BUFFER_SIZE) { tail = 0; }
		  
		    tx_buffer_tail = tail;
		  }
		  
		  // Turn off Data Register Empty Interrupt to stop tx-streaming if this concludes the transfer
		  if (tail == tx_buffer_head) {
		  //==========================================
//		   UCSR0B &= ~(1 << UDRIE0);						//�����������ף��ر�TXE�ж�
		   USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
		   //==========================================
		  }
	}
}


//==========================================

