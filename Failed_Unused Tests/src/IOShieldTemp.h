/* IOShieldTemp.h


*/

#ifndef IO_SHIELD_TEMP_h
#define IO_SHIELD_TEMP_h

#include "Wire.h"                  //

//Device Constants
#define IOShieldTempAddr	0x48

//Configuration settings
#define IOSHIELDTEMP_ONESHOT	 0x80 //One Shot mode
#define IOSHIELDTEMP_NONSHOT   0x00 //disable one shot mode
#define IOSHIELDTEMP_RES9		   0x00 //9-bit resultion
#define IOSHIELDTEMP_RES10		 0x20 //10-bit resolution
#define IOSHIELDTEMP_RES11	 	 0x40 //11-bit resolution
#define IOSHIELDTEMP_RES12		 0x60 //12-bit resolution
#define IOSHIELDTEMP_FAULT1		 0x00 //1 fault queue bits
#define IOSHIELDTEMP_FAULT2		 0x08 //2 fault queue bits
#define IOSHIELDTEMP_FAULT4		 0x10 //4 fault queue bits
#define IOSHIELDTEMP_FAULT6		 0x18 //6 fault queue bits
#define IOSHIELDTEMP_ALERTLOW	 0x00 //Alert bit active-low
#define IOSHIELDTEMP_ALERTHIGH 0x04 //Alert bit active-high
#define IOSHIELDTEMP_CMPMODE	 0x00 //comparator mode
#define IOSHIELDTEMP_INTMODE	 0x02 //interrupt mode
#define IOSHIELDTEMP_SHUTDOWN	 0x01 //Shutdown enabled
#define	IOSHIELDTEMP_STARTUP	 0x00 //Shutdown Disabled

          /* startup configuration for our project, using set parameters */
#define IOSHIELDTEMP_CONF_DEFAULT	(IOSHIELDTEMP_NONSHOT | IOSHIELDTEMP_RES12 | IOSHIELDTEMP_FAULT6 | IOSHIELDTEMP_ALERTHIGH | IOSHIELDTEMP_CMPMODE | IOSHIELDTEMP_STARTUP)
#define	IOSHIELDTEMP_MIN		20.0  //Minimum input tempreture for the Hyst/Limit registers
#define	IOSHIELDTEMP_MAX		21.5  //Maximum input tempreture for the Hyst/Limit registers


class IOShieldTempClass
{
  private:
	static uint8_t	lastConfig;
	static void setTempHystPriv(uint16_t);
	static void setTempLimitPriv(uint16_t);
  public:
	IOShieldTempClass();
	void config(uint8_t);

  float getTemp(){
    float temp;
    
    return temp;
  }

  void setTempHyst(float);
	void setTempLimit(float);
	float convFtoC(float);
	float convCtoF(float);
};

extern IOShieldTempClass IOShieldTemp;


#endif
