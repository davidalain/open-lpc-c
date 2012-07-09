/**************************************************************************//**
 *
 * @file     ATCommandManager.c
 * @author	 David Alain <dnascimento@fitec.org.br>
 * @brief    Analog to Digital Converter Controller
 * @version  V1.0
 *
 ******************************************************************************/

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
static bool _enabledADC[ADC_CHANNELS] = {false};
static uint32_t _readedValue[ADC_CHANNELS] = {0};

extern FunctionPointer _userHandlerPtr[NUMBER_IO_PINS];

/******************************************************************************************
 *
 *				 	Function's Headers
 *
 ******************************************************************************************/

#if defined (TARGET_LPC13XX) || defined (TARGET_LPC111X)
extern LPC_GPIO_TypeDef (* const LPC_GPIO[4]);
#elif defined (TARGET_LPC17XX)
extern LPC_GPIO_TypeDef (* const LPC_GPIO[5]);
#endif

#if defined (TARGET_LPC111X) || defined (TARGET_LPC13XX)
volatile uint32_t* AnalogIn_getLPC_IOCON_PIO(PinName pin);
#elif defined (TARGET_LPC17XX)
uint32_t AnalogIn_readLPC_ADC_ADDRn(uint8_t channel);
#endif

uint8_t AnalogIn_getChannelNum(PinName pin);
PinName AnalogIn_getPinName(uint8_t pin);
uint32_t AnalogIn_readLPC_ADC_Value(PinName pin);



/******************************************************************************************
 *
 *				 	AnalogIn functions
 *
 ******************************************************************************************/

/**
 * Initializes the Analog to Digital peripheral and configures this pin to analog input.
 *
 * @param pin PinName related to AD channel
 * @param useIRQ Defines if the AD reading is by interruption or blocking;
 *
 * When useIRQ is true, the start and stop control of conversions must be do by the user (using AnalogIn_startConversion and AnalogIn_stopConversion functions);
 * Using useIRQ equals to true the last conversation result is stored in a global variable an returned when getting.
 *
 * When useIRQ is false, the conversion is locking and return only the conversion is finished.
 *
 * @see PinName
 * @see bool
 */
void AnalogIn_Init(PinName pin, bool useIRQ)
{
	uint8_t channel = AnalogIn_getChannelNum(pin);
	_useIRQ = useIRQ;
	_enabledADC[channel] = true;

	NVIC_DisableIRQ(ADC_IRQn);

#if defined (TARGET_LPC111X) || defined (TARGET_LPC13XX)

	/* Disable Power down bit to the ADC block. */
	CLEAR_BIT(LPC_SYSCON->PDRUNCFG, 4);

	/* Enable AHB clock to the ADC. */
	SET_BIT(LPC_SYSCON->SYSAHBCLKCTRL, 13);
	/* enable clock for GPIO      */
	SET_BIT(LPC_SYSCON->SYSAHBCLKCTRL, 6);
	/* enable clock for IOCON     */
	SET_BIT(LPC_SYSCON->SYSAHBCLKCTRL, 16);

	volatile uint32_t* lpc_iocon_pioReg = AnalogIn_getLPC_IOCON_PIO(pin);

	(*lpc_iocon_pioReg) &= ~0x8F;	/* Clear last configuration */
	(*lpc_iocon_pioReg) |= (0x02);	/* Set to AD */

	uint32_t portNum = GET_PORT_NUM(pin);
	uint32_t mask = GET_MASK_NUM(pin);

#if defined (TARGET_LPC13XX) || defined (TARGET_LPC111X)
	LPC_GPIO[portNum]->DIR &= ~mask; 	/* configure GPIO as input */
#elif defined (TARGET_LPC17XX)
	LPC_GPIO[portNum]->FIODIR &= ~mask;	/* configure GPIO as input */
#endif

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
	//LPC_ADC->CR |= (23 << 8);

	if(_useIRQ){

		NVIC_EnableIRQ(ADC_IRQn);

		uint8_t channel = AnalogIn_getChannelNum(pin);

#if defined (TARGET_LPC111X) || defined (TARGET_LPC13XX)
		LPC_ADC->INTEN |= (1 << 8) | (0x1 << channel);		/* Enable Global ADC interrupts and for this channel too */
#elif defined (TARGET_LPC17XX)
		LPC_ADC->ADINTEN |= 0x1 << channel;	/* Enable ADC interrupts for this channel*/
#endif

		LPC_ADC->CR |= 1 << channel;

		//AnalogIn_startConversion();
	}else{

		NVIC_DisableIRQ(ADC_IRQn);

#if defined (TARGET_LPC111X) || defined (TARGET_LPC13XX)
		LPC_ADC->INTEN = 0x00;		/* Disable all ADC interrupts */
#elif defined (TARGET_LPC17XX)
		LPC_ADC->ADINTEN = 0x00;		/* Disable all ADC interrupts */
#endif

	}

}


/**
 * Sets the user's function that will called when a end conversion interrupt is generated.
 *
 * @param pin PinName related to AD channel
 * @param usrHandler user function
 */
void AnalogIn_setUserHandler(PinName pin, FunctionPointer usrHandler){
	uint8_t pinIndex = GET_PIN_INDEX(pin);
	_userHandlerPtr[pinIndex] = usrHandler;
}


/**
 * Sets the clock of AD peripheral.
 *
 * @param clock New clock.
 *
 * Verify in user manual of your device, the minimum and maximum values for AD clock.
 */
void AnalogIn_setClock(uint32_t clock){
	_clock = clock;
#if defined (TARGET_LPC111X) || defined (TARGET_LPC13XX)

	LPC_ADC->CR &= 0xFFFF00FF;
	LPC_ADC->CR |= ((((SystemCoreClock/LPC_SYSCON->SYSAHBCLKDIV)/_clock) - 1) & 0xFF) << 8;


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

/**
 * Enables conversion of the AD channel related to pin.
 * Note: this function not run the conversion, only enables the channel to be converted.
 *
 * @param pin AD channel.
 *
 * @see AnalogIn_startConversion
 * @see AnalogIn_stopConversion
 */
void AnalogIn_enableConversion(PinName pin){
	uint8_t channel = AnalogIn_getChannelNum(pin);
#if defined (TARGET_LPC111X) || defined (TARGET_LPC13XX)
	LPC_ADC->CR |= (0x1 << channel);
#elif defined (TARGET_LPC17XX)

#endif
}

/**
 * Disables conversion of the AD channel related to pin.
 * Note: this function not stop the conversion, only disables the channel to be converted.
 *
 * @param pin AD channel.
 *
 * @see AnalogIn_startConversion
 * @see AnalogIn_stopConversion
 */
void AnalogIn_disableConversion(PinName pin){
	uint8_t channel = AnalogIn_getChannelNum(pin);
#if defined (TARGET_LPC111X) || defined (TARGET_LPC13XX)
	LPC_ADC->CR &= ~(0x1 << channel);
#elif defined (TARGET_LPC17XX)

#endif
}

/**
 * Starts the conversion of enabled AD channels.
 */
void AnalogIn_startConversion(){
#if defined (TARGET_LPC111X) || defined (TARGET_LPC13XX)
	LPC_ADC->CR |= (0x1 << 24);
#elif defined (TARGET_LPC17XX)

#endif
}


/**
 * Stops the all conversions (all channels).
 */
void AnalogIn_stopConversion(){
#if defined (TARGET_LPC111X) || defined (TARGET_LPC13XX)
	LPC_ADC->CR &= 0xF8FFFFFF;	/* stop ADC now */
#elif defined (TARGET_LPC17XX)
	LPC_ADC->ADCR &= 0xF8FFFFFF;	/* stop ADC now */
#endif
}



/**
 * Reads the value in a AD channel specified by pin.
 *
 * @param pin PinName related to AD channel.
 * @return the conversation result of specified channel.
 *
 * @see AnalogIn_Init
 * Notes:
 * 	If this peripheral was initialized using interruption then the result is the last conversation done in this channel, because this is managed by interruption;
 * 	And if this peripheral was initialized using no interruption, then the channel is instantly read and the result value is returned only when the conversation finish.
 *
 *
 */
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

		AnalogIn_stopConversion();

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

/**
 * Handler to process interruption of end of conversion.
 * This function is call always a AD channel ends some conversion.
 */
void AnalogIn_default_handler()
{
	uint32_t regVal;
	uint32_t statReg;
	uint8_t channel;
	uint8_t flagChannel = 0;
	uint32_t result_GDR_Reg = 0;

	AnalogIn_stopConversion();
	LPC_ADC->INTEN &= ~(0x1 << 8);	/* Disable Global ADC interruption */

#if defined (TARGET_LPC111X) || defined (TARGET_LPC13XX)
	statReg = LPC_ADC->STAT;		/* Read ADC will clear the interrupt */
#elif defined (TARGET_LPC17XX)
	statReg = LPC_ADC->ADSTAT;		/* Read ADC will clear the interrupt */
#endif

	regVal = LPC_ADC->GDR;
	channel = (regVal >> 24) & 0x07;	/* Get channel referred to last conversion */
	result_GDR_Reg = (regVal >> 6) & 0x3FF;		/* Get the result value from ADC conversion */

	flagChannel = (statReg & (0x101 << channel));

	if(_enabledADC[channel] && flagChannel){

		if ( statReg & (0x100 << channel) )	/* check OVERRUN error first */
		{
			/* if overrun, just read ADDR to clear */
			/* regVal variable has been reused. */

#if defined (TARGET_LPC111X) || defined (TARGET_LPC13XX)
			regVal = LPC_ADC->DR[channel];
#elif defined (TARGET_LPC17XX)
			regVal = AnalogIn_readLPC_ADC_ADDRn(channel);
#endif

		}
		else if ( statReg & AnalogIn_STAT_ADINT_MASK )
		{
			if ( statReg & (0x1 << channel) )
			{

#if defined (TARGET_LPC111X) || defined (TARGET_LPC13XX)
				//_readedValue[channel] = result_GDR_Reg;
				_readedValue[channel] = ( LPC_ADC->DR[channel] >> 6 ) & 0x3FF;
#elif defined (TARGET_LPC17XX)
				_readedValue[channel] = ( AnalogIn_readLPC_ADC_ADDRn(channel) >> 4 ) & 0xFFF;
#endif

				uint32_t pinIndex = GET_PIN_INDEX(AnalogIn_getPinName(channel));
				if(_userHandlerPtr[pinIndex] != NULL){
					(_userHandlerPtr[pinIndex])();
				}
			}

		}// if ( ADINT )

	}// if enabled ADC

	LPC_ADC->INTEN |= (1 << 8); 	/* Enable Global ADC interruption */

	//AnalogIn_startConversion();
}


/**
 * Auxiliary function to read result of AD conversion directly on memory address.
 *
 * @param pin AD channel.
 * @return result of AD conversion.
 */
uint32_t AnalogIn_readLPC_ADC_Value(PinName pin){

	return *(uint32_t *)
			(LPC_ADC_BASE + AnalogIn_ADDR_OFFSET + AnalogIn_ADDR_INDEX * AnalogIn_getChannelNum(pin));

}

/**
 * Return the correspondent PinName of AD channel.
 *
 * @param channel AD channel.
 * @return PinName related to channel.
 */
PinName AnalogIn_getPinName(uint8_t channel){

	PinName pin = AD0;

	switch(channel){
	case 0: pin = AD0;	break;
	case 1: pin = AD1;	break;
	case 2: pin = AD2; 	break;
	case 3:	pin = AD3;	break;
	case 4: pin = AD4;	break;
	case 5: pin = AD5;	break;
	case 6: pin = AD6;	break;
	case 7: pin = AD7;	break;
	default:
		break;
	}

	return pin;
}

/**
 * Return the correspondent AD channel of PinName.
 *
 * @param pin PinName related to channel.
 * @return AD channel.
 */
uint8_t AnalogIn_getChannelNum(PinName pin){

	uint32_t channel = 0;

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
/**
 * Auxiliary function to return a pointer to address memory of IOCON pin related to PinName.
 */
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
/**
 * Auxiliary function to read result of AD conversion directly on memory address.
 *
 * @param pin AD channel.
 * @return result of AD conversion.
 */
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
