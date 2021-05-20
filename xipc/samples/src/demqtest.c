/********************************** DEMQTEST.C *******************************
**
**	Name:	DEMQTEST.C
**	----
**
**	Title: Xipc QueSys Producer/Consumer Demo
**	-----
**
**	Description:
**	-----------
**	A Producer/Consumer QueSys Demo.  First parameter specifies whether program
**	will produce (QueSend) or consume (QueReceive) messages.
**
**	Parameters:
**	----------
**	Mode		- 'p' or 'c' (producer or consumer).
**	Cycles	- How many iterations to perform.
**	MsgSize - Size of sent messages or receive buffer.
** QueSize	- Msg capacity of created queue.
** Instance - XIPC instance (optional - may be specified via XIPC env var)
**
**	Copyright (C) Momentum Software Corporation, 1997
**
*****************************************************************************/

/* include header files */

#include <stdio.h>
#include <string.h>
#include <time.h>
#ifdef __STDC__
#include <stdlib.h>
#endif
#include "xipc.h"

/* macros & constants */

#define PRODUCER	1
#define CONSUMER	2

/* function prototypes */
#ifndef WIN32
extern CHAR * CDECL getenv();
#endif

/* main */

INT XCDECL
main(ArgC,ArgV)
int ArgC;
CHAR **ArgV;
{
	XINT Mode;
	XINT Cycles;
	XINT MsgSize;
	XINT QueSize;
	CHAR *Instance = NULL;

	XINT i;
	XINT Qid;
	XINT StartTime;
	XINT FinTime;
	XINT RetCode;
	QIDLIST QidList;
	CHAR *Buf;
	CHAR *UserName;

	if (ArgC < 5)
	{
		printf("Usage: %s <mode>{'p'|'c'} <cycles> <msgsize> <quesize> [<instance>]\n", ArgV[0]);
		exit(1);
	}

	if (*ArgV[1] == 'p' || *ArgV[1] == 'P')
	{
		Mode = PRODUCER;
		UserName = "producer";
	}
	else
	{
		Mode = CONSUMER;
		UserName = "consumer";
	}

	Cycles = atol(ArgV[2]);
	MsgSize = atol(ArgV[3]);
	QueSize = atol(ArgV[4]);

	if (ArgC > 5) Instance = ArgV[5];
	else Instance = getenv("XIPC");

	if (Instance == NULL)
	{
		printf("Instance must be specified as parameter or XIPC environment variable\n");
		exit(1);
	}

	printf("Instance = %s\n", Instance);
	printf("Cycles=%d, MsgSize=%d, QueSize=%d, Mode=%s\n",
				Cycles, MsgSize, QueSize, ((Mode==PRODUCER) ? "PRODUCER" : "CONSUMER"));

	Buf = (CHAR *)malloc(MsgSize);
	memset(Buf, 'X', MsgSize);
	Buf[MsgSize-1] = '\0';

	RetCode = XipcLogin(Instance, UserName);
	if (RetCode < 0)
	{
		printf("Login to instance %s failed; RetCode = %d, %s\n",
					Instance, RetCode, XipcError(RetCode) );
		exit(1);
	}

	Qid = QueCreate("qx", QueSize, QUE_NOLIMIT);            /* create queue - if it exists, access it */
	if (Qid == QUE_ER_DUPLICATE)
	{
		Qid = QueAccess("qx");
		if (Qid < 0)
		{
			printf("QueAccess failed; RetCode = %d, %s\n", Qid, XipcError(Qid));
			XipcLogout();
			exit(1);
		}
	}
	else if (Qid < 0)
	{
		printf("QueCreate failed; RetCode = %d, %s\n", Qid, XipcError(Qid));
		XipcLogout();
		exit(1);
	}

	StartTime = time(NULL);

	if (Mode == PRODUCER)
	{
		QueListBuild(QidList, Qid, QUE_EOL);
		for (i = 1; i <= Cycles; i++)
		{
sprintf(Buf, "Message #: %d\n", i);
			RetCode = QueSend(QUE_Q_ANY, QidList, Buf, MsgSize, 99, NULL, QUE_WAIT);
			if (RetCode < 0)
			{
				printf("DemQTest: QueSend failed, Cycle = %d, RetCode = %d, %s\n",
							i, RetCode, XipcError(RetCode));
				break;
			}
		}
	}
	else		/* Mode == CONSUMER */
	{
		QueListBuild(QidList, QUE_M_HP(Qid), QUE_EOL);
		for (i = 1; i <= Cycles; i++)
		{
			RetCode = QueReceive(QUE_Q_ANY, QidList, Buf, MsgSize, NULL, NULL, QUE_WAIT);
			if (RetCode < 0)
			{
				printf("DemQTest: QueReceive failed, Cycle = %d, RetCode = %d, %s\n",
							i, RetCode, XipcError(RetCode));
				break;
			}
		}
	}

	FinTime = time(NULL);
	printf("elapsed time = %ld seconds\n", FinTime - StartTime);

	XipcLogout();

	return(0);

}	/* main */

/*
 ********** END of DEMQTEST.C *********************************************
 */
