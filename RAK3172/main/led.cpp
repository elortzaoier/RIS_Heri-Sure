#include <Arduino.h>
#include "led.hpp"

/*************************************************************************************************/
/**
  * @brief Preparing pin corresponding to LED
  * @return none
  */
void LED_Init(void) 
{
    pinMode(LED_BUILTIN, OUTPUT);
}

/*************************************************************************************************/
/**
  * @brief  Turn ON the LED
  * @return none
  */
void LED_On(void) 
{
    digitalWrite(LED_BUILTIN, HIGH);
}


/*************************************************************************************************/
/**
  * @brief  Turn OFF the LED
  * @return none
  */
void LED_Off(void) 
{  
    digitalWrite(LED_BUILTIN, LOW);
}

/*** End of file *********************************************************************************/
