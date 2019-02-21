/***************************************************************************//**
 * @file module.c
 * @brief Script Loadable Module
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
#include <stdio.h>
#include "sysdefs.h"
#include "pawn_module.h"


/*******************************************************************************
 **************************   LOCAL FUNCTIONS   ********************************
 ******************************************************************************/

/***************************************************************************//**
 * @details
 *   Every native function must have the following prototype:
 *
 *     cell AMX_NATIVE_CALL func(AMX *amx, const cell *params);
 *
 *     amx       == pointer to abstract machine
 *     params[0] == number of bytes for the additional parameters passed
 *                  to the native function
 *     params[1] == first argument
 *     etc.
 *
 *   TODO: Add or remove your functions implementation below!
 ******************************************************************************/

/***************************************************************************//**
 * @brief
 *   Calculate average over floating point values provided as
 *   function arguments.
 *
 * @pawn
 *   native Float:modAvgF(Float:val1, Float:val2, Float:...);
 ******************************************************************************/
static cell AMX_NATIVE_CALL modAvgF(AMX *amx, const cell *params)
{
  int iNumParams = params[0]/sizeof(cell); /* argument count */
  float fAvg = 0.0f;
  int i;

  /* sanity check */
  if(iNumParams <= 0)
    return(0);

  /* sum up values */
  for(i=0; i<iNumParams; i++)
  {
    if(i < 2)
    {
      /* first 2 arguments are passed by value */
      fAvg += amx_ctof(params[1+i]);
    }
    else
    {
      /* variable arguments are passed by reference */
      cell *cptr;

      /* verify value address and get pointer to value */
      cptr = amx_VerifyAddress(amx,params[1+i],0);

      /* access value through cell pointer */
      fAvg += amx_ctof(*cptr);
    }
  }
  /* calculate average */
  fAvg /= iNumParams;

  /* return floating point average as cell */
  return(amx_ftoc(fAvg));
}

/***************************************************************************//**
 * @brief
 *   Calculate average over floating point values provided as array.
 *
 * @pawn
 *   native Float:modAvgArrayF(const Float:arrayF[], size = sizeof arrayF);
 ******************************************************************************/
static cell AMX_NATIVE_CALL modAvgArrayF(AMX *amx, const cell *params)
{
  int iNumFloats = params[2]; /* 'sizeof arrayF' gives the number of cells */
  float fAvg = 0.0f;
  cell *cptr;
  int i;

  /* sanity check */
  if(iNumFloats <= 0)
    return(0);

  /* verify array address and get pointer to array */
  cptr = amx_VerifyAddress(amx,params[1],iNumFloats*sizeof(cell));
  if(cptr == NULL)
    return(0);

  /* sum up values */
  for(i=0; i<iNumFloats; i++, cptr++)
  {
    /* access value through cell pointer */
    fAvg += amx_ctof(*cptr);
  }
  /* calculate average */
  fAvg /= iNumFloats;

  /* return floating point average as cell */
  return(amx_ftoc(fAvg));
}

/***************************************************************************//**
 * @brief
 *   Show information about this module.
 *
 * @pawn
 *   native modInfo();
 ******************************************************************************/
#define _STR(x)  __STR(x)
#define __STR(x) #x

#if defined __GNUC__
  const char* cszModule_Compiler = "GCC";
#else
  const char* cszModule_Compiler = "ARMCC";
#endif

#if defined __SOFTFP__
  const char* cszModule_FPU = "soft FPU";
#else
  const char* cszModule_FPU = "hard FPU";
#endif

static u8 u8Module_MajorVersion = MODULE_MAJOR_VERSION;
static u8 u8Module_MinorVersion = MODULE_MINOR_VERSION;

static struct
{
  const char* name;
  const u8* major;
  const u8* minor;
} sModule_Info =
{
  _STR(MODULE_NAME),
  &u8Module_MajorVersion,
  &u8Module_MinorVersion,
};

static cell AMX_NATIVE_CALL modInfo(AMX *amx, const cell *params)
{
  /* test of runtime relocations */
  printf("%s %02dv%03d (%s, %s)\r\n", sModule_Info.name,
    *sModule_Info.major, *sModule_Info.minor,
    cszModule_Compiler, cszModule_FPU);

  return(0);
}


/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
 * @brief
 *   List of native functions provided by this module.
 *
 * @details
 *   TODO: Add or remove your functions here and in file module.inc!
 ******************************************************************************/
#define MODULE_NATIVES \
  _NATIVE_DEF(modAvgF) \
  _NATIVE_DEF(modAvgArrayF) \
  _NATIVE_DEF(modInfo) \
/* end of define */

/***************************************************************************//**
 * @brief
 *   Automatic creation of native functions enumeration.
 *   !! DO NOT CHANGE !!
 ******************************************************************************/
#undef _NATIVE_DEF
#define _NATIVE_DEF(_func) MODULE_##_func,
typedef enum
{
  MODULE_NATIVES
  MODULE_NUM_NATIVES
} TModule_Natives;

/***************************************************************************//**
 * @brief
 *   Module native functions table.
 *   !! DO NOT CHANGE !!
 ******************************************************************************/
AMX_NATIVE_INFO amx_ModuleNatives[MODULE_NUM_NATIVES];

/***************************************************************************//**
 * @brief
 *   Module initialization function.
 *
 * @details
 *   This function gets called when the module is loaded.
 ******************************************************************************/
int AMXEXPORT AMXAPI amx_ModuleInit(AMX *amx)
{
  (void)amx;

  /* runtime initialization of module native functions table!
     !! DO NOT CHANGE !! */
#undef _NATIVE_DEF
#define _NATIVE_DEF(_func) \
    amx_ModuleNatives[MODULE_##_func].name = (#_func); \
    amx_ModuleNatives[MODULE_##_func].func = (_func);
  MODULE_NATIVES;
  pawn_ScriptModule.nNatives = MODULE_NUM_NATIVES;

  /*
   * TODO: Add your module initialization code here!
   */

  return amx_Register(amx, amx_ModuleNatives, MODULE_NUM_NATIVES);
}

/***************************************************************************//**
 * @brief
 *   Module cleanup function.
 *
 * @details
 *   This function gets called when the module/script is unloaded.
 ******************************************************************************/
int AMXEXPORT AMXAPI amx_ModuleCleanup(AMX *amx)
{
  (void)amx;

  /*
   * TODO: Add your module cleanup code here!
   */

  return AMX_ERR_NONE;
}
