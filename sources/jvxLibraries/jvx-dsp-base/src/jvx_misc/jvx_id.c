#include <stdio.h>

unsigned int jvxID = 0;

unsigned int jvx_id(const char *description, unsigned int pid)
{
  jvxID++;
  if(description)
    printf("[%s/PID%i/ID%i]\n", description, pid, jvxID);
  return jvxID;
}
