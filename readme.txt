rapidM2M Script Loadable Module Template
----------------------------------------

The template is provided as project for Eclipse CDT on Windows.

Prerequisites:
 - Arm Compiler 5 or GNU Arm Embedded Toolchain
 - Eclipse IDE for C/C++ Developers

Arm Compiler 5:
 Use either as
 - standalone tool from https://developer.arm.com/products/software-development-tools/compilers/arm-compiler/downloads/version-5
 or
 - bundled with Keil MDK

GNU Arm Embedded Toolchain:
 - Install version 7-2017-q4-major from https://developer.arm.com/open-source/gnu-toolchain/gnu-rm.

Eclipse IDE for C/C++ Developers:
 - Install by downloading and running Eclipse Installer from http://www.eclipse.org/downloads.
 - Select "Eclipse IDE for C/C++ Developer" as package to install.
 - Import template project into your workspace.
 - Select your appropriate build configuration and change the path in the project properties 
   "C/C++ Build" -> "Settings" -> "Tool Settings" -> "Cross Settings" -> "Path"
   according to your installation.

To avoid any problems, change only the files in "src" subdirectory!
 - sysdefs.h
   Contains configuration definitions (MODULE_NAME has to be a valid ANSI C variable name;
   MODULE_STACK_SIZE depends on build configuration)
 - module.c
   Contains module native function implementations.
   Positions where to insert your code are marked with "TODO:".
 - module.inc
   Contains the Pawn native function prototypes of the module.

When the build succeeded, a file ${ProjName}.inc is created in "pawn" subdirectory.

Runtime Environment:
 The module code will run in a memory protected, unprivileged environment with full
 access to abstract machines data memory but read only access to the abstract machine
 itself (AMX *amx). Where needed, the environment provides supervisor calls
 (e.g. amx_RaiseError()).
