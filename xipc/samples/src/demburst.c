/********************************** DEMBURST.C *******************************
**
**	Name:	DEMBURST.C
**	----
**
**	Title: QueBurst Producer Demo
**	-----
**
**	Description:
**	-----------
**	A producer program utilizing QueBurst functionality.  DemQTest should be 
**	used as the consumer.  For comparison purposes (i.e. comparing speeds of 
**	QueSend and QueBurstSend) use a remote network instance or set environment
**	variable XIPCUSENET to force programs to use network.
**
**	Parameters:
**	----------
** Cycles			- How many iterations to perform.
**	MsgSize			- Size of sent messages.
** QueSize			- Msg capacity of created queue.
**	SyncInterval	- How man msgs to Send before doing QueBurstSendSync.
**	ReadAhead		- Size for XIPC QueBurst read-ahead buffer.
** Instance			- XIPC instance (optional - may be specified via XIPC env var).
**
**	Copyright (C) Momentum Software Corporation, 1997
**
*****************************************************************************/

/* include header files */

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "xipc.h"

/* function prototypes */

extern CHAR * getenv();

ASYNCRESULT ErrorAcb;

VOID ErrorFunction(Acb)
ASYNCRESULT *Acb;
{
	printf("got error on BurstSend Seq# %d\n", Acb->Api.QueBurstSend.SeqNo);
}

/* main */

main(ArgC,ArgV)
int ArgC;
CHAR **ArgV;
{
	XINT Cycles;
	XINT MsgSize;
	XINT QueSize;
	XINT SyncInterval;
	XINT ReadAhead;
	CHAR *Instance = NULL;

	CHAR *Buf;
	QIDLIST QidList;
	XINT Qid;
	XINT RetCode;
	XINT StartTime;
	XINT FinTime;
	XINT i;
	XINT MsgsToSend;
	XINT InBurst;

#ifdef WINNT
	HANDLE AsyncIOEventObj;		/* NT event object to wait on for async error report */
#endif

	if (ArgC < 6)
	{
		printf("Usage: %s <cycles> <msgsize> <quesize> <sync-interval> <read-ahead> [<instance>]\n", ArgV[0]);
		exit(1);
	}

	Cycles = atol(ArgV[1]);
	MsgSize = atol(ArgV[2]);
	QueSize = atol(ArgV[3]);
	SyncInterval = atol(ArgV[4]);
	ReadAhead = atol(ArgV[5]);

	if (ArgC > 6) Instance = ArgV[6];
	else Instance = getenv("XIPC");

	if (Instance == NULL)
	{
		printf("Instance must be specified as parameter or XIPC environment variable\n");
		exit(1);
	}

	printf("Instance = %s\n", Instance);
	printf("Cycles=%d, MsgSize=%d, QueSize=%d, SyncInterval=%d, ReadAhead=%d\n",
				Cycles, MsgSize, QueSize, SyncInterval, ReadAhead);

	Buf = (char *)malloc(MsgSize);
	memset(Buf, 'X', MsgSize);
	Buf[MsgSize-1] = '\0';

	RetCode = XipcLogin(Instance, "brstprod");
	if (RetCode < 0)
	{
		printf("Login to instance %s failed; RetCode = %d, %s\n",
					Instance, RetCode, XipcError(RetCode) );
		exit(1);
	}

	Qid = QueCreate("qx", QueSize, QUE_NOLIMIT);
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

#ifdef WINNT
	AsyncIOEventObj = (HANDLE)XipcAsyncIoDescriptor();
#endif

	QueListBuild(QidList, Qid, QUE_EOL);

	StartTime = time(NULL);

	MsgsToSend = Cycles;
	InBurst = 0;

	while (MsgsToSend > 0)
	{
		RetCode = QueBurstSendStart(QUE_Q_ANY, QidList, MsgSize, ReadAhead,
					QUE_NOWAIT, QUE_CALLBACK((XIPC_CALLBACK_FUNC *)ErrorFunction, &ErrorAcb));
		if (RetCode < 0)
		{
			printf("QueBurstSendStart failed, RetCode = %d, %s\n", RetCode, XipcError(RetCode));
			XipcLogout();
			exit(1);
		}
		InBurst = 1;

		for (i = 1; i <= MsgsToSend && InBurst; i++)
		{
			RetCode = QueBurstSend(QUE_NULL_QID, Buf, MsgSize, 99);
			if (RetCode < 0)
			{
				printf("QueBurstSend failed, Cycle = %d, RetCode = %d, %s\n",
							i, RetCode, XipcError(RetCode));
				XipcLogout();
				exit(1);
			}

			if (i%SyncInterval == 0)
			{
				RetCode = QueBurstSendSync(QUE_WAIT);
				if (RetCode < 0)
				{
		   		printf("QueBurstSendSync failed, Cycle = %d, RetCode = %d, %s\n",
								i, RetCode, XipcError(RetCode));
					XipcLogout();
					exit(1);
				}
				if (RetCode != i)
				{
					printf("QueBurstSendSync got SeqNo=%d on Cycle %d; resending lost messages\n",
								RetCode, i);
					MsgsToSend -= (RetCode);
					InBurst = 0;
					RetCode = QueBurstSendStop();
					printf("QueBurstSendStop retcode = %d, %s\n", RetCode, XipcError(RetCode));
					break;
				}
			}
		} /* for loop - sending messages in burst */
		if (InBurst) MsgsToSend=0;
	} /* while loop - starting burst and sending as many as possible */

	RetCode = QueBurstSendStop();

	FinTime = time(NULL);
	printf("elapsed time = %ld seconds\n", FinTime - StartTime);

#ifdef WINNT
	if (WaitForSingleObject(AsyncIOEventObj, 300) == WAIT_OBJECT_0) 
		XipcAsyncEventHandler();
#else
	XipcSleep(300);	/* sleep 300ms (note: this API not documented) */
#endif

	XipcLogout();

	return(0);

} /* main */



/*
 ********** END of DEMBURST.C *********************************************
 */
