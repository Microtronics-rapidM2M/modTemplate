@echo off

:: extract load regions from elf file
..\objcopy -O binary -j ER1 -j ER2 -j GOT %2 ./ER1
..\objcopy -O binary -j RW1 --set-section-flags RW1=alloc,load,contents %2 ./RW1tmp
..\objcopy -O binary -j STACK --set-section-flags STACK=alloc,load,contents %2 ./STACK

:: extract RW1 relocations from elf file
..\rel_dump %2 .relER2
..\rel_dump %2 .relRW1

COPY /b .\RW1tmp + .\.relER2 + .\.relRW1 + .\STACK .\RW1

:: build pawn module
..\mod_compile .\ER1 .\RW1 .\module.i

:: create module include file by preprocessing ..\src\module.inc
if [%3] equ [] (
  echo #include "sysdefs.h" > .\%1.i
  echo #include "module.inc" >> .\%1.i
  ..\mcpp -@old -P -k -D S_=/ -D H_define=#define -I . -I ..\src -o ..\pawn\%1.inc .\%1.i
)
::if [%3] equ [gcc] (
::  arm-none-eabi-cpp -traditional-cpp -C -P -nostdinc -I . -imacros ..\src\sysdefs.h -D S_=/ -o ..\pawn\%1.inc ..\src\module.inc
::)

:: clean up
DEL /F /Q .\ER*
DEL /F /Q .\RW*
DEL /F /Q .\STACK*
DEL /F /Q .\*.i
