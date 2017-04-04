/* projectwork.c

*/

#include <stdint.h>
#include <pic32mx.h>
#include "project.h"

int enableswitch = 0;
int btn          = 0;
int currentangle;

#define TEMP_SENSOR1_ADDR 			0x48

typedef enum TempSensorReg TempSensorReg;
enum TempSensorReg {
	TEMP_SENSOR_REG_TEMP,
	TEMP_SENSOR_REG_CONF,
	TEMP_SENSOR_REG_HYST,
	TEMP_SENSOR_REG_LIMIT,
};




void user_isr( void ){

  if (IFS(1) & 0x1){
    int counter2 = 0;
    int counter1 = 0;
    char buf[32], *s, *t;
    int tempin;

    display_image1(0, manualmode);
    while (counter1 < 10000000){
      delay(10000000);
      counter1++;
    }

    display_update();

    while(motorbtns() != 0x2){
      if(motorbtns() == 0x3){

        if(motorsw() == 0){                             // 0 degrees
          updateangle(0);
          startPWM();
          setPWM(currentangle);
          delay(500);
          stopPWM();
        }



          if (motorsw() == 1){
            updateangle(-22);
            startPWM();
            setPWM(currentangle);   //how to set negative angle?
            delay(500);
            stopPWM();
          }

          if (motorsw() == 2){
            updateangle(22);
            startPWM();
            setPWM(currentangle);
            delay(500);
            stopPWM();
          }

          if (motorsw() == 3){
            updateangle(-45);    // negative
            startPWM();
            setPWM(currentangle);
            delay(500);
            stopPWM();
          }

          if (motorsw() == 4){
            updateangle(45);
            startPWM();
            setPWM(currentangle);
            delay(500);
            stopPWM();
          }

          if (motorsw() == 5){
            updateangle(180);
            startPWM();
            setPWM(currentangle);
            delay(500);
            stopPWM();
          }
        }
          if (currentangle == 0){
            display_image(96, closedwindow);
          }
          else {
            display_image(96, openwindow);
          }


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


        display_string(3, s);
        display_update1();


      }

    display_image1(0, bye);
    while (counter2 < 10000000){
      delay(10000000);
      counter2++;
    }
    display_update();

    IFSCLR(1) = 0x1;

  }

    return;
}

void initialise(void){

    TRISD    |= 0xfe0;          /* set bits 5-11 to use buttons and switches as inputs */
    TRISE    &= ~0xff;          /* set bits 0-7 to use LEDs as outputs */

    // change notice interrupt for buttons
    CNCONSET  = 0x8000;         // enables the CN module
    CNEN      = 0x10000;        // enable individual input pin (for button 4)
    // enable optional pull-ups
    // read register to clear interrupt
    IPCSET(6) = 0x100000;       // set priority to 4 (bits 20:18 of IPC6)
    IFSCLR(1) = 0x0;            // clear CN interrupt flag (bit 0 of IFS1)
    // configure interrupt for a specific edge-detection
    IECSET(1) = 0x1;            // enables CN interrupt bit

    motorsw();
    motorbtns();

    enable_interrupt();         /* enable global interrupts for microcontroller */

    return;
}

void setPWM(int currentangle){

/* set PWM register value to that of length using simple arithmetic */
  int temp = 0;


  if (currentangle >= 180){
    temp = 0xa73;
  }

  if (currentangle <= 0){
    temp = 0x1e5;
  }

  if ((currentangle > 0) || (currentangle < 180)){
    temp = currentangle * 973;
    temp = temp / 100;
    temp += 388;
    temp *= 125;
    temp = temp / 100;
  }

  OC1RS = temp;
  return;
}

void startPWM(void){
  T2CON     = 0x0;            /* stops timer */
  T2CONSET  = 0x60;           /* sets 64-bit prescaler */
  TMR2      = 0x0;            /* clear timer register */
  PR2       = 0x3e80;         /* load period register with 16,000 for 20ms */
  OC1CON    = 0x6;            /* set the output compare to PWM mode */


  T2CONSET  = 0x8000;         /* start the timer */
  OC1CON    |= 0x8000;        /* start the PWM */
}

void stopPWM(void){
  T2CON     = 0x0;            /* stops timer */
  T2CONSET  = 0x60;           /* sets 64-bit prescaler */
  TMR2      = 0x0;            /* clear timer register */
  PR2       = 0x3e80;         /* load period register with 16,000 for 20ms */
  OC1CON    = 0x6;            /* set the output compare to PWM mode */


  T2CONSET  = 0x8000;         /* start the timer */
  OC1CON    = 0x0000;         /* stop the PWM */
}

void resetPWM(void){
  currentangle = 0;

  startPWM();
  setPWM(currentangle);
  delay(500);
  stopPWM();
}

void projectwork(void){

  int sw = motorsw();
  btn = motorbtns();




}
