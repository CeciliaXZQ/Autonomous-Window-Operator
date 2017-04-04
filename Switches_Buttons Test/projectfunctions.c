/* projectfunctions.c
  This file contains all the functions used in the project
  */

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include <stdbool.h>
#include "project.h"  /* Function declarations for the project */

/* Declare a helper function which is local to this file */
static void num32asc( char * s, int );

#define DISPLAY_CHANGE_TO_COMMAND_MODE (PORTFCLR = 0x10)
#define DISPLAY_CHANGE_TO_DATA_MODE (PORTFSET = 0x10)

#define DISPLAY_ACTIVATE_RESET (PORTGCLR = 0x200)
#define DISPLAY_DO_NOT_RESET (PORTGSET = 0x200)

#define DISPLAY_ACTIVATE_VDD (PORTFCLR = 0x40)
#define DISPLAY_ACTIVATE_VBAT (PORTFCLR = 0x20)

#define DISPLAY_TURN_OFF_VDD (PORTFSET = 0x40)
#define DISPLAY_TURN_OFF_VBAT (PORTFSET = 0x20)



void delay1(int cyc) {
	int i;
	for(i = cyc; i > 0; i--);
}


int getsw(void){
  /* the four least significant bits of the return value should contain data
   from switches SW4-SW1, SW1 corresponds to the LSB. All other bits of
   return value must be 0 */

//   SW1 = RD8 =  PORTD with index 8
//   SW2 = RD9 =  PORTD with index 9
//   SW3 = RD10 = PORTD with index 10
//   SW4 = RD11 = PORTD with index 11

int switch1 = ((PORTD >> 8) & 1);
int switch2 = ((PORTD >> 9) & 1);
int switch3 = ((PORTD >> 10) & 1);
int switch4 = ((PORTD >> 11) & 1);

  int onOff = 0;

  if (switch1 && switch2 && switch3 && switch4){
    return 1000;
  }
  if ((switch1 && switch2 && switch3) || (switch1 && switch2 && switch4) || (switch1 && switch3 && switch4) || (switch2 && switch3 && switch4)){
    return 1000;
  }
  if ((switch1 && switch2) || (switch1 && switch3) || (switch1 && switch4) || (switch2 && switch3) || (switch2 && switch4) || (switch3 && switch4)){
    return 1000;
  }

  if (switch1){
    onOff = 0x1;
  }
  if (switch2){
    onOff = 0x2;
  }
  if (switch3){
    onOff = 0x3;
  }
  if (switch4){
    onOff = 0x4;
  }
  return onOff;
}

int getbtns(void){
  /* the three least significant bits of the return value should contain data
     from buttons BTN4-BTN2, BTN2 corresponds to the LSB. All other bits of
     return value must be 0 */

  int btn1 = ((PORTD >> 4) & 1);  //  BTN1 = RD4 = PORTD with index 4
  int btn2 = ((PORTD >> 5) & 1);  //  BTN2 = RD5 = PORTD with index 5
  int btn3 = ((PORTD >> 6) & 1);  //  BTN3 = RD6 = PORTD with index 6
  int btn4 = ((PORTD >> 7) & 1);  //  BTN4 = RD7 = PORTD with index 7

    int onOff = 0;

    if (btn1){
      onOff += 0x1;
    }
    if (btn2){
      onOff += 0x2;
    }
    if (btn3){
      onOff += 0x4;
    }
    if (btn4){
      onOff += 0x8;
    }

    return onOff;
}


int motorbtns(void){
  int onOff = 0;

  int btn2 = ((PORTD >> 5) & 1);  //  BTN2 = RD5 = PORTD with index 5
  int btn3 = ((PORTD >> 6) & 1);  //  BTN3 = RD6 = PORTD with index 6
  int btn4 = ((PORTD >> 7) & 1);  //  BTN4 = RD7 = PORTD with index 7

  if (btn3){
    onOff = 0x3;
  }
  if (btn2){
    onOff = 0x2;
  }
  return onOff;
}

int motorsw(void){
  /* the four least significant bits of the return value should contain data
   from switches SW4-SW1, SW1 corresponds to the LSB. All other bits of
   return value must be 0 */

//   SW1 = RD8 =  PORTD with index 8
//   SW2 = RD9 =  PORTD with index 9
//   SW3 = RD10 = PORTD with index 10
//   SW4 = RD11 = PORTD with index 11

  int switch1 = ((PORTD >> 8) & 1);
  int switch2 = ((PORTD >> 9) & 1);
  int switch3 = ((PORTD >> 10) & 1);
  int switch4 = ((PORTD >> 11) & 1);

  if (switch1 && switch2 && switch3 && switch4){      //180 degrees
    return 0x5;
  }

  if ((switch1 && switch2 && switch3) || (switch1 && switch2 && switch4) || (switch1 && switch3 && switch4) || (switch2 && switch3 && switch4)){
     return 1000;
  }

  if ((switch1 && switch2) || (switch1 && switch3) || (switch1 && switch4) || (switch2 && switch3) || (switch2 && switch4) || (switch3 && switch4)){
     return 1000;
  }

  if (!switch1 && !switch2 && !switch3 && !switch4){  //0 degrees
     return 0x0;
  }

  if (switch1){                                       //-22.5 degrees
 	   return 0x1;
  }

  if (switch2){                                       //+22.5 degrees
 	   return 0x2;
  }

  if (switch3){                                       //-45 degrees
 	   return 0x3;
  }

  if (switch4){                                       //+45 degrees
 	   return 0x4;
  }
}


/* updateangle will take an angle as input and add/subtract it
	 to the current angle that the motor is at to provide incremental
	 operation. If the total angle is <0º or >180º it will not increment
	 the value, but will return the value for 0/180.
*/
void updateangle(int x){
  if(x == 0){
    currentangle = 0;
		return;
  }
  if(x == 180){
    currentangle = 180;
		return;
  }
  currentangle += x;
  if (currentangle > 180){
    currentangle = 180;
  }
  if (currentangle < 0){
    currentangle = 0;
  }
  return;
}

/* temptoangle will read the value from the i2c device and return
	 an angle which is then called with setPWM(); during auto operation.
	 Temperature register values:
	 <15> 	 sign bit
	 <14:8>	 2^6:2^0
	 <7:4>	 2^-1:2^-4
	 <3:0>   no value
*/
int temptoangle(uint16_t tempin){
	int x = 0;
	if(tempin & 0x4000){
		x += 64;
	}
	if(tempin & 0x2000){
		x += 32;
	}
	if(tempin & 0x1000){
		x += 16;
	}
	if(tempin & 0x800){
		x += 8;
	}
	if(tempin & 0x400){
		x += 4;
	}
	if(tempin & 0x200){
		x += 2;
	}
	if(tempin & 0x100){
		x += 1;
	}
	if(tempin & 0x8000){					// if it is a negative value
		tempin = ~tempin + 1;				// two's complement
		x = tempin;
	}

// return values to open/close window
	if(x <= 20){
		return 0;
	}
	if(x <= 21){
		return 30;
	}
	if(x <= 22){
		return 60;
	}
	if(x <= 23){
		return 90;
	}
	if(x <= 24){
		return 120;
	}
	if(x <= 25){
		return 150;
	}
	if(x >= 26){
		return 180;
	}
}

/* does the same as temptoangle, but has different return arguments
	 so that it can be used as a demo.
*/
int temptoangledemo(uint16_t tempin){
	int x = 0;
	if(tempin & 0x4000){
		x += 64;
	}
	if(tempin & 0x2000){
		x += 32;
	}
	if(tempin & 0x1000){
		x += 16;
	}
	if(tempin & 0x800){
		x += 8;
	}
	if(tempin & 0x400){
		x += 4;
	}
	if(tempin & 0x200){
		x += 2;
	}
	if(tempin & 0x100){
		x += 1;
	}
	if(tempin & 0x8000){					// if it is a negative value
		tempin = ~tempin + 1;
		x = tempin;
	}

	if(x <= 25){
		return 0;
	}
	if(x <= 26){
		return 45;
	}
	if(x <= 27){
		return 90;
	}
	if(x <= 28){
		return 135;
	}
	if(x >= 29){
		return 180;
	}
}

/* quicksleep:
   A simple function to create a small delay.
   Very inefficient use of computing resources,
   but very handy in some special cases. */
void quicksleep(int cyc) {
	int i;
	for(i = cyc; i > 0; i--);
}

/* tick:
   Add 1 to time in memory, at location pointed to by parameter.
   Time is stored as 4 pairs of 2 NBCD-digits.
   1st pair (most significant byte) counts days.
   2nd pair counts hours.
   3rd pair counts minutes.
   4th pair (least significant byte) counts seconds.
   In most labs, only the 3rd and 4th pairs are used. */
void tick( unsigned int * timep )
{
  /* Get current value, store locally */
  register unsigned int t = * timep;
  t += 1; /* Increment local copy */

  /* If result was not a valid BCD-coded time, adjust now */

  if( (t & 0x0000000f) >= 0x0000000a ) t += 0x00000006;
  if( (t & 0x000000f0) >= 0x00000060 ) t += 0x000000a0;
  /* Seconds are now OK */

  if( (t & 0x00000f00) >= 0x00000a00 ) t += 0x00000600;
  if( (t & 0x0000f000) >= 0x00006000 ) t += 0x0000a000;
  /* Minutes are now OK */

  if( (t & 0x000f0000) >= 0x000a0000 ) t += 0x00060000;
  if( (t & 0x00ff0000) >= 0x00240000 ) t += 0x00dc0000;
  /* Hours are now OK */

  if( (t & 0x0f000000) >= 0x0a000000 ) t += 0x06000000;
  if( (t & 0xf0000000) >= 0xa0000000 ) t = 0;
  /* Days are now OK */

  * timep = t; /* Store new value */
}

/* display_debug
   A function to help debugging.

   After calling display_debug,
   the two middle lines of the display show
   an address and its current contents.

   There's one parameter: the address to read and display.

   Note: When you use this function, you should comment out any
   repeated calls to display_image; display_image overwrites
   about half of the digits shown by display_debug.
*/
void display_debug( volatile int * const addr )
{
  display_string( 1, "Addr" );
  display_string( 2, "Data" );
  num32asc( &textbuffer[1][6], (int) addr );
  num32asc( &textbuffer[2][6], *addr );
  display_update();
}

uint8_t spi_send_recv(uint8_t data) {
	while(!(SPI2STAT & 0x08));
	SPI2BUF = data;
	while(!(SPI2STAT & 1));
	return SPI2BUF;
}

void display_init(void) {
        DISPLAY_CHANGE_TO_COMMAND_MODE;
	quicksleep(10);
	DISPLAY_ACTIVATE_VDD;
	quicksleep(1000000);

	spi_send_recv(0xAE);
	DISPLAY_ACTIVATE_RESET;
	quicksleep(10);
	DISPLAY_DO_NOT_RESET;
	quicksleep(10);

	spi_send_recv(0x8D);
	spi_send_recv(0x14);

	spi_send_recv(0xD9);
	spi_send_recv(0xF1);

	DISPLAY_ACTIVATE_VBAT;
	quicksleep(10000000);

	spi_send_recv(0xA1);
	spi_send_recv(0xC8);

	spi_send_recv(0xDA);
	spi_send_recv(0x20);

	spi_send_recv(0xAF);
}

void display_string(int line, char *s) {
	int i;
	if(line < 0 || line >= 4)
		return;
	if(!s)
		return;

	for(i = 0; i < 16; i++)
		if(*s) {
			textbuffer[line][i] = *s;
			s++;
		} else
			textbuffer[line][i] = ' ';
}

void display_image(int x, const uint8_t *data) {
	int i, j;

	for(i = 0; i < 4; i++) {
		DISPLAY_CHANGE_TO_COMMAND_MODE;

		spi_send_recv(0x22);
		spi_send_recv(i);

		spi_send_recv(x & 0xF);
		spi_send_recv(0x10 | ((x >> 4) & 0xF));

		DISPLAY_CHANGE_TO_DATA_MODE;

		for(j = 0; j < 32; j++)
			spi_send_recv(~data[i*32 + j]);
	}
}

/* will display a full-screen image by sending/recieving data to a larger array
*/
void display_image1(int x, const uint8_t *data) {
	int i, j;

	for(i = 0; i < 4; i++) {
		DISPLAY_CHANGE_TO_COMMAND_MODE;

		spi_send_recv(0x22);
		spi_send_recv(i);

		spi_send_recv(x & 0xF);
		spi_send_recv(0x10 | ((x >> 4) & 0xF));

		DISPLAY_CHANGE_TO_DATA_MODE;

		for(j = 0; j < 128; j++)
			spi_send_recv(~data[i*128 + j]);
	}
}

void display_update(void) {
	int i, j, k;
	int c;
	for(i = 0; i < 4; i++) {
		DISPLAY_CHANGE_TO_COMMAND_MODE;
		spi_send_recv(0x22);
		spi_send_recv(i);

		spi_send_recv(0x0);
		spi_send_recv(0x10);

		DISPLAY_CHANGE_TO_DATA_MODE;

		for(j = 0; j < 16; j++) {
			c = textbuffer[i][j];
			if(c & 0x80)
				continue;

			for(k = 0; k < 8; k++)
				spi_send_recv(font[c*8 + k]);
		}
	}
}

/* will only update pixels 0-95 of display, prevents image flicker.
 	 one character is 8 pixels, so by removing 2 pairs of characters,
	 we do not modify the last 32 bits of the display (used for small image).
*/
void display_update1(void){
	int i, j, k;
	int c;
	for(i = 0; i < 4; i++) {
		DISPLAY_CHANGE_TO_COMMAND_MODE;
		spi_send_recv(0x22);
		spi_send_recv(i);

		spi_send_recv(0x0);
		spi_send_recv(0x10);

		DISPLAY_CHANGE_TO_DATA_MODE;

		for(j = 0; j < 16; j++) {
			c = textbuffer[i][j];
			if(c & 0x80)
				continue;

			for(k = 0; k < 6; k++)
				spi_send_recv(font[c*8 + k]);
		}
	}
}

/* flashes the display as a welcome */
void welcomeflash(void){
	int counter = 0;
	int counter2 = 0;

	while (counter < 10){					// how long the flash will be for
		while(counter2 < 12){
			display_image1(0, welcome);
			delay(12);
			counter2++;
		}
		counter2 = 0;

		while(counter2 < 12){
			display_image1(0, blackwelcome);
			delay(12);
			counter2++;
		}
		counter2 = 0;

		counter++;
	}
	counter = 0;
}


/* Helper function, local to this file.
   Converts a number to hexadecimal ASCII digits. */
static void num32asc( char * s, int n )
{
  int i;
  for( i = 28; i >= 0; i -= 4 )
    *s++ = "0123456789ABCDEF"[ (n >> i) & 15 ];
}

/*
 * nextprime
 *
 * Return the first prime number larger than the integer
 * given as a parameter. The integer must be positive.
 */
#define PRIME_FALSE   0     /* Constant to help readability. */
#define PRIME_TRUE    1     /* Constant to help readability. */
int nextprime( int inval )
{
   register int perhapsprime = 0; /* Holds a tentative prime while we check it. */
   register int testfactor; /* Holds various factors for which we test perhapsprime. */
   register int found;      /* Flag, false until we find a prime. */

   if (inval < 3 )          /* Initial sanity check of parameter. */
   {
     if(inval <= 0) return(1);  /* Return 1 for zero or negative input. */
     if(inval == 1) return(2);  /* Easy special case. */
     if(inval == 2) return(3);  /* Easy special case. */
   }
   else
   {
     /* Testing an even number for primeness is pointless, since
      * all even numbers are divisible by 2. Therefore, we make sure
      * that perhapsprime is larger than the parameter, and odd. */
     perhapsprime = ( inval + 1 ) | 1 ;
   }
   /* While prime not found, loop. */
   for( found = PRIME_FALSE; found != PRIME_TRUE; perhapsprime += 2 )
   {
     /* Check factors from 3 up to perhapsprime/2. */
     for( testfactor = 3; testfactor <= (perhapsprime >> 1) + 1; testfactor += 1 )
     {
       found = PRIME_TRUE;      /* Assume we will find a prime. */
       if( (perhapsprime % testfactor) == 0 ) /* If testfactor divides perhapsprime... */
       {
         found = PRIME_FALSE;   /* ...then, perhapsprime was non-prime. */
         goto check_next_prime; /* Break the inner loop, go test a new perhapsprime. */
       }
     }
     check_next_prime:;         /* This label is used to break the inner loop. */
     if( found == PRIME_TRUE )  /* If the loop ended normally, we found a prime. */
     {
       return( perhapsprime );  /* Return the prime we found. */
     }
   }
   return( perhapsprime );      /* When the loop ends, perhapsprime is a real prime. */
}

/*
 * itoa
 *
 * Simple conversion routine
 * Converts binary to decimal numbers
 * Returns pointer to (static) char array
 *
 * The integer argument is converted to a string
 * of digits representing the integer in decimal format.
 * The integer is considered signed, and a minus-sign
 * precedes the string of digits if the number is
 * negative.
 *
 * This routine will return a varying number of digits, from
 * one digit (for integers in the range 0 through 9) and up to
 * 10 digits and a leading minus-sign (for the largest negative
 * 32-bit integers).
 *
 * If the integer has the special value
 * 100000...0 (that's 31 zeros), the number cannot be
 * negated. We check for this, and treat this as a special case.
 * If the integer has any other value, the sign is saved separately.
 *
 * If the integer is negative, it is then converted to
 * its positive counterpart. We then use the positive
 * absolute value for conversion.
 *
 * Conversion produces the least-significant digits first,
 * which is the reverse of the order in which we wish to
 * print the digits. We therefore store all digits in a buffer,
 * in ASCII form.
 *
 * To avoid a separate step for reversing the contents of the buffer,
 * the buffer is initialized with an end-of-string marker at the
 * very end of the buffer. The digits produced by conversion are then
 * stored right-to-left in the buffer: starting with the position
 * immediately before the end-of-string marker and proceeding towards
 * the beginning of the buffer.
 *
 * For this to work, the buffer size must of course be big enough
 * to hold the decimal representation of the largest possible integer,
 * and the minus sign, and the trailing end-of-string marker.
 * The value 24 for ITOA_BUFSIZ was selected to allow conversion of
 * 64-bit quantities; however, the size of an int on your current compiler
 * may not allow this straight away.
 */
#define ITOA_BUFSIZ ( 24 )
char * itoaconv( int num )
{
  register int i, sign;
  static char itoa_buffer[ ITOA_BUFSIZ ];
  static const char maxneg[] = "-2147483648";

  itoa_buffer[ ITOA_BUFSIZ - 1 ] = 0;   /* Insert the end-of-string marker. */
  sign = num;                           /* Save sign. */
  if( num < 0 && num - 1 > 0 )          /* Check for most negative integer */
  {
    for( i = 0; i < sizeof( maxneg ); i += 1 )
    itoa_buffer[ i + 1 ] = maxneg[ i ];
    i = 0;
  }
  else
  {
    if( num < 0 ) num = -num;           /* Make number positive. */
    i = ITOA_BUFSIZ - 2;                /* Location for first ASCII digit. */
    do {
      itoa_buffer[ i ] = num % 10 + '0';/* Insert next digit. */
      num = num / 10;                   /* Remove digit from number. */
      i -= 1;                           /* Move index to next empty position. */
    } while( num > 0 );
    if( sign < 0 )
    {
      itoa_buffer[ i ] = '-';
      i -= 1;
    }
  }
  /* Since the loop always sets the index i to the next empty position,
   * we must add 1 in order to return a pointer to the first occupied position. */
  return( &itoa_buffer[ i + 1 ] );
}
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


/* these functions write the value from the temp register into a string */
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
