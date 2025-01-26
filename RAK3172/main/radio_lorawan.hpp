/**
   @file radio_lorawan.hpp
   @brief Abstracting the radio modem
**/

#ifndef RADIO_H
#define RADIO_H

bool radio_Init(void);
void radio_Run(void);

void radio_startPayload(uint8_t port);
bool radio_sendPayload(void);

void radio_addUint16_t(uint16_t value);
void radio_addTemperature(float value);
void radio_addCLPPTemperature(float value);
void radio_addHumidity(float value);
void radio_addCLPPHumidity(float value);
void radio_addAlarm(uint8_t value);
void radio_addCLPPAlarm(uint8_t value);

int16_t radio_downlinkRead(void);

// provisional
#define OTAA_BAND     (RAK_REGION_EU868)

#endif
