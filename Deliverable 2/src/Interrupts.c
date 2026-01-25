#include "Interrupts.h"
#include "onboardLEDs.h"
#include "Ports.h"
#include "VL531X.h"

// AH - Enable interrupts
void EnableInt(void) {
    __asm("    cpsie   i\n");
}

// AH - Disable interrupts
void DisableInt(void) {
    __asm("    cpsid   i\n");
}

// AH - Wait for interrupt
void WaitForInt(void) {
    __asm("    wfi\n");
}

// Initialize Port J interrupts
void PortJ_Interrupt_Init(void) {
    // Configure edge-triggered interrupts
    GPIO_PORTJ_IS_R = 0;          // Edge-sensitive
    GPIO_PORTJ_IBE_R = 0;         // Single edge trigger
    GPIO_PORTJ_IEV_R = 0;         // Falling edge event
    GPIO_PORTJ_ICR_R = 0x03;      // Clear both interrupt flags
    GPIO_PORTJ_IM_R = 0x03;       // Enable interrupts for PJ0 and PJ1

    // Configure NVIC
    NVIC_EN1_R = 0x00080000;      // Enable interrupt 51
    NVIC_PRI12_R = 0xA0000000;    // Set priority 5

    EnableInt();                   // Enable global interrupts
}

void GPIOJ_IRQHandler(void) {
    TOFParameters currentState;
    
    // AH - PJ0 Motor Control
    if (GPIO_PORTJ_RIS_R & 0x01) {
        currentState = TOF_GetState();
        
        // AH - Toggle scanning state
        if (currentState.status == TOF_SCANCOMPLETE) {
            currentState.status = TOF_SCANNING;
            TOF_SetState(currentState);
            SetMeasurementLED();
            TOF_ScanRotation();
        } else {
            currentState.status = TOF_SCANCOMPLETE;
            TOF_SetState(currentState);
            ResetMeasurementLED();
        }
        
        GPIO_PORTJ_ICR_R |= 0x01;  // Clear interrupt
    }
    
    // AH - PJ1 (Data Transmission/ acquisition)
    if (GPIO_PORTJ_RIS_R & 0x02) {
        currentState = TOF_GetState();
        
        currentState.state = 1;
        TOF_SetState(currentState);
        ToggleStatusLED();
        
        GPIO_PORTJ_ICR_R |= 0x02;
    }
}