#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include <stdbool.h>

int timeoutcount = 0;
void enable_interrupt(void);

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

  int btn1 = ((PORTD >> 4) & 1);  //  BTN1 = RD4 = PORTD with index 4
  int btn2 = ((PORTD >> 5) & 1);  //  BTN2 = RD5 = PORTD with index 5
  int btn3 = ((PORTD >> 6) & 1);  //  BTN3 = RD6 = PORTD with index 6
  int btn4 = ((PORTD >> 7) & 1);  //  BTN4 = RD7 = PORTD with index 7

  if (btn1 && btn2 && btn3 && btn4){
    return 1000;
  }
  if ((btn1 && btn2 && btn3) || (btn1 && btn2 && btn4) || (btn1 && btn3 && btn4) || (btn2 && btn3 && btn4)){
    return 1000;
  }
  if ((btn1 && btn2) || (btn1 && btn3) || (btn1 && btn4) || (btn2 && btn3) || (btn2 && btn4) || (btn3 && btn4)){
    return 1000;
  }
  if (btn3 || btn4){
    return 1000;
  }

  if (btn1){
    onOff = 0x1;
  }
  if (btn2){
    onOff = 0x2;
  }
  if (btn3){
    onOff = 0x3;
  }
  if (btn4){
    onOff = 0x4;
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

  int onOff = 10;

  if (switch1 && switch2 && switch3 && switch4){
    return 1000;
  }
  if ((switch1 && switch2 && switch3) || (switch1 && switch2 && switch4) || (switch1 && switch3 && switch4) || (switch2 && switch3 && switch4)){
    return 1000;
  }
  if ((switch1 && switch2) || (switch1 && switch3) || (switch2 && switch3) || (switch2 && switch4) || (switch3 && switch4)){
    return 1000;
  }
  if (!switch4){
    onOff = 1000;
  }

  if (switch4 && !switch1){                   // forward operation
    onOff = 0x0;
  }
  if (switch4 && switch1){                    // reverse operation
    onOff = 0x1;
  }

  return onOff;
}

int a;

void user_isr( void ){

  if (IFS(0) & 0x8000){           // how do we know that external interrupt 3 corresponds to switch 3?
    a = 0;
  }

  IFSCLR(0) = 0x0100;             /* clear timer 2 interrupt flag */
  IFSCLR(0) = 0x8000;
    return;
}

int sendswitch(void){
  return a;
}

void initialise(void){


    T2CON     = 0x0;            /* stops timer */
    T2CONSET  = 0x60;           /* sets 64-bit prescaler */
    TMR2      = 0x0;            /* clear timer register */
    PR2       = 0x3e80;         /* load period register with 16,000 for 20ms */

    OC1RS     = 0x136;          /* pulse duration for 0º */
    OC1CON    = 0x6;            /* set the output compare to PWM mode */

    IPCSET(2) = 0xC;            /* set priority level to 3 - bits 2:4 */
    IFSCLR(0) = 0x0100;         /* clear timer interrupt flag */

    IECSET(0) = 0x100;          /* enable timer 2 interrupt */

    T2CONSET  = 0x8000;         /* start the timer */

    IPCSET(3) = 0xC000000;
    IFSCLR(0) = 0x8000;
    IECSET(0) = 0x8000;

    motorsw();
    motorbtns();

    OC1CON    |= 0x8000;        /* start the PWM */


    /* SURPRISE!
    IPCSET(3) = 0x0C000000;     // set priority level to 3 as timer interrupt - bits 26:28
    IFSCLR(0) = 0x8000;         // clear external interrupt flag
    IECSET(0) = 0x8000;         // enable external interrupt 3 */

    enable_interrupt();         /* enable global interrupts for microcontroller */

    return;
}

void setPWM(int length){

/* set PWM register value to that of length using simple arithmetic */
  int temp = 0;

  if (length >= 180){
    temp = 0x6b0;
  }

  if (length <= 0){
    temp = 0x184;
  }

  if ((length > 0) || (length < 180)){
    temp = length * 973;
    temp = temp / 100;
    temp += 388;
    temp *= 100;
    temp = temp / 125;
    temp = temp / 100;
  }


  T2CON     = 0x0;            /* stops timer */
  T2CONSET  = 0x60;           /* sets 64-bit prescaler */
  TMR2      = 0x0;            /* clear timer register */
  PR2       = 0x3e80;         /* load period register with 16,000 for 20ms */

  OC1RS     = temp;           /* pulse duration for xº */
  OC1CON    = 0x6;            /* set the output compare to PWM mode */

  IPCSET(2) = 0xC;            /* set priority level to 3 - bits 2:4 */
  IFSCLR(0) = 0x0100;         /* clear timer interrupt flag */

  IECSET(0) = 0x100;          /* enable timer 2 interrupt */

  T2CONSET  = 0x8000;         /* start the timer */
  OC1CON    |= 0x8000;        /* start the PWM */

  return;
}



/*void delayMicroseconds(uint16_t delay){
    if(timeoutcount == delay){
      timeoutcount = 0;
      return;
    }
}*/
