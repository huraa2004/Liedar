#ifndef VL531X_H
#define VL531X_H

#include <stdint.h>
#include "Motor.h"

// AH - TOF Sensor Constants
#define TOF_ADDRESS          0x29
#define NUM_SAMPLES         32
#define MAX_MEASUREMENTS    20
#define SCAN_DELAY         50
#define MASK              0xFF
#define STEP_REQ            64

// AH - TOF States and Parameters
typedef enum {
    TOF_SCANNING,
    TOF_SCANCOMPLETE
} ScanStatus;

typedef enum {
    TOF_SCAN_MODE,
    TOF_TRANSMIT_MODE
} ScanState;

typedef struct {
    ScanStatus status;
    ScanState state;
} TOFParameters;

// AH - Function prototypes
void VL53L1X_XSHUT(void);
void TOF_Init(void);
uint16_t TOF_GetDistance(void);
void TOF_ScanRotation(void);
void TOF_TransmitData(void);
TOFParameters TOF_GetState(void);
void TOF_SetState(TOFParameters newState);
void SubMain(void);

extern volatile uint16_t transmittedData[MAX_MEASUREMENTS][NUM_SAMPLES];
extern volatile TOFParameters tofState;

#endif