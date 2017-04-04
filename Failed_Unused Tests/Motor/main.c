#include <pic32mx.h>
#include <stdint.h>
#include <stdbool.h>
#include "displayfunctions.h"
#include "Projectfunctions.h"


#define TEMP_SENSOR1_ADDR 			0x48


typedef enum TempSensorReg TempSensorReg;
enum TempSensorReg {
  TEMP_SENSOR_REG_TEMP,
  TEMP_SENSOR_REG_CONF,
  TEMP_SENSOR_REG_HYST,
  TEMP_SENSOR_REG_LIMIT,
};

/* Setup function for control and status registers:
 * Control bit 0: enable start condition
 * Control bit 1: enable restart condition
 * Control bit 2: enable stop condition
 * Control bit 3: enable receive mode
 * Control bit 4: enable acknowledge bit
 * Status bit 14: enable master transmit */



void i2c1_idle(){
  while(I2C1CON & 0x1F || I2C1STAT & (1 << 14));
}

/* Send function (initialization)
 * the return value lets the master know that the slave devices are
 * on the bus and are ready to operate */

bool i2c1_send(uint8_t data) {
  i2c1_idle();
  I2C1TRN = data;									//
  i2c1_idle();
  return !(I2C1STAT & (1 << 15)); //ACKSTAT
}

uint8_t i2c1_recv() {
  i2c1_idle();
  I2C1CONSET = 1 << 3; //RCEN = 1
  i2c1_idle();
  I2C1STATCLR = 1 << 6; //I2COV = 0
  return I2C1RCV;
}

void i2c1_ack() {												// initiates not acknowledge condition for I2C
  i2c1_idle();
  I2C1CONCLR = 1 << 5; 									// set so that an acknowledge is sent
  I2C1CONSET = 1 << 4; 									// start acknowledge sequence
}

void i2c1_nack() {											// initiates not acknowledge condition for I2C
  i2c1_idle();
  I2C1CONSET = 1 << 5; 									// set so that a not-acknowledge is sent
  I2C1CONSET = 1 << 4; 									// start acknowledge sequence
}

void i2c1_start() {											// initates start condition for I2C
  i2c1_idle();
  I2C1CONSET = 1 << 0; 									// start condition enable bit (bit 0) set to 1
  i2c1_idle();
}

void i2c1_restart() {										// initiates restart condition for I2C
  i2c1_idle();
  I2C1CONSET = 1 << 1; 									// restart condition enable bit (bit 1) set to 1
  i2c1_idle();
}

void i2c1_stop() {											// initiates stop condition for I2C
  i2c1_idle();
  I2C1CONSET = 1 << 2; 									// stop condition enable bit (bit 2) set to 1
  i2c1_idle();
}


uint32_t strlen(char *str) {
  uint32_t n = 0;
  while(*str++)
    n++;
  return n;
}

char *fixed_to_string(uint16_t num, char *buf) {
  bool neg = false;
  uint32_t n;
  char *tmp;

  if(num & 0x8000) {
    num = ~num + 1;
    neg = true;
  }

  buf += 4;
  n = num >> 8;
  tmp = buf;
  do {
    *--tmp = (n  % 10) + '0';
    n /= 10;
  } while(n);
  if(neg)
    *--tmp = '-';

  n = num;
  if(!(n & 0xFF)) {
    *buf = 0;
    return tmp;
  }
  *buf++ = '.';
  while((n &= 0xFF)) {
    n *= 10;
    *buf++ = (n >> 8) + '0';
  }
  *buf = 0;

  return tmp;
}


int main(void) {

  uint16_t tempin;
  char buf[32], *s, *t;

	/* Set up peripheral bus clock */
  //OSCCONCLR = 0x100000;
  OSCCON &= ~0x180000;
  OSCCON |= 0x080000;

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

	/* Clear SPIROV*/
	SPI2STATCLR &= ~0x40;
	/* Set CKP = 1, MSTEN = 1; */
  SPI2CON |= 0x60;
	/* Turn on SPI */
	SPI2CONSET = 0x8000;

	/* Set up i2c for internal thermometer */
	I2C1CON = 0x0;
	I2C1BRG = 0x0C2;
	I2C1STAT = 0x0;
	I2C1CONSET = 1 << 13; //SIDL = 1
	I2C1CONSET = 1 << 15; // ON = 1
	tempin = I2C1RCV;

  TRISDSET = (1 << 8);
  TRISFSET = (1 << 1);


  TRISD |= 0xff0;

  //initialise();

	display_init();
	display_string(0, "");
	display_string(1, "");
	display_string(2, "Window:");
	display_string(3, "");
	display_update();

  display_image(96, closedwindow);

while(1){
  //initialise();
  //int x = sendswitch();

  // forward operation
  if (motorsw() == 1 || motorsw() == 0){
    display_string(0, "Manual Mode");
    display_update();
  }

  // reverse operation
  if (motorsw() != 1 && motorsw() != 0){
    display_string(0, "Auto Mode");
    display_update();

  }



    do {
      i2c1_start();																						// initialise
    } while(!i2c1_send(TEMP_SENSOR1_ADDR << 1));							// if acknowledge was not received, keep trying to initialise (shift 1 step to the left to make 8-bit data value)
    i2c1_send(TEMP_SENSOR_REG_CONF);													// I will now send to the config!
    i2c1_send(0x7C);																					// Value of config
    i2c1_stop();


    for(;;) {
      do {
        i2c1_start();																					// initialise
      } while(!i2c1_send(TEMP_SENSOR1_ADDR << 1));						// if acknowledge was not received, keep trying to initialise

      i2c1_send(TEMP_SENSOR_REG_TEMP);												// I will now point to the temp!
      //i2c1_stop();


      do {
        i2c1_start();																					// initialise
      } while(!i2c1_send((TEMP_SENSOR1_ADDR << 1) | 1));			// this will always be false so we exit loop

      tempin = i2c1_recv() << 8;
      i2c1_ack();
      tempin |= i2c1_recv();
    //  display_string(1, "Hello!");
    //  display_update();
      i2c1_nack();
      i2c1_stop();


      s = fixed_to_string(tempin, buf);
      t = s + strlen(s);
      *t++ = ' ';
      *t++ = 7;
      *t++ = 'C';
      *t++ = 0;

      display_string(1, s);
      display_update();


      //s = 'Hello!';
      display_image(96, closedwindow);
      delay1(1000000);




  display_image(96, closedwindow);
}
}
  return 0;
}
