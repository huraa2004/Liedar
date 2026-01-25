#include <stdint.h>
#include "tm4c1294ncpdt.h"
#include "VL53L1X_api.h"
#include "onboardLEDs.h"
#include "SysTick.h"
#include "VL531X.h"
#include "uart.h"
#include "I2C.h"
#include "Ports.h"
#include "Interrupts.h"

static uint16_t dev = TOF_ADDRESS;
static int status = 0;
static uint8_t measurementNum = 0;
static uint8_t dataReady = 0;
static uint8_t sensorState = 0;
static uint8_t rangeStatus;
static uint16_t signalRate;
static uint16_t ambientRate;
static uint16_t spadNum;
static uint16_t distance = 0;

volatile uint16_t transmittedData[MAX_MEASUREMENTS][NUM_SAMPLES] = {0};

volatile TOFParameters tofState = {
    .status = TOF_SCANCOMPLETE,
    .state = TOF_SCAN_MODE
};

void VL53L1X_XSHUT(void) {
    // AH - Reset sensor using XSHUT pin
    GPIO_PORTG_DIR_R |= 0x01;
    GPIO_PORTG_DATA_R &= 0b11111110;
    FlashAllLEDs();
    SysTick_Wait10ms(10);
    GPIO_PORTG_DIR_R &= ~0x01;
}

void TOF_Init(void) {
    // AH - Initialize and boot sensor
    uint16_t wordData;
    
    UART_printf("AH - Program Starting\r\n");
    
    // AH - Get sensor ID
    status = VL53L1X_GetSensorId(dev, &wordData);
    sprintf(printf_buffer, "(Model_ID, Module_type)=0x%x\r\n", wordData);
    UART_printf(printf_buffer);
    
    // AH - Wait for sensor boot
    while(sensorState == 0) {
        status = VL53L1X_BootState(dev, &sensorState);
        SysTick_Wait10ms(20);
    }
    
    FlashAllLEDs();
    UART_printf("AH - TOF Sensor Booted\r\n");
    
    // AH - Initialize sensor
    status = VL53L1X_ClearInterrupt(dev);
    status = VL53L1X_SensorInit(dev);
    Status_Check("SensorInit", status);
}

uint16_t TOF_GetDistance(void) {
    // AH - Wait for data ready
    while(dataReady == 0) {
        status = VL53L1X_CheckForDataReady(dev, &dataReady);
        FlashLED3(1);
        VL53L1_WaitMs(dev, 5);
    }
    dataReady = 0;
    
    // AH - Get sensor measurements
    status = VL53L1X_GetRangeStatus(dev, &rangeStatus);
    status = VL53L1X_GetDistance(dev, &distance);
    status = VL53L1X_GetSignalRate(dev, &signalRate);
    status = VL53L1X_GetAmbientRate(dev, &ambientRate);
    status = VL53L1X_GetSpadNb(dev, &spadNum);
    
    status = VL53L1X_ClearInterrupt(dev);
    return distance;
}

void TOF_ScanRotation(void) {
    // AH - Check measurement limit and state
    if(measurementNum >= MAX_MEASUREMENTS || tofState.state != TOF_SCAN_MODE) {
        return;
    }

    SetMeasurementLED();
    tofState.status = TOF_SCANNING;
    status = VL53L1X_StartRanging(dev);
    uint8_t i;
    for(i = 0; i < NUM_SAMPLES; i++) {
        if(tofState.state != TOF_SCAN_MODE) {
            break;
        }

        stepperMotor(0, 1); // AH - Rotate motor
        FlashLED3(1);

        uint16_t currentDistance = TOF_GetDistance();
        transmittedData[measurementNum][i] = currentDistance;

        sprintf(printf_buffer, "%u,%u,%u,%u,%u\r\n", 
                rangeStatus, currentDistance, signalRate, ambientRate, spadNum);
        UART_printf(printf_buffer);

        SysTick_Wait10ms(SCAN_DELAY/10);
    }

    stepperMotor(0, -1); // AH - Reverse motor
    VL53L1X_StopRanging(dev);
    tofState.status = TOF_SCANCOMPLETE;
    measurementNum++;
    ResetMeasurementLED();
}

void TOF_TransmitData(void) {
    // AH - Indicate transmission mode
    SetUARTLED();
    TOFParameters newState = TOF_GetState();
    newState.state = TOF_TRANSMIT_MODE;
    TOF_SetState(newState);

    // AH - Add "data starts" marker
    UART_printf("DATA STARTS\r\n");

    // AH - Send measurement parameters
    UART_OutChar(measurementNum);
    UART_OutChar(NUM_SAMPLES);

    // AH - Transmit all collected data
    uint8_t i,j;
    for(i = 0; i < measurementNum; i++) {
        for(j = 0; j < NUM_SAMPLES; j++) {
            uint16_t distance = transmittedData[i][j];
            // AH - Little Endian format
            UART_OutChar((uint8_t)(distance & 0xFF));
            UART_OutChar((uint8_t)((distance >> 8) & 0xFF));
        }
    }

    UART_printf("DATA ENDS\r\n");

    // AH - Reset scanner state
    tofState.state = TOF_SCAN_MODE;
    tofState.status = TOF_SCANCOMPLETE;
    measurementNum = 0;

    // AH - Reset LED
    SysTick_Wait10ms(100);
    ResetUARTLED();
    ToggleStatusLED();
}
TOFParameters TOF_GetState(void) {
    return tofState;
}

// AH - Set TOF state
void TOF_SetState(TOFParameters newState) {
    tofState = newState;
}

void SubMain(void) {
    // AH - Initialize sensor and peripherals
    I2C_Init();
    UART_Init();
    VL53L1X_XSHUT();
    TOF_Init();
    SetStatusLED();
    PortJ_Interrupt_Init();
}

