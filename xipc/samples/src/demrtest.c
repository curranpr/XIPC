/********************************** DEMRTEST.C *******************************
**
**	Name:	DEMRTEST.C
**	----
**
**	Title: Resource Semaphore Demo
**	-----
**
**	Description:
**	-----------
**	Repeatedly acquires and releases a resource semaphore.	Sleeps 'Work1' ms
**	while holding the resource semaphore, and sleeps 'Work2' ms after releasing
**	it before attempting to acquire it again.  Multiple copies of this rogram
**	may be run simultaneously to simulate processes competing for a non-sharable
**	resource. Use semview to view the activity.
**	Note: XipcSleep(ms) is used.  This causes process to sleep for specified
**	number of milliseconds.  (This function is not documented).
**
**	Parameters:
**	----------
** SemName			- Name of resource semaphore to create or access.
** Cycles			- How many iterations to perform.
**	Work1				- Time to sleep (ms) once semaphore has been acquired.
**	Work2				- Time to sleep (ms) after semaphore has been released.
** Instance 		- XIPC instance (optional - may be specified via XIPC env var).
**
**	Copyright (C) Momentum Software Corporation, 1997
**
*****************************************************************************/

/* include header files */

#include <stdio.h>
#include <signal.h>
#include <time.h>
#if defined (__DECC) && defined(VMS)
#include <unixlib>
#endif
#ifdef __STDC__
#include <stdlib.h>
#endif
#include "xipc.h"

#ifdef WIN32
#define getpid GetCurrentProcessId
#endif

#ifndef WIN32
extern CHAR * CDECL getenv();
#endif


INT XCDECL
main(ArgC, ArgV)
INT ArgC;
CHAR **ArgV;
{
	CHAR *SemName;
	XINT Cycles;
	XINT Work1;
	XINT Work2;
	CHAR *Instance = NULL;

	CHAR PidString[16];
	XINT Sid;
	SIDLIST SidList;
	XINT i;
	XINT RetCode;
	XINT RetSid;
	XINT StartTime, FinTime;

	if (ArgC < 5)
	{
		printf("Usage: %s <sem> <cycles> <work1> <work2> [<instance>]\n", ArgV[0]);
		exit(1);
	}
	SemName = ArgV[1];
	Cycles = atol(ArgV[2]);
	Work1 = atol(ArgV[3]);
	Work2 = atol(ArgV[4]);


	if (ArgC > 5) Instance = ArgV[5];
	else Instance = getenv("XIPC");

	if (Instance == NULL)
	{
		printf("Instance must be specified as parameter or XIPC environment variable\n");
		exit(1);
	}

	printf("Instance = %s\n", Instance);
	printf("SemName=%s, Cycles=%d Work1=%d ms., Work2=%d ms.\n",
				SemName, Cycles, Work1, Work2);

	sprintf(PidString, "P%04ld", getpid());

	RetCode = XipcLogin(Instance, PidString);
	if (RetCode < 0)
	{
		printf("Login to instance %s failed; RetCode = %d, %s\n",
					Instance, RetCode, XipcError(RetCode) );
		exit(1);
	}

	/* create new, or access existing, resource semaphore */
	Sid = SemCreate(SemName, 1);
	if (Sid == SEM_ER_DUPLICATE)
	{
		Sid = SemAccess(SemName);
		if (Sid < 0)
		{
			printf("SemAccess failed; RetCode = %d, %s\n", Sid, XipcError(Sid));
			XipcLogout();
			exit(1);
		}
	}
	else if (Sid < 0)
	{
		printf("SemCreate failed; RetCode = %d, %s\n", Sid, XipcError(Sid));
		XipcLogout();
		exit(1);
	}

	SemListBuild(SidList, Sid, SEM_EOL);

	StartTime = time(NULL);

	for (i = 0; i < Cycles; i++)
	{
		RetCode = SemAcquire(SEM_ALL, SidList, &RetSid, SEM_WAIT);
		if (RetCode < 0)
		{
			printf("DemRTest: SemAcquire failed, RetCode = %d, %s\n",
						RetCode, XipcError(RetCode));
			break;
		}
		if (Work1 > 0) XipcSleep(Work1);
		RetCode = SemRelease(SidList, &RetSid);
		if (RetCode < 0)
		{
			printf("DemRTest: SemRelease failed, RetCode = %d, %s\n",
						RetCode, XipcError(RetCode));
			break;
		}
		if (Work2 > 0) XipcSleep(Work2);
	}

	FinTime = time(NULL);

	printf("DemRTest: Time = %ld\n", FinTime - StartTime);

	XipcLogout();

	return(0);

} /* main */


/*
 ********** END of DEMRTEST.C *********************************************
 */
