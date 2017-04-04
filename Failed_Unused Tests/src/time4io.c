

#include <stdint.h>
#include <pic32mx.h>
#include "mipslab.h"



int getsw(void){
  /* the four least significant bits of the return value should contain data
     from switches SW4-SW1, SW1 corresponds to the LSB. All other bits of
     return value must be 0 */

  //   SW1 = RD8 =  PORTD with index 8
  //   SW2 = RD9 =  PORTD with index 9
  //   SW3 = RD10 = PORTD with index 10
  //   SW4 = RD11 = PORTD with index 11

     int onOff = 0;

     if (((PORTD >> 8) & 1) == 1){
       onOff += 0x1;
     }
     if (((PORTD >> 9) & 1) == 1){
       onOff += 0x2;
     }
     if (((PORTD >> 10) & 1) == 1){
       onOff += 0x4;
     }
     if (((PORTD >> 11 ) & 1) == 1){
       onOff += 0x8;
     }

     return onOff;
}

int getbtns(void){
  /* the three least significant bits of the return value should contain data
     from buttons BTN4-BTN2, BTN2 corresponds to the LSB. All other bits of
     return value must be 0 */

  //  BTN2 = RD5 = PORTD with index 5
  //  BTN3 = RD6 = PORTD with index 6
  //  BTN4 = RD7 = PORTD with index 7


    int onOff = 0;

    if (((PORTD >> 5) & 1) == 1){
      onOff += 0x1;
    }
    if (((PORTD >> 6) & 1) == 1){
      onOff += 0x2;
    }
    if (((PORTD >> 7) & 1) == 1){
      onOff += 0x4;
    }

    return onOff;
}
