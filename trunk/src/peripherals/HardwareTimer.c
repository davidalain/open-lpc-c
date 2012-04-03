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

#include "peripherals/HardwareTimer.h"

#if defined (TARGET_LPC111X) || defined (TARGET_LPC13XX)

#define TIMER_NUM 4
static uint32_t _timer_counter[TIMER_NUM];
static uint32_t _timer_capture[TIMER_NUM];



LPC_TMR_TypeDef* HardwareTimer_getLPC_TMR(HardwareTimerNum timerNum)
{

	switch(timerNum){
	case HARDWARE_TIMER_16_0:
		return LPC_TMR16B0;
	case HARDWARE_TIMER_16_1:
		return LPC_TMR16B1;
	case HARDWARE_TIMER_32_0:
		return LPC_TMR32B0;
	case HARDWARE_TIMER_32_1:
		return LPC_TMR32B1;
	}

	return NULL;
}

void HardwareTimer_default_handler(HardwareTimerNum timerNum)
{
	LPC_TMR_TypeDef* LPC_TMR = HardwareTimer_getLPC_TMR(timerNum);

	if ( LPC_TMR->IR & 0x01 )
	{
		LPC_TMR->IR = 1;				/* clear interrupt flag */
		_timer_counter[timerNum]++;
	}
	if ( LPC_TMR->IR & (0x1<<4) )
	{
		LPC_TMR->IR = 0x1<<4;			/* clear interrupt flag */
		_timer_capture[timerNum]++;
	}

}

void HardwareTimer_delayMs(HardwareTimerNum timerNum, uint32_t delayInMs)
{

	LPC_TMR_TypeDef* LPC_TMR = HardwareTimer_getLPC_TMR(timerNum);

	/* setup timer for delay */
	LPC_TMR->TCR = 0x02;		/* reset timer */
	LPC_TMR->PR  = 0x00;		/* set prescaler to zero */
#if defined (TARGET_LPC111X)
	LPC_TMR->MR0 = delayInMs * ((SystemCoreClock/(LPC_TMR->PR+1)) / 1000);
#elif defined (TARGET_LPC13XX)
	LPC_TMR->MR0 = delayInMs * ((SystemFrequency/LPC_SYSCON->SYSAHBCLKDIV) / 1000);
#endif
	LPC_TMR->IR  = 0xff;		/* reset all interrrupts */
	LPC_TMR->MCR = 0x04;		/* stop timer on match */
	LPC_TMR->TCR = 0x01;		/* start timer */

	/* wait until delay time has elapsed */
	while (LPC_TMR->TCR & 0x01);

}




void HardwareTimer_enable(HardwareTimerNum timerNum)
{
	LPC_TMR_TypeDef* LPC_TMR = HardwareTimer_getLPC_TMR(timerNum);

	LPC_TMR->TCR = 1;
}


void HardwareTimer_disable(HardwareTimerNum timerNum)
{
	LPC_TMR_TypeDef* LPC_TMR = HardwareTimer_getLPC_TMR(timerNum);

	LPC_TMR->TCR = 0;
}


void HardwareTimer_reset(HardwareTimerNum timerNum)
{

	LPC_TMR_TypeDef* LPC_TMR = HardwareTimer_getLPC_TMR(timerNum);

	LPC_TMR->TCR |= 0x02;
}


void HardwareTimer_init(HardwareTimerNum timerNum, uint16_t timerInterval)
{
	LPC_TMR_TypeDef* LPC_TMR = HardwareTimer_getLPC_TMR(timerNum);

	if ( timerNum == HARDWARE_TIMER_32_0 )
	{
		/* Some of the I/O pins need to be carefully planned if
    you use below module because JTAG and TIMER CAP/MAT pins are muxed. */
		LPC_SYSCON->SYSAHBCLKCTRL |= (1<<9);
		LPC_IOCON->PIO1_5 &= ~0x07;	/*  Timer0_32 I/O config */
		LPC_IOCON->PIO1_5 |= 0x02;	/* Timer0_32 CAP0 */
		LPC_IOCON->PIO1_6 &= ~0x07;
		LPC_IOCON->PIO1_6 |= 0x02;	/* Timer0_32 MAT0 */
		LPC_IOCON->PIO1_7 &= ~0x07;
		LPC_IOCON->PIO1_7 |= 0x02;	/* Timer0_32 MAT1 */
		LPC_IOCON->PIO0_1 &= ~0x07;
		LPC_IOCON->PIO0_1 |= 0x02;	/* Timer0_32 MAT2 */
#ifdef __JTAG_DISABLED
		LPC_IOCON->JTAG_TDI_PIO0_11 &= ~0x07;
		LPC_IOCON->JTAG_TDI_PIO0_11 |= 0x03;	/* Timer0_32 MAT3 */
#endif

		_timer_counter[timerNum] = 0;
		_timer_capture[timerNum] = 0;

		LPC_TMR->MR0 = timerInterval;
#if TIMER_MATCH
		LPC_TMR->EMR &= ~(0xFF<<4);
		LPC_TMR->EMR |= ((0x3<<4)|(0x3<<6)|(0x3<<8)|(0x3<<10));	/* MR0/1/2/3 Toggle */
#else
		/* Capture 0 on rising edge, interrupt enable. */
		LPC_TMR->CCR = (0x1<<0)|(0x1<<2);
#endif
		LPC_TMR->MCR = 3;			/* Interrupt and Reset on MR0 */

		/* Enable the TIMER0 Interrupt */
		NVIC_EnableIRQ(TIMER_32_0_IRQn);

	}
	else if ( timerNum == HARDWARE_TIMER_32_1 )
	{
		/* Some of the I/O pins need to be clearfully planned if
    you use below module because JTAG and TIMER CAP/MAT pins are muxed. */
		LPC_SYSCON->SYSAHBCLKCTRL |= (1<<10);
#ifdef __JTAG_DISABLED
		LPC_IOCON->JTAG_TMS_PIO1_0  &= ~0x07;	/*  Timer1_32 I/O config */
		LPC_IOCON->JTAG_TMS_PIO1_0  |= 0x03;	/* Timer1_32 CAP0 */
		LPC_IOCON->JTAG_TDO_PIO1_1  &= ~0x07;
		LPC_IOCON->JTAG_TDO_PIO1_1  |= 0x03;	/* Timer1_32 MAT0 */
		LPC_IOCON->JTAG_nTRST_PIO1_2 &= ~0x07;
		LPC_IOCON->JTAG_nTRST_PIO1_2 |= 0x03;	/* Timer1_32 MAT1 */
		LPC_IOCON->ARM_SWDIO_PIO1_3  &= ~0x07;
		LPC_IOCON->ARM_SWDIO_PIO1_3  |= 0x03;	/* Timer1_32 MAT2 */
#endif
		LPC_IOCON->PIO1_4 &= ~0x07;
		LPC_IOCON->PIO1_4 |= 0x02;		/* Timer0_32 MAT3 */

		_timer_counter[timerNum] = 0;
		_timer_capture[timerNum] = 0;

		LPC_TMR->MR0 = timerInterval;
#if TIMER_MATCH
		LPC_TMR->EMR &= ~(0xFF<<4);
		LPC_TMR->EMR |= ((0x3<<4)|(0x3<<6)|(0x3<<8)|(0x3<<10));	/* MR0/1/2 Toggle */
#else
		/* Capture 0 on rising edge, interrupt enable. */
		LPC_TMR->CCR = (0x1<<0)|(0x1<<2);
#endif
		LPC_TMR->MCR = 3;			/* Interrupt and Reset on MR0 */

		/* Enable the TIMER1 Interrupt */
		NVIC_EnableIRQ(TIMER_32_1_IRQn);


	}else if(timerNum == HARDWARE_TIMER_16_0){

		/* Some of the I/O pins need to be clearfully planned if
		    you use below module because JTAG and TIMER CAP/MAT pins are muxed. */
		LPC_SYSCON->SYSAHBCLKCTRL |= (1<<7);
		LPC_IOCON->PIO0_2           &= ~0x07;	/*  Timer0_16 I/O config */
		LPC_IOCON->PIO0_2           |= 0x02;		/* Timer0_16 CAP0 */
		LPC_IOCON->PIO0_8           &= ~0x07;
		LPC_IOCON->PIO0_8           |= 0x02;		/* Timer0_16 MAT0 */
		LPC_IOCON->PIO0_9           &= ~0x07;
		LPC_IOCON->PIO0_9           |= 0x02;		/* Timer0_16 MAT1 */
#ifdef __JTAG_DISABLED
		LPC_IOCON->JTAG_TCK_PIO0_10 &= ~0x07;
		LPC_IOCON->JTAG_TCK_PIO0_10 |= 0x03;		/* Timer0_16 MAT2 */
#endif

		_timer_counter[timerNum] = 0;
		_timer_capture[timerNum] = 0;

		LPC_TMR->PR  = MHZ_PRESCALE; /* set prescaler to get 1 M counts/sec */
		LPC_TMR->MR0 = TIME_INTERVALmS * 10; /* Set up 10 mS interval */
#if TIMER_MATCH
		LPC_TMR->EMR &= ~(0xFF<<4);
		LPC_TMR->EMR |= ((0x3<<4)|(0x3<<6));
#else
		/* Capture 0 on rising edge, interrupt enable. */
		LPC_TMR->CCR = (0x1<<0)|(0x1<<2);
#endif
		LPC_TMR->MCR = 3;				/* Interrupt and Reset on MR0 and MR1 */

		/* Enable the TIMER0 Interrupt */
		NVIC_EnableIRQ(TIMER_16_0_IRQn);

	}else if(timerNum == HARDWARE_TIMER_16_1){

		/* Some of the I/O pins need to be clearfully planned if
	    you use below module because JTAG and TIMER CAP/MAT pins are muxed. */
		LPC_SYSCON->SYSAHBCLKCTRL |= (1<<8);
		LPC_IOCON->PIO1_8           &= ~0x07;	/*  Timer1_16 I/O config */
		LPC_IOCON->PIO1_8           |= 0x01;		/* Timer1_16 CAP0 */
		LPC_IOCON->PIO1_9           &= ~0x07;
		LPC_IOCON->PIO1_9           |= 0x01;		/* Timer1_16 MAT0 */
		LPC_IOCON->PIO1_10          &= ~0x07;
		LPC_IOCON->PIO1_10          |= 0x02;		/* Timer1_16 MAT1 */

		_timer_counter[timerNum] = 0;
		_timer_capture[timerNum] = 0;

		LPC_TMR->PR  = MHZ_PRESCALE; /* set prescaler to get 1 M counts/sec */
		LPC_TMR->MR0 = TIME_INTERVALmS * 10; /* Set up 10 mS interval */
#if TIMER_MATCH
		LPC_TMR->EMR &= ~(0xFF<<4);
		LPC_TMR->EMR |= ((0x3<<4)|(0x3<<6));
#else
		/* Capture 0 on rising edge, interrupt enable. */
		LPC_TMR16B1->CCR = (0x1<<0)|(0x1<<2);
#endif
		LPC_TMR16B1->MCR = 3;				/* Interrupt and Reset on MR0 and MR1 */

		/* Enable the TIMER1 Interrupt */
		NVIC_EnableIRQ(TIMER_16_1_IRQn);

	}
	return;
}

#else

#error "Falta implmentar para o LPC17XX"

#endif //if defined target archteture

/******************************************************************************
 **                            End Of File
 ******************************************************************************/
