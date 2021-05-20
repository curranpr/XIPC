/********************************************************** DEMWATCH.C
**											  (MemSys - Memory System)
**
**
**	Name:	DEMWATCH.C
**	----
**
**	Title:	 MemSys Watch Demo
**	-----
**
**	Description:
**	-----------
**	This file contains the program that demonstrates the 'watch'
**	facility of the Memory System.
**
**
**
**	Copyright (C) Momentum Software Corporation, 1997
**
*********************************************************************/

/*********************************************************************
** header files
*********************************************************************/

#include <stdio.h>
#include <signal.h>
#ifdef __STDC__
#include <stdlib.h>
#endif
#include "xipc.h"

/*********************************************************************
** global constants
*********************************************************************/

#define SIZE_SEG	512L

#ifndef TRUE
#define TRUE	1
#endif

#ifndef FALSE
#define FALSE	0
#endif

/*********************************************************************
** global variables
*********************************************************************/

XINT FlagMore = 1;

/*********************************************************************
** functions
*********************************************************************/

#ifndef WIN32
extern CHAR * getenv();
#endif
static VOID ErrorCheck ();


/*********************************************************************
**
**	Name:		Demo (main)
**	----
**
**	Purpose:	MemSys Watch Demo
**	-------
**
**	Parmeters:		[Command Line]
**	----------
**		SegName 		Name of MemSys Segement
**		DirCode 		Indication of direction to move (L, R, U, D)
**		Sleep			Period to sleep between loops
**		Lock			Indication if locking required (L) or not (-)
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

INT XCDECL
main(ArgC, ArgV)
INT ArgC;
CHAR **ArgV;
{

	CHAR *ConfigName = NULL;
	CHAR *DirCode;
	CHAR *LockStr;
	CHAR *SegName;
	FLAG FlagLock;
	MIDLIST MidList;
	SECTION RetSec;
	XINT FlagBatch = 0;
	XINT i;
	XINT Mid;
	XINT RetCode = 0;
	XINT SleepTime;
	XINT Start;

	if (ArgC < 6)
	{
		printf("Usage: demwatch <SegName> (R|L|U|D) <Sleep> (L|N) <Config>\n");
		exit(1);
	}
	SegName = ArgV[1];
	DirCode = ArgV[2];
	SleepTime = atol(ArgV[3]);
	LockStr = ArgV[4];
	if (ArgC > 5)
		ConfigName = ArgV[5];
	if (ConfigName == NULL)
		ConfigName = getenv(XIPC_ENVNAME);
	if (ConfigName == NULL)
	{
		printf("DemWatch: No configuration specified.\n");
		exit(1);
	}

	if (*LockStr == 'L' || *LockStr == 'l')
		FlagLock = TRUE;
	else
		FlagLock = FALSE;

	RetSec.Mid = RetSec.Offset = RetSec.Size = (XINT)0;
	RetCode = XipcLogin(ConfigName, "DemWatch");
	ErrorCheck("XipcLogin", RetCode, RetSec);

	RetCode = MemCreate(SegName, SIZE_SEG);
	if (RetCode < 0)
	{
		if (RetCode == MEM_ER_DUPLICATE)
		{
			RetCode = MemAccess(SegName);
			if (RetCode < 0)
			{
				fprintf(stderr,
						"DemWatch: MemAccess failed, RetCode = %ld\n", RetCode);
				fprintf(stderr, "DemWatch: %s\n", XipcError(RetCode));
				XipcLogout();
				exit(1);
			}
		}
		else
		{
			fprintf(stderr,
					"DemWatch: MemCreate failed, RetCode = %ld\n", RetCode);
			fprintf(stderr, "DemWatch: %s\n", XipcError(RetCode));
			XipcLogout();
			exit(1);
		}
	}
	Mid = RetCode;

	if (!FlagLock)
	{
		if (MemSecDef(MemSection(Mid, 0L, SIZE_SEG)) == 0)
			MemWrite(Mid, 0L, SIZE_SEG, MEM_FILL(' '), MEM_TIMEOUT(60));
	}
	if (*DirCode == 'R' || *DirCode == 'r')
	{
		while (FlagMore)
		{
			for (i = 0; FlagMore && (i < SIZE_SEG - 5L); i++)
			{
				if (FlagLock)
				{
					MemListBuild(MidList,MemSection(Mid, (XINT) i, 5L), MEM_EOL);
					RetCode = MemLock(MEM_ALL,MidList,&RetSec,MEM_WAIT);
					ErrorCheck("MemLock", RetCode, RetSec);
				}
				RetCode = MemWrite(Mid, (XINT) i, 5L, " XIPC", MEM_WAIT);
				RetSec = MemSection(Mid, (XINT) i, 5L);
				ErrorCheck("MemWrite", RetCode, RetSec);

				if (FlagLock)
				{
					MemListBuild(MidList,MemSection(Mid, (XINT) i, 5L), MEM_EOL);
					RetCode = MemUnlock(MidList,&RetSec);
					ErrorCheck("MemUnlock", RetCode, RetSec);
				}
				XipcSleep(SleepTime);
			}

			if (FlagLock)
			{
				MemListBuild(MidList,MemSection(Mid, (XINT) (i - 1), 5L), MEM_EOL);
				RetCode = MemLock(MEM_ALL,MidList,&RetSec,MEM_WAIT);
				ErrorCheck("MemLock", RetCode, RetSec);
			}
			RetCode = MemWrite(Mid, (XINT) (i - 1), 5L, "     ", MEM_WAIT);
			RetSec = MemSection(Mid, (XINT) (i - 1), 5L);
			ErrorCheck("MemWrite", RetCode, RetSec);

			if (FlagLock)
			{
				MemListBuild(MidList,MemSection(Mid, (XINT) (i - 1), 5L), MEM_EOL);
				RetCode = MemUnlock(MidList,&RetSec);
				ErrorCheck("MemUnlock", RetCode, RetSec);
			}
		}
	}
	else if (*DirCode == 'D' || *DirCode == 'd')
	{
		Start = 0;
		while (FlagMore)
		{
			for (i = Start; FlagMore && (i < SIZE_SEG - 8L); i += 20)
			{
				if (FlagLock)
				{
					MemListBuild(MidList,MemSection(Mid, (XINT) i, 4L), MEM_EOL);
					RetCode = MemLock(MEM_ALL,MidList,&RetSec,MEM_WAIT);
					ErrorCheck("MemLock", RetCode, RetSec);
				}
				RetCode = MemWrite(Mid, (XINT) i, 4L, "XIPC", MEM_WAIT);
				RetSec = MemSection(Mid, (XINT) i, 4L);
				ErrorCheck("MemWrite", RetCode, RetSec);

				if (FlagLock)
				{
					MemListBuild(MidList,MemSection(Mid, (XINT) i, 4L), MEM_EOL);
					RetCode = MemUnlock(MidList,&RetSec);
					ErrorCheck("MemUnlock", RetCode, RetSec);
				}
				XipcSleep(SleepTime);

				if (FlagLock)
				{
					MemListBuild(MidList,MemSection(Mid, (XINT) i, 4L), MEM_EOL);
					RetCode = MemLock(MEM_ALL,MidList,&RetSec,MEM_WAIT);
					ErrorCheck("MemLock", RetCode, RetSec);
				}
				RetCode = MemWrite(Mid, (XINT) i, 4L, "    ", MEM_WAIT);
				RetSec = MemSection(Mid, (XINT) i, 4L);
				ErrorCheck("MemWrite", RetCode, RetSec);

				if (FlagLock)
				{
					MemListBuild(MidList,MemSection(Mid, (XINT) i, 4L), MEM_EOL);
					RetCode = MemUnlock(MidList,&RetSec);
					ErrorCheck("MemUnlock", RetCode, RetSec);
				}
			}

			if (FlagLock)
			{
				MemListBuild(MidList,MemSection(Mid, (XINT) (i - 20), 4L), MEM_EOL);
				RetCode = MemLock(MEM_ALL,MidList,&RetSec,MEM_WAIT);
				ErrorCheck("MemLock", RetCode, RetSec);
			}
			RetCode = MemWrite(Mid, (XINT) (i - 20), 4L, "    ", MEM_WAIT);
			RetSec = MemSection(Mid, (XINT) (i - 20), 4L);
			ErrorCheck("MemWrite", RetCode, RetSec);

			if (FlagLock)
			{
				MemListBuild(MidList,MemSection(Mid, (XINT) (i - 20), 4L), MEM_EOL);
				RetCode = MemUnlock(MidList,&RetSec);
				ErrorCheck("MemUnlock", RetCode, RetSec);
			}
			Start++;
			if (Start == 17)
				Start = 0;
		}
	}
	else if (*DirCode == 'L' || *DirCode == 'l')
	{
		while (FlagMore)
		{
			for (i = SIZE_SEG - 5L; FlagMore && (i >= 0); i--)
			{
				if (FlagLock)
				{
					MemListBuild(MidList, MemSection(Mid, (XINT) i, 5L), MEM_EOL);
					RetCode = MemLock(MEM_ALL, MidList, &RetSec, MEM_WAIT);
					ErrorCheck("MemLock", RetCode, RetSec);
				}
				RetCode = MemWrite(Mid, (XINT) i, 5L, "xipc ", MEM_WAIT);
				RetSec = MemSection(Mid, (XINT) i, 5L);
				ErrorCheck("MemWrite", RetCode, RetSec);

				if (FlagLock)
				{
					MemListBuild(MidList, MemSection(Mid, (XINT) i, 5L), MEM_EOL);
					RetCode = MemUnlock(MidList,&RetSec);
					ErrorCheck("MemUnlock", RetCode, RetSec);
				}
				XipcSleep(SleepTime);
			}

			if (FlagLock)
			{
				MemListBuild(MidList,MemSection(Mid, 0L, 5L), MEM_EOL);
				RetCode = MemLock(MEM_ALL,MidList,&RetSec,MEM_WAIT);
				ErrorCheck("MemLock", RetCode, RetSec);
			}
			MemWrite(Mid, 0L, 5L, "     ", MEM_WAIT);
			RetSec = MemSection(Mid, 0L, 5L);
			ErrorCheck("MemWrite", RetCode, RetSec);

			if (FlagLock)
			{
				MemListBuild(MidList,MemSection(Mid, 0L, 5L), MEM_EOL);
				RetCode = MemUnlock(MidList, &RetSec);
				ErrorCheck("MemUnlock", RetCode, RetSec);
			}
		}
	}
	else if (*DirCode == 'U' || *DirCode == 'u')
	{
		Start = SIZE_SEG - 8L;

		while (FlagMore)
		{
			for (i = Start; FlagMore && (i >= 0); i -= 20)
			{
				if (FlagLock)
				{
					MemListBuild(MidList,MemSection(Mid, (XINT) i, 4L), MEM_EOL);
					RetCode = MemLock(MEM_ALL,MidList,&RetSec,MEM_WAIT);
					ErrorCheck("MemLock", RetCode, RetSec);
				}
				RetCode = MemWrite(Mid, (XINT) i, 4L, "xipc", MEM_WAIT);
				RetSec = MemSection(Mid, (XINT) i, 4L);
				ErrorCheck("MemWrite", RetCode, RetSec);

				if (FlagLock)
				{
					MemListBuild(MidList,MemSection(Mid, (XINT) i, 4L), MEM_EOL);
					RetCode = MemUnlock(MidList,&RetSec);
					ErrorCheck("MemUnlock", RetCode, RetSec);
				}
				XipcSleep(SleepTime);

				if (FlagLock)
				{
					MemListBuild(MidList, MemSection(Mid, (XINT) i, 4L), MEM_EOL);
					RetCode = MemLock(MEM_ALL,MidList,&RetSec,MEM_WAIT);
					ErrorCheck("MemLock", RetCode, RetSec);
				}
				RetCode = MemWrite(Mid, (XINT) i, 4L, "    ", MEM_WAIT);
				RetSec = MemSection(Mid, (XINT) i, 4L);
				ErrorCheck("MemWrite", RetCode, RetSec);

				if (FlagLock)
				{
					MemListBuild(MidList, MemSection(Mid, (XINT) i, 4L), MEM_EOL);
					RetCode = MemUnlock(MidList, &RetSec);
					ErrorCheck("MemUnlock", RetCode, RetSec);
				}
			}

			if (FlagLock)
			{
				MemListBuild(MidList,MemSection(Mid, 0L, 4L), MEM_EOL);
				RetCode = MemLock(MEM_ALL,MidList,&RetSec,MEM_WAIT);
				ErrorCheck("MemLock", RetCode, RetSec);
			}
			MemWrite(Mid, 0L, 4L, "    ", MEM_WAIT);
			RetSec = MemSection(Mid, 0L, 4L);
			ErrorCheck("MemWrite", RetCode, RetSec);

			if (FlagLock)
			{
				MemListBuild(MidList,MemSection(Mid, 0L, 4L), MEM_EOL);
				RetCode = MemUnlock(MidList,&RetSec);
				ErrorCheck("MemUnlock", RetCode, RetSec);
			}
			Start--;
			if (Start == SIZE_SEG - 25L) Start = SIZE_SEG - 8L;
		}
	}
	if ((RetCode = XipcLogout()) < 0)
	{
		fprintf(stderr,
			"\nDemWatch: XipcLogout failed. RetCode = %ld.\n", RetCode);
		fprintf(stderr, "DemWatch: %s\n", XipcError(RetCode));
	}
	else
		printf("\nDemWatch: XipcLogout succeeded.\n");

	return(0);

} /* main */

/*********************************************************************
**
**	Name:	ErrorCheck
**	----
**
**	Purpose:	Error Checking Service Routine
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
ErrorCheck(FunctionName, RetCode, Section)
CHAR		   *FunctionName;
XINT			RetCode;
SECTION 		Section;

{

	if (RetCode < 0)
	{
		fprintf(stderr, "DemWatch: %s failed, RetCode = %ld, Section = (%ld %ld %ld)\n",
				FunctionName, RetCode,
				Section.Mid, Section.Offset, Section.Size);
		fprintf(stderr, "DemWatch: %s\n", XipcError(RetCode));
		if ((RetCode = XipcLogout()) < 0)
		{
			fprintf(stderr, "\nDemWatch: XipcLogout failed. RetCode = %ld.\n",
					RetCode);
			fprintf(stderr, "DemWatch: %s\n", XipcError(RetCode));
		}
		else
			printf("\nDemWatch: XipcLogout succeeded.\n");
		exit(255);
	}
}				/* ErrorCheck */

/*
 * END **************************************************** DEMWATCH.C
 */
