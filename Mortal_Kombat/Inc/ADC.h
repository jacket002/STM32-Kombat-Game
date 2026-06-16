#ifndef ADC_H
#define ADC_H

void ADC_PIN_Init(void);
void ADC_Common_Config(void);
void ADC_Init(void);
void ADC_Wakeup(void);
void read_Joystick(uint32_t *x_axis, uint32_t *y_axis);

#endif
