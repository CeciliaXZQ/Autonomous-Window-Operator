/* windowprojectmain.c

 */

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include <stdbool.h>
#include "project.h"  /* Declarations for these labs */

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

  char buf[32], *s, *t;

	/* Set up peripheral bus clock */
  /* OSCCONbits.PBDIV = 1; */
  OSCCONCLR = 0x100000; /* clear PBDIV bit 1 */
//	OSCCONSET = 0x080000; /* set PBDIV bit 0 */

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
	/* SPI2STAT bit SPIROV = 0; */
	SPI2STATCLR = 0x40;
	/* SPI2CON bit CKP = 1; */
  SPI2CONSET = 0x40;
	/* SPI2CON bit MSTEN = 1; */
	SPI2CONSET = 0x20;
	/* SPI2CON bit ON = 1; */
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

	int counter = 0;
	display_image1(0, blackwelcome);
	while (counter < 10000000){
		delay(5000000);
		counter++;
	}

	welcomeflash();

	counter = 0;
	display_image1(0, welcome);
	while (counter < 10000000){
		delay(5000000);
		counter++;
	}

	display_string(0, "Climate");
	display_string(1, "George!");
	display_string(2, "Temp:");
	display_string(3, "");
	display_update();

	initialise(); /* Do initial project initialization */
  resetPWM();

  do {
		i2c1_start();																						// initialise
	} while(!i2c1_send(TEMP_SENSOR1_ADDR << 1));							// if acknowledge was not received, keep trying to initialise (shift 1 step to the left to make 8-bit data value)
	i2c1_send(TEMP_SENSOR_REG_CONF);													// I will now send to the config!
	i2c1_send(0x7E);																					// Value of config (resolution + interrupt mode)
	i2c1_stop();

	do {
		i2c1_start();																						// initialise
	} while(!i2c1_send(TEMP_SENSOR1_ADDR << 1));							// if acknowledge was not received, keep trying to initialise (shift 1 step to the left to make 8-bit data value)
	i2c1_send(TEMP_SENSOR_REG_HYST);													// I will now send to the lower limit!
	i2c1_send(0x80);																					// Value of lower limit (0ºC)
	i2c1_stop();

	//display_image(96, openwindow);

	do {
	i2c1_start();																						// initialise
	} while(!i2c1_send(TEMP_SENSOR1_ADDR << 1));						// if acknowledge was not received, keep trying to initialise

	i2c1_send(TEMP_SENSOR_REG_TEMP);												// I will now point to the temp!
//i2c1_stop();

int count = 0;
	while( 1 )
	{
		count++;
		PORTE = count;																						// LEDs show that the main is running or has been interrupted

		  do {
		    i2c1_start();																					// initialise
		  } while(!i2c1_send((TEMP_SENSOR1_ADDR << 1) | 1));			// this will always be false so we exit loop

		  tempin = i2c1_recv() << 8;															// read values from i2c temperature register
		  i2c1_ack();
		  tempin |= i2c1_recv();
		  i2c1_nack();
		  i2c1_stop();

			int autoangle = temptoangledemo(tempin);								// convert i2c value to angle

			if (autoangle == 180){																	// display graphics if really hot or cold
				display_image(96, hot);
			}
			if(autoangle == 0){
				display_image(96, cold);
			}
			if(autoangle != 180 && autoangle != 0){
				display_image(96, blank);
			}

			startPWM();																						// control motor using angle from temperature
			setPWM(autoangle);
			delay(500);
			stopPWM();

		  s = fixed_to_string(tempin, buf);											// display string from i2c temperature value 
		  t = s + strlen(s);
		  *t++ = ' ';
		  *t++ = 7;
		  *t++ = 'C';
		  *t++ = 0;


		display_string(3, s);
		display_update1();


	}
	return 0;
}
