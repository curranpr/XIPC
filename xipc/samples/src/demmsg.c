/********************************** DEMMSG.C *********************************
**
**	Name:	DEMMSG.C
**	----
**
**	Title: QueSys Message Producer/Consumer
**	-----
**
**	Description:
**	-----------
**
**	Parameters:
**	----------
**	Mode		- 'p' or 'c' (producer or consumer).
** Queues	- Number of queues (6 Max)
**	Messages	- Total number of messages to produce or consume.
**	Sleep		- Time to wait between producing/consuming messages.
** Instance - XIPC instance (optional - may be specified via XIPC env var).
**
**	Copyright (C) Momentum Software Corporation, 1997
**
*****************************************************************************/


#include <stdio.h>
#include <ctype.h>
#include <time.h>
#include <string.h>
#include <signal.h>
#ifdef __STDC__
#include <stdlib.h>
#endif
#include "xipc.h"


/* macros & constants */

#define PRODUCER	1
#define CONSUMER	2

#define  DATASIZE	24


CHAR *Data[] =
{
	"A * 94 7/8 +1/2",
	"APPL 34 3/4 -3/8 - Dividends reported",
	"APPL 35 3/4 -1/2",
	"AUD * 24 1/16 -1/8",
	"CCI * 94 7/8 +1/2",
	"DEC * 95 1/8 +1/4",
	"DEC * 94 7/8 +1/2",
	"DG * 24 1/16 -1/8",
	"DG 23 7/16 UNCH",
	"DIS 102 1/8 +3/8",
	"DIS 102 1/8 +3/8",
	"GE 104 5/8 -1/4 - Patent Approval for Water Powered Engine",
	"IBM 102 1/8 +3/8 - See news dated 12/7/90",
	"IBM 104 5/8 -1/4",
	"K * 94 7/8 +1/2",
	"MCIC 34 3/4 -3/8",
	"MER 23 5/16 UNCH - Earnings report delayed",
	"MER 23 7/16 UNCH - Layoffs announced",
	"N * 24 1/16 -1/8",
	"P 34 3/4 -3/8 - Possible buyout by UAL",
	"T 34 3/4 -3/8",
	"DJ - Dec. 9, 1989:  GCI has reportedly filed suit in Federal Court \
against USTT for infrigment of business and false marketing tactics. The \
suit claims that these practices have resulted in unspecified damages to \
GCI's international marketing. 2:37 PM.",
	"WSJ - Dec. 10, 1989:  HAL has reported a 4th Quater profit of $1.8 \
billion. The latest report comes amid growing approval of the companies \
ability to adjust to the changing computer market. 11:13 AM.",
	"Reuters - Dec. 11, 1989:  in an effort to promote its UX and PM \
operating system envioronments, HAL has announced that it will begin to \
provide a suite of development tools for both products. The announced \
tools focus on the area of developing complex multitasking applications. \
This move is seen as an acknowledgment by HAL of the growing demand for \
more powerful development tools to help meet software backlogs prevalent \
amoungst Fourtune 500 organizations.  10:25 AM."
};


#ifndef WIN32
extern CHAR * XCDECL getenv();
#endif


INT XCDECL
main(ArgC, ArgV)
int ArgC;
CHAR **ArgV;
{
	XINT Mode;
	XINT NQueues;
	XINT NMessages;
	XINT SleepTime;
	CHAR *Instance = NULL;

	CHAR *UserName;
	CHAR Buf[1024];
	CHAR QName[10][8];
	MSGHDR MsgHdr;
	QIDLIST gQidList;
	QIDLIST pQidList;
	XINT BufLen = (XINT) sizeof(Buf);
	XINT i;
	XINT j;
	XINT MsgLen;
	XINT n;
	XINT Priority;
	XINT QQid[10];
	XINT QSelCode;
	XINT RandNum;
	XINT RetCode;
	XINT RetQid, Qid;
	XINT StartTime, FinTime;


	if (ArgC < 5)
	{
		printf("Usage: %s <mode>{'p'|'c'} <queues> <messages> <sleep> [<instance>]\n", ArgV[0]);
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

	NQueues = atol(ArgV[2]);
	NMessages = atol(ArgV[3]);
	SleepTime = atol(ArgV[4]);

	if (ArgC > 5) Instance = ArgV[5];
	else Instance = getenv("XIPC");

	if (Instance == NULL)
	{
		printf("Instance must be specified as parameter or XIPC environment variable\n");
		exit(1);
	}

	if (NQueues > 6)
	{
		printf("DemMsg: too many queues (6 max for this demo)\n");
		exit(1);
	}

	printf("Instance = %s\n", Instance);
	printf("Queues=%d, Messages=%d, SleepTime=%d, Mode=%s\n",
				NQueues, NMessages, SleepTime, ((Mode==PRODUCER) ? "PRODUCER" : "CONSUMER"));

	RetCode = XipcLogin(Instance, UserName);
	if (RetCode < 0)
	{
		printf("Login to instance %s failed; RetCode = %d, %s\n",
					Instance, RetCode, XipcError(RetCode) );
		exit(1);
	}

	for (i = 0; i < NQueues; i++)
	{
		sprintf(QName[i], "q%d", i);
		Qid = RetCode = QueCreate(QName[i], 7500, 10*1024L);
		if (RetCode < 0)
		{
			if (RetCode == QUE_ER_DUPLICATE)
			{
				Qid = RetCode = QueAccess(QName[i]);
				if (RetCode < 0)
				{
					printf("DemMsg: QueAccess failed, RetCode = %ld\n", RetCode);
					printf("DemMsg: %s\n", XipcError(RetCode));
					XipcLogout();
					exit(1);
				}
			}
			else
			{
				printf("DemMsg: QueCreate failed, RetCode = %ld\n", RetCode);
				printf("DemMsg: %s\n", XipcError(RetCode));
				XipcLogout();
				exit(1);
			}
		}
		QQid[i] = Qid;
	}

	QueListBuild(pQidList, QUE_EOL);
	for (i=0; i<NQueues; i++) QueListAdd(pQidList, QQid[i], QUE_EOL);

	QueListBuild(gQidList, QUE_EOL);
	switch (NQueues)							/* note: this switch has no breaks */
	{
		case 6: QueListAdd(gQidList, QUE_M_LA(QQid[5]), QUE_EOL);
		case 5: QueListAdd(gQidList, QUE_M_HP(QQid[4]), QUE_EOL);
		case 4: QueListAdd(gQidList, QUE_M_LP(QQid[3]), QUE_EOL);
		case 3: QueListAdd(gQidList, QUE_M_LA(QQid[2]), QUE_EOL);
		case 2: QueListAdd(gQidList, QUE_M_EA(QQid[1]), QUE_EOL);
		case 1: QueListAdd(gQidList, QUE_M_EA(QQid[0]), QUE_EOL);
	}

	StartTime = time(NULL);

	for (j = 0; j < NMessages; j++)
	{
		RandNum = rand();
		if (Mode == PRODUCER)
		{

			if (j % 100 == 0)
				sleep(5);

			RetCode = QueWrite(&MsgHdr,
					Data[RandNum % DATASIZE],
					(XINT) (strlen(Data[RandNum % DATASIZE]) + 1), QUE_WAIT);
			if (RetCode < 0)
			{
				printf("DemMsg: QueWrite failed, RetCode = %ld\n", RetCode);
				printf("DemMsg: %s\n", XipcError(RetCode));
				break;
			}
			if (RandNum % 2)
				QSelCode = QUE_Q_SHQ;
			else if (RandNum % 3)
				QSelCode = QUE_Q_HPQ;
			else if (RandNum % 5)
				QSelCode = QUE_Q_LAQ;
			else
				QSelCode = QUE_Q_LPQ;

			n = ((RandNum % 10) + 1) * 100;

			RetCode = QuePut(&MsgHdr, QSelCode, pQidList, n, &RetQid, QUE_WAIT);
			if (RetCode < 0)
			{
				{
					printf("DemMsg: QuePut failed, RetCode = %ld\n", RetCode);
					printf("DemMsg: %s\n", XipcError(RetCode));
					break;
				}
			}
		}
		if (Mode == CONSUMER)
		{
			n = RandNum % 100;
			if (n >= 0 && n < 20)
				QSelCode = QUE_Q_EA;
			else if (n >= 20 && n < 40)
				QSelCode = QUE_Q_LNQ;
			else if (n >= 40 && n < 60)
				QSelCode = QUE_Q_LA;
			else if (n >= 60 && n < 80)
				QSelCode = QUE_Q_HP;
			else
				QSelCode = QUE_Q_LP;
			RetCode = QueGet(&MsgHdr, QSelCode, gQidList, &Priority, &RetQid, QUE_WAIT);
			if (RetCode < 0)
			{
				printf("DemMsg: QueGet failed, RetCode = %ld\n", RetCode);
				printf("DemMsg: %s\n", XipcError(RetCode));
				break;
			}
			MsgLen = QueRead(&MsgHdr, Buf, BufLen);
			if (MsgLen < 0L)
			{
				printf("DemMsg: QueRead failed, RetCode = %ld\n", MsgLen);
				printf("DemMsg: %s\n", XipcError(RetCode));
				break;
			}
		}
		if (SleepTime > 0)
			XipcSleep(SleepTime);
	}

	FinTime = time(NULL);
	printf("DemMsg: Time = %ld\n", FinTime - StartTime);

	XipcLogout();

	return(0);

} /* main */

