#include <pic32mx.h>
#include <stdint.h>
#include <stdbool.h>
#include "project.h"


#define TEMP_SENSOR1_ADDR 			0x48

typedef enum TempSensorReg TempSensorReg;
enum TempSensorReg {
	TEMP_SENSOR_REG_TEMP,
	TEMP_SENSOR_REG_CONF,
	TEMP_SENSOR_REG_HYST,
	TEMP_SENSOR_REG_LIMIT,
};


int main(void) {
	uint16_t tempin;
	//uint16_t tempout = exttemp();
	char buf[32], *s, *t;
	//char buf2[32], *u, *v;
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

		delay1(1000000);
	}

	return 0;
}
