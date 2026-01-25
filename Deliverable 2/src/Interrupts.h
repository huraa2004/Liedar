#ifndef __INTERRUPTS_H__
#define __INTERRUPTS_H__

#include <stdint.h>
#include "tm4c1294ncpdt.h"
#include "VL531X.h"

void EnableInt(void);
void DisableInt(void);
void WaitForInt(void);
void PortJ_Interrupt_Init(void);

#endif