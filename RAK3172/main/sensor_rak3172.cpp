/* sensor_rak3172.cpp
 * Pseudo temperature sensor
 */

#include <stdint.h>
#include "sensor_rak3172.hpp"
#include "rak1901.h"
#include "led.hpp"

rak1901 rak1901;

#define DEFAULT_MAX_TEMPERATURE 37.5

static float max_temperature = DEFAULT_MAX_TEMPERATURE;
uint8_t alarm = 0;

void sensor_Init(void)
{
    Wire.begin();

    rak1901.init();
}

float temperature_read(void)
{
    static float temp = 0.0;
    if (rak1901.update()) 
    {
        temp = rak1901.temperature();
        if(temp > max_temperature){
          alarm = 1;
          LED_On();
        }
    }

    return temp;
}

float humidity_read(void)
{
    static float hum = 0.0;
    if (rak1901.update()) 
    {
        hum = rak1901.humidity();
    }

    return hum;
}

void set_max_temperature(float max_temp)
{
    max_temperature = max_temp;
}

void switch_alarm_off(void)
{
    alarm = 0;
    LED_Off();
}