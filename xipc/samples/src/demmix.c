/************************************************************ DEMMIX.C
**			 (XIPC - Extended Interprocess Communications Faclilities)
**
**
**	Name:	DEMMIX.C
**	----
**
**	Title:	XIPC Mixed Sub-Systems Demo
**	-----
**
**	Description:
**	-----------
**	This file contains the program that demonstrates an application
**	using all XIPC sub-systems.
**
**
**	Copyright (C) Momentum Software Corporation, 1989
**
*********************************************************************/

/*********************************************************************
** header files
*********************************************************************/

#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <string.h>

#if defined(__DECC) && defined(VMS)
#include unixlib
#endif

#if defined ( __STDC__ ) || defined (DOS)
#include <stdlib.h>
#endif


#include "xipc.h"

/*********************************************************************
** global constants
*********************************************************************/

#define SIZE_SEG	512L
#define SIZE_BUF	512L		/* must 2^n */

#ifndef TRUE
#define TRUE	1
#endif

#ifndef FALSE
#define FALSE	0
#endif

#ifndef MAX
#define MAX(a,b)	( ((a) > (b)) ? (a) : (b) )
#endif

#ifndef ABS
#define ABS(a)	((a) >= 0 ? (a) : -(a))
#endif
/*********************************************************************
** global variables
*********************************************************************/

/*********************************************************************
** functions
*********************************************************************/

#ifndef WIN32
extern CHAR * getenv();
#endif
static VOID SemErrorCheck();
static VOID QueErrorCheck();
static VOID MemErrorCheck();
static VOID DemLogout();

/*********************************************************************
**
**	Name:	Demo (main)
**	----
**
**	Purpose:	XIPC Mixed Sub-Systems Demo
**	-------
**
**	Parmeters:		[Command Line]
**	----------
**		Cycles			Number of cycles
**		Sleep1			Time to sleep between steps (1)
**		Sleep2			Time to sleep between steps (2)
**		Direction		Memory move direction
**		ProdCons		Queue operation type
**		ConfigName		Name of configuration
**
**	Returns:		None.
**	-------
**
**	Description:
**	-----------
**	This program creates avtivity on a MemSys segment.
**
*********************************************************************/

main(ArgC, ArgV)
INT 			ArgC;
CHAR		  **ArgV;
{
	CHAR *ConfigName = NULL;
	CHAR *DirCode;
	CHAR *QueCode;
	CHAR Buf[SIZE_BUF];
	CHAR PidString[16];
	FLAG First;
	MIDLIST MidList;
	QIDLIST gQidList;
	QIDLIST pQidList;
	SECTION RetSec;
	SIDLIST SidList;
	XINT BufLen;
	XINT Cycles;
	XINT i, j;
	XINT Mid;
	XINT Time;
	XINT PrioIn, PrioOut;
	XINT Qid;
	XINT RetCode;
	XINT RetQid;
	XINT RetSid;
	XINT Sid;
	XINT Sleep1;
	XINT Sleep2;
	XINT Start;
	XINT TimeStart, TimeEnd;
	SECTION MemSec;

	if (ArgC < 4)
	{
		printf("Usage: %s <Cycles> <Sleep1> <Sleep2> [R|L|U|D] [P|C] {<Config>}\n",
				ArgV[0]);
		exit(0);
	}
	Cycles = atol(ArgV[1]);
	Sleep1 = atol(ArgV[2]);
	Sleep2 = atol(ArgV[3]);
	DirCode = (char *) ArgV[4];
	QueCode = (char *) ArgV[5];

	if (ArgC > 6)
		ConfigName = ArgV[6];
	if (ConfigName == NULL)
		ConfigName = getenv(XIPC_ENVNAME);
	if (ConfigName == NULL)
	{
		printf("DemMix: No configuration specified.\n");
		exit(1);
	}

	printf("Cycles = %ld\n", Cycles);

	Time = time(NULL);
	sprintf(PidString, "%c%c%06d", *DirCode, *QueCode, Time);

	First = TRUE;

	if ((RetCode = XipcLogin(ConfigName, PidString)) < 0)
	{
		printf("DemMix: XipcLogin failed, RetCode = %ld\n", RetCode);
		exit(1);
	}

	/* *** SEMAPHORES BEGIN *** */

	if ((RetCode = SemCreate("sx", 1L)) < 0)
	{
		if ((RetCode = SemAccess("sx")) < 0)
			SemErrorCheck("SemAcess", RetCode, -1L);
	}
	Sid = RetCode;
	SemListBuild(SidList, Sid, SEM_EOL);

	/* *** SEMAPHORES END *** */

	/* *** QUEUES BEGIN *** */

	if ((RetCode = QueCreate("qx", 20L, 2048L)) < 0)
	{
		if ((RetCode = QueAccess("qx")) < 0)
			QueErrorCheck("QueAcess", RetCode, -1L);
	}
	Qid = RetCode;
	QueListBuild(pQidList, Qid, QUE_EOL);
	QueListBuild(gQidList, QUE_M_HP(Qid), QUE_EOL);
	PrioOut = ABS((Time % 1000) + 1);
	srand((INT)Time);

	/* *** QUEUES END *** */

	/* *** MEMORY BEGIN *** */

	if ((RetCode = MemCreate("mx", SIZE_SEG)) < 0)
	{
		if ((RetCode = MemAccess("mx")) < 0)
			MemErrorCheck("MemAcess", RetCode, MemSection(-1L, 0L, 0L));
	}
	Mid = RetCode;
	MemSec = MemSection(Mid, 0L, SIZE_SEG);
	MemListBuild(MidList, MemSec, MEM_EOL);
	if ((RetCode = MemLock(MEM_ALL, MidList, &MemSec, MEM_NOWAIT)) == 0)
	{
		if ((RetCode = MemWrite(Mid, 0L, SIZE_SEG,
				MEM_FILL('-'), MEM_WAIT)) < 0)
		{
			MemErrorCheck("MemWrite", RetCode, MemSec);
		}
		if ((RetCode = MemUnlock(MidList, &MemSec)) == 0)
		{
			MemErrorCheck("MemUnlock", RetCode, MemSec);
		}
	}

	/* *** MEMORY END *** */

	TimeStart = time( NULL);

	for (j = 0; j < Cycles; j++)
	{
		/* *** SEMAPHORES BEGIN *** */

		RetCode = SemAcquire(SEM_ALL, SidList, &RetSid, SEM_WAIT);
		if (RetCode < 0 && RetCode != SEM_ER_CANCEL)
			SemErrorCheck("SemAcquire", RetCode, RetSid);

		if (Sleep1 > 0)
			XipcSleep(Sleep1);

		RetCode = SemRelease(SidList, &RetSid);
		if (RetCode < 0 && RetCode != SEM_ER_SEMNOTHELD)
			SemErrorCheck("SemRelease", RetCode, RetSid);

		if (Sleep2 > 0)
			XipcSleep(Sleep2);

		/* *** SEMAPHORES END *** */

		/* *** QUEUES BEGIN *** */

		switch (*QueCode)
		{

		case 'P':
		case 'p':

			memset(Buf, 0, (size_t)SIZE_BUF);
			sprintf(Buf, "%s-%010ld", PidString, j);
			BufLen = (XINT)rand() & (SIZE_BUF - 1L);
			BufLen = MAX((BufLen + 1L), ((XINT)strlen(Buf)));
			RetCode = QueSend(QUE_Q_ANY, pQidList, Buf, BufLen,
					PrioOut, &RetQid, QUE_WAIT);
			if (RetCode < 0)
				QueErrorCheck("QueSend", RetCode, RetQid);
			if (Sleep1 > 0)
				XipcSleep(Sleep1);
			break;

		case 'C':
		case 'c':

			RetCode = QueReceive(QUE_Q_HP, gQidList, Buf, (XINT)sizeof(Buf),
					&PrioIn, &RetQid, QUE_WAIT);
			if (RetCode < 0)
				QueErrorCheck("QueReceive", RetCode, RetQid);
			if (Sleep2 > 0)
				XipcSleep(Sleep2);
			break;

		default:
			break;

		}

		/* *** QUEUES END *** */

		/* *** MEMORY BEGIN *** */

		switch (*DirCode)
		{

		case 'R':
		case 'r':

			if (First)
			{
				Start = i = 0;
				First = 0;
			}
			MemListBuild(MidList,
					MemSection(Mid, (XINT) i, 5L), MEM_EOL);
			RetCode = MemLock(MEM_ALL,
					MidList,
					&RetSec,
					MEM_WAIT
					);
			MemErrorCheck("MemLock", RetCode, RetSec);

			RetCode = MemWrite(Mid, (XINT) i, 5L, " XIPC", MEM_WAIT);
			RetSec = MemSection(Mid, (XINT) i, 5L);
			MemErrorCheck("MemWrite", RetCode, RetSec);

			MemListBuild(MidList,
					MemSection(Mid, (XINT) i, 5L), MEM_EOL);
			RetCode = MemUnlock(
					MidList,
					&RetSec
					);
			MemErrorCheck("MemUnlock", RetCode, RetSec);

			XipcSleep(Sleep1);

			i++;
			if (i >= SIZE_SEG - 5L)
			{
				MemListBuild(MidList,
						MemSection(Mid, (XINT) (i - 1), 5L), MEM_EOL);
				RetCode = MemLock(MEM_ALL,
						MidList,
						&RetSec,
						MEM_WAIT
						);
				MemErrorCheck("MemLock", RetCode, RetSec);

				RetCode = MemWrite(Mid, (XINT) (i - 1), 5L, "     ", MEM_WAIT);
				RetSec = MemSection(Mid, (XINT) (i - 1), 5L);
				MemErrorCheck("MemWrite", RetCode, RetSec);

				MemListBuild(MidList,
						MemSection(Mid, (XINT) (i - 1), 5L), MEM_EOL);
				RetCode = MemUnlock(
						MidList,
						&RetSec
						);
				MemErrorCheck("MemUnlock", RetCode, RetSec);

				i = 0;
			}
			break;

		case 'L':
		case 'l':

			if (First)
			{
				Start = i = 0;
				First = 0;
			}
			MemListBuild(MidList,
					MemSection(Mid, (XINT) i, 5L), MEM_EOL);
			RetCode = MemLock(MEM_ALL,
					MidList,
					&RetSec,
					MEM_WAIT
					);
			MemErrorCheck("MemLock", RetCode, RetSec);

			RetCode = MemWrite(Mid, (XINT) i, 5L, "xipc ", MEM_WAIT);
			RetSec = MemSection(Mid, (XINT) i, 5L);
			MemErrorCheck("MemWrite", RetCode, RetSec);

			MemListBuild(MidList,
					MemSection(Mid, (XINT) i, 5L), MEM_EOL);
			RetCode = MemUnlock(
					MidList,
					&RetSec
					);
			MemErrorCheck("MemUnlock", RetCode, RetSec);

			XipcSleep(Sleep1);

			i--;
			if (i < 0)
			{
				MemListBuild(MidList,
						MemSection(Mid, 0L, 5L), MEM_EOL);
				RetCode = MemLock(MEM_ALL,
						MidList,
						&RetSec,
						MEM_WAIT
						);
				MemErrorCheck("MemLock", RetCode, RetSec);

				MemWrite(Mid, 0L, 5L, "     ", MEM_WAIT);
				RetSec = MemSection(Mid, 0L, 5L);
				MemErrorCheck("MemWrite", RetCode, RetSec);

				MemListBuild(MidList,
						MemSection(Mid, 0L, 5L), MEM_EOL);
				RetCode = MemUnlock(
						MidList,
						&RetSec
						);
				MemErrorCheck("MemUnlock", RetCode, RetSec);

				i = SIZE_SEG - 5L;
			}
			break;

		case 'D':
		case 'd':

			if (First)
			{
				Start = i = 0;
				First = 0;
			}
			MemListBuild(MidList,
					MemSection(Mid, (XINT) i, 4L), MEM_EOL);
			RetCode = MemLock(MEM_ALL,
					MidList,
					&RetSec,
					MEM_WAIT
					);
			MemErrorCheck("MemLock", RetCode, RetSec);

			RetCode = MemWrite(Mid, (XINT) i, 4L, "XIPC", MEM_WAIT);
			RetSec = MemSection(Mid, (XINT) i, 4L);
			MemErrorCheck("MemWrite", RetCode, RetSec);

			MemListBuild(MidList,
					MemSection(Mid, (XINT) i, 4L), MEM_EOL);
			RetCode = MemUnlock(
					MidList,
					&RetSec
					);
			MemErrorCheck("MemUnlock", RetCode, RetSec);

			XipcSleep(Sleep1);

			MemListBuild(MidList,
					MemSection(Mid, (XINT) i, 4L), MEM_EOL);
			RetCode = MemLock(MEM_ALL,
					MidList,
					&RetSec,
					MEM_WAIT
					);
			MemErrorCheck("MemLock", RetCode, RetSec);

			RetCode = MemWrite(Mid, (XINT) i, 4L, "    ", MEM_WAIT);
			RetSec = MemSection(Mid, (XINT) i, 4L);
			MemErrorCheck("MemWrite", RetCode, RetSec);

			MemListBuild(MidList,
					MemSection(Mid, (XINT) i, 4L), MEM_EOL);
			RetCode = MemUnlock(
					MidList,
					&RetSec
					);
			MemErrorCheck("MemUnlock", RetCode, RetSec);

			i += 20;
			if (i >= SIZE_SEG - 8L)
			{
				MemListBuild(MidList,
						MemSection(Mid, (XINT) (i - 20), 4L), MEM_EOL);
				RetCode = MemLock(MEM_ALL,
						MidList,
						&RetSec,
						MEM_WAIT
						);
				MemErrorCheck("MemLock", RetCode, RetSec);

				RetCode = MemWrite(Mid, (XINT) (i - 20), 4L, "    ", MEM_WAIT);
				RetSec = MemSection(Mid, (XINT) (i - 20), 4L);
				MemErrorCheck("MemWrite", RetCode, RetSec);

				MemListBuild(MidList,
						MemSection(Mid, (XINT) (i - 20), 4L), MEM_EOL);
				RetCode = MemUnlock(
						MidList,
						&RetSec
						);
				MemErrorCheck("MemUnlock", RetCode, RetSec);

				Start++;
				if (Start == 17)
					Start = 0;
				i = Start;
			}
			break;

		case 'U':
		case 'u':

			if (First)
			{
				Start = i = SIZE_SEG - 8L;
				First = 0;
			}
			MemListBuild(MidList,
					MemSection(Mid, (XINT) i, 4L), MEM_EOL);
			RetCode = MemLock(MEM_ALL,
					MidList,
					&RetSec,
					MEM_WAIT
					);
			MemErrorCheck("MemLock", RetCode, RetSec);

			RetCode = MemWrite(Mid, (XINT) i, 4L, "xipc", MEM_WAIT);
			RetSec = MemSection(Mid, (XINT) i, 4L);
			MemErrorCheck("MemWrite", RetCode, RetSec);

			MemListBuild(MidList,
					MemSection(Mid, (XINT) i, 4L), MEM_EOL);
			RetCode = MemUnlock(
					MidList,
					&RetSec
					);
			MemErrorCheck("MemUnlock", RetCode, RetSec);

			XipcSleep(Sleep1);

			MemListBuild(MidList,
					MemSection(Mid, (XINT) i, 4L), MEM_EOL);
			RetCode = MemLock(MEM_ALL,
					MidList,
					&RetSec,
					MEM_WAIT
					);
			MemErrorCheck("MemLock", RetCode, RetSec);

			RetCode = MemWrite(Mid, (XINT) i, 4L, "    ", MEM_WAIT);
			RetSec = MemSection(Mid, (XINT) i, 4L);
			MemErrorCheck("MemWrite", RetCode, RetSec);

			MemListBuild(MidList,
					MemSection(Mid, (XINT) i, 4L), MEM_EOL);
			RetCode = MemUnlock(
					MidList,
					&RetSec
					);
			MemErrorCheck("MemUnlock", RetCode, RetSec);

			i -= 20;
			if (i < 0)
			{
				MemListBuild(MidList,
						MemSection(Mid, 0L, 4L), MEM_EOL);
				RetCode = MemLock(MEM_ALL,
						MidList,
						&RetSec,
						MEM_WAIT
						);
				MemErrorCheck("MemLock", RetCode, RetSec);

				MemWrite(Mid, 0L, 4L, "    ", MEM_WAIT);
				RetSec = MemSection(Mid, 0L, 4L);
				MemErrorCheck("MemWrite", RetCode, RetSec);

				MemListBuild(MidList,
						MemSection(Mid, 0L, 4L), MEM_EOL);
				RetCode = MemUnlock(
						MidList,
						&RetSec
						);
				MemErrorCheck("MemUnlock", RetCode, RetSec);

				Start--;
				if (Start == SIZE_SEG - 25L)
					Start = SIZE_SEG - 8L;
				i = Start;
			}
			break;

		default:
			break;

		}

		/* *** MEMORY END *** */

	}

	TimeEnd = time( NULL);

	printf("DemMix: Time = %ld\n", TimeEnd - TimeStart);

	if ((RetCode = XipcLogout()) < 0)
	{
		printf("DemMix: XipcLogout failed, RetCode = %ld\n", RetCode);
	}

	return(0);

} /* main */

/*********************************************************************
**
**	Name:	SemErrorCheck
**	----
**
**	Purpose:	SemSys Error Checking Service Routine
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
**	This functions checks for errors and reports them.
**
*********************************************************************/

static VOID
SemErrorCheck(FunctionName, RetCode, Sid)
CHAR		   *FunctionName;
XINT			RetCode;
XINT			Sid;

{

	if (RetCode < 0)
	{
		fprintf(stderr, "DemMix: %s failed, RetCode = %ld, Sid = %ld\n",
				FunctionName, RetCode, Sid);
		DemLogout();
	}

} /* SemErrorCheck */

/*********************************************************************
**
**	Name:	QueErrorCheck
**	----
**
**	Purpose:	SemSys Error Checking Service Routine
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
**	This functions checks for errors and reports them.
**
*********************************************************************/

static VOID
QueErrorCheck(FunctionName, RetCode, Qid)
CHAR		   *FunctionName;
XINT			RetCode;
XINT			Qid;

{

	if (RetCode < 0)
	{
		fprintf(stderr, "DemMix: %s failed, RetCode = %ld, Qid = %ld\n",
				FunctionName, RetCode, Qid);
		DemLogout();
	}

} /* QueErrorCheck */

/*********************************************************************
**
**	Name:	MemErrorCheck
**	----
**
**	Purpose:	MemSys Error Checking Service Routine
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
**	This functions checks for errors and reports them.
**
*********************************************************************/

static VOID
MemErrorCheck(FunctionName, RetCode, Section)
CHAR		   *FunctionName;
XINT			RetCode;
SECTION 		Section;

{

	if (RetCode < 0)
	{
		fprintf(stderr,
				"DemMix: %s failed, RetCode = %ld, Section = (%ld %ld %ld)\n",
				FunctionName, RetCode,
				Section.Mid, Section.Offset, Section.Size);
		DemLogout();
	}

} /* MemErrorCheck */

/*********************************************************************
**
**	Name:	DemLogout
**	----
**
**	Purpose:	MemSys Error Checking Service Routine
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
**	This functions checks for errors and reports them.
**
*********************************************************************/

static VOID
DemLogout()

{
	XINT RetCode;

	if ((RetCode = XipcLogout()) < 0)
		printf("\nDemMix: XipcLogout failed. RetCode = %ld.\n",
				RetCode);
	else
		printf("\nDemMix: XipcLogout succeeded.\n");

	exit(255);

} /* DemLogout */

