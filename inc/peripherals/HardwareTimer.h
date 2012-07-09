/**************************************************************************//**
 *
 * @file     HardwareTimer.h
 * @author	 David Alain <dnascimento@fitec.org.br>
 * @brief    File with function to control hardware timers.
 * @version  V1.0
 *
 ******************************************************************************/

#ifndef _HARDWARE_TIMER_H_
#define _HARDWARE_TIMER_H_

#include "core/Types.h"
#include "core/cmsis.h"


typedef enum {
	HARDWARE_TIMER_16_0,
	HARDWARE_TIMER_16_1,
	HARDWARE_TIMER_32_0,
	HARDWARE_TIMER_32_1,
}HardwareTimerNum;



/* MHZ_PRESCALE is a value to set the prescaler to in order to
   clock the timer at 1 MHz. Clock needs to be a multiple of 1 MHz or
   this will not work. */
//#define MHZ_PRESCALE    (SystemCoreClock/1000000)

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
void HardwareTimer_Init(HardwareTimerNum timerNum, uint32_t timerInterval);

void HardwareTimer_delay_uS(HardwareTimerNum timerNum, uint32_t delay_uS);

void HardwareTimer_setUserHandler(HardwareTimerNum timerNum, FunctionPointer ptr);



#endif //end of file
