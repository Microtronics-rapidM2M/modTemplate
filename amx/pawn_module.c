/***************************************************************************//**
 * @file pawn_module.c
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
#include "sysdefs.h"
#include "pawn_module.h"


extern AMX_NATIVE_INFO amx_ModuleNatives[];
extern int AMXEXPORT AMXAPI amx_ModuleInit(AMX *amx);
extern int AMXEXPORT AMXAPI amx_ModuleCleanup(AMX *amx);


TPawn_ScriptModule pawn_ScriptModule;

/* size of stack frame on exception entry (see programming manual) */
#if defined __SOFTFP__
  /* module is compiled for SW floating point support,
   * it should be safe to assume, that stack frame does never
   * include floating-point storage */
  #define MODULE_STACK_FRAME ((8)*sizeof(u32))
#else
  /* module is compiled for HW floating point support,
   * stack frame may include floating-point storage */
  #define MODULE_STACK_FRAME ((8+17)*sizeof(u32))
#endif


/* The resulting stack:
 *   (stack usage from module functions + stack frame) & 8 byte aligned
 *   +1 because we can't control the aligning within pawn script */
#if defined __GNUC__
  u32 pawn_ModuleStack[((MODULE_STACK_SIZE+MODULE_STACK_FRAME+7)&~7)/sizeof(u32)+1] __attribute__((section(".module_stack"), used));
#else
  u32 pawn_ModuleStack[((MODULE_STACK_SIZE+MODULE_STACK_FRAME+7)&~7)/sizeof(u32)+1] __attribute__((section(".module_stack"), zero_init, used));
#endif


#define _STR(x)  __STR(x)
#define __STR(x) #x

const TPawn_ScriptModuleInfo pawn_ModuleInfo __attribute__((section(".module_info"), used)) = {
  .crc = 0, .size = 0,
  .version = (MODULE_MAJOR_VERSION)<<8 | (MODULE_MINOR_VERSION),
  .ro_base = sizeof(TPawn_ScriptModuleInfo),
  .rw_base = 0,
  .name = _STR(MODULE_NAME),
};


/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

#if defined __GNUC__
extern u32 __got_start, __got_end, __rw_base;
/* -fpic Generate position-independent code (PIC) suitable for use in a shared library,
         if supported for the target machine. Such code accesses all constant addresses
         through a global offset table (GOT). */
static void __patch_got(void)
{
  register u32 __sb_r9 __asm("r9");
  u32 offset;
  u32* got_entry;

  /* calculate offset of the table based on the __rw_base position */
  offset = __sb_r9 - (u32)&__rw_base;

  /* patch every entry in the table */
  got_entry = (u32*)(offset + (u32)&__got_start);
  while((u32)got_entry < (offset + (u32)&__got_end))
  {
    *got_entry += offset;
    got_entry++;
  }
}
#endif

/* linker entry point for unused section optimization & runtime initialization */
void* pawn_ModuleInit(void) __attribute__((section(".module_init"), used));
void* pawn_ModuleInit(void)
{
#if defined __GNUC__
  /* only for gcc! global offset table patching */
  __patch_got();
#endif

  /* runtime initialization of position independent code/data! */
  pawn_ScriptModule.size = sizeof(pawn_ScriptModule);
  pawn_ScriptModule.pfInit = amx_ModuleInit;
  pawn_ScriptModule.pfCleanup = amx_ModuleCleanup;
  pawn_ScriptModule.nativelist = amx_ModuleNatives;

  return(&pawn_ScriptModule);
}

int AMXAPI Pawn_GetBinary(unsigned char *dest, const cell *source, size_t size)
{
  if(((cell)dest%sizeof(cell) == 0) && (size%sizeof(cell) == 0))
  {
    /* special case: cell aligned data and size */
    cell* cpDest = (cell*)dest;
    size = size/sizeof(cell);
    for(;size > 0; size--)
      *cpDest++ = __REV(*source++); // Reverse byte order (32 bit)
  }
  else
  {
    /* copy byte by byte */
    cell c = 0;
    int i = sizeof(cell)-1;

    while(size)
    {
      if(i==sizeof(cell)-1)
        c=*source++;

      *dest++ = (unsigned char)(c >> i*8);
      i=(i+sizeof(cell)-1) % sizeof(cell);
      size--;
    }
  }

  return AMX_ERR_NONE;
}

int AMXAPI Pawn_SetBinary(cell *dest, const unsigned char *source, size_t size)
{
  int i = sizeof(cell)-1;
  
  if((((int)dest) & (sizeof(cell)-1)) != 0)
  {
    /* destination pointer is not cell aligned */
    i -= ((int)dest) & (sizeof(cell)-1);
    
    /* align pointer */
    dest = (cell*)(((int)dest) & ~((sizeof(cell)-1)));
  }
  
  if(((cell)source%sizeof(cell) == 0) && (size%sizeof(cell) == 0))
  {
    /* special case: cell aligned data and size */
    const cell* cpSource = (const cell*)source;
    size = size/sizeof(cell);
    for(;size > 0; size--)
      *dest++ = __REV(*cpSource++); // Reverse byte order (32 bit)
  }
  else
  {
    /* copy byte by byte */
    while(size)
    {
      *dest &= ~(0xFF << i*8);
      *dest |= (*source++ << i*8);

      if(i==0) dest++;
      i=(i+sizeof(cell)-1) % sizeof(cell);
      size--;
    }
  }

  return AMX_ERR_NONE;
}
