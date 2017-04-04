/* main */

#include <stdint.h>       /* Declarations of uint_32 and the like */
#include <pic32mx.h>      /* Declarations of system-specific addresses etc */
#include "IOShieldTemp.h" /* Declarations for internal temp sensor */
#include "Wire.h"

void setup(){

  // configure register with required parameters
  IOShieldTemp.config(IOSHIELDTEMP_NONSHOT | IOSHIELDTEMP_RES12 | IOSHIELDTEMP_FAULT6 | IOSHIELDTEMP_ALERTHIGH | IOSHIELDTEMP_CMPMODE | IOSHIELDTEMP_STARTUP);


  IOShieldTemp.setTempHyst(20.0);       // set minimum temperature threshold
  IOShieldTemp.setTempLimit(21.5);      // set maximum temperature threshold

}

void loop(void){

  float temp;
  temp = IOShieldTemp.getTemp();
  display_string(2, temp);
  display_image(96, icon);
  display_update();
  delay(1000);

}

int main(void){
  /* Set up peripheral bus clock */
        /* OSCCONbits.PBDIV = 1; */
        OSCCONCLR = 0x100000; /* clear PBDIV bit 1 */
  OSCCONSET = 0x080000; /* set PBDIV bit 0 */

  /* Set up output pins */
  AD1PCFG = 0xFFFF;
  ODCE = 0x0;
  TRISECLR = 0xFF;
  PORTE = 0x0;

  /* Output pins for display signals */
  PORTF = 0xFFFF;
  PORTG = (1 << 9);
  ODCF = 0x0;
  ODCG = 0x0;
  TRISFCLR = 0x70;
  TRISGCLR = 0x200;

  /* Set up input pins */
  TRISDSET = (1 << 8);
  TRISFSET = (1 << 1);

  /* Set up SPI as master */
  SPI2CON = 0;
  SPI2BRG = 4;
  /* SPI2STAT bit SPIROV = 0; */
  SPI2STATCLR = 0x40;
  /* SPI2CON bit CKP = 1; */
        SPI2CONSET = 0x40;
  /* SPI2CON bit MSTEN = 1; */
  SPI2CONSET = 0x20;
  /* SPI2CON bit ON = 1; */
  SPI2CONSET = 0x8000;

  display_init();
  display_string(0, "Indoor Temperature:");
  display_string(1, "00000ºC");
  display_string(2, "Outdoor Temperature: ");
  display_string(3, "");
  display_update();

  display_image(96, icon);

  labinit(); /* Do any lab-specific initialization */

  while( 1 )
  {
    loop();
  }
  return 0;
}
