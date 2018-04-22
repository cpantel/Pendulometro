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

#include "myClocks.h"
#include "myLcd.h"

#define DEBOUNCE_HITS           5       // debounce count
#define TIMER_VALUE            32       //
#define SIGNAL              0x200       // signal threshold

#define RED_ON              0x0001      // Enable and turn on the red LED
#define RED_OFF             0xFFFE      // Turn off the red LED
#define GREEN_ON            0x0080      // Enable and turn on the green LED
#define GREEN_OFF           0xFF7F      // Turn off the green LED
#define ENABLE_PINS         0xFFFE      // Required to use inputs and outputs
#define BUTTON1             0x0002      // p1.1 is button 1
#define BUTTON2             0x0004      // p1.2 is button 2

typedef enum {
    WAIING_FALLING_EDGE,
    DEBOUNCING_FALLING_EDGE,
    FALLING_EDGE_DETECTED,
    WAITING_RISING_EDGE,
    DEBOUNCING_RISING_EDGE,
    RISING_EDGE_DETECTED
} FSMtimer_t;

void ADC_setup() {
    #define ADC12_SHT_16       0x0200         // 16 clock cycles for sample and hold
    #define ADC12_ON           0x0010         // ADC12
    #define ADC12_SHT_SRC_SEL  0x0200         // source for sample and hold
    #define ADC12_12BIT        0x0020
    #define ADC12_P92          0x000A

    ADC12CTL0  = ADC12_ON | ADC12_SHT_16;     // turn on and set sample and hold time
    ADC12CTL1  = ADC12_SHT_SRC_SEL;           // specify sample and hold clock source
    ADC12CTL2  = ADC12_12BIT;                 // 12 bit conversion result
    ADC12MCTL0 = ADC12_P92;                   // use p9.2 as analog input
    ADC12IER0  = ADC12IE0;                    // hook ISR to ADC conversion complete
    ADC12CTL0  = ADC12CTL0 | ADC12ENC;        // enable conversion
}

void Timer_setup() {
    #define ACLK                0x0100        // Timer_A ACLK source
    #define UP                  0x0010        // Timer_A UP mode

    TA0CCR0 = TIMER_VALUE;                    // Set timer interval
    TA0CTL = ACLK + UP;                       // Set timer mode
    TA0CCTL0 = CCIE;                          // Enable interrupt for Timer_0
}

void DigitalIO_setup(){
    P1DIR |= RED_ON;                    // set red led as output
    P1OUT &= RED_OFF;                   // turn off red led

    P9DIR |= GREEN_ON;                  // set green led as output
    P9OUT &= GREEN_OFF;                 // turn off green led

    P1OUT = P1OUT|BUTTON1;              // set p1.1 as input
    P1REN = P1REN|BUTTON1;              // enable pull-up resistor

    P1OUT = P1OUT|BUTTON2;              // set p1.2 as input
    P1REN = P1REN|BUTTON2;              // enable pull-up resistor

    PM5CTL0 = ENABLE_PINS;              // enable inputs and outputs

    P1IE  = BIT1;                       // hook interrupt to push button1?
    P1IES = BIT1;                       // hook interrupt to push button1?
    P1IFG = 0x00;                       // enable interrupt?
}

unsigned int tickCount = 0;
/**
 * main.c
 */
int main(void)
{

    WDTCTL = WDTPW | WDTHOLD;                  // stop watchdog timer

    // Set LFXT (low freq crystal pins) to crystal input (rather than GPIO)
    GPIO_setAsPeripheralModuleFunctionInputPin(
            GPIO_PORT_PJ,
            GPIO_PIN4 +                 // LFXIN  on PJ.4
            GPIO_PIN5 ,                 // LFXOUT on PJ.5
            GPIO_PRIMARY_MODULE_FUNCTION
    );

    initClocks();
    myLCD_init();

    ADC_setup();
    Timer_setup();
    DigitalIO_setup();

    myLCD_showChar('T',1);
    myLCD_showChar('I',2);
    myLCD_showChar('M',3);
    myLCD_showChar('E',4);
    myLCD_showChar('R',5);

    _BIS_SR(GIE);                              // enable interrupts

    ADC12CTL0 = ADC12CTL0 | ADC12SC;           // start first conversion

    while (1) {

    }
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer0_CCR0_MATCH(void){
    ++tickCount;

}

#pragma vector=PORT1_VECTOR
__interrupt void Port1_ISR(void) {
    switch (P1IV) {
        case 6:
            P1OUT = P1OUT ^ BIT0;
        break;
        case 4:
            P9OUT = P9OUT ^ BIT7;
        break;
    }
    P1IFG &= ~(BIT1);
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
