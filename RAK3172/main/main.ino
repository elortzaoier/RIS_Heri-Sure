/***
 *  This example shows LoRaWan protocol joining the network in OTAA mode, class A, region EU868.
 *  Device will send uplink every 20 seconds.
***/

#include "led.hpp"
#include "sensor_rak3172.hpp"
#include "credentials.h"
#include "radio_lorawan.hpp"

#define MEASUREMENT_PERIOD   (20000)

void setup()
{
    Serial.begin(115200, RAK_AT_MODE);
    delay(5000);
  
    Serial.println("RAKwireless LoRaWan OTAA Example");
    Serial.println("------------------------------------------------------");
  
    LED_Init();
    LED_On();
    delay(500);
    LED_Off();

    radio_Init();

    sensor_Init();
}

void loop()
{
    static uint64_t last = 0;
    static uint64_t elapsed;
    float temperature, humidity;
  
    if ((elapsed = millis() - last) > MEASUREMENT_PERIOD) 
    {
        temperature = temperature_read();
        humidity = humidity_read();
        radio_startPayload(66);
        radio_addCLPPTemperature(temperature);
        radio_addCLPPHumidity(humidity);
        radio_addCLPPAlarm(alarm);
        Serial.printf("Temperature = %.2f。C\r\n", temperature);
        Serial.printf("Humidity = %.2f%%\r\n", humidity);
        radio_sendPayload();   
  
        last = millis();
    }
    
    api.system.sleep.all(MEASUREMENT_PERIOD);
    
}
