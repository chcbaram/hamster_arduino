//============================================================================
// Name        : main.cpp
// Author      :
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++
//============================================================================

#include <stdio.h>
#include <conio.h>
#include "main.h"

void mainInit(void);

extern void setup();
extern void loop();

int main()
{ 
  mainInit();
  setup();

  while (1)
  {
    loop();
  }
  printf("exit\n");
  return 0;
}

void mainInit(void)
{
  uartInit();
}