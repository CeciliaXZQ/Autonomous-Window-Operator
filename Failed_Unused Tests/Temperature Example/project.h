/* project.h
  Header file for project, declares all functions used
  */


  /* Declare display-related functions from projectfunctions.c */
  void display_image(int x, const uint8_t *data);
  void display_init(void);
  void display_string(int line, char *s);
  void display_update(void);
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
/* Declare text buffer for display output */
extern char textbuffer[4][16];


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
char *fixed_to_string(uint16_t num, char *buf);
void startPWM(void);
void stopPWM(void);
