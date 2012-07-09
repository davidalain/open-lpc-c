/**************************************************************************//**
 * @file     I2C.c
 * @author	 David Alain <dnascimento@fitec.org.br>
 * @brief    Manages the reads/writes on I2C ports.
 * @version  V1.0
 *
 ******************************************************************************/

#include "peripherals/I2C.h"
#include "core/Types.h"
#include <string.h>

static uint8_t _txBuffer[I2C_BUFFER_SIZE][I2C_NUM];
static uint8_t _rxBuffer[I2C_BUFFER_SIZE][I2C_NUM];
static uint32_t _rxBytesToRead[I2C_NUM];
static uint32_t _txBufferCurrentSize[I2C_NUM];
static uint32_t _rxIndex[I2C_NUM];
static uint32_t _txIndex[I2C_NUM];
static uint32_t _currentState[I2C_NUM];
static volatile uint32_t _timeout;

static FunctionPointer _userHandler[I2C_NUM] = {NULL};



uint32_t I2C_engine( I2CPortNum port );


/**
 * Initializes I2C peripheral given a I2CPortNum.
 *
 * @param port I2CPortNum.
 *
 * @see I2CPortNum.
 */
void I2C_Init(I2CPortNum port){

	/* It seems to be bit0 is for I2C, different from
	  UM. To be retested along with SSP reset. SSP and I2C
	  reset are overlapped, a known bug, for now, both SSP
	  and I2C use bit 0 for reset enable. Once the problem
	  is fixed, change to "#if 1". */
#if 1
	LPC_SYSCON->PRESETCTRL |= (0x1<<1);
#else
	LPC_SYSCON->PRESETCTRL |= (0x1<<0);
#endif
	LPC_SYSCON->SYSAHBCLKCTRL |= (1<<5);
	LPC_IOCON->PIO0_4 &= ~0x3F;	/*  I2C I/O config */
	LPC_IOCON->PIO0_4 |= 0x01;		/* I2C SCL */
	LPC_IOCON->PIO0_5 &= ~0x3F;
	LPC_IOCON->PIO0_5 |= 0x01;		/* I2C SDA */

	/*--- Clear flags ---*/
	LPC_I2C->CONCLR = I2C_CONCLR_AAC | I2C_CONCLR_SIC | I2C_CONCLR_STAC | I2C_CONCLR_I2ENC;

	/*--- Reset registers ---*/
#if FAST_MODE_PLUS
	LPC_IOCON->PIO0_4 |= (0x1<<9);
	LPC_IOCON->PIO0_5 |= (0x1<<9);
	LPC_I2C->SCLL   = I2SCLL_HS_SCLL;
	LPC_I2C->SCLH   = I2SCLH_HS_SCLH;
#else
	LPC_I2C->SCLL   = I2C_I2SCLL_SCLL;
	LPC_I2C->SCLH   = I2C_I2SCLH_SCLH;
#endif

	//	if ( mode == Mode_Slave )
	//	{
	//		LPC_I2C->ADR0 = DEFAULT_DEVICE_I2C_ADDRESS;
	//	}

	/* Enable the I2C Interrupt */
	NVIC_EnableIRQ(I2C_IRQn);

	LPC_I2C->CONSET = I2C_CONSET_I2EN;

	_currentState[port] = I2C_IDLE;
}



/**
 * Default handler that process interruptions request from I2C.
 * This function contains the state machine to control data flow on I2C.
 *
 * @param port A I2CPortNum.
 *
 * @see I2CPortNum.
 */
void I2C_default_handler(I2CPortNum port){

#if defined (TARGET_LPC111X) || defined (TARGET_LPC13XX)
	uint8_t statReg;

	_timeout = 0;

	/* this handler deals with master read and master write only */
	statReg = LPC_I2C->STAT;
	switch ( statReg )
	{
	case 0x08:			/* A Start condition is issued. */
		_txIndex[port] = 0;
		LPC_I2C->DAT = (_txBuffer[port])[(_txIndex[port])++];
		LPC_I2C->CONCLR = (I2C_CONCLR_SIC | I2C_CONCLR_STAC);
		//_currentState[port] = I2C_STARTED;
		break;

	case 0x10:			/* A repeated started is issued */
		_rxIndex[port] = 0;
		/* Send SLA with R bit set, */
		LPC_I2C->DAT = (_txBuffer[port])[(_txIndex[port])++];
		LPC_I2C->CONCLR = (I2C_CONCLR_SIC | I2C_CONCLR_STAC);
		//_currentState[port] = I2C_RESTARTED;
		break;

	case 0x18:			/* Regardless, it's a ACK */
		if ( _txBufferCurrentSize[port] == 1 )
		{
			LPC_I2C->CONSET = I2C_CONSET_STO;
			_currentState[port] = I2C_NO_DATA;
		}else{
			LPC_I2C->DAT = (_txBuffer[port])[(_txIndex[port])++];
		}
		LPC_I2C->CONCLR = I2C_CONCLR_SIC;
		break;

	case 0x28:	/* Data byte has been transmitted, regardless ACK or NACK */
		if ( _txIndex[port] < _txBufferCurrentSize[port] )
		{
			LPC_I2C->DAT = (_txBuffer[port])[(_txIndex[port])++]; /* this should be the last one */
		}
		else
		{
			if ( _rxBytesToRead[port] != 0 )
			{
				LPC_I2C->CONSET = I2C_CONSET_STA;	/* Set Repeated-start flag */
			}
			else
			{
				LPC_I2C->CONSET = I2C_CONSET_STO;      /* Set Stop flag */
				_currentState[port] = I2C_OK;
			}
		}
		LPC_I2C->CONCLR = I2C_CONCLR_SIC;
		break;
	case 0x30:
		_currentState[port] = I2C_NACK_ON_DATA;
		LPC_I2C->CONSET = I2C_CONSET_STO;      /* Set Stop flag */
		LPC_I2C->CONCLR = I2C_CONCLR_SIC;
		break;

	case 0x40:	/* Master Receive, SLA_R has been sent */
		if ( (_rxIndex[port] + 1) < _rxBytesToRead[port] )
		{
			/* Will go to State 0x50 */
			LPC_I2C->CONSET = I2C_CONSET_AA;	/* assert ACK after data is received */
		}
		else
		{
			/* Will go to State 0x58 */
			LPC_I2C->CONCLR = I2C_CONCLR_AAC;	/* assert NACK after data is received */
		}
		LPC_I2C->CONCLR = I2C_CONCLR_SIC;
		break;

	case 0x50:	/* Data byte has been received, regardless following ACK or NACK */
		(_rxBuffer[port])[(_rxIndex[port])++] = LPC_I2C->DAT;
		if ( (_rxIndex[port] + 1) < _rxBytesToRead[port] )
		{
			//_currentState[port] = I2C_DATA_ACK;
			LPC_I2C->CONSET = I2C_CONSET_AA;	/* assert ACK after data is received */
		}
		else
		{
			//_currentState[port] = I2C_DATA_NACK;
			LPC_I2C->CONCLR = I2C_CONCLR_AAC;	/* assert NACK on last byte */
		}
		LPC_I2C->CONCLR = I2C_CONCLR_SIC;
		break;

	case 0x58:
		(_rxBuffer[port])[(_rxIndex[port])++] = LPC_I2C->DAT;
		_currentState[port] = I2C_OK;
		LPC_I2C->CONSET = I2C_CONSET_STO;	/* Set Stop flag */
		LPC_I2C->CONCLR = I2C_CONCLR_SIC;	/* Clear SI flag */
		break;

	case 0x20:		/* regardless, it's a NACK */
	case 0x48:
		_currentState[port] = I2C_NACK_ON_ADDRESS;
		LPC_I2C->CONSET = I2C_CONSET_STO;
		LPC_I2C->CONCLR = I2C_CONCLR_SIC;
		break;

	case 0x38:		/* Arbitration lost, in this example, we don't
						deal with multiple master situation */
	default:
		LPC_I2C->CONCLR = I2C_CONCLR_SIC;
		break;
	}


#else
#error "Implementar pro LPC17XX"
#endif

	if(_userHandler[port] != NULL){
		(_userHandler[port])();
	}

}

/**
 * Sets the user function handler that will be called when a I2C interrupt occur.
 * Note: the user function handler only will be called if this it is not NULL.
 *
 * @param port A I2CPortNum.
 * @param ptr User function handler.
 *
 * @see I2CPortNum
 */
void I2C_setUserHandler(I2CPortNum port, FunctionPointer ptr){
	_userHandler[port] = ptr;
}

/**
 * Starts I2C transmissions.
 *
 * @param port A I2CPortNum
 *
 * @see I2CPortNum
 */
bool I2C_start(I2CPortNum port){

	uint32_t _timeout = 0;
	bool retVal = false;

#if defined (TARGET_LPC111X) || defined (TARGET_LPC13XX)

	/*--- Issue a start condition ---*/
	LPC_I2C->CONSET = I2C_CONSET_STA;	/* Set Start flag */

	/*--- Wait until START transmitted ---*/
	while( 1 )
	{
		if ( _currentState[port] == I2C_STARTED ) //this state is changed by interruption
		{
			retVal = true;
			break;
		}
		if ( _timeout >= I2C_MAX_TIMEOUT )
		{
			retVal = false;
			break;
		}
		_timeout++;
	}

#else
#error "Implementar pro LPC17XX"
#endif

	return( retVal );

}

/**
 * Stops I2C transmissions.
 *
 * @param port A I2CPortNum
 *
 * @see I2CPortNum
 */
void I2C_stop(I2CPortNum port){

	LPC_I2C->CONSET = I2C_CONSET_STO;  /* Set Stop flag */
	LPC_I2C->CONCLR = I2C_CONCLR_SIC;  /* Clear SI flag */

	/*--- Wait for STOP detected ---*/
	while( LPC_I2C->CONSET & I2C_CONSET_STO );

}

/**
 * Writes data on I2C port.
 *
 * @param port A I2CPortNum
 * @param deviceAddress Device address of slave I2C device (7 bits LSB)
 * @param data Data to be written
 * @param size Size of data.
 *
 * @see I2CPortNum
 */
void I2C_write(I2CPortNum port, uint8_t deviceAddress, uint8_t* data, uint32_t size){

	if(I2C_BUFFER_SIZE < size + 1){
		return;
	}

	deviceAddress <<= 1; // LSB bit is Read or Write flag

	memcpy(_txBuffer[port], &deviceAddress , 1);
	memcpy(&((_txBuffer[port])[1]), data, size);

	_txBufferCurrentSize[port] = size + 1;

	_rxBytesToRead[port] = 0; // Nothing to read

	I2C_engine(port);
}


/**
 * Reads data from I2C port.
 *
 * @param port A I2CPortNum.
 * @param deviceAddress Device address of slave I2C device (7 bits LSB).
 * @param rxBuffer Allocated buffer where data will be stored.
 * @param bufferSize Size of rxBuffer.
 * @param bytesToRead Number of bytes that must be read.
 *
 * @see I2CPortNum
 */
void I2C_read(I2CPortNum port, uint8_t deviceAddress, uint8_t* txBuffer, uint32_t bytesToWrite, uint8_t* allocatedRxBuffer, uint32_t bytesToRead){

	//Address write to
	_txBuffer[port][0] = (deviceAddress << 1);
	memcpy(&((_txBuffer[port])[1]), txBuffer, bytesToWrite);

	//Address read from
	_txBuffer[port][bytesToWrite + 1] = (deviceAddress << 1) | 1;

	_txBufferCurrentSize[port] = bytesToWrite + 1; // addressToWrite + txBuffer
	_rxBytesToRead[port] = bytesToRead;

	I2C_engine(port);

	memcpy(allocatedRxBuffer, _rxBuffer[port], bytesToRead);
}


/**
 * Auxiliary function that start the communication (read or write).
 *
 * @param port A I2CPortNum.
 *
 * @return _currentState.
 *
 * @see I2CPortNum
 */
uint32_t I2C_engine( I2CPortNum port )
{
	_currentState[port] = I2C_BUSY;

	/*--- Issue a start condition ---*/
	LPC_I2C->CONSET = I2C_CONSET_STA;	/* Set Start flag */

	while ( _currentState[port] == I2C_BUSY )
	{
		if ( _timeout >= I2C_MAX_TIMEOUT )
		{
			_currentState[port] = I2C_TIME_OUT;
			break;
		}
		_timeout++;
	}
	LPC_I2C->CONCLR = I2C_CONCLR_STAC;

	return ( _currentState[port] );
}

