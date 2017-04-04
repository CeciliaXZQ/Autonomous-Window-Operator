#include "OneWire.h"
#include <pic32mx.h>
#include <stdint.h>
#include <stdbool.h>


// OneWire DS18S20, DS18B20, DS1822 Temperature Example
//
// http://www.pjrc.com/teensy/td_libs_OneWire.html
//
// The DallasTemperature library can do all this work for you!
// http://milesburton.com/Dallas_Temperature_Control_Library

//OneWire (TRISF & 0x8);  // on pin 3 (a 4.7K resistor is necessary)



uint16_t exttemp(void) {
  uint8_t i;
  uint8_t present = 0;
  uint8_t type_s;
  uint8_t data[12];
  uint8_t addr[8];
  float celsius;

/*  if (!ds.search(addr)) {
    display_string("No more addresses.");
    Serial.println();
    ds.reset_search();
    delay(250);
    return;
  }*/

  /*string ROM = "ROM = ";
  for( i = 0; i < 8; i++) {
    ROM += i;
    display_string(0, ROM);
  }

  if (crc8(addr, 7) != addr[7]) {
      display_string(0, "CRC is not valid!");
      return;
  }
  display_update();

  // the first ROM byte indicates which chip
  switch (addr[0]) {
    case 0x10:
      display_string(1, "  Chip = DS18S20");  // or old DS1820
      type_s = 1;
      break;
    case 0x28:
      display_string(1, "  Chip = DS18B20");
      type_s = 0;
      break;
    case 0x22:
      display_string(1, "  Chip = DS1822");
      type_s = 0;
      break;
    default:
      display_string(1, "Not DS18x20 family device.");
      return;
  }
  display_update();
*/
  reset(TRISF & 0x8);                   // we use bit 3 of port F for our external thermometer
  select(TRISF & 0x8);
  write(0x44, (TRISF & 0x8));        // start conversion, with parasite power on at the end

  delay(1000);     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.

  present = reset(TRISF & 0x8);
  select(addr);
  write(0xBE, TRISF & 0x8);         // Read Scratchpad

/*  Serial.print("  Data = ");
  Serial.print(present, HEX);
  Serial.print(" ");*/
  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = read(TRISF & 0x8);
    /*Serial.print(data[i], HEX);
    Serial.print(" ");*/
  }
/*  Serial.print(" CRC=");
  Serial.print(OneWire::crc8(data, 8), HEX);
  Serial.println();*/

  // Convert the data to actual temperature
  // because the result is a 16 bit signed integer, it should
  // be stored to an "int16_t" type, which is always 16 bits
  // even when compiled on a 32 bit processor.
  int16_t raw = (data[1] << 8) | data[0];
  if (type_s) {
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10) {
      // "count remain" gives full 12 bit resolution
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } else {
    uint8_t cfg = (data[4] & 0x60);
    // at lower res, the low bits are undefined, so let's zero them
    if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
    //// default is 12 bit resolution, 750 ms conversion time
  }
  celsius = (float)raw / 16.0;
  //print_string(2, "Temperature = ");
  /*display_string(3, celsius);
  display_update();*/
  /*Serial.print(" Celsius, ");
  Serial.print(fahrenheit);
  Serial.println(" Fahrenheit");*/
  return celsius;
}
