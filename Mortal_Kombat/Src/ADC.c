#include "stm32l476xx.h"
#include "ADC.h"

// Set up the GPIO pins used for analog input (PA1)
void ADC_PIN_Init(void) {

	// Enable clock for GPIOA
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

	// Set GPIOA pins (ADC1_Channel_7 -> PA2: Joystick X-Axis, ADC1_Channel_8 -> PA3: Joystick Y Axis) to analog mode using MODER
	GPIOA->MODER &= ~(GPIO_MODER_MODE2 | GPIO_MODER_MODE3);

	GPIOA->MODER |= (3UL << GPIO_MODER_MODE2_Pos) | (3UL << GPIO_MODER_MODE3_Pos);

	// Enable analog switches for the ADC using ASCR
	GPIOA->ASCR |= (GPIO_ASCR_ASC2 | GPIO_ASCR_ASC3);

	// Enable internal pull-down for PA3 and PA2 (10)
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD2 | GPIO_PUPDR_PUPD3);
	GPIOA->PUPDR |= (GPIO_PUPDR_PUPD2_1 | GPIO_PUPDR_PUPD3_1);

}

// Configure shared ADC settings for all ADCs (like ADC1, ADC2) via\
   the common ADC register block
void ADC_Common_Config(void) {

	// Enable I/O analog switches voltage booster
	/*
	 * Important step when operating at low VDD (<= 2.4 V):
	 * enables a voltage booster for I/O analog switches (internal
	 * switches connecting to the ADC input to the pin)
	 *
	 * These switches can have increased resistance at low supply voltages,
	 * which can distort or delay the ADC reading.
	 * Enabling the booster reduces the on-resistance and esnures accurate
	 * ADC input voltage tracking
	 */
	SYSCFG->CFGR1 |= SYSCFG_CFGR1_BOOSTEN;

	// Enable conversion of internal channels by setting ADC_CCR_VREFEN in\
	   the ADC_123_COMMON->CCR register
	ADC123_COMMON->CCR |= ADC_CCR_VREFEN;

	// Configure the ADC prescaler to have the clock not divided (Which\
	   involves setting all bits to zero) ADC_CCR_PRESC in ADC123_COMMON->CCR
	ADC123_COMMON->CCR &= ~(ADC_CCR_PRESC);

	// Select synchronous clock mode (HCLK/1) (0b01) via ADC_CCR_CKMODE in\
	   the ADC123_COMMON->CCR register
	ADC123_COMMON->CCR |= ADC_CCR_CKMODE;

	// Configure all ADCs as independent by clearing ADC_CCR_DUAL in the\
	   ADC123_COMMON->CCR register
	ADC123_COMMON->CCR &= ~(ADC_CCR_DUAL);

}

// Main ADC initialization routine, sets everything so ADC is ready\
   to perform conversions
void ADC_Init(void) {

	// Enable the ADC clock bit RCC_AHB2ENR_ADCEN in RCC->AHB2ENR register
	RCC->AHB2ENR |= RCC_AHB2ENR_ADCEN;

	// Disable ADC1 by clearing ADC_CR_ADEN bits in ADC1->CR\
	   so we can modify settings
	ADC1->CR &= ~(ADC_CR_ADEN);

	// Call functions defined earlier
	ADC_PIN_Init();
	ADC_Common_Config();
	ADC_Wakeup();

	// Configure the ADC to have 12-bit result inside the result register by\
	   clearing ADC_CFGR_ALIGN in ADC1->CFGR register
	ADC1->CFGR &= ~(ADC_CFGR_ALIGN);

	// Select 2 conversions in the regular channel conversion sequence\
	   by clearing ADC_SQR1_L in ADC1->SQR1 and setting the second bit of the SQR1 register
	ADC1->SQR1 &= ~(ADC_SQR1_L);
	ADC1->SQR1 |= (1UL << ADC_SQR1_L_Pos);

	// Map the ADC1 sequencing
	// First slow (SQ1): Channel 7 (PA2)
	// Second slot (SQ2): Channel 8 (PA3)
	ADC1->SQR1 &= ~(ADC_SQR1_SQ1 | ADC_SQR1_SQ2);
	ADC1->SQR1 |= ((7 << ADC_SQR1_SQ1_Pos) | (8 << ADC_SQR1_SQ2_Pos));

	// Set to single-ended by clearing ADC_DIFSEL_DIFSEL_6 in\
	   ADC1->DIFSL register for channels 5 and 6
	ADC1->DIFSEL &= ~(ADC_DIFSEL_DIFSEL_7 | ADC_DIFSEL_DIFSEL_8);

	// Select the ADC sampling time. Set it to "247.5 ADC clock cycle"
	ADC1->SMPR1 |= (6 <<ADC_SMPR1_SMP7_Pos) | (6 << ADC_SMPR1_SMP8_Pos);

	// Find this line around line 103 in your ADC.c:
		ADC1->CFGR &= ~(ADC_CFGR_EXTEN | ADC_CFGR_EXTSEL | ADC_CFGR_CONT | ADC_CFGR_DISCEN);

		// ADD THESE LINES RIGHT BELOW IT:
		ADC1->CFGR |= ADC_CFGR_DISCEN;       // Enable discontinuous mode
		ADC1->CFGR &= ~(ADC_CFGR_DISCNUM);   // 000 means 1 conversion per subgroup trigger

	ADC1->CR |= ADC_CR_ADEN;
	while(!(ADC1->ISR & ADC_ISR_ADRDY));


}
/*
ADC Wakeup: By default, the ADC is in deep-power-down mode, where its supply is
internally switched off to reduce leakage currents.
*/
void ADC_Wakeup(void) {
	int wait_time;
	// Exit deep power down mode if still in that state
	// DEEPPWD = 0: ADC is not in deep power down
	// DEEPPWD = 1: ADC is in deep-power-down (default reset state)
	if ((ADC1->CR & ADC_CR_DEEPPWD) == ADC_CR_DEEPPWD)
	ADC1->CR &= ~ADC_CR_DEEPPWD;
	// Enable the ADC internal voltage regulator
	// Before performing any operations, such as launching a calibration or enabling the ADC,
	// the ADC voltage regulator must first be enabled, and the software must wait for the
	// regulator start-up time.
	ADC1->CR |= ADC_CR_ADVREGEN;
	// Wait for the ADC voltage regulator start-up time
	// The software must wait for the startup time of the ADC voltage regulator
	// (T_ADCVREG_STUP, i.e., 20 μs) before launching a calibration or enabling the ADC.
	wait_time = 20 * (72000000 / 1000000);

	while (wait_time != 0) {
	    wait_time--;
	}

}

/*
void read_Joystick(uint32_t *x_axis, uint32_t *y_axis) {
	// Clear any residual flags by writing a 1 to them
	ADC1->ISR |= (ADC_ISR_EOC | ADC_ISR_EOS);

	// --- Trigger 1: Converts SQ1 (Channel 7 -> X-Axis) ---
	ADC1->CR |= ADC_CR_ADSTART;
	while (!(ADC1->ISR & ADC_ISR_EOC));
	*x_axis = ADC1->DR; // Reading DR clears EOC

	// --- Trigger 2: Converts SQ2 (Channel 8 -> Y-Axis) ---
	ADC1->CR |= ADC_CR_ADSTART;
	while (!(ADC1->ISR & ADC_ISR_EOC));
	*y_axis = ADC1->DR; // Reading DR clears EOC

	// Wait for the entire sequence block to finish and clear flag
	while (!(ADC1->ISR & ADC_ISR_EOS));
	ADC1->ISR |= ADC_ISR_EOS;
}

*/
