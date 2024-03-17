/**
 * Driver for ping sensor
 * @file ping.c
 * @author
 */

#include "ping.h"
#include "Timer.h"

volatile unsigned long START_TIME = 0;
volatile unsigned long END_TIME = 0;
volatile enum{LOW, HIGH, DONE} STATE = LOW; // State of ping echo pulse

void ping_init (void){

  // YOUR CODE HERE

	SYSCTL_RCGCGPIO_R |= 0x2;
    SYSCTL_RCGCTIMER_R |= 0x8;

    while ((SYSCTL_PRGPIO_R & 0x2) == 0) {};
    while ((SYSCTL_PRTIMER_R & 0x8) == 0) {};

    GPIO_PORTB_DIR_R |= 0x8;
    GPIO_PORTB_DEN_R |= 0x8;
    GPIO_PORTB_AFSEL_R |= 0x8;

    GPIO_PORTB_PCTL_R &= ~0x7000;
    GPIO_PORTB_PCTL_R |= 0x7000;

    TIMER3_CTL_R &= ~0x100;

    TIMER3_CFG_R |= 0x4; //
    TIMER3_TBMR_R &= ~0x10;
    TIMER3_TBMR_R |= 0x7;
	

	TIMER3_CTL_R |= 0xC00;

    TIMER3_TBPR_R = 0xFF;

    TIMER3_TBILR_R = 0xFFFF;
	
	TIMER3_IMR_R |= 0x400;
	
	TIMER3_ICR_R |= 0x400;

    //TIMER3_CTL_R |= 0x100;
	NVIC_EN1_R |= 0x10;


    NVIC_PRI0_R |= 0x20;

    IntRegister(INT_TIMER3B, TIMER3B_Handler);

    IntMasterEnable();
    // Configure and enable the timer
    TIMER3_CTL_R |= 0x100;

    //enable portb, and portb pin, den,
}

void ping_trigger (void){
    STATE = LOW;
    // Disable timer and disable timer interrupt
    TIMER3_CTL_R = ~0x100;
    TIMER3_IMR_R = ~0x400;
    // Disable alternate function (disconnect timer from port pin)
    GPIO_PORTB_AFSEL_R = ~0x08;                                                           //

    //dir
    GPIO_PORTB_DIR_R |= 0x08;       //use as output
    // YOUR CODE HERE FOR PING TRIGGER/START PULSE
    GPIO_PORTB_DATA_R &= ~0x08;
    GPIO_PORTB_DATA_R |= 0x08;
    STATE = HIGH;
    timer_waitMicros(5);
    // 5 us-> .000005 sec
    //GPIO_PORTB_DIR_R &= ~0x08;
    STATE = LOW;
    GPIO_PORTB_DATA_R &= ~0x08;

    // Clear an interrupt that may have been erroneously triggered
    //TIMER3_ICR_R |= 0x400;                                                ////////
    // Re-enable alternate function, timer interrupt, and timer
    GPIO_PORTB_AFSEL_R |= 0x08;
    TIMER3_IMR_R |= 0x400;
    TIMER3_CTL_R |= 0x100;
    //GPIO_PORTB_DIR_R = ~0x08;           //use as input
}

void TIMER3B_Handler(void){

    if(TIMER3_MIS_R & 0x400) {
        TIMER3_ICR_R |= 0x400;
        if(STATE == LOW) {
            START_TIME = TIMER3_TBR_R;
            STATE = HIGH;
        }
        else if(STATE == HIGH) {
            END_TIME = TIMER3_TBR_R;
            STATE = DONE;
        }
    }
  // YOUR CODE HERE
  // As needed, go back to review your interrupt handler code for the UART lab.
  // What are the first lines of code in the ISR? Regardless of the device, interrupt handling
  // includes checking the source of the interrupt and clearing the interrupt status bit.
  // Checking the source: test the MIS bit in the MIS register (is the ISR executing
  // because the input capture event happened and interrupts were enabled for that event?
  // Clearing the interrupt: set the ICR bit (so that same event doesn't trigger another interrupt)
  // The rest of the code in the ISR depends on actions needed when the event happens.

}

float ping_getDistance (void){

    ping_trigger();
    while(STATE != DONE) {}
    int start = (START_TIME);
    int end = (END_TIME);
    int clock_cycles = start - end;
    double time = (1.0/16000000) * clock_cycles;
    double width = time * 34300/2;
    return width;
    // YOUR CODE HERE

}
