void SetMeasurementLED(void);
void ResetMeasurementLED(void);
void SetUARTLED(void);
void ResetUARTLED(void);
void SetStatusLED(void);
void ResetStatusLED(void);
void resetPLLLED(void);
void ToggleMeasurementLED(void);
void ToggleUARTLED(void);
void ToggleStatusLED(void);
void TogglePLLLED(void);

void FlashLED1(int count);
void FlashLED2(int count);
void FlashLED3(int count);
void FlashLED4(int count);

void FlashAllLEDs(void);

void FlashI2CError(int count);
void FlashI2CTx(void);
void FlashI2CRx(void);

void onboardLEDs_Init(void);
