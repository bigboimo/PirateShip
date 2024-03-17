/**
 * Driver for ping sensor
 * @file ping.c
 * @author
 */

#include "servo.h"
#include "Timer.h"

void servo_init (void){
    SYSCTL_RCGCGPIO_R |= 0x2;
    SYSCTL_RCGCTIMER_R |= 0x2;

    while ((SYSCTL_PRGPIO_R & 0x2) == 0) {};
    while ((SYSCTL_PRTIMER_R & 0x2) == 0) {};

    GPIO_PORTB_DEN_R |= 0x20;  //0xFF
    GPIO_PORTB_DIR_R |= 0x20; //
    GPIO_PORTB_AFSEL_R |= 0x20;

    GPIO_PORTB_PCTL_R &= ~0x700000;
    GPIO_PORTB_PCTL_R |= 0x700000;

    TIMER1_CTL_R &= ~0x100;

    TIMER1_CFG_R = 0x4; //

    TIMER1_TBMR_R = 0x8;

    TIMER1_TBPR_R = 0x4;

    TIMER1_TBILR_R = 0xE200;

    TIMER1_CTL_R |= 0x100;
}

void servo_move(uint16_t degrees) {

    int pulse_width = 0;
    pulse_width = ((28992 * (degrees / 180.0)) + 874);

    TIMER1_TBPMR_R = ((320000 - pulse_width) >> 16);

    TIMER1_TBMATCHR_R = ((320000 - pulse_width) & 0xFFFF);

    timer_waitMillis(50);

}


//volatile unsigned long match_val;
////int pulse_width = 0;
//
//void servo_init(void) {
//    // Use Timer1B, GPIO Port B pin 5 (PB5)
//    // Set up PWM mode
//    // 16 bit timer
//    // 8 bit prescaler is used creating a 24 bit timer
//    // Timer is 24-bit countdown counter in PWM mode
//    // Use periodic mode
//
//
//    // Step 1: Port B Initialization
//
//    // Enable Port B
//    SYSCTL_RCGCGPIO_R |= 0x2;
//
//    // Enable alternate function on bitfield 7
//    GPIO_PORTB_AFSEL_R |= 0x20; // bit 5 = 1 = 10|0000
//
//    GPIO_PORTB_DEN_R |= 0x20;
//
//    GPIO_PORTB_DIR_R |= 0x20;
//
//    // Configure for GPIO Pin 5, which is bits 23:20, and set to 7
//    GPIO_PORTB_PCTL_R |= 0x00700000;
//
//
//    // Step 2: Timer Initialization
//
//    // Enable clock to Timer 1B, bitfield 1 = 1
//    SYSCTL_RCGCTIMER_R |= 0x2;
//
//    // Check that clock is ready
//    while ((SYSCTL_RCGCTIMER_R & 0x2) == 0) {};
//
//    // Disable Timer B, bitfield 8 = 0
//    TIMER1_CTL_R &= ~0x100;
//
//    // Set to 16 bit mode
//    TIMER1_CFG_R |= 0x00000004;
//
//
//    /**
//    * Step 3: Set up PWM mode
//    * Enable periodic timer mode, bitfield 1:0 = 10
//    * Set capture mode to edge-count mode, bitfield 2 = 0
//    * Enable PWM mode, bitfield 3 = 1
//    * Set count direction to down, bitfield 4 = 0
//    * = 0b01010
//    */
//    TIMER1_TBMR_R |= 0b01010; // moves here for some reason
//
//    // configure output state of pwm signal
//    // tnpwml field
//    TIMER1_CTL_R &= ~0x4000;
//
//    // Load in start value
//    TIMER1_TBPR_R = 0x4D815 >> 16; // upper 8 bits on prescaler
//    TIMER1_TBILR_R = 0x4D815 & 0xFFFF; // lower 16 bits on timer
//
//    // Re-enable Timer B, bitfield 8 = 1
//    TIMER1_CTL_R |= 0x100; // moves all the way to the left for some reason
//
//}
//
//
///**
//* Movement of servo:
//* Rotates to a position driven by a pulse with a
//* specific period and high pulse width.
//* Pulse every 20 milliseconds.
//* Length of pulse depends on length and direction motor turns.
//*
//* Pulse is high for 2 milliseconds = 180 degrees
//* Pulse is high for 1.5 milliseconds = 90 degrees
//* Pulse is high for 1 millisecond = 0 degrees
//*
//* y = mx + b between pulse widths and servo position
//* y = pulse widths, x = servo position in degrees
//*
//* Degree to counter conversion:
//* start_count - end_count / Range of motion in degrees
//*
//* N = 90, pulse is high for 1.5 milliseconds
//* match = (degree u turn to / 180) + 1 * period
//*/
//void servo_move(uint16_t degrees) {
//
//    // Set PWM signal high
//    GPIO_PORTB_DATA_R |= 0x20;
//
////    float temp = (1/180.0) * degrees + 1;         //Pulse width in millis
////    temp /= 1000;                                 //Pulse width in seconds
////    uint32_t highPulse = temp * 16000000;       //Convert to Clock steps
////    uint32_t lowPulse = 320000 - highPulse;      //Calculate low pulse
//
////    uint32_t lowPulse = rightBound – ((rightBound – leftBound) / 180) * degrees;
////    rightBound = leftBound + 32000 = 323022
//
//    // Find match value
//     match_val = 311792 - (((311792 - 279792) / 180) * degrees);
//
//
//    // Load in match value
//    TIMER1_TBPMR_R = match_val >> 16; // upper 8 bits on prescaler
//    TIMER1_TBMATCHR_R = match_val & 0xFFFF; // lower 16 bits on timer
//
//    // Stay high until current counter equals match value
//    GPIO_PORTB_DATA_R &= ~0x20; // Set PWM signal low
//}
