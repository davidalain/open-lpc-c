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

#ifndef OPENLPC_I2C_H_
#define OPENLPC_I2C_H_

#include "core/PinNames.h"
#include "core/Types.h"

typedef enum {
	I2C_0 = 0,
#if defined (TARGET_LPC17XX)
	I2C_1 = 1,
	I2C_2 = 2
#endif

}I2CPortNum;

typedef enum {
	Mode_Master,
	Mode_Slave
}I2CMode;

#if defined (TARGET_LPC111X) || defined (TARGET_LPC13XX)
#define I2C_NUM	1
#elif defined (TARGET_LPC17XX)
#define I2C_NUM	3
#endif

#define I2C_CONSET_I2EN		0x00000040  /* I2C Control Set Register */
#define I2C_CONSET_AA		0x00000004
#define I2C_CONSET_SI		0x00000008
#define I2C_CONSET_STO		0x00000010
#define I2C_CONSET_STA		0x00000020

#define I2C_CONCLR_AAC		0x00000004  /* I2C Control clear Register */
#define I2C_CONCLR_SIC		0x00000008
#define I2C_CONCLR_STAC		0x00000020
#define I2C_CONCLR_I2ENC	0x00000040

#define I2C_I2DAT_I2C		0x00000000  /* I2C Data Reg */
#define I2C_I2ADR_I2C		0x00000000  /* I2C Slave Address Reg */
#define I2C_I2SCLH_SCLH		0x00000180  /* I2C SCL Duty Cycle High Reg */
#define I2C_I2SCLL_SCLL		0x00000180  /* I2C SCL Duty Cycle Low Reg */
#define I2C_I2SCLH_HS_SCLH	0x00000020  /* Fast Plus I2C SCL Duty Cycle High Reg */
#define I2C_I2SCLL_HS_SCLL	0x00000020  /* Fast Plus I2C SCL Duty Cycle Low Reg */

#define I2C_IDLE				0
#define I2C_STARTED				1
#define I2C_RESTARTED			2
#define I2C_REPEATED_START		3
#define I2C_DATA_ACK			4
#define I2C_DATA_NACK			5
#define I2C_BUSY            	6
#define I2C_NO_DATA         	7
#define I2C_NACK_ON_ADDRESS 	8
#define I2C_NACK_ON_DATA    	9
#define I2C_ARBITRATION_LOST	10
#define I2C_TIME_OUT          	11
#define I2C_OK                	12

#define I2C_BUFFER_SIZE						6
#define I2C_MAX_TIMEOUT						0x00FFFFFF
#define I2C_DEFAULT_DEVICE_I2C_ADDRESS		0xA0

void I2C_Init(I2CPortNum port);
void I2C_default_handler(I2CPortNum port);
bool I2C_start(I2CPortNum port);
void I2C_stop(I2CPortNum port);

#endif
