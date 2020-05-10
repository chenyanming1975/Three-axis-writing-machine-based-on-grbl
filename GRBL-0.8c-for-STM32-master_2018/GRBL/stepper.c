/*
  stepper.c - stepper motor driver: executes motion plans using stepper motors
  Part of Grbl

  The MIT License (MIT)

  GRBL(tm) - Embedded CNC g-code interpreter and motion-controller
  Copyright (c) 2009-2011 Simen Svale Skogsrud
  Copyright (c) 2011-2012 Sungeun K. Jeon

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
	  ִ�ж�������������ʱ�������������������Ӧ�Ķ���
*/


//#include <avr/interrupt.h>
//#include "stepper.h"
//#include "config.h"
//#include "settings.h"
//#include "planner.h"

#include "include.h"

// һЩ���õĳ���
#define TICKS_PER_MICROSECOND (F_CPU/1000000)	//ÿus���ٸ�tick
#define CYCLES_PER_ACCELERATION_TICK ((TICKS_PER_MICROSECOND*1000000)/ACCELERATION_TICKS_PER_SECOND)

// ����״̬�����������������ݺ����α�����
typedef struct {
  //���ڲ���ɭ��ķ���㷨
  int32_t counter_x,        // ��bresenham�߸������ļ�����������
          counter_y, 
          counter_z;
  uint32_t event_count;
  uint32_t step_events_completed;  // ��ǰ�˶������µĽ�Ծ�¼���

  // Used by the trapezoid generator
  uint32_t cycles_per_step_event;        // ÿ�������¼�֮��Ļ���������
  uint32_t trapezoid_tick_cycle_counter; // ���ϴ����ο�ʼ�����ڡ������ڲ����䵥����ʱ������������ȶ����ٶ����ɽ���
                                              
  uint32_t trapezoid_adjusted_rate;      // ����������������step_events��ǰ����
  uint32_t min_safe_rate;  // ȫ�����ʽ��Ͳ������С��ȫϵ���������step_rate��
} stepper_t;

static stepper_t st;
static block_t *current_block;  // ָ��ǰ���ڸ��ٵĿ��ָ��

// �������������ж�
static uint8_t step_pulse_time; // ��Ծ������Ľ�Ծ���帴λʱ��
static uint8_t out_bits;        // ��һ��Ҫ����Ĳ���λ
static volatile uint8_t busy;   // ��SIG_OUTPUT_COMPARE1A������ʱ����ȷ�ġ����ڱ������´����ô������

#if STEP_PULSE_DELAY > 0
  static uint8_t step_bits;  // �洢out_bits�������ɲ��������ӳ�
#endif

//         __________________________
//        /|                        |\     _________________         ^
//       / |                        | \   /|               |\        |
//      /  |                        |  \ / |               | \       s
//     /   |                        |   |  |               |  \      p
//    /    |                        |   |  |               |   \     e
//   +-----+------------------------+---+--+---------------+----+    e
//   |               BLOCK 1            |      BLOCK 2          |    d
//
//                           time ----->
// 
//  The trapezoid is the shape the speed curve over time. It starts at block->initial_rate, accelerates by block->rate_delta
//  during the first block->accelerate_until step_events_completed, then keeps going at constant speed until 
//  step_events_completed reaches block->decelerate_after after which it decelerates until the trapezoid generator is reset.
//  The slope of acceleration is always +/- block->rate_delta and is applied at a constant rate following the midpoint rule
//  by the trapezoid generator, which is called ACCELERATION_TICKS_PER_SECOND times per second.

static void set_step_events_per_minute(uint32_t steps_per_minute);

// ����״̬��ʼ����ֻ����������st.cycle_start��־ʱ��ѭ����Ӧ�ÿ�ʼ��
// ����init��limit���ô˺���������Ӧ������ѭ����
void st_wake_up() 
{
  // ͨ�����ò��������ö˿������ò�����
  if (bit_istrue(settings.flags,BITFLAG_INVERT_ST_ENABLE)) { 
//==========================================
    //STEPPERS_DISABLE_PORT |= (1<<STEPPERS_DISABLE_BIT);   	//STEPPERS_DISABLE����=1
  	HW_GPIO_OUT(STEP_GPIOx,STEPPERS_DISABLE_PIN,1);
//==========================================
  } else {
//========================================== 
    //STEPPERS_DISABLE_PORT &= ~(1<<STEPPERS_DISABLE_BIT);
	HW_GPIO_OUT(STEP_GPIOx,STEPPERS_DISABLE_PIN,0);				//STEPPERS_DISABLE����=0
//==========================================
  }
  if (sys.state == STATE_CYCLE) {
    // Initialize stepper output bits
    out_bits = (0) ^ (settings.invert_mask); 
    // Initialize step pulse timing from settings. Here to ensure updating after re-writing.
    #ifdef STEP_PULSE_DELAY
      // Set total step pulse time after direction pin set. Ad hoc computation from oscilloscope.
      step_pulse_time = -(((settings.pulse_microseconds+STEP_PULSE_DELAY-2)*TICKS_PER_MICROSECOND) >> 3);
      // Set delay between direction pin write and step command.
      OCR2A = -(((settings.pulse_microseconds)*TICKS_PER_MICROSECOND) >> 3);
    #else // Normal operation
	//==========================================
      // Set step pulse time. Ad hoc computation from oscilloscope. Uses two's complement.
     // step_pulse_time = -(((settings.pulse_microseconds-2)*TICKS_PER_MICROSECOND) >> 3);		
	 //ȡ��ֵ�൱��0xff�����ֵ��step_pulse_time��һ��u8�ͱ�����AVR_TIM2����8λ��ʱ��
	 //settings.pulse_microseconds-2�ļ�2����Ϊ��ʱ��������ǰ��Ҫ����2��AVR��ָ��������ʱ��΢�뼶��ʱ��ͱ���ȽϾ�׼

	step_pulse_time = settings.pulse_microseconds-2;		
	//��ʱnus����2�Ǹ���ƽ�仯ָ��Ԥ��ʱ�䣬ARR=9*n-1��PSC=8-1	
				
		//��ΪSTM32�Ķ�ʱ��ģʽ��AVR��ͬ��STM32�Ǵ�0��ʼ�������������趨ֵ������ж�,���Բ���Ҫȡ��
	  //==========================================
    #endif
    // Enable stepper driver interrupt
	//==========================================
    //TIMSK1 |= (1<<OCIE1A); 		//�����ⶨʱ��	
	TIM3->CR1|=1<<0;				//TIM3����
	//==========================================

  }
}

// �����ر�
void st_go_idle() 
{
  // Disable stepper driver interrupt
  //==========================================
  //TIMSK1 &= ~(1<<OCIE1A);		//�ر��ⶨʱ��
  TIM3->CR1&=~(1<<0); 			//TIM3�ر�
  //==========================================
  // ֻ��ϵͳ����������û�����Ϊ������ʱ���ò�������
  if ((settings.stepper_idle_lock_time != 0xff) || bit_istrue(sys.execute,EXEC_ALARM)) {
    // Force stepper dwell to lock axes for a defined amount of time to ensure the axes come to a complete
    // stop and not drift from residual inertial forces at the end of the last movement.
    delay_ms(settings.stepper_idle_lock_time);
    if (bit_istrue(settings.flags,BITFLAG_INVERT_ST_ENABLE)) {
	//========================================== 
      //STEPPERS_DISABLE_PORT &= ~(1<<STEPPERS_DISABLE_BIT); 		//STEPPERS_DISABLE����=0
	  HW_GPIO_OUT(STEP_GPIOx,STEPPERS_DISABLE_PIN,0);				
	  //==========================================
    } else { 
	//==========================================
     //STEPPERS_DISABLE_PORT |= (1<<STEPPERS_DISABLE_BIT);			//STEPPERS_DISABLE����=1
	  HW_GPIO_OUT(STEP_GPIOx,STEPPERS_DISABLE_PIN,1); 
	  //==========================================
    }   
  }
}

// This function determines an acceleration velocity change every CYCLES_PER_ACCELERATION_TICK by
// keeping track of the number of elapsed cycles during a de/ac-celeration. The code assumes that 
// step_events occur significantly more often than the acceleration velocity iterations.
static uint8_t iterate_trapezoid_cycle_counter() 
{
  st.trapezoid_tick_cycle_counter += st.cycles_per_step_event;  
  if(st.trapezoid_tick_cycle_counter > CYCLES_PER_ACCELERATION_TICK) {
    st.trapezoid_tick_cycle_counter -= CYCLES_PER_ACCELERATION_TICK;
    return(true);
  } else {
    return(false);
  }
}          

// "The Stepper Driver Interrupt" - This timer interrupt is the workhorse of Grbl. It is executed at the rate set with
// config_step_timer. It pops blocks from the block_buffer and executes them by pulsing the stepper pins appropriately. 
// It is supported by The Stepper Port Reset Interrupt which it uses to reset the stepper port after each pulse. 
// The bresenham line tracer algorithm controls all three stepper outputs simultaneously with these two interrupts.
//==========================================
//ISR(TIMER1_COMPA_vect)		   		//�ⶨʱ��
//{ 
void TIM3_IRQHandler(void) 
{
	u8 bit_temp=0;
	if(TIM3->SR&0X0001)//����ж�
	{     
//==========================================
		  if (busy) { return; } // The busy-flag is used to avoid reentering this interrupt
		  
		  // Set the direction pins a couple of nanoseconds before we step the steppers
		  //==========================================
		  //STEPPING_PORT = (STEPPING_PORT & ~DIRECTION_MASK) | (out_bits & DIRECTION_MASK); 	//�����˶�����
			bit_temp=out_bits&(1<<X_DIRECTION_BIT);
			HW_GPIO_OUT(STEP_GPIOx,X_DIRECTION_PIN,bit_temp);
			bit_temp=out_bits&(1<<Y_DIRECTION_BIT);
			HW_GPIO_OUT(STEP_GPIOx,Y_DIRECTION_PIN,bit_temp);
			bit_temp=out_bits&(1<<Z_DIRECTION_BIT);
			HW_GPIO_OUT(STEP_GPIOx,Z_DIRECTION_PIN,bit_temp);
		
		  //==========================================
		  // Then pulse the stepping pins
		  #ifdef STEP_PULSE_DELAY
		  //==========================================
//		    step_bits = (STEPPING_PORT & ~STEP_MASK) | out_bits; // Store out_bits to prevent overwriting.
			 step_bits = out_bits;
			//==========================================
		  #else  // Normal operation
		  //==========================================
//		    STEPPING_PORT = (STEPPING_PORT & ~STEP_MASK) | out_bits;	   //�������ֵ
				bit_temp=out_bits&(1<<X_STEP_BIT);
			  	HW_GPIO_OUT(STEP_GPIOx,X_STEP_PIN,bit_temp);
				bit_temp=out_bits&(1<<Y_STEP_BIT);
				HW_GPIO_OUT(STEP_GPIOx,Y_STEP_PIN,bit_temp);
				bit_temp=out_bits&(1<<Z_STEP_BIT);
				HW_GPIO_OUT(STEP_GPIOx,Z_STEP_PIN,bit_temp);
				
				bit_temp=out_bits&(1<<X_DIRECTION_BIT);
				HW_GPIO_OUT(STEP_GPIOx,X_DIRECTION_PIN,bit_temp);
				bit_temp=out_bits&(1<<Y_DIRECTION_BIT);
				HW_GPIO_OUT(STEP_GPIOx,Y_DIRECTION_PIN,bit_temp);
				bit_temp=out_bits&(1<<Z_DIRECTION_BIT);
				HW_GPIO_OUT(STEP_GPIOx,Z_DIRECTION_PIN,bit_temp);
			//==========================================
		  #endif
		  // Enable step pulse reset timer so that The Stepper Port Reset Interrupt can reset the signal after
		  // exactly settings.pulse_microseconds microseconds, independent of the main Timer1 prescaler.
		  //==========================================
//		  TCNT2 = step_pulse_time; // Reload timer counter				   //�ڶ�ʱ�����ʱ������
//		  TCCR2B = (1<<CS21); // Begin timer2. Full speed, 1/8 prescaler   //�����ڶ�ʱ����8��Ƶ

//			TIM4->ARR=step_pulse_time;	
//			TIM4->PSC=72*8-1;

			TIM4->ARR=step_pulse_time-1;							   //�޸ĺ���0��255us��Χ���޸�����
			TIM4->PSC=TICKS_PER_MICROSECOND-1;		  				   //72��Ƶ����ʱ1us
			
			TIM4->CR1|=1<<0;			//�����ڶ�ʱ��
		  //==========================================
		  busy = true;
		  // Re-enable interrupts to allow ISR_TIMER2_OVERFLOW to trigger on-time and allow serial communications
		  // regardless of time in this handler. The following code prepares the stepper driver for the next
		  // step interrupt compare and will always finish before returning to the main program.
		  sei();	//AVRû���ж�Ƕ�ף�����ڴ˴��жϣ�Ҳ���Բ���
		  
		  // If there is no current block, attempt to pop one from the buffer
		  if (current_block == NULL) {
		    // Anything in the buffer? If so, initialize next motion.
		    current_block = plan_get_current_block();
		    if (current_block != NULL) {
		      if (sys.state == STATE_CYCLE) {
		        // During feed hold, do not update rate and trap counter. Keep decelerating.
		        st.trapezoid_adjusted_rate = current_block->initial_rate;
		        set_step_events_per_minute(st.trapezoid_adjusted_rate); // Initialize cycles_per_step_event
		        st.trapezoid_tick_cycle_counter = CYCLES_PER_ACCELERATION_TICK/2; // Start halfway for midpoint rule.
		      }
		      st.min_safe_rate = current_block->rate_delta + (current_block->rate_delta >> 1); // 1.5 x rate_delta
		      st.counter_x = -(current_block->step_event_count >> 1);
		      st.counter_y = st.counter_x;
		      st.counter_z = st.counter_x;
		      st.event_count = current_block->step_event_count;
		      st.step_events_completed = 0;     
		    } else {
		      st_go_idle();
		      bit_true(sys.execute,EXEC_CYCLE_STOP); // Flag main program for cycle end
		    }    
		  } 
		
		  if (current_block != NULL) {
		    // Execute step displacement profile by bresenham line algorithm
		    out_bits = current_block->direction_bits;
		    st.counter_x += current_block->steps_x;
		    if (st.counter_x > 0) {
		      out_bits |= (1<<X_STEP_BIT);
		      st.counter_x -= st.event_count;
		      if (out_bits & (1<<X_DIRECTION_BIT)) { sys.position[X_AXIS]--; }
		      else { sys.position[X_AXIS]++; }
		    }
		    st.counter_y += current_block->steps_y;
		    if (st.counter_y > 0) {
		      out_bits |= (1<<Y_STEP_BIT);
		      st.counter_y -= st.event_count;
		      if (out_bits & (1<<Y_DIRECTION_BIT)) { sys.position[Y_AXIS]--; }
		      else { sys.position[Y_AXIS]++; }
		    }
		    st.counter_z += current_block->steps_z;
		    if (st.counter_z > 0) {
		      out_bits |= (1<<Z_STEP_BIT);
		      st.counter_z -= st.event_count;
		      if (out_bits & (1<<Z_DIRECTION_BIT)) { sys.position[Z_AXIS]--; }
		      else { sys.position[Z_AXIS]++; }
		    }
		    
		    st.step_events_completed++; // Iterate step events
		
		    // While in block steps, check for de/ac-celeration events and execute them accordingly.
		    if (st.step_events_completed < current_block->step_event_count) {
		      if (sys.state == STATE_HOLD) {
		        // Check for and execute feed hold by enforcing a steady deceleration from the moment of 
		        // execution. The rate of deceleration is limited by rate_delta and will never decelerate
		        // faster or slower than in normal operation. If the distance required for the feed hold 
		        // deceleration spans more than one block, the initial rate of the following blocks are not
		        // updated and deceleration is continued according to their corresponding rate_delta.
		        // NOTE: The trapezoid tick cycle counter is not updated intentionally. This ensures that 
		        // the deceleration is smooth regardless of where the feed hold is initiated and if the
		        // deceleration distance spans multiple blocks.
		        if ( iterate_trapezoid_cycle_counter() ) {                    
		          // If deceleration complete, set system flags and shutdown steppers.
		          if (st.trapezoid_adjusted_rate <= current_block->rate_delta) {
		            // Just go idle. Do not NULL current block. The bresenham algorithm variables must
		            // remain intact to ensure the stepper path is exactly the same. Feed hold is still
		            // active and is released after the buffer has been reinitialized.
		            st_go_idle();
		            bit_true(sys.execute,EXEC_CYCLE_STOP); // Flag main program that feed hold is complete.
		          } else {
		            st.trapezoid_adjusted_rate -= current_block->rate_delta;
		            set_step_events_per_minute(st.trapezoid_adjusted_rate);
		          }      
		        }
		        
		      } else {
		        // The trapezoid generator always checks step event location to ensure de/ac-celerations are 
		        // executed and terminated at exactly the right time. This helps prevent over/under-shooting
		        // the target position and speed. 
		        // NOTE: By increasing the ACCELERATION_TICKS_PER_SECOND in config.h, the resolution of the 
		        // discrete velocity changes increase and accuracy can increase as well to a point. Numerical 
		        // round-off errors can effect this, if set too high. This is important to note if a user has 
		        // very high acceleration and/or feedrate requirements for their machine.
		        if (st.step_events_completed < current_block->accelerate_until) {
		          // Iterate cycle counter and check if speeds need to be increased.
		          if ( iterate_trapezoid_cycle_counter() ) {
		            st.trapezoid_adjusted_rate += current_block->rate_delta;
		            if (st.trapezoid_adjusted_rate >= current_block->nominal_rate) {
		              // Reached nominal rate a little early. Cruise at nominal rate until decelerate_after.
		              st.trapezoid_adjusted_rate = current_block->nominal_rate;
		            }
		            set_step_events_per_minute(st.trapezoid_adjusted_rate);
		          }
		        } else if (st.step_events_completed >= current_block->decelerate_after) {
		          // Reset trapezoid tick cycle counter to make sure that the deceleration is performed the
		          // same every time. Reset to CYCLES_PER_ACCELERATION_TICK/2 to follow the midpoint rule for
		          // an accurate approximation of the deceleration curve. For triangle profiles, down count
		          // from current cycle counter to ensure exact deceleration curve.
		          if (st.step_events_completed == current_block-> decelerate_after) {
		            if (st.trapezoid_adjusted_rate == current_block->nominal_rate) {
		              st.trapezoid_tick_cycle_counter = CYCLES_PER_ACCELERATION_TICK/2; // Trapezoid profile
		            } else {  
		              st.trapezoid_tick_cycle_counter = CYCLES_PER_ACCELERATION_TICK-st.trapezoid_tick_cycle_counter; // Triangle profile
		            }
		          } else {
		            // Iterate cycle counter and check if speeds need to be reduced.
		            if ( iterate_trapezoid_cycle_counter() ) {  
		              // NOTE: We will only do a full speed reduction if the result is more than the minimum safe 
		              // rate, initialized in trapezoid reset as 1.5 x rate_delta. Otherwise, reduce the speed by
		              // half increments until finished. The half increments are guaranteed not to exceed the 
		              // CNC acceleration limits, because they will never be greater than rate_delta. This catches
		              // small errors that might leave steps hanging after the last trapezoid tick or a very slow
		              // step rate at the end of a full stop deceleration in certain situations. The half rate 
		              // reductions should only be called once or twice per block and create a nice smooth 
		              // end deceleration.
		              if (st.trapezoid_adjusted_rate > st.min_safe_rate) {
		                st.trapezoid_adjusted_rate -= current_block->rate_delta;
		              } else {
		                st.trapezoid_adjusted_rate >>= 1; // Bit shift divide by 2
		              }
		              if (st.trapezoid_adjusted_rate < current_block->final_rate) {
		                // Reached final rate a little early. Cruise to end of block at final rate.
		                st.trapezoid_adjusted_rate = current_block->final_rate;
		              }
		              set_step_events_per_minute(st.trapezoid_adjusted_rate);
		            }
		          }
		        } else {
		          // No accelerations. Make sure we cruise exactly at the nominal rate.
		          if (st.trapezoid_adjusted_rate != current_block->nominal_rate) {
		            st.trapezoid_adjusted_rate = current_block->nominal_rate;
		            set_step_events_per_minute(st.trapezoid_adjusted_rate);
		          }
		        }
		      }            
		    } else {   
		      // If current block is finished, reset pointer 
		      current_block = NULL;
		      plan_discard_current_block();
		    }
		  }
		  out_bits ^= settings.invert_mask;  // Apply step and direction invert mask    
		  busy = false;
//==========================================
	}				   
	TIM3->SR&=~(1<<0);//����жϱ�־λ 
}
//==========================================

// This interrupt is set up by ISR_TIMER1_COMPAREA when it sets the motor port bits. It resets
// the motor port after a short period (settings.pulse_microseconds) completing one step cycle.
// NOTE: Interrupt collisions between the serial and stepper interrupts can cause delays by
// a few microseconds, if they execute right before one another. Not a big deal, but can
// cause issues at high step rates if another high frequency asynchronous interrupt is 
// added to Grbl.
//==========================================
//ISR(TIMER2_OVF_vect)
//{
void TIM4_IRQHandler(void)
{
	u8 bit_temp=0;
	if(TIM4->SR&0X0001)//����ж�
	{
//==========================================

//==========================================
		// Reset stepping pins (leave the direction pins)
//		STEPPING_PORT = (STEPPING_PORT & ~STEP_MASK) | (settings.invert_mask & STEP_MASK);	//����������0
		bit_temp=settings.invert_mask&(1<<X_STEP_BIT); 
		HW_GPIO_OUT(STEP_GPIOx,X_STEP_PIN,bit_temp);
		bit_temp=settings.invert_mask&(1<<Y_STEP_BIT);
		HW_GPIO_OUT(STEP_GPIOx,Y_STEP_PIN,bit_temp);
		bit_temp=settings.invert_mask&(1<<Z_STEP_BIT);
		HW_GPIO_OUT(STEP_GPIOx,Z_STEP_PIN,bit_temp);

/*--------����Ĵ�����Ҫ33�л�����--------*/
/*һ��ָ������Ϊ6*1/72Mhz=0.08us��33�л������
��Ҫ2.75us��ƽ����ƽ�仯ִ��ʱ��2.75us/2=2us����
��ҪԤ������2usʱ���ָ��ִ�У�
*/

//==========================================

//==========================================
//		TCCR2B = 0; // Disable Timer2 to prevent re-entering this interrupt when it's not needed. 
		TIM4->CR1&=~(1<<0);			//�ر��ڶ�ʱ��
//==========================================

//==========================================
  	}				   
	TIM4->SR&=~(1<<0);//����жϱ�־λ 
}
//==========================================
//}

#ifdef STEP_PULSE_DELAY
  // This interrupt is used only when STEP_PULSE_DELAY is enabled. Here, the step pulse is
  // initiated after the STEP_PULSE_DELAY time period has elapsed. The ISR TIMER2_OVF interrupt
  // will then trigger after the appropriate settings.pulse_microseconds, as in normal operation.
  // The new timing between direction, step pulse, and step complete events are setup in the
  // st_wake_up() routine.
  ISR(TIMER2_COMPA_vect) 
  { 
    STEPPING_PORT = step_bits; // Begin step pulse.
  }
#endif

// Reset and clear stepper subsystem variables
void st_reset()
{
  memset(&st, 0, sizeof(st));
  set_step_events_per_minute(MINIMUM_STEPS_PER_MINUTE);
  current_block = NULL;
  busy = false;
}

// Initialize and start the stepper motor subsystem
void st_init()
{
	u8 bit_temp=0;
  // Configure directions of interface pins
//==========================================
//  STEPPING_DDR |= STEPPING_MASK;										//��ʼ����������������
//  STEPPING_PORT = (STEPPING_PORT & ~STEPPING_MASK) | settings.invert_mask;		 //���θ�������λ(���������ʧ�ܽ�)
	bit_temp=settings.invert_mask&(1<<X_STEP_BIT);
	HW_GPIO_OUT(STEP_GPIOx,X_STEP_PIN,bit_temp);
	bit_temp=settings.invert_mask&(1<<Y_STEP_BIT);
	HW_GPIO_OUT(STEP_GPIOx,Y_STEP_PIN,bit_temp);
	bit_temp=settings.invert_mask&(1<<Z_STEP_BIT);
	HW_GPIO_OUT(STEP_GPIOx,Z_STEP_PIN,bit_temp);

	bit_temp=settings.invert_mask&(1<<X_DIRECTION_BIT);
	HW_GPIO_OUT(STEP_GPIOx,X_DIRECTION_PIN,bit_temp);
	bit_temp=settings.invert_mask&(1<<Y_DIRECTION_BIT);
	HW_GPIO_OUT(STEP_GPIOx,Y_DIRECTION_PIN,bit_temp);
	bit_temp=settings.invert_mask&(1<<Z_DIRECTION_BIT);
	HW_GPIO_OUT(STEP_GPIOx,Z_DIRECTION_PIN,bit_temp);
//  STEPPERS_DISABLE_DDR |= 1<<STEPPERS_DISABLE_BIT;					//��ʼ�����ʧ������
//====================================	======


//==========================================
  // waveform generation = 0100 = CTC
//  TCCR1B &= ~(1<<WGM13);
//  TCCR1B |=  (1<<WGM12);
//  TCCR1A &= ~(1<<WGM11); 
//  TCCR1A &= ~(1<<WGM10);
//
//  // output mode = 00 (disconnected)
//  TCCR1A &= ~(3<<COM1A0); 
//  TCCR1A &= ~(3<<COM1B0); 
//	
//  // Configure Timer 2
//  TCCR2A = 0; // Normal operation									   //������ʱ
//  TCCR2B = 0; // Disable timer until needed.						   //�رն�ʱ��2����ʹ��ʱ�ٴ�
//  TIMSK2 |= (1<<TOIE2); // Enable Timer2 Overflow interrupt  		   //������ʱ2������ж�

/*------HW_TIM_Init()------*/   
  #ifdef STEP_PULSE_DELAY
    TIMSK2 |= (1<<OCIE2A); // Enable Timer2 Compare Match A interrupt
  #endif
//==========================================
  // Start in the idle state, but first wake up to check for keep steppers enabled option.
  st_wake_up();
  st_go_idle();
}

// Configures the prescaler and ceiling of timer 1 to produce the given rate as accurately as possible.
// Returns the actual number of cycles per interrupt
static uint32_t config_step_timer(uint32_t cycles)	//����Ҫ��TIM3��ʱʱ������λtick��������ʵ�ʵ�TIM3��ʱʱ������λtick��
{
  uint16_t ceiling;
  uint16_t prescaler;
  uint32_t actual_cycles;		   

//===================================
  if (cycles <= 0xffffL) {
    ceiling = cycles;
    prescaler = 1; // prescaler: ����Ƶ
    actual_cycles = ceiling;
  } else if (cycles <= 0x7ffffL) {
    ceiling = cycles >> 3;
    prescaler = 8; // prescaler: 8��Ƶ
    actual_cycles = ceiling * 8L;
  } else if (cycles <= 0x3fffffL) {
    ceiling =  cycles >> 6;
    prescaler = 64; // prescaler: 64��Ƶ
    actual_cycles = ceiling * 64L;
  } else if (cycles <= 0xffffffL) {
    ceiling =  (cycles >> 8);
    prescaler = 256; // prescaler: 256��Ƶ
    actual_cycles = ceiling * 256L;
  } else if (cycles <= 0x3ffffffL) {   //��ʱ��û�е�u32����0xffffffff��������AVR�ķ�Ƶ�����1024��Ƶ����˽�ֹ
    ceiling = (cycles >> 10);
    prescaler = 1024; // prescaler: 1024��Ƶ
    actual_cycles = ceiling * 1024L;    
  } else {
    // Okay, that was slower than we actually go. Just set the slowest speed
	//����1024�ķ�Ƶ��Χ�Ļ�������Ķ�ʱ�����������ٶ�
    ceiling = 0xffff;
    prescaler = 1024;
    actual_cycles = 0xffff * 1024;
  }

/*
	��һ��if�ǽ��´ε�TIM3�Ķ�ʱֵװ��Ĵ�����

	�������õ���ceilingֵ���´ζ�ʱʱ���������
	ʵ�ʵĶ�ʱֵ����λ����о�����ʧ����ʹ��if��
	������Ϊu16��Χ���ƣ�����λ����ΪAVRֻ�ṩ0,
	8,64,256,1024�⼸�ַ�Ƶ��
	
	��stm32�¶�ʱ����0~65535������ֵ��Ƶ�������и�
	���ռ䡣

	����ifԽ���£�TIM3��ʱԽ��.	�����ᶨʱʱ��
	����1024�ķ�Ƶ��Χ��Ȼ����û�г���u32��Χ������
	Ҳ���ԸĽ���
*/


//==========================================
  // Set prescaler
//  TCCR1B = (TCCR1B & ~(0x07<<CS10)) | (prescaler<<CS10); 	//���÷�Ƶ
//  // Set ceiling
//  OCR1A = ceiling;	//���ü���ֵ

	TIM3->ARR=ceiling-1;		//�������Զ���װֵ 1s
	TIM3->PSC=((uint16_t)(prescaler*9/SPEED_FINE_TUNING)>>1)-1;	//Ԥ��Ƶϵ�� 
	//PS:��16M��72M����ʱ��PSC��ARR����Ӹ�ת�����ӣ�ʵ�ʶ�ʱЧ���Ż���ͬ��������72/16=4.5=9>>1
	//������һ����ӡ�ٶ�΢�����ӡ�
//==========================================					   

  return(actual_cycles);
}

static void set_step_events_per_minute(uint32_t steps_per_minute) 
{
  if (steps_per_minute < MINIMUM_STEPS_PER_MINUTE) { steps_per_minute = MINIMUM_STEPS_PER_MINUTE; }		
  //������С�ٶȣ��ٶȹ�СʹTIN3��ʱֵ���󣬳���u32��Χ
  //config_step_timer()ʵ�ʿ��Խ��ܵ����Χ��u32���ٴ�Ļ���ȡ��λ�ͻ�����⣬���Ҫ�ڴ�������
  //u32���ֵ=(TICKS_PER_MICROSECOND*60*1000000)/steps_per_minute�����	steps_per_minute=1
  //MINIMUM_STEPS_PER_MINUTE��config.h������
  st.cycles_per_step_event = config_step_timer(((TICKS_PER_MICROSECOND*60)/steps_per_minute)*1000000);
  //��TICKS_PER_MICROSECOND����ʵ���ٶȻ���Ϊ��ʱ����Ҫ�ĵ�tick���������һ��tickΪ1/72 us(��1/TICKS_PER_MICROSECOND us)��
  //steps_per_minute������ÿ���ӣ�һ������Ҫ1/steps_per_minute���ӣ�Ҳ����(1000000*60)/steps_per_minute	us
}

// Planner external interface to start stepper interrupt and execute the blocks in queue. Called
// by the main program functions: planner auto-start and run-time command execution.
void st_cycle_start() 
{
  if (sys.state == STATE_QUEUED) {
    sys.state = STATE_CYCLE;
    st_wake_up();
  }
}

// Execute a feed hold with deceleration, only during cycle. Called by main program.
void st_feed_hold() 
{
  if (sys.state == STATE_CYCLE) {
    sys.state = STATE_HOLD;
    sys.auto_start = false; // Disable planner auto start upon feed hold.
  }
}

// Reinitializes the cycle plan and stepper system after a feed hold for a resume. Called by 
// runtime command execution in the main program, ensuring that the planner re-plans safely.
// NOTE: Bresenham algorithm variables are still maintained through both the planner and stepper
// cycle reinitializations. The stepper path should continue exactly as if nothing has happened.
// Only the planner de/ac-celerations profiles and stepper rates have been updated.
void st_cycle_reinitialize()
{
  if (current_block != NULL) {
    // Replan buffer from the feed hold stop location.
    plan_cycle_reinitialize(current_block->step_event_count - st.step_events_completed);
    // Update initial rate and timers after feed hold.
    st.trapezoid_adjusted_rate = 0; // Resumes from rest
    set_step_events_per_minute(st.trapezoid_adjusted_rate);
    st.trapezoid_tick_cycle_counter = CYCLES_PER_ACCELERATION_TICK/2; // Start halfway for midpoint rule.
    st.step_events_completed = 0;
    sys.state = STATE_QUEUED;
  } else {
    sys.state = STATE_IDLE;
  }
}
