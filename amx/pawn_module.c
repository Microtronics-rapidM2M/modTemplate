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
#if !defined __GNUC__
  #include <rt_heap.h>
#endif

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
  #if defined MODULE_HEAP_SIZE
    u32 pawn_ModuleHeap[((MODULE_HEAP_SIZE+3)&~3)/sizeof(u32)] __attribute__((section(".module_heap"), used));
  #endif
  u32 pawn_ModuleStack[((MODULE_STACK_SIZE+MODULE_STACK_FRAME+7)&~7)/sizeof(u32)+1] __attribute__((section(".module_stack"), used));
#else
  #if defined MODULE_HEAP_SIZE
    /* The parameters of _init_alloc(base, top) must be eight-byte aligned */
    uint64_t pawn_ModuleHeap[((MODULE_HEAP_SIZE+7)&~7)/sizeof(uint64_t)] __attribute__((section(".module_heap"), zero_init, used));
  #endif
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
 **************************   STATIC FUNCTIONS   *******************************
 ******************************************************************************/

#define ELF_SHT_REL 0x09   /* ELF relocation entries without explicit addends */

typedef struct
{
  u32 tag;
  u32 length;
  u32 value;
} tlv_t;

static void __check_tlv_relocs(tlv_t* tlv, u32 size, u32 offset)
{
  while(size)
  {
    u32 len;

    switch(tlv->tag)
    {
      /* relocation table */
      case ELF_SHT_REL:
      {
        u32 reloc_num = tlv->length / sizeof(u32);
        u32* reloc_entry = &tlv->value;

        /* patch RW1 relocations */
        while(reloc_num)
        {
          u32* reloc_pos;

          reloc_pos = (u32*)(*reloc_entry + offset);
          *reloc_pos += offset;
          reloc_entry++;
          reloc_num--;
        }
        break;
      }
    }

    /* next TLV */
    len = 2*sizeof(u32) + tlv->length;
    if(size > len)
    {
      tlv = (tlv_t*)((u32)tlv + len);
      size -= len;
    }
    else
      size = 0;
  }
}


#if defined __GNUC__
extern u32 __got_start, __got_end, __rw_base, __rw_end;
/* -fpic Generate position-independent code (PIC) suitable for use in a shared library,
         if supported for the target machine. Such code accesses all constant addresses
         through a global offset table (GOT). */
static void __patch_module_relocs(void)
{
  register u32 __sb_r9 __asm("r9");
  u32 offset;
  u32* got_entry;
  u32* got_end;
  u32* stack_base;
  u32  stack_size;

  /* calculate offset of the table based on the __rw_base position */
  offset = __sb_r9 - (u32)&__rw_base;

  /* calculate start & end addresses of GOT before it is patched! */
  got_entry = (u32*)(offset + (u32)&__got_start);
  got_end = (u32*)(offset + (u32)&__got_end);

  /* patch every entry in the GOT */
  while(got_entry < got_end)
  {
    *got_entry += offset;
    got_entry++;
  }

  /* check for TLV structure which may be inserted in the module between heap and stack */
  stack_base = &__rw_end;
  stack_size = (offset + *((volatile u32*)&pawn_ModuleInfo.size)) - (u32)&__rw_end;
  if(stack_size > sizeof(pawn_ModuleStack))
    __check_tlv_relocs((tlv_t*)stack_base, stack_size - sizeof(pawn_ModuleStack), offset);
}
#else
extern unsigned int Image$$ER1$$Base;
extern unsigned int Image$$RW1$$ZI$$Limit;

static void __patch_module_relocs(void)
{
  u32 offset;
  u32* stack_base;
  u32  stack_size;

  /* get offset of the module */
  offset = (u32)&Image$$ER1$$Base;

  /* check for TLV structure which may be inserted in the module between heap and stack */
  stack_base = (u32*)&Image$$RW1$$ZI$$Limit;
  stack_size = (offset + *((volatile u32*)&pawn_ModuleInfo.size)) - (u32)&Image$$RW1$$ZI$$Limit;
  if(stack_size > sizeof(pawn_ModuleStack))
    __check_tlv_relocs((tlv_t*)stack_base, stack_size - sizeof(pawn_ModuleStack), offset);
}
#endif

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/* linker entry point for unused section optimization & runtime initialization */
void* pawn_ModuleInit(void) __attribute__((section(".module_init"), used));
void* pawn_ModuleInit(void)
{
  /* patch module link time absolute relocations */
  __patch_module_relocs();

#if !defined __GNUC__
  /* only for armcc bare metal applications using heap */
  #if defined MODULE_HEAP_SIZE
    _init_alloc((uintptr_t)pawn_ModuleHeap, (uintptr_t)pawn_ModuleHeap + sizeof(pawn_ModuleHeap));
  #endif
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
