/*
 * open-lpc - ARM Cortex-M library
 * Authors:
 *    * Cristóvão Zuppardo Rufino <cristovaozr@gmail.com>
 *    * David Alain do Nascimento <davidalain89@gmail.com>
 * Version 1.0
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

//After implement to LPC17XX, this conditional target arch will must removed from here.
#if defined (TARGET_LPC111X) || defined (TARGET_LPC13XX)

#include "peripherals/PWM.h"

void PWM_init(HardwareTimerNum timerNum, uint32_t period, uint8_t match_enable, uint8_t cap_enabled)
{

	HardwareTimer_disable(timerNum);
	LPC_TMR_TypeDef* LPC_TMR = HardwareTimer_getLPC_TMR(timerNum);

	if (timerNum == HARDWARE_TIMER_32_0)
	{

		/* Some of the I/O pins need to be clearfully planned if
			    you use below module because JTAG and TIMER CAP/MAT pins are muxed. */
		LPC_SYSCON->SYSAHBCLKCTRL |= (1<<9);

		/* Setup the external match register */
		LPC_TMR->EMR = (1<<EMC3)|(2<<EMC2)|(1<<EMC1)|(1<<EMC0)|(1<<3)|(match_enable);

		/* Setup the outputs */
		/* If match0 is enabled, set the output */
		if (match_enable & 0x01)
		{
			//	 		LPC_IOCON->PIO1_6           &= ~0x07;
			//		  	LPC_IOCON->PIO1_6           |= 0x02;		/* Timer0_32 MAT0 */
		}
		/* If match1 is enabled, set the output */
		if (match_enable & 0x02)
		{
			LPC_IOCON-> PIO1_7           &= ~0x07;
			LPC_IOCON->PIO1_7           |= 0x02;		/* Timer0_32 MAT1 */
		}
		/* If match2 is enabled, set the output */
		if (match_enable & 0x04)
		{
			LPC_IOCON->PIO0_1           &= ~0x07;
			LPC_IOCON->PIO0_1           |= 0x02;		/* Timer0_32 MAT2 */
		}
		/* If match3 is enabled, set the output */
		if (match_enable & 0x08)
		{
			LPC_IOCON->R_PIO0_11 &= ~0x07;
			LPC_IOCON->R_PIO0_11 |= 0x03;		/* Timer0_32 MAT3 */
		}

		/* Enable the selected PWMs and enable Match3 */
		LPC_TMR->PWMC = (1<<3)|(match_enable);

		/* Setup the match registers */
		/* set the period value to a global variable */
		LPC_TMR->MR3 	= period;
		LPC_TMR->MR0	= period;	///2;
		LPC_TMR->MR1	= period/2;
		LPC_TMR->MR2	= period/2;

		LPC_TMR->MCR = 1<<10;				/* Reset on MR3 */

		/* Enable the TIMER0 Interrupt */
		NVIC_EnableIRQ(TIMER_32_0_IRQn);

	}
	else if (timerNum == HARDWARE_TIMER_32_1)
	{

		/* Some of the I/O pins need to be clearfully planned if
			    you use below module because JTAG and TIMER CAP/MAT pins are muxed. */
		LPC_SYSCON->SYSAHBCLKCTRL |= (1<<10);

		/* Setup the external match register */
		LPC_TMR->EMR = (1<<EMC3)|(1<<EMC2)|(2<<EMC1)|(1<<EMC0)|(1<<3)|(match_enable);

		/* Setup the outputs */
		/* If match0 is enabled, set the output */
		if (match_enable & 0x01)
		{
			LPC_IOCON->R_PIO1_1  &= ~0x07;
			LPC_IOCON->R_PIO1_1  |= 0x03;		/* Timer1_32 MAT0 */
		}
		/* If match1 is enabled, set the output */
		if (match_enable & 0x02)
		{
			LPC_IOCON->R_PIO1_2 &= ~0x07;
			LPC_IOCON->R_PIO1_2 |= 0x03;		/* Timer1_32 MAT1 */
		}
		/* If match2 is enabled, set the output */
		if (match_enable & 0x04)
		{
			LPC_IOCON->SWDIO_PIO1_3   &= ~0x07;
			LPC_IOCON->SWDIO_PIO1_3   |= 0x03;		/* Timer1_32 MAT2 */
		}
		/* If match3 is enabled, set the output */
		if (match_enable & 0x08)
		{
			LPC_IOCON->PIO1_4           &= ~0x07;
			LPC_IOCON->PIO1_4           |= 0x02;		/* Timer1_32 MAT3 */
		}

		//#ifdef __JTAG_DISABLED
		//	  PIO1_0_JTAG_TMS  &= ~0x07;	/*  Timer1_32 I/O config */
		//	  PIO1_0_JTAG_TMS  |= 0x03;		/* Timer1_32 CAP0 */
		//#endif

		/* Enable the selected PWMs and enable Match3 */
		LPC_TMR->PWMC = (1<<3)|(match_enable);

		/* Setup the match registers */
		/* set the period value to a global variable */
		LPC_TMR->MR3 	= period;
		LPC_TMR->MR0	= period/2;
		LPC_TMR->MR1	= period/2;
		LPC_TMR->MR2	= period/2;

		/* */
		LPC_TMR->MCR = 1<<10;				/* Reset on MR3 */

		/* Enable the TIMER0 Interrupt */
		NVIC_EnableIRQ(TIMER_32_1_IRQn);


	}else if (timerNum == HARDWARE_TIMER_16_0){

		/* Some of the I/O pins need to be clearfully planned if
				you use below module because JTAG and TIMER CAP/MAT pins are muxed. */
		LPC_SYSCON->SYSAHBCLKCTRL |= (1<<7);

		/* Setup the external match register */
		LPC_TMR->EMR = (1<<EMC3)|(1<<EMC2)|(1<<EMC1)|(1<<EMC0)|(1<<3)|(match_enable);

		/* Setup the outputs */
		/* If match0 is enabled, set the output */
		if (match_enable & 0x01)
		{
			LPC_IOCON->PIO0_8           &= ~0x07;
			LPC_IOCON->PIO0_8           |= 0x02;		/* Timer0_16 MAT0 			*/
		}
		/* If match1 is enabled, set the output */
		if (match_enable & 0x02)
		{
			LPC_IOCON->PIO0_9           &= ~0x07;
			LPC_IOCON->PIO0_9           |= 0x02;		/* Timer0_16 MAT1 			*/
		}
		/* If match2 is enabled, set the output */
		if (match_enable & 0x04)
		{
			LPC_IOCON->SWCLK_PIO0_10 &= ~0x07;
			LPC_IOCON->SWCLK_PIO0_10 |= 0x03;		/* Timer0_16 MAT2 */
		}

		//	  PIO0_2           &= ~0x07;	/* Timer0_16 I/O config */
		//	  PIO0_2           |= 0x02;		/* Timer0_16 CAP0 			*/

		/* Enable the selected PWMs and enable Match3 */
		LPC_TMR->PWMC = (1<<3)|(match_enable);

		/* Setup the match registers */
		/* set the period value to a global variable */
		LPC_TMR->MR3 = period;
		LPC_TMR->MR0 = period/2;
		LPC_TMR->MR1 = period/2;
		LPC_TMR->MR2 = period/2;

		/* Set the match control register */
		LPC_TMR->MCR = 1<<10;				/* Reset on MR3 */

		/* Enable the TIMER1 Interrupt */
		NVIC_EnableIRQ(TIMER_16_0_IRQn);

	}else if (timerNum == HARDWARE_TIMER_16_1){

		/* Some of the I/O pins need to be clearfully planned if
				you use below module because JTAG and TIMER CAP/MAT pins are muxed. */
		LPC_SYSCON->SYSAHBCLKCTRL |= (1<<8);

		/* Setup the external match register */
		LPC_TMR->EMR = (1<<EMC3)|(1<<EMC2)|(1<<EMC1)|(2<<EMC0)|(1<<3)|(match_enable);

		/* Setup the outputs */
		/* If match0 is enabled, set the output */
		if (match_enable & 0x01)
		{
			LPC_IOCON->PIO1_9           &= ~0x07;
			LPC_IOCON->PIO1_9           |= 0x01;		/* Timer1_16 MAT0 */
		}
		/* If match1 is enabled, set the output */
		if (match_enable & 0x02)
		{
			LPC_IOCON->PIO1_10          &= ~0x07;
			LPC_IOCON->PIO1_10          |= 0x02;		/* Timer1_16 MAT1 */
		}

		/* Enable the selected PWMs and enable Match3 */
		LPC_TMR->PWMC = (1<<3)|(match_enable);

		/* Setup the match registers */
		/* set the period value to a global variable */
		LPC_TMR->MR3 	= period;
		LPC_TMR->MR0	= period/2;
		LPC_TMR->MR1	= period/2;
		LPC_TMR->MR2	= period/2;

		/* Set match control register */
		LPC_TMR->MCR = 1<<10;// | 1<<9;				/* Reset on MR3 */

		if (cap_enabled)
		{
			LPC_IOCON->PIO1_8 &= ~0x07;						/*  Timer1_16 I/O config */
			LPC_IOCON->PIO1_8 |= 0x01 | (2<<3);				/* Timer1_16 CAP0 */
			LPC_GPIO1->DIR &= ~(1<<8);
			LPC_TMR->IR = 0xF;							/* clear interrupt flag */

			/* Set the capture control register */
			LPC_TMR->CCR = 7;

		}
		/* Enable the TIMER1 Interrupt */
		NVIC_EnableIRQ(TIMER_16_1_IRQn);

	}


}


void PWM_setMatch (HardwareTimerNum timerNum, uint8_t matchNum, uint32_t value)
{

	LPC_TMR_TypeDef* LPC_TMR = HardwareTimer_getLPC_TMR(timerNum);

	switch(matchNum){
	case 0:
		LPC_TMR->MR0 = value;
		break;
	case 1:
		LPC_TMR->MR1 = value;
		break;
	case 2:
		LPC_TMR->MR2 = value;
		break;
	case 3:
		LPC_TMR->MR3 = value;
		break;
	}


}

#else

#error "Falta implmentar para o LPC17XX"

#endif //if defined target archteture

