/* project.h
  Header file for project, declares all functions used
  */

#include <stdbool.h>

  /* Declare display-related functions from projectfunctions.c */
  void display_image(int x, const uint8_t *data);
  void display_image1(int x, const uint8_t *data);
  void display_init(void);
  void display_string(int line, char *s);
  void display_update(void);
  void display_update1(void);
  uint8_t spi_send_recv(uint8_t data);

  /* Declare lab-related functions from projectfunctions.c */
  char * itoaconv( int num );
  void projectwork(void);                 /***************************/
  int nextprime( int inval );
  void quicksleep(int cyc);
  void tick( unsigned int * timep );

  /* Declare display_debug - a function to help debugging.

   After calling display_debug,
   the two middle lines of the display show
   an address and its current contents.

   There's one parameter: the address to read and display.

   Note: When you use this function, you should comment out any
   repeated calls to display_image; display_image overwrites
   about half of the digits shown by display_debug.
*/
void display_debug( volatile int * const addr );

/* Declare bitmap array containing font */
extern const uint8_t const font[128*8];
/* Declare bitmap array containing icons */
extern const uint8_t const openwindow[128];
extern const uint8_t const halfwindow[128];
extern const uint8_t const closedwindow[128];
extern const uint8_t const hot[128];
extern const uint8_t const cold[128];
extern const uint8_t const blank[128];
extern const uint8_t const welcome[512];
extern const uint8_t const blackwelcome[512];
extern const uint8_t const notwelcome[512];
extern const uint8_t const manualmode[512];
extern const uint8_t const bye[512];
extern const uint8_t const blackbye[512];
/* Declare text buffer for display output */
extern char textbuffer[4][16];
extern int currentangle;

extern uint16_t tempin;

extern char *s;
extern char *t;
extern char buf[32];


/* Declare project-specific functions */
void delay(int);
void time2string(char*, int);
int getbtns(void);
int getsw(void);

int motorbtns(void);
int motorsw(void);
void setPWM(int);
//void user_isr(void);
void initialise(void);
void startPWM(void);
void stopPWM(void);
void updateangle(int);
int temptoangle(uint16_t);

void welcomeflash(void);
void delay1(int);
void i2c1_idle(void);
bool i2c1_send(uint8_t data);
uint8_t i2c1_recv(void);
void i2c1_start(void);
void i2c1_ack(void);
void i2c1_nack(void);
void i2c1_restart(void);
void i2c1_stop(void);
uint32_t strlen(char *str);
char *fixed_to_string(uint16_t num, char *buf);
