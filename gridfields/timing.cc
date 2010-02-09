#include "timing.h"
#include <math.h>
#define DEC 10000

float gettime(){
  float ClockTicks;
  struct tms Time;
  float StarttimeSeconds;

  times(&Time);  

  ClockTicks = (double) sysconf(_SC_CLK_TCK);
  
  return floor(DEC*Time.tms_utime/ClockTicks)/DEC;
}
