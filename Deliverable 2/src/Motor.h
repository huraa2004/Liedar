#ifndef MOTOR_H
#define MOTOR_H

#include <stdint.h>

#define STEPS_PER_SEQUENCE 4
#define LOOPING_STEPS 16
#define MOTOR_DELAY 1
#define FULL_ROTATION 2048
#define STEP_REQ      64  //64 steps for 11.25 degrees (16*4)

typedef enum {
    DIRECTION_CW = 1,
    DIRECTION_CCW = 0
} MotorDirection;
typedef enum {
    OPERATION_NORMAL,
    OPERATION_HOME  // Not used in this code, but can be used for homing operation (deliverable 1 content)
} MotorOperation;
typedef struct {
    MotorDirection direction;
    uint16_t currentStep;
    MotorOperation operation;
} MotorParameters;

void stepperMotor(int delay, int direction);
void motorStep(MotorDirection dir, uint16_t steps);
extern volatile MotorParameters motorState;
#endif