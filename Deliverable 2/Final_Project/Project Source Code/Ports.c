#include "Ports.h"
#include "tm4c1294ncpdt.h"
#include <stdint.h>

void PortJ_Init(void) {
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R8;                  // Activate the clock for Port J
    while ((SYSCTL_PRGPIO_R & SYSCTL_RCGCGPIO_R8) == 0) {}    // Allow time for clock to stabilize
    GPIO_PORTJ_DIR_R &= ~(BIT(0) | BIT(1));                   // Make PJ0-PJ1 inputs
    GPIO_PORTJ_DEN_R |= (BIT(0) | BIT(1));
    GPIO_PORTJ_PCTL_R &= ~0x0000FFFF;                         // Configure PJ0-PJ1 as GPIO
    GPIO_PORTJ_AMSEL_R &= ~0xFF;                              // Disable analog functionality on PJ0-PJ1
    GPIO_PORTJ_PUR_R |= 0x03;                                 // Enable pull-up resistors on PJ0-PJ1
}

void PortN_Init(void) {
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R12;                 // Activate the clock for Port N
    while ((SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R12) == 0) {}     // Allow time for clock to stabilize
    GPIO_PORTN_DIR_R |= (BIT(0) | BIT(1));                    // Make PN0-PN1 outputs
    GPIO_PORTN_DEN_R |= (BIT(0) | BIT(1));
    GPIO_PORTN_PCTL_R &= ~0x0000FFFF;                         // Configure PN0-PN1 as GPIO
    GPIO_PORTN_AMSEL_R &= ~0xFF;                              // Disable analog functionality on PN0-PN1
}

void PortF_Init(void) {
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R5;                  // Activate the clock for Port F
    while ((SYSCTL_PRGPIO_R & SYSCTL_RCGCGPIO_R5) == 0) {}    // Allow time for clock to stabilize
    GPIO_PORTF_DIR_R |= (BIT(0) | BIT(4));                    // Make PF0 and PF4 outputs
    GPIO_PORTF_DEN_R |= (BIT(0) | BIT(4));
    GPIO_PORTF_PCTL_R &= ~0x000F000F;                         // Configure PF0 and PF4 as GPIO
    GPIO_PORTF_AMSEL_R &= ~0xFF;                              // Disable analog functionality on PF0 and PF4
}

void PortH_Init(void) {
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R7;                  // Activate the clock for Port H
    while ((SYSCTL_PRGPIO_R & SYSCTL_RCGCGPIO_R7) == 0) {}    // Allow time for clock to stabilize
    GPIO_PORTH_DIR_R |= (BIT(0) | BIT(1) | BIT(2) | BIT(3));  // Make PH0-PH3 outputs
    GPIO_PORTH_DEN_R |= (BIT(0) | BIT(1) | BIT(2) | BIT(3));
    GPIO_PORTH_PCTL_R &= ~0x0000FFFF;                         // Configure PH0-PH3 as GPIO
    GPIO_PORTH_AMSEL_R &= ~0xFF;                              // Disable analog functionality on PH0-PH3
}
//The VL53L1X needs to be reset using XSHUT.  We will use PG0
void PortG_Init(void){
    //Use PortG0
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R6;                // activate clock for Port N
    while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R6) == 0){};    // allow time for clock to stabilize
    GPIO_PORTG_DIR_R &= 0x00;                                        // make PG0 in (HiZ)
  GPIO_PORTG_AFSEL_R &= ~0x01;                                     // disable alt funct on PG0
  GPIO_PORTG_DEN_R |= 0x01;                                        // enable digital I/O on PG0
                                                                                                    // configure PG0 as GPIO
  //GPIO_PORTN_PCTL_R = (GPIO_PORTN_PCTL_R&0xFFFFFF00)+0x00000000;
  GPIO_PORTG_AMSEL_R &= ~0x01;                                     // disable analog functionality on PN0

    return;
}
void Ports_init(void) {
    PortJ_Init(); // Initialize Port J (Buttons)
    PortN_Init(); // Initialize Port N (LEDs)
    PortF_Init(); // Initialize Port F (LEDs)
    PortH_Init(); // Initialize Port H (Motor)
    PortG_Init(); // Initialize Port G (XSHUT)
}