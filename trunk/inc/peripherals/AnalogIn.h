/**************************************************************************//**
 *
 * @file     ATCommandManager.h
 * @author	 David Alain <dnascimento@fitec.org.br>
 * @brief    Analog to Digital Converter Controller
 * @version  V1.0
 *
 ******************************************************************************/

#ifndef _ANALOG_IN_H_
#define _ANALOG_IN_H_

#include "core/PinNames.h"
#include "core/Types.h"

#define AnalogIn_STAT_DONE_MASK		0x000000FF
#define AnalogIn_STAT_OVERRUN_MASK	0x0000FF00
#define AnalogIn_STAT_ADINT_MASK	0x00010000

#define AnalogIn_DEFAULT_CLOCK		2000000 	/* set to 2Mhz */
#define AnalogIn_ADDR_OFFSET		0x10	 	/* The first ADC channel starts in 0x4001C000 (ADC address base) + 0x10 (offset)*/
#define AnalogIn_ADDR_INDEX			4 			/* Each ADC address channel has 4 bytes of offset */

#define AnalogIn_CHANNEL_DONE_MASK		0x80000000
#define AnalogIn_CHANNEL_OVERRUN_MASK	0x40000000
#define AnalogIn_CHANNEL_ADINT_MASK		0x00010000


void AnalogIn_Init(PinName pin, bool useIRQ);
int32_t AnalogIn_read(PinName pin);
void AnalogIn_default_handler ();

void AnalogIn_startConversion();
void AnalogIn_stopConversion();

void AnalogIn_enableConversion(PinName pin);
void AnalogIn_disableConversion(PinName pin);


void AnalogIn_storeInterrupFlags();
void AnalogIn_clearInterrupFlags();
void AnalogIn_setClock(uint32_t clock);
void AnalogIn_setUserHandler(PinName pin, FunctionPointer usrHandler);

#endif

