#include "modTemplate.inc"

/* forward declarations of public functions */
forward public Timer5s();

/* application entry point */
main()
{
  new aId[TrM2M_Id];
  new iIdx, iResult;

  /* Retrieve identification of HW module */
  rM2M_GetId(aId);

  /* output module information */
  printf("Id: %s\r\n", aId.string);
  printf("Module: %s\r\n", aId.module);
  printf("HW: %d.%d\r\n", aId.hwmajor, aId.hwminor);

  /* init a oneshot 5s Timer */
  iIdx = funcidx("Timer5s");
  iResult = rM2M_TimerAddExt(iIdx, false, 5000);
  if(iResult < OK)
    printf("rM2M_TimerAddExt(%d) = %d\r\n", iIdx, iResult);

  printf("---- load modTemplate ----\r\n");
  printf("  modInfo()      %s\r\n", exists("modInfo") ? "OK" : "N/A");
  printf("  modAvgF()      %s\r\n", exists("modAvgF") ? "OK" : "N/A");
  printf("  modAvgArrayF() %s\r\n", exists("modAvgArrayF") ? "OK" : "N/A");

  iResult = loadmodule(modTemplate);
  if(iResult < OK)
    printf("loadmodule() = %d\r\n", iResult);
  else
    printf("loaded modTemplate successfully\r\n");

  printf("  modInfo()      %s\r\n", exists("modInfo") ? "OK" : "N/A");
  printf("  modAvgF()      %s\r\n", exists("modAvgF") ? "OK" : "N/A");
  printf("  modAvgArrayF() %s\r\n", exists("modAvgArrayF") ? "OK" : "N/A");
}

/* oneshot 5s Timer */
public Timer5s()
{
  new Float:fAvg;
  new Float:faValues[] = [1.0, 2.0, 3.0, 4.0, 5.0];

  printf("---- run modTemplate ----\r\n");
  modInfo();

  fAvg = modAvgF(1.0, 2.0, 3.0, 4.0, 5.0);
  printf("modAvgF = %f\r\n", fAvg);

  fAvg = modAvgArrayF(faValues);
  printf("modAvgArrayF = %f\r\n", fAvg);
}
