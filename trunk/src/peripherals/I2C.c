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

#include "peripherals/I2C.h"
#include "core/Types.h"

static uint8_t _txBuffer[I2C_BUFFER_SIZE][I2C_NUM];
static uint8_t _rxBuffer[I2C_BUFFER_SIZE][I2C_NUM];
static uint32_t _rxBufferCurrentSize[I2C_NUM];
static uint32_t _txBufferCurrentSize[I2C_NUM];
static uint32_t _rxIndex[I2C_NUM];
static uint32_t _txIndex[I2C_NUM];
static uint32_t _currentState[I2C_NUM];

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




void I2C_default_handler(I2CPortNum port){

#if defined (TARGET_LPC111X) || defined (TARGET_LPC13XX)
	uint8_t statReg;

	/* this handler deals with master read and master write only */
	statReg = LPC_I2C->STAT;
	switch ( statReg )
	{
	case 0x08:			/* A Start condition is issued. */
		_txIndex[port] = 0;
		LPC_I2C->DAT = (_txBuffer[port])[(_txIndex[port])++];
		LPC_I2C->CONCLR = (I2C_CONCLR_SIC | I2C_CONCLR_STAC);
		_currentState[port] = I2C_STARTED;
		break;

	case 0x10:			/* A repeated started is issued */
		_rxIndex[port] = 0;
		/* Send SLA with R bit set, */
		LPC_I2C->DAT = (_txBuffer[port])[(_txIndex[port])++];
		LPC_I2C->CONCLR = (I2C_CONCLR_SIC | I2C_CONCLR_STAC);
		_currentState[port] = I2C_RESTARTED;
		break;

	case 0x18:			/* Regardless, it's a ACK */
		if ( _currentState[port] == I2C_STARTED )
		{
			LPC_I2C->DAT = (_txBuffer[port])[(_txIndex[port])++];
			_currentState[port] = I2C_DATA_ACK;
		}
		LPC_I2C->CONCLR = I2C_CONCLR_SIC;
		break;

	case 0x28:	/* Data byte has been transmitted, regardless ACK or NACK */
	case 0x30:
		if ( _txIndex[port] < _txBufferCurrentSize[port] )
		{
			LPC_I2C->DAT = (_txBuffer[port])[(_txIndex[port])++]; /* this should be the last one */
			_currentState[port] = I2C_DATA_ACK;
		}
		else
		{
			if ( _rxBufferCurrentSize[port] != 0 )
			{
				LPC_I2C->CONSET = I2C_CONSET_STA;	/* Set Repeated-start flag */
				_currentState[port] = I2C_REPEATED_START;
			}
			else
			{
				LPC_I2C->CONSET = I2C_CONSET_STO;      /* Set Stop flag */
				_currentState[port] = I2C_DATA_NACK;
			}
		}
		LPC_I2C->CONCLR = I2C_CONCLR_SIC;
		break;

	case 0x40:	/* Master Receive, SLA_R has been sent */
		if ( _rxBufferCurrentSize[port] == 1 )
		{
			/* Will go to State 0x58 */
			LPC_I2C->CONCLR = I2C_CONCLR_AAC;	/* assert NACK after data is received */
		}
		else
		{
			/* Will go to State 0x50 */
			LPC_I2C->CONSET = I2C_CONSET_AA;	/* assert ACK after data is received */
		}
		LPC_I2C->CONCLR = I2C_CONCLR_SIC;
		break;

	case 0x50:	/* Data byte has been received, regardless following ACK or NACK */
		(_rxBuffer[port])[(_rxIndex[port])++] = LPC_I2C->DAT;
		if ( _rxIndex[port] < _rxBufferCurrentSize[port] )
		{
			_currentState[port] = I2C_DATA_ACK;
			LPC_I2C->CONSET = I2C_CONSET_AA;	/* assert ACK after data is received */
		}
		else
		{
			_currentState[port] = I2C_DATA_NACK;
			LPC_I2C->CONCLR = I2C_CONCLR_AAC;	/* assert NACK on last byte */
		}
		LPC_I2C->CONCLR = I2C_CONCLR_SIC;
		break;

	case 0x58:
		(_rxBuffer[port])[(_rxIndex[port])++] = LPC_I2C->DAT;
		_currentState[port] = I2C_DATA_NACK;
		LPC_I2C->CONSET = I2C_CONSET_STO;	/* Set Stop flag */
		LPC_I2C->CONCLR = I2C_CONCLR_SIC;	/* Clear SI flag */
		break;

	case 0x20:		/* regardless, it's a NACK */
	case 0x48:
		LPC_I2C->CONCLR = I2C_CONCLR_SIC;
		_currentState[port] = I2C_DATA_NACK;
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

}

bool I2C_start(I2CPortNum port){

	uint32_t timeout = 0;
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
		if ( timeout >= I2C_MAX_TIMEOUT )
		{
			retVal = false;
			break;
		}
		timeout++;
	}

#else
#error "Implementar pro LPC17XX"
#endif

	return( retVal );

}

void I2C_stop(I2CPortNum port){

	LPC_I2C->CONSET = I2C_CONSET_STO;  /* Set Stop flag */
	LPC_I2C->CONCLR = I2C_CONCLR_SIC;  /* Clear SI flag */

	/*--- Wait for STOP detected ---*/
	while( LPC_I2C->CONSET & I2C_CONSET_STO );

}
