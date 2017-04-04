#include <pic32mx.h>
#include <stdint.h>
#include <stdbool.h>
#include "displayfunctions.h"
#include "stringfunctions.h"
//#include "TemperatureExample.c"



#define TEMP_SENSOR1_ADDR 			0x48

//#define TEMP_SENSOR2_ADDR				0x40								// slave address register for I2C2
//#define TEMP_SENSOR_REG_CONF	0x5C
//#define TEMP_SENSOR_REG_HYST 	20.0
//#define TEMP_SENSOR_REG_LIMIT 21.5



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
void i2c2_idle(){
	while(I2C2CON & 0x1F || I2C2STAT & (1 << 14));
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
bool i2c2_send(uint8_t data){
	i2c2_idle();
	I2C2TRN = data;
	i2c2_idle();
	return !(I2C2STAT & (1 << 15));
}

uint8_t i2c1_recv() {
	i2c1_idle();
	I2C1CONSET = 1 << 3; //RCEN = 1
	i2c1_idle();
	I2C1STATCLR = 1 << 6; //I2COV = 0
	return I2C1RCV;
}
uint8_t i2c2_recv(){
	i2c2_idle();
	I2C2CONSET = 1 << 3;
	i2c2_idle();
	I2C2STATCLR = 1 << 6;
	return I2C2RCV;
}

void i2c1_ack() {												// initiates not acknowledge condition for I2C
	i2c1_idle();
	I2C1CONCLR = 1 << 5; 									// set so that an acknowledge is sent
	I2C1CONSET = 1 << 4; 									// start acknowledge sequence
}
void i2c2_ack(){
	i2c2_idle();
	I2C2CONCLR = 1 << 5;
	I2C2CONSET = 1 << 4;
}

void i2c1_nack() {											// initiates not acknowledge condition for I2C
	i2c1_idle();
	I2C1CONSET = 1 << 5; 									// set so that a not-acknowledge is sent
	I2C1CONSET = 1 << 4; 									// start acknowledge sequence
}
void i2c2_nack(){
	i2c2_idle();
	I2C2CONSET = 1 << 5;
	I2C2CONSET = 1 << 4;
}

void i2c1_start() {											// initates start condition for I2C
	i2c1_idle();
	I2C1CONSET = 1 << 0; 									// start condition enable bit (bit 0) set to 1
	i2c1_idle();
}
void i2c2_start(){
	i2c2_idle();
	I2C2CONSET = 1 << 0;
	i2c2_idle();
}

void i2c1_restart() {										// initiates restart condition for I2C
	i2c1_idle();
	I2C1CONSET = 1 << 1; 									// restart condition enable bit (bit 1) set to 1
	i2c1_idle();
}
void i2c2_restart(){
	i2c2_idle();
	I2C2CONSET = 1 << 1;
	i2c2_idle();
}

void i2c1_stop() {											// initiates stop condition for I2C
	i2c1_idle();
	I2C1CONSET = 1 << 2; 									// stop condition enable bit (bit 2) set to 1
	i2c1_idle();
}
void i2c2_stop(){
	i2c2_idle();
	I2C2CONSET = 1 << 2;
	i2c2_idle();
}


uint32_t strlen(char *str) {
	uint32_t n = 0;
	while(*str++)
		n++;
	return n;
}

int main(void) {
	uint16_t tempin;
	uint16_t tempout = exttemp();
	char buf[32], *s, *t;
	char buf2[32], *u, *v;
	//volatile unsigned TEMP_SENSOR2_ADDR = I2C1ADD;

	/* Set up peripheral bus clock */
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

	/* Set up i2c for external thermometer
	I2C2CON = 0x0;
	I2C2BRG = 0x0C2;				// baud rate generator not compatible with external sensor
	I2C2STAT = 0x0;
	I2C2CONSET = 1 << 13;
	I2C2CONSET = 1 << 15;
	tempout = I2C2RCV;*/

	/* Set up input pins */
	TRISDSET = (1 << 8);
	TRISFSET = (1 << 1);


	display_init();
	display_string(0, "Board Temp:");
	display_string(1, "");
	display_string(2, "External Temp:");
	display_string(3, "");
	display_update();

	do {
		i2c1_start();																						// initialise
	} while(!i2c1_send(TEMP_SENSOR1_ADDR << 1));							// if acknowledge was not received, keep trying to initialise (shift 1 step to the left to make 8-bit data value)
	i2c1_send(TEMP_SENSOR_REG_CONF);													// I will now send to the config!
	i2c1_send(0x7C);																					// Value of config
	i2c1_stop();

	/*do {
		i2c2_start();
	} while(!i2c2_send(TEMP_SENSOR2_ADDR << 1));
	i2c2_send(EXTTEMP_SENSOR_REG_CONF);
	i2c2_send(0x0);
	i2c2_stop();*/

	for(;;) {
		do {
			i2c1_start();																					// initialise
		} while(!i2c1_send(TEMP_SENSOR1_ADDR << 1));						// if acknowledge was not received, keep trying to initialise

		i2c1_send(TEMP_SENSOR_REG_TEMP);												// I will now point to the temp!
		i2c1_stop();

		/*do {
			i2c2_start();
		} while(!i2c2_send(TEMP_SENSOR2_ADDR << 1));

		i2c2_send(EXTTEMP_SENSOR_REG_TEMP);*/

		do {
			i2c1_start();																					// initialise
		} while(!i2c1_send((TEMP_SENSOR1_ADDR << 1) | 1));			// this will always be false so we exit loop

		/*do {
			i2c2_start();
		} while(!i2c2_send((TEMP_SENSOR2_ADDR << 1) | 1));*/

		tempin = i2c1_recv() << 8;
		i2c1_ack();
		tempin |= i2c1_recv();
		i2c1_nack();
		i2c1_stop();

		/*tempout = i2c2_recv() << 8;
		i2c2_ack();
		tempout |= i2c2_recv();
		i2c2_nack();
		i2c2_stop();*/

		s = fixed_to_string(tempin, buf);
		t = s + strlen(s);
		*t++ = ' ';
		*t++ = 7;
		*t++ = 'C';
		*t++ = 0;

		u = fixed_to_string(tempout, buf2);
		v = u + strlen(u);
		*v++ = ' ';
		*v++ = 7;
		*v++ = 'C';
		*v++ = 0;

		display_string(1, s);
		display_update();
		display_string(3, u);
		display_update();
		delay(1000000);
	}

	return 0;
}
