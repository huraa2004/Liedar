#include <stdint.h>
#include "tm4c1294ncpdt.h"
#include "SysTick.h"
#include "Motor.h"
#include "Ports.h"
volatile MotorParameters motorState = {
    .direction = DIRECTION_CW,
    .currentStep = 0,
    .operation = OPERATION_NORMAL
};
// AH - Motor step sequences
const uint8_t STEP_SEQUENCE_CW[4] = {0b00001001, 0b00000011, 0b00000110, 0b00001100};
const uint8_t STEP_SEQUENCE_CCW[4] = {0b00001100, 0b00000110, 0b00000011, 0b00001001};

// Ah - Quad step function
void motorStep(MotorDirection dir, uint16_t steps) {
    const uint8_t* sequence = (dir == DIRECTION_CW) ? STEP_SEQUENCE_CW : STEP_SEQUENCE_CCW;
    uint16_t i;
    for(i = 0; i <= steps; i++) {
        // AH - Clear existing motor pins and set new values
        GPIO_PORTH_DATA_R &= ~0xF;
        GPIO_PORTH_DATA_R |= sequence[i % STEPS_PER_SEQUENCE];
        SysTick_Wait10ms(MOTOR_DELAY);
        
        // AH - Update current position
        if(dir == DIRECTION_CW) {
            motorState.currentStep = (motorState.currentStep + 1) % FULL_ROTATION; // AH - Modulo operation to wrap around
        } else {
            motorState.currentStep = (motorState.currentStep - 1 + FULL_ROTATION) % FULL_ROTATION;
        }
    }
}

void stepperMotor(int delay, int direction) {
    MotorDirection dir = (direction == 1) ? DIRECTION_CW : DIRECTION_CCW;
    uint16_t steps = (direction == 1) ? STEP_REQ: FULL_ROTATION;
    motorStep(dir, steps);
}
