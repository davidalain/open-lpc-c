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

#ifndef __HARDWARE_TIMER_H
#define __HARDWARE_TIMER_H

#include "core/Types.h"
#include "core/cmsis.h"


typedef enum {
	HARDWARE_TIMER_32_0 = 0,
	HARDWARE_TIMER_32_1,
	HARDWARE_TIMER_16_0,
	HARDWARE_TIMER_16_1,
}HardwareTimerNum;



/* MHZ_PRESCALE is a value to set the prescaler to in order to
   clock the timer at 1 MHz. Clock needs to be a multiple of 1 MHz or
   this will not work. */
#define MHZ_PRESCALE    (SystemCoreClock/1000000)

/* TIME_INTERVALmS is a value to load the timer match register with
   to get a 1 mS delay */
#define TIME_INTERVALmS	1000

#define TIME_INTERVAL	(SystemCoreClock/100 - 1)
/* depending on the SystemFrequency and SystemAHBFrequency setting,
if SystemFrequency = 60Mhz, SystemAHBFrequency = 1/4 SystemAHBFrequency,
10mSec = 150.000-1 counts */

/* The test is either MAT_OUT or CAP_IN. Default is MAT_OUT. */
#define TIMER_MATCH		0

#define EMC0	4
#define EMC1	6
#define EMC2	8
#define EMC3	10

#define MATCH0	(1<<0)
#define MATCH1	(1<<1)
#define MATCH2	(1<<2)
#define MATCH3	(1<<3)


LPC_TMR_TypeDef* HardwareTimer_getLPC_TMR(HardwareTimerNum timerNum);

void HardwareTimer_default_handler(HardwareTimerNum timerNum);

void HardwareTimer_enable(HardwareTimerNum timerNum);
void HardwareTimer_disable(HardwareTimerNum timerId);
void HardwareTimer_reset(HardwareTimerNum timerNum);
void HardwareTimer_init(HardwareTimerNum timerNum, uint16_t timerInterval);

void HardwareTimer_delayMs(HardwareTimerNum timerNum, uint32_t delayInMs);



#endif //end of file
