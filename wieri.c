/*

Using isr.c from the WiringPi library
https://github.com/WiringPi/WiringPi/blob/master/examples/isr.c

Compile:
    - cmake .
    - make

Run:
    sudo ./wieri

*/

#include <wieri.h>


// -------------------------------------------------------------------------
// Vars

pthread_t tid;
static volatile unsigned char buffer[25];
static volatile int bitCounter = 0;
static volatile bool receiving = false;

typedef struct
{
  int timeout;
  int data0;
  int data1;
  int facilityCodeStart;
  int facilityCodeEnd;
  int cardNumberStart;
  int cardNumberEnd;
} conf;
conf config;
// -------------------------------------------------------------------------
// Signals handlers

void sig_handler(int signum)
{
  printf("\nExiting application\n");
  pthread_join(tid, NULL);
  exit(EXIT_SUCCESS);
}

// -------------------------------------------------------------------------
// Functions

void *resetRead(void *vargp)
{
  int timeout = (int) vargp;
  delay(timeout);
  if(receiving)
  {
    receiving = false;
    bitCounter = 0;
    printf("Reading discarded after %d millis\n\n", timeout);
  }
}

/* Adds a bit to the buffer */
void addBit(const unsigned char bit)
{
  buffer[bitCounter] = bit;
  if(!receiving)
  {
    receiving = true;
    printf("New reading...\n");
    if(pthread_create(&tid, NULL, resetRead, (void *)config.timeout))
    {
      perror("ERROR creating timeout thread.");
    }
  }

  bitCounter ++;
  if(bitCounter == 26)
  {
    receiving = false;
    bitCounter = 0;
    printf("Raw binary frame: %s \n", buffer);
    unsigned char* bufferP = (unsigned char*) buffer;
    if (parityCheck(bufferP))
    {
      int facilityCode = binSlice2Dec(bufferP, config.facilityCodeStart, config.facilityCodeEnd);
      printf("facility code: %d \n", facilityCode);
      int cardNumber = binSlice2Dec(bufferP, config.cardNumberStart, config.cardNumberEnd);
      printf("card number: %d \n\n", cardNumber);
    }
  }
}


/* Counts ones from a char slice */
int sliceOnesCount(unsigned char* bits, int start, int stop)
{
  unsigned int i;
  int ones = 0;
  for(i=start; i<=stop; i++)
  {
    if(bits[i] == '1'){ ones++; }
  }
  return ones;
}


/* Checks parity for Wiegand frame */
bool parityCheck(unsigned char* bits)
{
  int ones;

  // Even parity check
  ones = sliceOnesCount(bits, 0, 12);
  printf("Even parity check: %c | %.12s\n", bits[0], &bits[1]);
  if(ones % 2)
  {
    printf("Bad Frame, even parity error\n\n");
    return false;
  }

  // Odd parity check
  ones = sliceOnesCount(bits, 13, 25);
  printf("Even parity check: %c | %.12s\n", bits[13], &bits[14]);
  if(!(ones % 2))
  {
    printf("Bad Frame, odd parity error\n\n");
    return false;
  }
  printf("Parity check ok\n");
  return true;
}


/* Converts binary char array slice to decimal */
int binSlice2Dec(unsigned char* bits, int start, int stop)
{
  int i;
  int sum = 0;
  int base = 0;
  for(i=stop; i>=start; i--)
  {
    sum += ((bits[i]-'0')*(1<<base++));
  }
  return sum;
}

// -------------------------------------------------------------------------
// Interrupts

void got0(void)
{
  addBit('0');
}

void got1(void)
{
  addBit('1');
}


// -------------------------------------------------------------------------
// Init handler

static int handler(void* pins, const char* section, const char* name,
                   const char* value)
{
  conf* pconfig = (conf*) pins;
  #define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
  if (MATCH("general", "timeout"))
  {
    pconfig->timeout = atoi(value);
  }
  else if (MATCH("pins", "data0"))
  {
    pconfig->data0 = atoi(value);
  }
  else if (MATCH("pins", "data1"))
  {
    pconfig->data1 = atoi(value);
  }
  else if (MATCH("facilityCode", "startBit"))
  {
    pconfig->facilityCodeStart = atoi(value);
  }
  else if (MATCH("facilityCode", "endBit"))
  {
    pconfig->facilityCodeEnd = atoi(value);
  }
  else if (MATCH("cardNumber", "startBit"))
  {
    pconfig->cardNumberStart = atoi(value);
  }
  else if (MATCH("cardNumber", "endBit"))
  {
    pconfig->cardNumberEnd = atoi(value);
  }
  else
  {
    return 0;
  }
  return 1;
}


// -------------------------------------------------------------------------
// main

int main(void)
{
  if (ini_parse("conf.ini", handler, &config) < 0)
  {
    printf("Can't load 'conf.ini'\n");
    return EXIT_FAILURE;
  }

  printf("Timeout set to %d milliseconds\n", config.timeout);
  printf("Data 0 will be read from pin %d\n", config.data0);
  printf("Data 1 will be read from pin %d\n\n", config.data1);

  if (signal(SIGINT, sig_handler) == SIG_ERR) { printf("Can't catch SIGINT! \n"); }

  if (wiringPiSetup () < 0)
  {
    fprintf (stderr, "Unable to setup wiringPi: %s\n", strerror (errno));
    return EXIT_FAILURE;
  }

  // Set Pin generate an interrupt on high-to-low transitions
  if ( wiringPiISR (config.data0, INT_EDGE_FALLING, &got0) < 0 )
  {
    fprintf (stderr, "Unable to setup ISR: %s\n", strerror (errno));
    return EXIT_FAILURE;
  }

  if ( wiringPiISR (config.data1, INT_EDGE_FALLING, &got1) < 0 )
  {
    fprintf (stderr, "Unable to setup ISR: %s\n", strerror (errno));
    return EXIT_FAILURE;
  }

  while(1)
  {
  }
  return EXIT_SUCCESS;
}
