#include <TIM.h>
#include "stm32l476xx.h"

volatile uint32_t step = 0;
volatile uint32_t transitionStep = 0;
volatile uint32_t rowStep = 0;
volatile uint32_t rowIndex = 0;

void configureSpeak(void) {

	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN; // enable clock for port A

	// Set GPIO PB5 as output pin in MODER (01)
	GPIOA->MODER &= ~(GPIO_MODER_MODE10); // clear
	GPIOA->MODER |= (GPIO_MODER_MODE10_0); // set
	GPIOA->ODR &= ~(GPIO_ODR_OD10); // initialize output of PA10 as LOW

}

// 4th Octave notes
#define F4 229225  // 349  Hz
#define G4 204080  // 392 Hz
#define A4 181817  // 440 Hz
#define B4 161942  // 494 Hz

// 5th Octave notes
#define C5 152962  // 523 Hz
#define D5 136285  // 587 Hz
#define E5 121395  // 659 Hz
#define G5 102039  // 784 Hz
#define A5 90908  // 880 Hz

#define Gn 0 // Ghost note (sets ARR to 0 and turns off TIM2 output --> mutes buzzer)

// 6th Octave notes
#define C6 76407
#define E6 60651
#define Bb6 85835

void playSong(void) {

	if (transitionStep < 2) {

		// Mortal Kombat theme song intro arrangement
		static const int mortalKombat_Intro[32] = {
				A4, A4, C5, A4, D5, A4, E5, D5,
				C5, C5, E5, C5, G5, C5, E5, C5,
				G4, G4, B4, G4, C5, G4, D5, C5,
				F4, F4, A4, F4, C5, F4, C5, B4,
		};

		// Clear timer count to prevent ARR overflow
		TIM2->CNT = 0;

		// Set ARR value for the designated note frequency (scaled by 2)
		TIM2->ARR = mortalKombat_Intro[step] * 2;

		// Hard set 50% duty cycle each note
		TIM2->CCR1 = (mortalKombat_Intro[step]  * 2) / 2;

		// Increment step
		step += 1;

		// If step >= 32 (total notes in bar), set step to 0 and increment transition
		if (step >= 32) {
			step = 0;

			transitionStep += 1;
		}

		// When transitionStep increments to 2 (intro bar played twice), switch to the prechorus section
	} else if (transitionStep >= 2 && transitionStep < 4) {

		// Mortal Kombat theme song pre-chorus arrangement
				static const int mortalKombat_PreChorus[28] = {
						A5, A5, A5, A5, G5, C6,
						A5, A5, A5, A5, G5, E5,
						A5, A5, A5, A5, G5, C6,
						A5, A5, A5, A5, A5, Gn, A5, A5, Gn
				};

				if (rowIndex != 3) {
					if (rowStep > 3) {
						TIM3->ARR = 1250;
					} else {
						TIM3->ARR = 1749;
						TIM3->PSC = 7199;
					}

					TIM2->CNT = 0;

					TIM2->ARR = mortalKombat_PreChorus[step] * 2;

					TIM2->CCR1 = (mortalKombat_PreChorus[step]  * 2) / 2;

					step += 1;
					rowStep += 1;

					if (rowStep >= 6) {
						rowStep = 0;
						rowIndex += 1;
					}

				} else {

					if (rowStep == 2) {
						TIM3->ARR = 1299;
					} else if (rowStep > 2) {

						TIM3->ARR = 699;

						if (rowStep == 5 || rowStep == 8) {

							TIM2->CCER &= ~TIM_CCER_CC1E;
							TIM3->ARR = 599;

							if (rowStep == 8) {

								TIM3->ARR = 29;

							}

						}

					} else {

						TIM3->ARR = 1749;
						TIM3->PSC = 7199;
					}

					TIM2->CNT = 0;

					TIM2->ARR = mortalKombat_PreChorus[step] * 2;

					TIM2->CCR1 = (mortalKombat_PreChorus[step]  * 2) / 2;

					step += 1;
					rowStep += 1;

					if (rowStep >= 9) {
						rowStep = 0;
						rowIndex += 1;
					}

				}

				if (step >= 28) {
					step = 0;
					rowStep = 0;
					rowIndex = 0;

					transitionStep += 1;
				}

	} else if (transitionStep >= 4 && transitionStep < 6) {

		static const int mortalKombat_Chorus[64] = {
								A5, E6, Gn, A5, C6, Gn, A5, Bb6, Gn, A5, C6, Gn, A5, Bb6, G5, Gn,
								A5, E6, Gn, A5, C6, Gn, A5, Bb6, Gn, A5, C6, Gn, A5, Bb6, G5, Gn,
								A5, E6, Gn, A5, C6, Gn, A5, Bb6, Gn, A5, C6, Gn, A5, Bb6, G5, Gn,
								A5, E6, Gn, A5, C6, Gn, G5, G5, Gn, G5, A5, Gn, A5, Gn, Gn, Gn

						};

		if (rowIndex != 3) {
			TIM3->ARR = 625;

		} else {
			TIM3->ARR = 625;
		}

		TIM2->CNT = 0;
		uint32_t currentPitch = mortalKombat_Chorus[step];

		if (currentPitch == Gn) {
			TIM2->CCR1 = 0;
			TIM2->CCER &= ~TIM_CCER_CC1E;
		} else {
			TIM2->ARR = currentPitch * 2;
			TIM2->CCR1 = currentPitch;
			TIM2->CCER |= TIM_CCER_CC1E;
		}

		step += 1;
		rowStep += 1;

		if (rowIndex < 3) {
			if (rowStep >= 16) {
				rowStep = 0;
				rowIndex += 1;
			}
		} else {
			if (rowStep >= 16) {
				rowStep = 0;
				rowIndex += 1;
			}
		}

		if (step >= 64) {
			step = 0;
			rowStep = 0;
			rowIndex = 0;

			if (transitionStep >= 6) {
				transitionStep = 0;
			} else {
				transitionStep += 1;
			}
			}

	} else {
		transitionStep = 0;
		TIM3->ARR = 1249;

	}


}
