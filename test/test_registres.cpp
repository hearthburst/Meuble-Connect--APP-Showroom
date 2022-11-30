#include <Arduino.h>
// Initiate Wifi with ADC2 on ESP32 workaround bellow:
#include "soc/sens_reg.h"    // needed for manipulating ADC2 control register
uint32_t adc_register;
uint32_t wifi_register;
// End initiation of Wifi with ADC2 on ESP32 workaround.




void setup() {
  // put your setup code here, to run once:
Serial.begin(115200);
  adc_register = READ_PERI_REG(SENS_SAR_READ_CTRL2_REG); // Wifi with ADC2 on ESP32 workaround.
//*** Connect WiFi / Bluetooth stuff here ***
  wifi_register = READ_PERI_REG(SENS_SAR_READ_CTRL2_REG); // Wifi with ADC2 on ESP32 workaround.

}

void loop() {
  // put your main code here, to run repeatedly:
  

  WRITE_PERI_REG(SENS_SAR_READ_CTRL2_REG, adc_register); // Wifi with ADC2 on ESP32 workaround.
  SET_PERI_REG_MASK(SENS_SAR_READ_CTRL2_REG, SENS_SAR2_DATA_INV);// Wifi with ADC2 on ESP32 workaround.
//*** Analog read stuff here***
  WRITE_PERI_REG(SENS_SAR_READ_CTRL2_REG, wifi_register); // Wifi with ADC2 on ESP32 workaround.

}


 