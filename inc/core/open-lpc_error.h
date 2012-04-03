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

#ifndef OPENLPC_ERROR_H
#define OPENLPC_ERROR_H

#include <stdlib.h>

/*
	open-lpc error library.

	Works as a printf-like function. If NO_DEBUG flag is defined anywhere in
	the code the error(...) function kills the application instantly,
	if not, error(...) prints a message through the standard error output and,
	then, kills the main program.

	E.g.:
		...
		if (num < 3) {
			error ("This function has an invalid value: %d", num);
		}
*/
#ifdef NO_DEBUG
	#define error (...)	(exit(1))
#else
	#include <stdio.h>
	#define error (...)	(fprintf (stderr, __VA_ARGS__), exit(1))
#endif

#endif

