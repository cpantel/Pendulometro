/*============================================================================
 * Licencia:
 * Autor:
 * Fecha:
 *===========================================================================*/

/*==================[inclusiones]============================================*/

#include "sapi.h"       // <= sAPI header
#include "os.h"         // <= freeOSEK

/*==================[definiciones y macros]==================================*/

/*==================[definiciones de datos internos]=========================*/

/*==================[definiciones de datos externos]=========================*/

/*==================[declaraciones de funciones internas]====================*/

/*==================[declaraciones de funciones externas]====================*/

/*==================[funcion principal]======================================*/

uint16_t muestra = 0;


int main( void )
{
   boardConfig();

   adcConfig(ADC_ENABLE); 

   StartOS(AppMode1);

   return 0;
}


void ErrorHook(void)
{
	ShutdownOS(0);
}

TASK(adcStartAsyncWrapperTask)
{
   gpioToggle( LEDB );

   adcStartAsync(CH1);

   TerminateTask();
}

ISR (adcReadAsyncWrapperISR)
{
   //GetResource(AnalogValue);

   gpioToggle( LED3 );

   muestra = adcReadAsync(CH1);

   Chip_PININT_ClearIntStatus( LPC_GPIO_PIN_INT,PININTCH( 0 ) );


   //ReleaseResource(AnalogValue);

}

/*==================[definiciones de funciones internas]=====================*/

/*==================[definiciones de funciones externas]=====================*/

/*==================[end of file]============================================*/
