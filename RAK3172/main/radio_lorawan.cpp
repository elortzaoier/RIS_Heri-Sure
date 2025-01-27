/**
   @file radio.cpp
   @brief Abstracting the radio modem
   Very basic. Some extra work needed for a real implementation
*/

#include <Arduino.h>
//#include <MKRWAN.h>
#include "radio_lorawan.hpp"
#include "credentials.h"  // LoRaWAN credentials
#include "sensor_rak3172.hpp"

static bool connected = false;

/** Packet buffer for sending */
static uint8_t uplink_payload[64];  
static uint8_t uplink_cnt = 0;

/* fport */
static uint8_t fport = 33;

void recvCallback(SERVICE_LORA_RECEIVE_T * data)
{
    if (data->BufferSize > 0) 
    {
      Serial.println("Something received");
      switch (data->Buffer[1]) 
      {
        case 0x01:
          switch_alarm_off();
          Serial.println("Alarm disabled!"); 
          break;
        case 0x67:
          int16_t temperature_encoded;
          float temperature;
          temperature_encoded = (data->Buffer[2] << 8) | data->Buffer[3]; // Eassemble the 16-bit value
          temperature = temperature_encoded / 10.0f; // Convert from coded value to original temperature
          set_max_temperature(temperature);
          Serial.printf("Max temperature set: %.2f。C\r\n", temperature);
          break;
        default:
          Serial.println("Unknown message");
      }
    }
}

void joinCallback(int32_t status)
{
    Serial.printf("Join status: %d\r\n", status);
}

/*************************************
 * enum type for LoRa Event
    RAK_LORAMAC_STATUS_OK = 0,
    RAK_LORAMAC_STATUS_ERROR,
    RAK_LORAMAC_STATUS_TX_TIMEOUT,
    RAK_LORAMAC_STATUS_RX1_TIMEOUT,
    RAK_LORAMAC_STATUS_RX2_TIMEOUT,
    RAK_LORAMAC_STATUS_RX1_ERROR,
    RAK_LORAMAC_STATUS_RX2_ERROR,
    RAK_LORAMAC_STATUS_JOIN_FAIL,
    RAK_LORAMAC_STATUS_DOWNLINK_REPEATED,
    RAK_LORAMAC_STATUS_TX_DR_PAYLOAD_SIZE_ERROR,
    RAK_LORAMAC_STATUS_DOWNLINK_TOO_MANY_FRAMES_LOSS,
    RAK_LORAMAC_STATUS_ADDRESS_FAIL,
    RAK_LORAMAC_STATUS_MIC_FAIL,
    RAK_LORAMAC_STATUS_MULTICAST_FAIL,
    RAK_LORAMAC_STATUS_BEACON_LOCKED,
    RAK_LORAMAC_STATUS_BEACON_LOST,
    RAK_LORAMAC_STATUS_BEACON_NOT_FOUND,
 *************************************/

void sendCallback(int32_t status)
{
    if (status == RAK_LORAMAC_STATUS_OK) {
        Serial.println("Successfully sent");
    } else {
        Serial.println("Sending failed");
    }
}


bool radio_Init(void)
{

    // OTAA Device EUI MSB first
    uint8_t node_device_eui[8] = OTAA_DEVEUI;
    // OTAA Application EUI MSB first
    uint8_t node_app_eui[8] = OTAA_APPEUI;
    // OTAA Application Key MSB first
    uint8_t node_app_key[16] = OTAA_APPKEY;

    /* Make sure module is in LoRaWAN mode */
    if(api.lorawan.nwm.get() != 1) /* check LoRaWAN mode */
    {
      if (api.lorawan.nwm.set())  /* set LoRaWAN mode */
      { 
        api.system.reboot();
      }
      else 
      {
        return false;
      }
    }

    /* Keys and IDs */
    if (!api.lorawan.appeui.set(node_app_eui, 8)) 
    {
        return false;
    }
    if (!api.lorawan.appkey.set(node_app_key, 16)) 
    {
        return false;
    }
    if (!api.lorawan.deui.set(node_device_eui, 8)) 
    {
        return false;
    }

    /* Set region. Check radio.h */
    if (!api.lorawan.band.set(OTAA_BAND)) 
    {
        return false;
    }

    /* Class A */
    if (!api.lorawan.deviceClass.set(RAK_LORA_CLASS_A)) 
    {
        return false;
    }

    /* Set the network join mode to OTAA */
    if (!api.lorawan.njm.set(RAK_LORA_OTAA))	
    {
        return false;
    }

    /* request joinning the network */
    if (!api.lorawan.join())	// Join to Gateway
    {
        return false;
    }
  
    /** Wait for Join success */
    while (api.lorawan.njs.get() == 0) 
    {
        Serial.print("Wait for LoRaWAN join...");
        api.lorawan.join(); // <--- esto no lo veo claro
        delay(10000);
    }
  
    /* ADR activated -> raro que sea ahora y no antes de ponerse con el join */
    if (!api.lorawan.adr.set(true)) 
    {
        return false;
    }

    /* times of retransmission of Confirm packet data */
    if (!api.lorawan.rety.set(1)) 
    {
        return false;
    }

    api.lorawan.registerRecvCallback(recvCallback);
    api.lorawan.registerJoinCallback(joinCallback);
    api.lorawan.registerSendCallback(sendCallback);

    //connected = true;
    return true;
}

void radio_startPayload(uint8_t port)
{
  uplink_cnt = 0;
  fport = port;
}

/* note that this schedules the transmission, doesn`t sends it immediately */  
bool radio_sendPayload(void)
{
  /* request sending data (aquí seguro que hay copia de llos datos al stack*/
  if (api.lorawan.send(uplink_cnt, (uint8_t *)&uplink_payload, fport, false, 1)) {
    return true;
  } 
  else 
  {
    return false;
  }
}

void radio_addUint16_t(uint16_t value)
{
  uplink_payload[uplink_cnt++] = (uint8_t)(value >> 8);    // MSB of counter
  uplink_payload[uplink_cnt++] = (uint8_t)(value & 0xFF);  // LSB of counter (but you decide)
}

void radio_addTemperature(float value)
{
  int16_t temperature_encoded;

  temperature_encoded = value * 10.0f; // two's complement
  uplink_payload[uplink_cnt++] = (uint8_t)(temperature_encoded >> 8);   // Encode MSB
  uplink_payload[uplink_cnt++] = (uint8_t)(temperature_encoded & 0xFF); // and LSB
}

// Cayenne LPP
void radio_addCLPPTemperature(float value)
{
  int16_t temperature_encoded;
  
  // Cayenne LPP temperature in channel 1  
  uplink_payload[uplink_cnt++] = (uint8_t) 0x01;  // Channel 1
  uplink_payload[uplink_cnt++] = (uint8_t) 0x67;  // Cayenne LPP temperature
  temperature_encoded = value * 10.0f; // two's complement
  uplink_payload[uplink_cnt++] = (uint8_t)(temperature_encoded >> 8);   // Encode MSB
  uplink_payload[uplink_cnt++] = (uint8_t)(temperature_encoded & 0xFF); // and LSB
}

void radio_addHumidity(float value)
{
  uint8_t humidity_encoded;

  humidity_encoded = value * 2.0f; 
  uplink_payload[uplink_cnt++] = humidity_encoded;   
}

// Cayenne LPP
void radio_addCLPPHumidity(float value)
{
  uint8_t humidity_encoded;
  
  // Cayenne LPP temperature in channel 2  
  uplink_payload[uplink_cnt++] = (uint8_t) 0x02;  // Channel 2
  uplink_payload[uplink_cnt++] = (uint8_t) 0x68;  // Cayenne LPP humidity
  humidity_encoded = value * 2.0f; 
  uplink_payload[uplink_cnt++] = humidity_encoded;   
}

void radio_addAlarm(uint8_t value)
{
  uplink_payload[uplink_cnt++] = value;
}

// Cayenne LPP
void radio_addCLPPAlarm(uint8_t value)
{
  // Cayenne LPP temperature in channel 2  
  uplink_payload[uplink_cnt++] = (uint8_t) 0x03;  // Channel 2
  uplink_payload[uplink_cnt++] = (uint8_t) 0x01;  // Cayenne LPP humidity
  uplink_payload[uplink_cnt++] = value;   
}

// returns -1 if no downlink data available or the first byte available in the data buffer 
int16_t radio_downlinkRead(void)
{
  /*
  if (modem.available())
  {
    return modem.read();
  } else {
    return -1;
  }
  */
}

/*** End of file *********************************************************************************/
