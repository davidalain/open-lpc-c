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

#ifndef CORE_H_
#define CORE_H_


#if defined (TARGET_LPC111X)

#include "core_cm0.h"

#elif defined (TARGET_LPC13XX) || defined (TARGET_LPC17XX)

#include "core_cm3.h"

#endif

#endif /* CORE_H_ */
