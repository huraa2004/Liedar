/*  Time of Flight for 2DX4 -- Studio W8-0
                Code written to support data collection from VL53L1X using the Ultra Light Driver.
                I2C methods written based upon MSP432E4 Reference Manual Chapter 19.
                Specific implementation was based upon format specified in VL53L1X.pdf pg19-21
                Code organized according to en.STSW-IMG009\Example\Src\main.c
                
                The VL53L1X is run with default firmware settings.


            Written by Tom Doyle
            Updated by  Hafez Mousavi Garmaroudi
            Last Update: March 17, 2020
						
						Last Update: March 03, 2022
						Updated by Hafez Mousavi
						__ the dev address can now be written in its original format. 
								Note: the functions  beginTxI2C and  beginRxI2C are modified in vl53l1_platform_2dx4.c file
								
						Modified March 16, 2023 
						by T. Doyle
							- minor modifications made to make compatible with new Keil IDE

*/
#include <stdint.h>
#include "SysTick.h"
#include "uart.h"
#include "onboardLEDs.h"
#include "tm4c1294ncpdt.h"
#include "VL53L1X_api.h"
#include "Ports.h"
#include "I2C.h"
#include "PLL.h"
#include "VL531X.h"
//*********************************************************************************************************
//*********************************************************************************************************
//***********					MAIN Function				*****************************************************************
//*********************************************************************************************************
//*********************************************************************************************************


void ShowPLLclock(void) {
  // Show PLL clock on LED
  uint16_t i;
  for (i = 0; i < 150; i++) {
    TogglePLLLED();
    SysTick_Wait10ms(50);
  }
  resetPLLLED();
}
int main(void) {


	//initialize
	PLL_Init();	
	SysTick_Init();
  Ports_init(); // Initialize Ports
  SubMain();
  ShowPLLclock(); // Show PLL clock on LED

  
  while (1) {
    if (tofState.state == TOF_TRANSMIT_MODE) {
      TOF_TransmitData();
    } else if (tofState.state == TOF_SCAN_MODE && tofState.status == TOF_SCANNING) {
      TOF_ScanRotation();
    }
  }
};