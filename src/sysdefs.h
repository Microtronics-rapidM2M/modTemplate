/***************************************************************************//**
 * @file sysdefs.h
 * @brief System configuration defines
 * @author Microtronics Engineering GmbH
 *******************************************************************************
 * @section License
 * <b>(C) Copyright 2018 Microtronics Engineering GmbH,
 * http://www.microtronics.at</b>
 *******************************************************************************
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Microtronics Engineering GmbH
 * is providing the Software "AS IS", with no express or implied warranties of
 * any kind, including, but not limited to, any implied warranties of
 * merchantability or fitness for any particular purpose or warranties against
 * infringement of any proprietary rights of a third party.
 *
 * Microtronics Engineering GmbH will not be liable for any consequential,
 * incidental, or special damages, or any other relief, or for any claim by any
 * third party, arising from your use of this Software.
 *
 ******************************************************************************/
#ifndef __SYSDEFS_H
#define __SYSDEFS_H

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

/* Module version number */
#define MODULE_MAJOR_VERSION 0
#define MODULE_MINOR_VERSION 2

/* Module name */
#define MODULE_NAME modTemplate

/* Maximum stack usage from module functions (depends on build configuration!).
 * The resulting stack is bigger (stack frame added & 8 byte aligned) */
#define MODULE_STACK_SIZE 64

/* Optional heap configuration for module */
#define MODULE_HEAP_SIZE 1024

#endif /*__SYSDEFS_H*/
