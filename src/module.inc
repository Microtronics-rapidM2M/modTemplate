S_*******************************************************************************
 * @file MODULE_NAME.inc
 * @brief Script Loadable Module
 * @version MODULE_MAJOR_VERSION.MODULE_MINOR_VERSION
 * @author 
 ******************************************************************************S_

#include "module.i"

S_*******************************************************************************
 * @brief
 *   Module definitions.
 ******************************************************************************S_
H_define TmodParams[.par1, .par2]
 
S_*******************************************************************************
 * @brief
 *   Module native functions.
 ******************************************************************************S_
native Float:modAvgF(Float:val1, Float:val2, Float:...);
native Float:modAvgArrayF(const Float:arrayF[], size = sizeof arrayF);
native modInfo();
