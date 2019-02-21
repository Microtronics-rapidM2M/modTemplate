#ifndef __AMX_SVC_H
#define __AMX_SVC_H

#ifndef SVCALL
  #if defined (__CC_ARM)
    #define SVCALL(number, return_type, signature) __svc(number) return_type signature

  #elif defined (__GNUC__)
    #define SVCALL(number, return_type, signature)          \
      _Pragma("GCC diagnostic push")                        \
      _Pragma("GCC diagnostic ignored \"-Wreturn-type\"")   \
      __attribute__((naked))                                \
      __attribute__((unused))                               \
      static return_type signature                          \
      {                                                     \
        __asm(                                              \
            "svc %0\n"                                      \
            "bx r14" : : "I" (number) : "r0"                \
        );                                                  \
      }                                                     \
      _Pragma("GCC diagnostic pop")

  #elif defined (__ICCARM__)
    #define PRAGMA(x) _Pragma(#x)
    #define SVCALL(number, return_type, signature)          \
      PRAGMA(swi_number = (number))                         \
      __swi return_type signature;

  #else
    #define SVCALL(number, return_type, signature) return_type signature
  #endif
#endif

enum
{
  SVC_SYS_EXCEPT = 0,             /* force supervisory call exception handler */
  SVC_SYS_EXISTS = 3,             /* check for existing supervisory call */
};

enum
{
  SVC_PAWN_DEBUG_TX  = 253,       /* transmit buffer to debug output */
  SVC_AMX_REGISTER   = 254,       /* Make native functions known */
  SVC_AMX_RAISEERROR = 255,       /* Flag an error */
};

SVCALL(SVC_SYS_EXCEPT, void, Sys_ForceExcept(unsigned long int u32Code, unsigned long int u32Param));
SVCALL(SVC_SYS_EXISTS, long int, Sys_ExistsSVC(unsigned char u8SVC));

SVCALL(SVC_PAWN_DEBUG_TX, int, Pawn_DebugTx(const char *ptr, int len));

#endif
