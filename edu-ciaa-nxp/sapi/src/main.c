#include "sapi.h"

#define DEBOUNCE_HITS           5       // debounce count
#define SIGNAL_LEVEL         0x20       // signal threshold

char* itoa(int value, char* result, int base);

typedef enum {
    WAIING_FALLING_EDGE,
    DEBOUNCING_FALLING_EDGE,
    FALLING_EDGE_DETECTED,
    WAITING_RISING_EDGE,
    DEBOUNCING_RISING_EDGE,
    RISING_EDGE_DETECTED
} FSMtimer_t;

int main(void){
   uint8_t low = 0;
   uint8_t high = 0;
    
   tick_t ticksUp = 0;
   tick_t ticksDown = 0;

   tick_t tickMarkUp = 0;
   tick_t tickMarkDown = 0;

   FSMtimer_t state = WAIING_FALLING_EDGE;

   boardConfig();
   adcConfig( ADC_ENABLE );
   lcdInit( 16, 2, 5, 8 );

   lcdClear(); 
   lcdGoToXY( 1, 1 );
   lcdSendStringRaw( "Timer" );

   char lcdBuffer[] = "0000000000";

   while(1) {
     uint16_t muestra = adcRead( CH1 );
     unsigned int belowThreshold = muestra  < SIGNAL_LEVEL;


     switch(state) {
        case WAIING_FALLING_EDGE:
            gpioWrite( LED1, 1 );
            gpioWrite( LED2, 0 );
            gpioWrite( LED3, 0 );

            if (belowThreshold) {
                state = DEBOUNCING_FALLING_EDGE;
                low = 1;
                high = 0;
            }

        break;

        /****************************************************************/
        case DEBOUNCING_FALLING_EDGE:
            gpioWrite( LED1, 0 );
            gpioWrite( LED2, 1 );
            gpioWrite( LED3, 0 );

            if (belowThreshold) {
                ++low;
                if ( low >= DEBOUNCE_HITS) {
                    state = FALLING_EDGE_DETECTED;
                }
            } else {
                low = 0;
            }
        break;

        /****************************************************************/
        case FALLING_EDGE_DETECTED:
            gpioWrite( LED1, 1 );
            gpioWrite( LED2, 1 );
            gpioWrite( LED3, 0 );

            tickMarkDown = tickRead();
            ticksUp = tickMarkDown - tickMarkUp;

            state = WAITING_RISING_EDGE;

        break;

        /****************************************************************/
        case WAITING_RISING_EDGE:
            gpioWrite( LED1, 0 );
            gpioWrite( LED2, 0 );
            gpioWrite( LED3, 1 );

            if (! belowThreshold) {
                state = DEBOUNCING_RISING_EDGE;
                low = 0;
                high = 1;
            }

        break;

        /****************************************************************/
        case DEBOUNCING_RISING_EDGE:
            gpioWrite( LED1, 1 );
            gpioWrite( LED2, 0 );
            gpioWrite( LED3, 1 );

            if (!belowThreshold) {
                ++high;
                if ( high >= DEBOUNCE_HITS) {
                    state = RISING_EDGE_DETECTED;
                }
            } else {
                high = 0;
            }

        break;

        /****************************************************************/
        case RISING_EDGE_DETECTED:
            gpioWrite( LED1, 0 );
            gpioWrite( LED2, 1 );
            gpioWrite( LED3, 1 );

            tickMarkUp = tickRead();
            ticksDown = tickMarkUp - tickMarkDown;

            lcdClear();
            itoa(ticksDown + ticksUp, lcdBuffer, 10);
            lcdGoToXY( 1, 1 );

            lcdSendStringRaw( lcdBuffer );

            gpioWrite( LED1, 1 );

            state = WAIING_FALLING_EDGE;

        break;

        /****************************************************************/
        default:
        break;

     }

   }

   return 0 ;
}

/**
 * C++ version 0.4 char* style "itoa":
 * Written by Lukás Chmela
 * Released under GPLv3.

 */
char* itoa(int value, char* result, int base) {
   // check that the base if valid
   if (base < 2 || base > 36) { *result = '\0'; return result; }

   char* ptr = result, *ptr1 = result, tmp_char;
   int tmp_value;

   do {
      tmp_value = value;
      value /= base;
      *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
   } while ( value );

   // Apply negative sign
   if (tmp_value < 0) *ptr++ = '-';
   *ptr-- = '\0';
   while(ptr1 < ptr) {
      tmp_char = *ptr;
      *ptr--= *ptr1;
      *ptr1++ = tmp_char;
   }
   return result;
}


