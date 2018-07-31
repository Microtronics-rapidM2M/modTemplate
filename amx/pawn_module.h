#ifndef __PAWN_MODULE_H
#define __PAWN_MODULE_H

#include "types.h"
#include "amx.h"

typedef struct
{
  u32 crc;
  u32 size;
  u32 version;
  u32 ro_base;
  u32 rw_base;
  ascii name[16];
} TPawn_ScriptModuleInfo;

struct SPawn_ScriptModule;
typedef struct SPawn_ScriptModule TPawn_ScriptModule;

struct SPawn_ScriptModule
{
  TPawn_ScriptModule* psNext;
  size_t size;
  TPawn_ScriptModuleInfo* psInfo;

  int AMXAPI (*pfInit)(AMX*);
  int AMXAPI (*pfCleanup)(AMX*);
  const AMX_NATIVE_INFO *nativelist;
  int nNatives;
};

extern TPawn_ScriptModule pawn_ScriptModule;

int AMXAPI Pawn_GetBinary(unsigned char*, const cell*, size_t);
int AMXAPI Pawn_SetBinary(cell*, const unsigned char*, size_t);

#endif
