/**
   @file sensor_rak3172.hpp
   @brief Abstracting a temperature sensor
**/
#ifndef SENSOR_RAK3172_H
#define SENSOR_RAK3172_H

//#define TEMPERATURE_REAL // comment to use the "fake" module

extern uint8_t alarm;

void sensor_Init(void);
float temperature_read(void);
float humidity_read(void);
void set_max_temperature(float max_temp);
void switch_alarm_off(void);

#endif
