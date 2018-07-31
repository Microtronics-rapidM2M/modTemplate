/***************************************************************************//**
 * @file types.h
 * @brief Common used type definitions
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
#ifndef __TYPES_H
#define __TYPES_H

/*******************************************************************************
 ********************************   TYPES   ************************************
 ******************************************************************************/

/** common used types */
//typedef unsigned char   bool;
#include <stdbool.h>
typedef unsigned char   u8;
typedef signed   char   s8;
typedef          char   ascii;
typedef unsigned short  u16;
typedef          short  s16;
typedef unsigned long   u32;
typedef          long   s32;
typedef          float  f32;
typedef          double f64;


/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

#if defined(FALSE) && (FALSE)
#undef FALSE
#endif

#if defined(TRUE) && !(TRUE)
#undef TRUE
#endif

/** Boolean values for 'bool' type */
#ifndef FALSE
#define FALSE    (0)
#endif
#ifndef TRUE
#define TRUE     (!(FALSE))
#endif

/** NULL definition */
#ifndef NULL
#define NULL     ((void*)0)
#endif

/** common function return values */
#define OK     0
#define ERROR -1

#endif /*__TYPES_H*/
