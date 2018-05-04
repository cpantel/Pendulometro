/*============================================================================
 * Licencia:
 * Autor:
 * Fecha:
 *===========================================================================*/

/*==================[inclusiones]============================================*/

#include "sapi.h"
#include "os.h" 

#define DEBOUNCE_HITS           5       // debounce count
#define SIGNAL_LEVEL         0x20       // signal threshold

char* itoa(int value, char* result, int base);

typedef enum {
   WAITING_FALLING_EDGE,
   DEBOUNCING_FALLING_EDGE,
   FALLING_EDGE_DETECTED,
   WAITING_RISING_EDGE,
   DEBOUNCING_RISING_EDGE,
   RISING_EDGE_DETECTED
} FSMtimer_t;

uint64_t tickCount = 0; // overflow check missing

int main( void )
{
   boardConfig();

   adcConfig(ADC_ENABLE); 

   uartConfig( UART_USB, 115200 );

   uartWriteString( UART_USB, "\nTimer ISR v1\n");

   StartOS(AppMode1);

   return 0;
}


void ErrorHook(void)
{

   char uartBuffer[] = "0000000000";
   itoa(tickCount, uartBuffer, 10);

   uartWriteString( UART_USB, "Tick count: ");
   uartWriteString( UART_USB, uartBuffer);
   uartWriteString( UART_USB, "\nShutdown os\n");

   ShutdownOS(0);
}

TASK(tickIncrementTask)
{
//   GetResource(TickCount);
   ++tickCount;
   if (tickCount % 100 == 0 ) gpioToggle( LED1);

//   uartWriteString( UART_USB, ".");
//   ReleaseResource(TickCount);

   TerminateTask();
}

TASK(adcStartAsyncWrapperTask)
{
   adcStartAsync(CH1);
   if (tickCount % 25 == 0 ) gpioToggle( LED2);
   TerminateTask();
}



ISR (adcReadAsyncWrapperISR)
{
  //   uartWriteString( UART_USB, "<");
   SetEvent(FSMTask, ADCRead);
   if (tickCount % 25 == 0 ) gpioToggle( LED3);


  //   uartWriteString( UART_USB, ">");
}

TASK (FSMTask) {
   static uint8_t low = 0;
   static uint8_t high = 0;

   static tick_t ticksUp = 0;
   static tick_t ticksDown = 0;

   static tick_t tickMarkUp = 0;
   static tick_t tickMarkDown = 0;

   static FSMtimer_t state = WAITING_FALLING_EDGE;

   uartWriteString( UART_USB, "pre while\n");

   while (1) {
      gpioToggle( LED3 );
      uartWriteString( UART_USB, "pre waitevent\n");
      WaitEvent(ADCRead);
      uartWriteString( UART_USB, "post waitevent\n");

      uint16_t sample = adcReadAsync( CH1 , !ADC_CLEAR_INT );
      unsigned int belowThreshold = sample  < SIGNAL_LEVEL;

      uartWriteString( UART_USB, "switch\n");
      switch(state) {
         case WAITING_FALLING_EDGE:
//            gpioWrite( LED1, 1 );
  //          gpioWrite( LED2, 0 );
    //        gpioWrite( LED3, 0 );

            if (belowThreshold) {
               state = DEBOUNCING_FALLING_EDGE;
               low = 1;
               high = 0;
            }

         break;

         case DEBOUNCING_FALLING_EDGE:
//            gpioWrite( LED1, 0 );
  //          gpioWrite( LED2, 1 );
    //        gpioWrite( LED3, 0 );

            if (belowThreshold) {
               ++low;
               if ( low >= DEBOUNCE_HITS) {
                  state = FALLING_EDGE_DETECTED;
               }
            } else {
               low = 0;
            }
         break;

         case FALLING_EDGE_DETECTED:
//            gpioWrite( LED1, 1 );
  //          gpioWrite( LED2, 1 );
    //        gpioWrite( LED3, 0 );

            GetResource(TickCount);
            tickMarkDown = tickCount;
            ReleaseResource(TickCount);
 
            ticksUp = tickMarkDown - tickMarkUp;

            state = WAITING_RISING_EDGE;

         break;

         case WAITING_RISING_EDGE:
      //      gpioWrite( LED1, 0 );
        //    gpioWrite( LED2, 0 );
          //  gpioWrite( LED3, 1 );

            if (! belowThreshold) {
               state = DEBOUNCING_RISING_EDGE;
               low = 0;
               high = 1;
            }

         break;

         case DEBOUNCING_RISING_EDGE:
//            gpioWrite( LED1, 1 );
  //          gpioWrite( LED2, 0 );
    //        gpioWrite( LED3, 1 );

            if (!belowThreshold) {
               ++high;
               if ( high >= DEBOUNCE_HITS) {
                  state = RISING_EDGE_DETECTED;
               }
            } else {
               high = 0;
            }

         break;

         case RISING_EDGE_DETECTED:
            //gpioWrite( LED1, 0 );
            //gpioWrite( LED2, 1 );
            //gpioWrite( LED3, 1 );

            GetResource(TickCount);
            tickMarkUp = tickCount;
            ReleaseResource(TickCount);
 
            ticksDown = tickMarkUp - tickMarkDown;

            char uartBuffer[] = "0000000000";
            itoa(ticksDown + ticksUp, uartBuffer, 10);
            uartWriteString( UART_USB, uartBuffer);
            uartWriteString( UART_USB, "\n");

            //gpioWrite( LED1, 1 );

            state = WAITING_FALLING_EDGE;

         break;
      }
      uartWriteString( UART_USB, "clear event\n");

      ClearEvent(ADCRead);
      uartWriteString( UART_USB, "clear interrupt\n");

      adcClearInterrupt(CH1);
      uartWriteString( UART_USB, "end loop\n");

   }
   TerminateTask();
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

