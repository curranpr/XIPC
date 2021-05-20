/********************************************************** DEMASYN.C
**										   (SemSys - Semaphore System)
**
**
**	Name:	DEMASYN.C
**	----
**
**	Title:	Asynchronous Operation Test using Resource Semaphores
**	-----
**
**	Description:
**	-----------
**	This file contains the program that tests SemSys using
**	resource semaphores.
**
**	Copyright (C) Momentum Software Corporation, 1989-97
**
*********************************************************************/

/*********************************************************************
** header files
*********************************************************************/

#include <stdio.h>
#include <signal.h>
#include <time.h>
#if defined(__DECC) && defined(VMS)
#include unixlib
#endif
#ifdef __STDC__
#include <stdlib.h>
#endif
#include "xipc.h"

#ifdef WIN32
#define getpid GetCurrentProcessId
#endif

/*********************************************************************
** global variables
*********************************************************************/

XINT Sid;
FLAG FlagMore = 1;
FLAG FlagAcquire = 0;

/*********************************************************************
** functions
*********************************************************************/

extern CHAR * getenv();
VOID DemoTrap(XINT);
VOID CbAcquire(ASYNCRESULT *AcbPtr);

/*********************************************************************
**
**	Name:	demasyn (main)
**	----
**
**	Purpose:	Test SemSys Using Resource Semaphores
**	-------
**
**	Parmeters:		[Command Line]
**	----------
**		SemName 		Name of semaphore
**		Cycles			Number of cycles to run
**		Work1			Wait period 1
**		Work2			Wait period 2
**		ConfigName		Name of configuration
**
**	Returns:		None.
**	-------
**
**	Description:
**	-----------
**	This program tests SemSys using resource semaphores.
**	It cycles thru a acquire/release sequence, waiting between
**	each operation for the specified periods of time.
**
*********************************************************************/

INT
main(ArgC, ArgV)
INT ArgC;
CHAR **ArgV;

{

	CHAR *ConfigName = NULL;
	CHAR *SemName;
	CHAR PidString[16];
	SIDLIST SidList;
	XINT i, j;
	XINT n;
	XINT RetCode;
	XINT RetSid;
	XINT TimeStart, TimeEnd;
	XINT Work1;
	XINT Work2;
	ASYNCRESULT Acb;

	if (ArgC < 5)
	{
		printf("Usage: %s <sem> <cycles> <Work1> <Work2> <config>\n",
				ArgV[0]);
		exit(0);
	}
	n = atol(ArgV[2]);
	Work1 = atol(ArgV[3]);
	Work2 = atol(ArgV[4]);
	if (ArgC > 5)
		ConfigName = ArgV[5];
	if (ConfigName == NULL)
		ConfigName = getenv(XIPC_ENVNAME);
	if (ConfigName == NULL)
	{
		printf("demasyn: No configuration specified.\n");
		exit(1);
	}

	sprintf(PidString, "P%04ld", getpid());

	if ((RetCode = XipcLogin(ConfigName, PidString)) < 0)
	{
		printf("demasyn: XipcLogin failed, RetCode = %ld\n", RetCode);
		printf("demasyn: %s\n", XipcError(RetCode));
		exit(1);
	}
	XipcSignal(XIPC_SIG_TERM, DemoTrap, XIPC_SIGA_ACCEPT);
	XipcSignal(XIPC_SIG_ATTN, DemoTrap, XIPC_SIGA_IGNORE);

	SemName = ArgV[1];
	RetCode = SemCreate(SemName, 1L);
	if (RetCode < 0)
	{
		if (RetCode == SEM_ER_DUPLICATE)
		{
			RetCode = SemAccess(SemName);
			if (RetCode < 0)
			{
				printf("demasyn: SemAccess failed, RetCode = %ld\n", RetCode);
				printf("demasyn: %s\n", XipcError(RetCode));
				XipcLogout();
				exit(1);
			}
		}
		else
		{
			printf("demasyn: SemCreate failed, RetCode = %ld\n", RetCode);
			printf("demasyn: %s\n", XipcError(RetCode));
			XipcLogout();
			exit(1);
		}
	}
	Sid = RetCode;
	SemListBuild(SidList, Sid, SEM_EOL);

	TimeStart = time( NULL);

	for (i = 0; i < n; i++)
	{
		if (!FlagMore)
			break;
		FlagAcquire = 0;
		RetCode = SemAcquire(SEM_ALL, SidList, &RetSid, 
				SEM_CALLBACK(CbAcquire, &Acb));
		if (RetCode != SEM_ER_ASYNC)
		{
			printf("demasyn: SemAcquire failed, RetCode = %ld\n", RetCode);
			printf("demasyn: %s\n", XipcError(RetCode));
			break;
		}

		while (!FlagAcquire)
			XipcSleep(1000);

		RetCode = Acb.Api.SemAcquire.RetCode;
		if (RetCode < 0 && RetCode != SEM_ER_CANCEL)
		{
         	printf(
				"demasyn: SemAcquire failed asynchronously, RetCode = %ld\n", 
				RetCode);
			printf("demasyn: %s\n", XipcError(RetCode));
		}

		if (Work1 > 0)
			XipcSleep(Work1);

		RetCode = SemRelease(SidList, &RetSid);
		if (RetCode < 0 && RetCode != SEM_ER_SEMNOTHELD)
		{
			printf("demasyn: SemRelease failed, RetCode = %ld\n", RetCode);
			printf("demasyn: %s\n", XipcError(RetCode));
			break;
		}
		if (Work2 > 0)
			XipcSleep(Work2);
	}

	TimeEnd = time( NULL);

	printf("demasyn: Time = %ld\n", TimeEnd - TimeStart);

	if ((RetCode = XipcLogout()) < 0)
	{
		printf("demasyn: XipcLogout failed, RetCode = %ld\n", RetCode);
		printf("demasyn: %s\n", XipcError(RetCode));
		exit(1);
	}

} /* main */

/*********************************************************************
**
**	Name:	DemoTrap
**	----
**
**	Purpose:	Trap Service Routine
**	-------
**
**	Parmeters:
**	----------
**
**	Returns:
**	-------
**
**	Description:
**	-----------
**	This program services any traps and signals.
**
*********************************************************************/

VOID DemoTrap(XINT SigNum)

{

	XipcSignal(SigNum, DemoTrap, XIPC_SIGA_ACKNOWLEDGE);

	XIPC_TRAP_FUNCTION_TEST(DemoTrap, SigNum)

	FlagMore = 0;

} /* DemoTrap */

/*********************************************************************
**
**	Name:	CbAcquire
**	----
**
**	Purpose:	SemAcquire Callback
**	-------
**
**	Parmeters:
**	----------
**		AcbPtr		Pointer to Acb
**
**	Returns:
**	-------
**
**	Description:
**	-----------
**		This function is invoked when a SemAcquire operation completes.
**		it sets a flag indicating the opration completed.
**
*********************************************************************/

VOID
CbAcquire(AcbPtr)
ASYNCRESULT *AcbPtr;
{

	FlagAcquire = 1;

} /* CbAcquire */
/*
** END **************************************************** DEMASYN.C
*/
