
/*********************************************************************
**
**    Copyright (C) Momentum Software Corporation, 1998
**
*********************************************************************/

/*********************************************************************
**
**    Name:           sample.c
**    ----
**
**    Purpose:        XIPC Sample Program
**    -------
**
**    Description:
**    -----------
**        This program creates activity on an XIPC instance.
**        Specifically, it loops a given number of cycles
**        during which it acquires and releases an XIPC resource semaphore,
**        sends or receives a message from an XIPC message queue,
**        and locks, writes, clears, and unlocks an area of
**        an XIPC shared memory segment.
**
**    Parmeters:      [Command Line]
**    ----------
**        Cycles  Number of cycles (Integer).
**        SleepVal    Time (in ms) to sleep between steps (Integer).
**        ProdCons    Queue operation type (Char: P or C).
**        FillChar    Character to fill memory with (Char).
**        Instance    Name of XIPC Instance to use (String)
**
**
**    Sample Usage:
**    ------------
**        (NOTE:  Atleast two programs must be started with
**                one being a Producer and another a Consumer).
**
**        sample 10000 50 P z @testinst
**        sample 10000 50 C x @testinst
**
**
**    Returns:            
**    -------
**        0   -   Success
**        1   -   Failure.
**
**
**
**    Note:
**    -----
**        This program does not include provisions for signals.
**
*********************************************************************/

/*********************************************************************
** header files
*********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "xipc.h"

#ifdef WIN32
#define getpid GetCurrentProcessId
#endif

/*********************************************************************
** global constants
*********************************************************************/

#define SIZE_SEG  260L
#define SIZE_BUF  512L        

/*********************************************************************
** functions
*********************************************************************/

VOID SemErrorCheck();
VOID QueErrorCheck();
VOID MemErrorCheck();
VOID Terminate();

/*********************************************************************
** Main Program
*********************************************************************/

XINT
main(ArgC, ArgV)
XINT  ArgC;
CHAR  **ArgV;

{
  /*
   * Declare necessary local stuff.
   */

  CHAR *Instance = NULL;
  CHAR LoginName[16];
  CHAR QueCode, FillChar;
  CHAR Buf[SIZE_BUF];
  MIDLIST MidList;
  QIDLIST QidList;
  SIDLIST SidList;
  SECTION MemSec, TempSec, RetSec;
  XINT Offset, Size, BufLen;
  XINT Cycles, i, j;
  XINT Mid, Qid, Sid;
  XINT PrioIn, PrioOut;
  XINT RetCode, RetQid, RetSid;
  XINT Pid, SleepVal;
  XINT TimeStart, TimeEnd;

  /********************************************************
   **
   ** Initilize stuff.
   **
  ********************************************************/

  /*
   * Test usage.
   */

  if (ArgC < 6)
  {
      printf(
      "Usage: sample <Cycles> <SleepVal> [P|C] <FillChar> <Instance>\n");
      exit(1);
  }

  /*
   * Read command line parameters.
   */

  Cycles = atol(ArgV[1]);
  SleepVal = atol(ArgV[2]);
  QueCode = ArgV[3][0];
  FillChar = ArgV[4][0];
  Instance = ArgV[5];

  /*
   * Create XIPC login name. Also initialize rand().
   */

  Pid = getpid();
  sprintf(LoginName, "Pgm%05d", Pid);
  srand((INT)Pid);

  /*
   * Login to target instance.
   */

  if ((RetCode = XipcLogin(Instance, LoginName)) < 0)
  {
      printf("sample: XipcLogin failed, RetCode = %d\n", RetCode);
      exit(1);
  }

  /*
   * Create resource semaphore having one resource copy.
   * Access it, if already exists. Also build SidList
   * to be used in loop below.
   */

  if ((RetCode = SemCreate("SampSem", 1L)) < 0)
  {
      RetCode = SemAccess("SampSem");
      SemErrorCheck("SemAcess", RetCode, -1L);
  }
  Sid = RetCode;
  SemListBuild(SidList, Sid, SEM_EOL);

  /*
   * Create message queue, having capacity for
   * 40 messages and 4096 bytes. Access it, if
   * already exists. Also build QidList to be
   * used in loop below.
   */

  if ((RetCode = QueCreate("SampQue", 40L, 4096L)) < 0)
  {
      RetCode = QueAccess("SampQue");
      QueErrorCheck("QueAcess", RetCode, -1L);
  }
  Qid = RetCode;
  QueListBuild(QidList, Qid, QUE_EOL);
  PrioOut = 1000L;

  /*
   * Create memory segment of SIZE_SEG bytes.
   * Access it, if already exists.
   */

  if ((RetCode = MemCreate("SampSeg", SIZE_SEG)) < 0)
  {
      RetCode = MemAccess("SampSeg");
      MemErrorCheck("MemAcess", RetCode, 
				MemSectionBuild(&TempSec, -1L, 0L, 0L));
  }
  Mid = RetCode;
  
  /********************************************************
  **
  ** Run the loop.
  **
  ********************************************************/

  TimeStart = time( NULL);

  for (j = 0; j < Cycles; j++)
  {
      /*................SemSys work......................*/

      /*
       * Acquire the resource semaphore.
       */

      RetCode = SemAcquire(SEM_ALL, SidList, &RetSid, SEM_WAIT);
      SemErrorCheck("SemAcquire", RetCode, RetSid);

      /*
       * pause ...
       */

      XipcSleep(SleepVal);

      /*
       * Release the resource semaphore.
       */

      RetCode = SemRelease(SidList, &RetSid);
      SemErrorCheck("SemRelease", RetCode, RetSid);

      /*................QueSys work......................*/

      /*
       * Determine if we are a consumer or producer.
       */

      switch (QueCode)
      {
          case 'P':   /* Producer */
          case 'p':

              /*
               * Send a random size message onto queue.
               */

              BufLen = ((XINT)rand() % SIZE_BUF) + 1;
              RetCode = QueSend(QUE_Q_ANY, QidList, Buf, BufLen,
                              PrioOut, &RetQid, QUE_WAIT);
              QueErrorCheck("QueSend", RetCode, RetQid);
              break;

          case 'C':   /* Consumer */
          case 'c':

              /*
               * Receive oldest message from queue.
               */
  
              RetCode = QueReceive(QUE_Q_EA, QidList, Buf,
                              (XINT)sizeof(Buf), &PrioIn,
                              &RetQid, QUE_WAIT);
              QueErrorCheck("QueReceive", RetCode, RetQid);
              break;

      }

      /*................MemSys work......................*/

      /*
       * Pick a random section to lock and fill.
       * Build MidList containing the section.
       */
      
      Offset = (XINT)rand() % SIZE_SEG;
      Size   = ((XINT)rand() % (SIZE_SEG - Offset)) + 1;
      MemSectionBuild(&MemSec, Mid, Offset, Size);
      MemListBuild(MidList, MemSec, MEM_EOL);

      /*
       * Lock the section.
       */

      RetCode = MemLock(MEM_ALL, MidList, &RetSec, MEM_WAIT);
      MemErrorCheck("MemLock", RetCode, RetSec);

      /*
       * Fill with FillChar.
       */

      RetCode = MemWrite(Mid, (XINT) Offset, Size, MEM_FILL(FillChar), MEM_WAIT);
      MemErrorCheck("MemWrite", RetCode, MemSec);

      /*
       * pause ...
       */

      XipcSleep(SleepVal);

      /*
       * Clear the section. (i.e., fill with blanks).
       */

      RetCode = MemWrite(Mid, (XINT) Offset, Size, MEM_FILL(' '), MEM_WAIT);
      MemErrorCheck("MemWrite", RetCode, MemSec);

      /*
       * Unlock the section.
       */

      RetCode = MemUnlock(MidList, &RetSec);
      MemErrorCheck("MemUnlock", RetCode, RetSec);
  }

  /********************************************************
  **
  ** Finish up.
  **
  ********************************************************/

  /*
   * Calculate elapsed time.
   */

  TimeEnd = time( NULL);
  printf("sample: Time = %ld\n", TimeEnd - TimeStart);
  
  /*
   * Say goodnight ...
   */

  Terminate(0);


} /* main */

/*********************************************************************
**
**    Name:   SemErrorCheck
**    ----
**
**    Description:
**    -----------
**        This functions checks for errors during SemSys operations.
**
*********************************************************************/

VOID
SemErrorCheck(FunctionName, RetCode, Sid)
CHAR   *FunctionName;
XINT  RetCode;
XINT  Sid;

{
  if (RetCode < 0)
  {
      fprintf(stderr, "sample: %s failed, RetCode = %d, Sid = %d\n",
              FunctionName, RetCode, Sid);
      Terminate(1);
  }
}

/*********************************************************************
**
**    Name:   QueErrorCheck
**    ----
**
**    Description:
**    -----------
**        This functions checks for errors during QueSys operations.
**
*********************************************************************/

VOID
QueErrorCheck(FunctionName, RetCode, Qid)
CHAR  *FunctionName;
XINT  RetCode;
XINT  Qid;

{
  if (RetCode < 0)
  {
      fprintf(stderr, "sample: %s failed, RetCode = %d, Qid = %d\n",
              FunctionName, RetCode, Qid);
      Terminate(1);
  }
} 

/*********************************************************************
**
**    Name:   MemErrorCheck
**    ----
**
**    Description:
**    -----------
**        This functions checks for errors during MemSys operations.
**
*********************************************************************/

VOID
MemErrorCheck(FunctionName, RetCode, Section)
CHAR  *FunctionName;
XINT  RetCode;
SECTION *Section;

{

  if (RetCode < 0)
  {
      fprintf(stderr,
          "sample: %s failed, RetCode = %d, Section = (%d %ld %ld)\n",
          FunctionName, RetCode, Section->Mid, Section->Offset, 
		  Section->Size);
      Terminate(1);
  }
}

/*********************************************************************
**
**    Name:   Terminate
**    ----
**
**    Description:
**    -----------
**        This functions logs user out from XIPC and terminates.
**
*********************************************************************/

VOID
Terminate(ExitCode)
XINT  ExitCode;

{
  XINT RetCode;

  if ((RetCode = XipcLogout()) < 0)
  {
      printf("\nsample: XipcLogout failed. RetCode = %d.\n", RetCode);
      ExitCode = 1;
  }
  exit(ExitCode);
}

/*
** END ***************************************************************
*/

