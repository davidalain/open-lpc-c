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

#include "core/Types.h"
#include "peripherals/AnalogIn.h"

/******************************************************************************************
 *
 *				Global variables of this file
 *
 ******************************************************************************************/


#define ADC_CHANNELS	8

static uint32_t _clock;
static bool _useIRQ;
static uint32_t _readedValue[ADC_CHANNELS];

/******************************************************************************************
 *
 *				 	Function's Headers
 *
 ******************************************************************************************/


#if defined (TARGET_LPC111X) || defined (TARGET_LPC13XX)
volatile uint32_t* AnalogIn_getLPC_IOCON_PIO(PinName pin);
#elif defined (TARGET_LPC17XX)
uint32_t AnalogIn_readLPC_ADC_ADDRn(uint8_t channel);
#endif

uint8_t AnalogIn_getChannelNum(PinName pin);
unsigned long AnalogIn_readLPC_ADC_Value(PinName pin);


/******************************************************************************************
 *
 *				 	AnalogIn functions
 *
 ******************************************************************************************/


void AnalogIn_Init(PinName pin, bool useIRQ)
{
	_useIRQ = useIRQ;

#if defined (TARGET_LPC111X) || defined (TARGET_LPC13XX)
	/* Disable Power down bit to the ADC block. */
	LPC_SYSCON->PDRUNCFG &= ~(0x1<<4);

	/* Enable AHB clock to the ADC. */
	LPC_SYSCON->SYSAHBCLKCTRL |= (1<<13);

	/* Unlike some other pins, for ADC test, all the pins need
	  to set to analog mode. Bit 7 needs to be cleared according
	  to design team. */

	volatile uint32_t* lpc_iocon_pioReg = AnalogIn_getLPC_IOCON_PIO(pin);

	(*lpc_iocon_pioReg) &= ~0x8F;
	(*lpc_iocon_pioReg) |= 0x01;

#elif defined (TARGET_LPC17XX)

	/* Enable CLOCK into ADC controller */
	LPC_SC->PCONP |= (1 << 12);

	/* all the related pins are set to ADC inputs, AD0.0~7 */
	LPC_PINCON->PINSEL0 |= 0x0F000000;	/* P0.12~13, A0.6~7, function 11 */
	LPC_PINCON->PINSEL1 &= ~0x003FC000;	/* P0.23~26, A0.0~3, function 01 */
	LPC_PINCON->PINSEL1 |= 0x00154000;
	LPC_PINCON->PINSEL3 |= 0xF0000000;	/* P1.30~31, A0.4~5, function 11 */

#endif

	AnalogIn_setClock(AnalogIn_DEFAULT_CLOCK); //Default clock

	if(_useIRQ){

		NVIC_EnableIRQ(ADC_IRQn);

#if defined (TARGET_LPC111X) || defined (TARGET_LPC13XX)
		LPC_ADC->INTEN = 0x1FF;		/* Enable all ADC interrupts */
#elif defined (TARGET_LPC17XX)
		LPC_ADC->ADINTEN = 0x1FF;	/* Enable all ADC interrupts */
#endif

	}else{

		NVIC_DisableIRQ(ADC_IRQn);

#if defined (TARGET_LPC111X) || defined (TARGET_LPC13XX)
		LPC_ADC->INTEN = 0x0;		/* Disable all ADC interrupts */
#elif defined (TARGET_LPC17XX)
		LPC_ADC->ADINTEN = 0x0;		/* Disable all ADC interrupts */
#endif

	}

}


void AnalogIn_setClock(uint32_t clock){
	_clock = clock;
#if defined (TARGET_LPC111X) || defined (TARGET_LPC13XX)
	LPC_ADC->CR = ((SystemCoreClock/LPC_SYSCON->SYSAHBCLKDIV)/_clock - 1)<<8;

#elif defined (TARGET_LPC17XX)

	/* By default, the PCLKSELx value is zero, thus, the PCLK for
		  all the peripherals is 1/4 of the SystemFrequency. */
	/* Bit 6~7 is for UART0 */
	pclkdiv = (LPC_SC->PCLKSEL0 >> 6) & 0x03;
	switch ( pclkdiv )
	{
	case 0x00:
	default:
		pclk = SystemCoreClock/4;
		break;
	case 0x01:
		pclk = SystemCoreClock;
		break;
	case 0x02:
		pclk = SystemCoreClock/2;
		break;
	case 0x03:
		pclk = SystemCoreClock/8;
		break;
	}

	LPC_ADC->ADCR = ( 0x01 << 0 ) | 	/* SEL=1,select channel 0 on ADC0 */
			( ( pclk  / _clock - 1 ) << 8 ) |  /* CLKDIV = Fpclk / _clock - 1 */
			( 0 << 16 ) | 		/* BURST = 0, no BURST, software controlled */
			( 0 << 17 ) |  		/* CLKS = 0, 11 clocks/10 bits */
			( 1 << 21 ) |  		/* PDN = 1, normal operation */
			( 0 << 24 ) |  		/* START = 0 A/D conversion stops */
			( 0 << 27 );		/* EDGE = 0 (CAP/MAT singal falling,trigger A/D conversion) */

#endif
}

int32_t AnalogIn_read(PinName pin){

	int32_t regVal, data;
	uint8_t channel = AnalogIn_getChannelNum(pin);

	//If IRQ is not to be used then the ADC value is will be get by polling
	if(!_useIRQ){

#if defined (TARGET_LPC111X) || defined (TARGET_LPC13XX)
		LPC_ADC->CR &= 0xFFFFFF00;
		LPC_ADC->CR |= (1 << 24) | (1 << channel);
		/* switch channel,start A/D convert */
#elif defined (TARGET_LPC17XX)
		LPC_ADC->ADCR &= 0xFFFFFF00;
		LPC_ADC->ADCR |= (1 << 24) | (1 << channel);
		/* switch channel,start A/D convert */
#endif

		while ( 1 )			/* wait until end of A/D convert */
		{
			regVal = AnalogIn_readLPC_ADC_Value(pin);
			/* read result of A/D conversion */
			if ( regVal & AnalogIn_CHANNEL_DONE_MASK )
			{
				break;
			}
		}

#if defined (TARGET_LPC111X) || defined (TARGET_LPC13XX)
		LPC_ADC->CR &= 0xF8FFFFFF;	/* stop ADC now */
#elif defined (TARGET_LPC17XX)
		LPC_ADC->ADCR &= 0xF8FFFFFF;	/* stop ADC now */
#endif

		if ( regVal & AnalogIn_CHANNEL_OVERRUN_MASK )	/* save data when it's not overrun, otherwise, return -1 */
		{
			return ( -1 );
		}

#if defined (TARGET_LPC111X) || defined (TARGET_LPC13XX)
		data = ( regVal >> 6 ) & 0x3FF;	// data value of ADC is in 15:6 bits (10 bits precision)
#elif defined (TARGET_LPC17XX)
		data = ( regVal >> 4 ) & 0xFFF; // data value of ADC is in 15:4 bits (12 bits precision)
#endif

		return ( data );	/* return A/D conversion value */
	}

	//ADC is using IRQ and the last converted data is stored in _readedValue;
	return _readedValue[channel];
}


void AnalogIn_default_handler()
{

	uint32_t regVal;
	uint32_t statReg;
	uint8_t channel;

#if defined (TARGET_LPC111X) || defined (TARGET_LPC13XX)
	statReg = LPC_ADC->STAT;		/* Read ADC will clear the interrupt */
#elif defined (TARGET_LPC17XX)
	statReg = LPC_ADC->ADSTAT;		/* Read ADC will clear the interrupt */
#endif

	for(channel = 0; channel < 8; channel++)
	{

		if ( statReg & AnalogIn_STAT_OVERRUN_MASK )	/* check OVERRUN error first */
		{
			regVal = ((statReg & AnalogIn_STAT_OVERRUN_MASK) & (0x1 << (channel + 0x08))); //Check if this ADC channel overrun bit is set
			/* if overrun, just read ADDR to clear */
			/* regVal variable has been reused. */
			if ( regVal )
			{
#if defined (TARGET_LPC111X) || defined (TARGET_LPC13XX)
				regVal = LPC_ADC->DR[channel];
#elif defined (TARGET_LPC17XX)
				regVal = AnalogIn_readLPC_ADC_ADDRn(channel);
#endif
			}

#if defined (TARGET_LPC111X) || defined (TARGET_LPC13XX)
			LPC_ADC->CR &= 0xF8FFFFFF;	/* stop ADC now */
#elif defined (TARGET_LPC17XX)
			LPC_ADC->ADCR &= 0xF8FFFFFF;	/* stop ADC now */
#endif

			return;
		}//if ( OVERRUN )

		if ( statReg & AnalogIn_STAT_ADINT_MASK )
		{
			if ( (statReg & 0xFF) & (0x1 << channel) )
			{

#if defined (TARGET_LPC111X) || defined (TARGET_LPC13XX)
				_readedValue[channel] = ( LPC_ADC->DR[channel] >> 6 ) & 0x3FF;
#elif defined (TARGET_LPC17XX)
				_readedValue[channel] = ( AnalogIn_readLPC_ADC_ADDRn(channel) >> 4 ) & 0xFFF;
#endif
			}


#if defined (TARGET_LPC111X) || defined (TARGET_LPC13XX)
			LPC_ADC->CR &= 0xF8FFFFFF;	/* stop ADC now */
#elif defined (TARGET_LPC17XX)
			LPC_ADC->ADCR &= 0xF8FFFFFF;	/* stop ADC now */
#endif

		}// if ( ADINT )

	}// for

	return;
}


unsigned long AnalogIn_readLPC_ADC_Value(PinName pin){

	return *(unsigned long *)
			(LPC_ADC_BASE + AnalogIn_ADDR_OFFSET + AnalogIn_ADDR_INDEX * AnalogIn_getChannelNum(pin));

}

uint8_t AnalogIn_getChannelNum(PinName pin){

	uint16_t channel = 0;

	switch(pin){
	case AD0:	channel = 0;	break;
	case AD1:	channel = 1;	break;
	case AD2:	channel = 2;	break;
	case AD3:	channel = 3;	break;
	case AD4:	channel = 4;	break;
	case AD5:	channel = 5;	break;
	case AD6:	channel = 6;	break;
	case AD7:	channel = 7;	break;
	default:
		//error("The pin P%1d_%2d have not a Analog-to-Digital function",_portNum,_mask);
		break;
	}

	return channel;
}

#if defined (TARGET_LPC111X) || defined (TARGET_LPC13XX)

volatile uint32_t* AnalogIn_getLPC_IOCON_PIO(PinName pin){

	uint8_t channel = AnalogIn_getChannelNum(pin);

	volatile uint32_t* lpc_iocon_pio[8] = {
			&(LPC_IOCON->R_PIO0_11),	/* ADC IN0 */
			&(LPC_IOCON->R_PIO1_0), 	/* ADC IN1 */
			&(LPC_IOCON->R_PIO1_1),		/* ADC IN2 */
			&(LPC_IOCON->R_PIO1_2),		/* ADC IN3 */
			&(LPC_IOCON->SWDIO_PIO1_3),	/* ADC IN4 */
			&(LPC_IOCON->PIO1_4),		/* ADC IN5 */
			&(LPC_IOCON->PIO1_10),		/* ADC IN6 */
			&(LPC_IOCON->PIO1_11)		/* ADC IN7 */
	};

#ifndef __SWD_DISABLED
	if(channel == 4){
		//error("You are using pin P1_3 (AD4) for Analog-to-Digital function but the flag __SWD_DISABLED is not set");
	}
#endif

	return lpc_iocon_pio[channel];
}

#endif // TARGET_LPC111X or TARGET_LPC13XX


#if defined (TARGET_LPC17XX)

uint32_t AnalogIn_readLPC_ADC_ADDRn(uint8_t channel){

	uint32_t regVal = 0;
	uint32_t lpc_adc_addr[8] = {
			&(LPC_ADC->ADDR0),
			&(LPC_ADC->ADDR1),
			&(LPC_ADC->ADDR2),
			&(LPC_ADC->ADDR3),
			&(LPC_ADC->ADDR4),
			&(LPC_ADC->ADDR5),
			&(LPC_ADC->ADDR6),
			&(LPC_ADC->ADDR7)};


	regVal = *(lpc_adc_addr[channel]);
	return regVal;

}

#endif // TARGET_LPC17XX
