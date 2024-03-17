#include "adc.h"

void adc_init(void)
{
    static uint8_t initialized = 0;

        //Check if already initialized
        if(initialized){
            return;
        }

        SYSCTL_RCGCADC_R |= 0x01; //  activate ADC0
        SYSCTL_RCGCGPIO_R |= 0x02; // activate clock for Port B
       // while ((SYSCTL_PRGPIO_R & 0x10) == 0) {}; // 3 for stabilization
        GPIO_PORTB_DIR_R &= ~0x10; // make PE4 input
        GPIO_PORTB_AFSEL_R |= 0x10; // enable alternate function on PE4
        GPIO_PORTB_DEN_R &= ~0x10; // disable digital I/O on PE4
        GPIO_PORTB_AMSEL_R |= 0x10;


        ADC0_PC_R &= ~0xF;
        ADC0_PC_R |= 0x1; // 8) configure for 125K samples/sec
        ADC0_SSPRI_R = 0x0123; // 9) Sequencer 3 is highest priority
        ADC0_ACTSS_R &= ~0x0008; // 10) disable sample sequencer 3
        ADC0_EMUX_R &= ~0xF000; // 11) seq3 is software trigger
        ADC0_SSMUX3_R &= ~0x000F;
        ADC0_SSMUX3_R += 10; // 12) set channel
        ADC0_SSCTL3_R = 0x0006; // 13) no TS0 D0, yes IE0 END0
        ADC0_IM_R &= ~0x0008; // 14) disable SS3 interrupts
        ADC0_ACTSS_R |= 0x0008; // 15) enable sample sequencer 3

        initialized = 1;
}

uint16_t adc_read(void)
{

    ADC0_PSSI_R = 0x08;
       while ((ADC0_RIS_R & 0x08) == 0) {};
       uint16_t useThis = 0;
       useThis = (ADC0_SSFIFO3_R & 0xFFF);
       ADC0_ISC_R = 0x08;
       return useThis;

}
