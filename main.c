/*
 *
 * hardware
 * @TODO pasar a milesimas
 * @TODO
 * @TODO selector de modo pendular vs circular
 *
 */

#include <msp430.h>
#include <driverlib.h>

#include "myGpio.h"
#include "myClocks.h"
#include "myLcd.h"

#define DEBOUNCE_HITS       5
#define TIMER_VALUE        32
#define SIGNAL          0x200

typedef enum {
    WAIING_FALLING_EDGE,
    DEBOUNCING_FALLING_EDGE,
    FALLING_EDGE_DETECTED,
    WAITING_RISING_EDGE,
    DEBOUNCING_RISING_EDGE,
    RISING_EDGE_DETECTED
} FSMtimer_t;



#define ENABLE_PINS     0xFFFE




void ADC_setup() {
    #define ADC12_SHT_16       0x0200         // 16 clock cycles for sample and hold
    #define ADC12_ON           0x0010         // ADC12
    #define ADC12_SHT_SRC_SEL  0x0200         // source for sample and hold
    #define ADC12_12BIT        0x0020
    #define ADC12_P92          0x000A

    ADC12CTL0  = ADC12_ON | ADC12_SHT_16  ;   // turn on and set sample and hold time
    ADC12CTL1  = ADC12_SHT_SRC_SEL ;          // specify sample and hold clock source
    ADC12CTL2  = ADC12_12BIT ;                // 12 bit conversion result
    ADC12MCTL0 = ADC12_P92 ;                  // use p9.2 as analog input
}

void Timer_setup() {
    #define ACLK                0x0100        // Timer_A ACLK source
    #define UP                  0x0010        // Timer_A UP mode

    TA0CCR0 = TIMER_VALUE;                    // Set timer interval
    TA0CTL = ACLK + UP;                       // Set timer mode
    TA0CCTL0 = CCIE;                          // Enable interrupt for Timer_0
}

unsigned int tickCount = 0;
/**
 * main.c
 */
int main(void)
{

    WDTCTL = WDTPW | WDTHOLD;                  // stop watchdog timer

    initGPIO();
    initClocks();
    myLCD_init();

    PM5CTL0 = ENABLE_PINS;                     // enable inputs and outputs

    P1DIR = BIT0;                              // set red led as output
    P9DIR = BIT7;                              // set green led as output

    P1OUT = 0x00;                              // turn off red led
    P9OUT = 0x00;                              // turn off green led

    ADC_setup();

    Timer_setup();

    ADC12IER0 = ADC12IE0;                      // hook ISR to ADC conversion complete

    myLCD_showChar('T',1);
    myLCD_showChar('I',2);
    myLCD_showChar('M',3);
    myLCD_showChar('E',4);
    myLCD_showChar('R',5);


    _BIS_SR(GIE);                              // enable interrupts

    ADC12CTL0 = ADC12CTL0 | ADC12ENC;          // enable conversion
    ADC12CTL0 = ADC12CTL0 | ADC12SC;           // start first conversion

    while (1) {

    }
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer0_CCR0_MATCH(void){
    ++tickCount;
}

#pragma vector=ADC12_VECTOR
__interrupt void ADC12_ISR(void){

    static int low = 0;
    static int high = 0;

    static FSMtimer_t state = WAIING_FALLING_EDGE;

    int justRead = ADC12MEM0 < SIGNAL;

    switch(state) {
        case WAIING_FALLING_EDGE:
            if (justRead) {
                state = DEBOUNCING_FALLING_EDGE;
                low = 1;
                high = 0;
            }
        break;

        case DEBOUNCING_FALLING_EDGE:
            if (justRead) {
                ++low;
                if ( low >= DEBOUNCE_HITS) {
                    state = FALLING_EDGE_DETECTED;
                }
            } else {
                low = 0;
            }
        break;

        case FALLING_EDGE_DETECTED:
            TA0CCR0 = 0;                   // stop timer
            myLCD_displayNumber(tickCount);
            myLCD_showSymbol(LCD_UPDATE,LCD_A3DP,0);
            tickCount = 0;
            TA0CCR0 = TIMER_VALUE;         // restart timer
            P1OUT = BIT0;
            state = WAITING_RISING_EDGE;

        break;

        case WAITING_RISING_EDGE:
            if (! justRead) {
                state = DEBOUNCING_RISING_EDGE;
                low = 0;
                high = 1;
            }
        break;
        case DEBOUNCING_RISING_EDGE:
            if (!justRead) {
                ++high;
                if ( high >= DEBOUNCE_HITS) {
                    state = RISING_EDGE_DETECTED;
                }
            } else {
                high = 0;
            }

        break;
        case RISING_EDGE_DETECTED:
            P1OUT = 0x00;
            state = WAIING_FALLING_EDGE;
        default:


    }
    ADC12CTL0 = ADC12CTL0 | ADC12SC;          // start new conversion
}
