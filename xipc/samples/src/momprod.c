/********************************************************** MOMPROD.C
**									   (MomSys Sample Program)
**
**
**	Name:	MOMPROD.C
**	----
**
**	Title:	MomSys Producer - Sample Program
**	-----
**
**	Description:
**	-----------
**	This file contains a producer program that illustrates the 
**		producer side of the MomSys Producer/Consumer paradigm. 
**		See "momcons.c" for the consumer side of the paradigm.
**
**	This sample is meant to be simple and straight-forward in 
**		demonstrating how to use MomSys for producer/consumer
**		communication. Timing code is provided as well. As such
**		these two programs may be used for performing simple
**		p/c benchmarks over MomSys.
**
**
**	Copyright (C) Momentum Software Corporation, 1997
**
*********************************************************************/

/*********************************************************************
** header files
*********************************************************************/

#include <stdio.h>
#include <time.h>
#ifdef __STDC__
#include <stdlib.h>
#else
#include <malloc.h>
#endif
#include "xipc.h"

/*********************************************************************
** Defined Constants
*********************************************************************/

/* Default values (if command-line input is <= 0): */

#define MSG_SIZE	1024
#define DELAY_MSECS	1000

void PrintSysError()
{
	CHAR Msg[1024];
	XINT RetCode = XipcSystemErrorReport(Msg, sizeof(Msg));
	if (Msg[0] != '\0')
	{
		fprintf(stderr, Msg);
	}
}

/*********************************************************************
** main()
*********************************************************************/

INT XCDECL
main(ArgC, ArgV)
INT ArgC;
CHAR **ArgV;
{

	XINT	MsgSize;
	XINT	NumMsgs;
	CHAR	*LocalInstance = NULL;
	CHAR	*AppQName = NULL;
	CHAR	*ProgramName = "momprod";
	XINT	AppQid;
	XINT	Delay;
	CHAR	*Buffer;
	XINT	BufferLen;
	XINT	MsgsSent;
	XINT	RetCode;
	XINT	STime, FTime, TotTime;
	XINT	i;


	/*
	 *	Usage
	 *	=====
	 */

	if (ArgC < 6)
	{
		printf("Usage: %s <MsgSize> <# Msgs> <LocalInstance> <AppQName> <DelayMsecs>\n", ArgV[0]);
		exit(1);
	}

	/*
	 *	Set Parameters
	 *	==============
	 */

	MsgSize = atol(ArgV[1]);
	if (MsgSize <= 0)
		MsgSize = MSG_SIZE;

	NumMsgs = atol(ArgV[2]);
	if (NumMsgs < 0)
		NumMsgs = 1;

	LocalInstance = ArgV[3];

	AppQName = ArgV[4];

	Delay = atol(ArgV[5]);
	if (Delay < 0)
		Delay = DELAY_MSECS;

	/*
	 *	Login to XIPC instance.
	 *	======================
	 */

	RetCode = XipcLogin(LocalInstance, "momprod");
	if (RetCode < 0)
	{
		fprintf(stderr, "XipcLogin(%s, %s) failed, error %d: %s; exiting.\n",
			LocalInstance, "momprod", RetCode, XipcError(RetCode));
        if (RetCode == XIPC_ER_SYSERR)
			PrintSysError();
		exit(1);
	}

	/*
	 *	Access Target AppQueue
	 *	======================
	 * 	NOVERIFY flag allows the producr program to produce messages
	 *	even if the targeted app-queue, or instance, or computer, or
	 *	network connection, is not currently up. Messages will be
	 * 	asynchronusly forwarded to the consumer whenever communication
	 * 	is established with it. Producer program does *** NOT *** need
	 *	to stay active until that time. It "fires" its messages and 
	 * 	then is allowed to "forget" about them. XIPC guarantees their
	 *	timely delivery.
	 */

	RetCode = MomAccess(MOM_NOVERIFY(AppQName));
	if (RetCode < 0)
	{
		fprintf(stderr, "%s: MomAccess failed, error %d: %s; exiting\n",
			ProgramName, RetCode, XipcError(RetCode));
        if (RetCode == XIPC_ER_SYSERR)
			PrintSysError();
		XipcLogout();
		exit(1);
	}
	AppQid = RetCode;

printf("AppQid = %d\n", AppQid);

	/*
	 *	Allocate message buffer
	 *	=======================
	 */

	Buffer = (CHAR *)malloc(MsgSize);
	if (Buffer == NULL)
	{	
		fprintf(stderr, "%s: malloc failed, exiting.\n", ProgramName);
		exit(1);
	}
	BufferLen = MsgSize;

	/*
	 *	MomSend in a loop
	 *	=================
	 */

	STime = time(NULL);

	for (MsgsSent = 0; MsgsSent < NumMsgs; MsgsSent++)
	{

		/*
		 *	MomSend
		 *	=======
		 */
		sprintf(Buffer, "Message %d of %d", MsgsSent+1, NumMsgs);
		RetCode = MomSend(
						AppQid,
						Buffer,
						BufferLen,
						MOM_PRIORITY_NORMAL,
						MOM_TRACK_SHIPPED,
						MOM_REPLY_NONE,	
						NULL,				
						MOM_WAIT);
		if (RetCode < 0)
		{
			fprintf(stderr, "%s: MomSend failed, error %d: %s.\n", 
				ProgramName, RetCode, XipcError(RetCode));
       		if (RetCode == XIPC_ER_SYSERR)
				PrintSysError();
			break;
		}

		XipcSleep(Delay);

	} /* end of loop */

	/*
	 *	Print Statistics
	 *	================
	 */

	FTime = time(NULL);
	TotTime = FTime - STime;
	if (TotTime == 0)
		TotTime = 1;

	fprintf(stdout, "%s: Sent %d %d-byte messages in %d second%s,\n", 
		ProgramName, MsgsSent, MsgSize, TotTime, TotTime > 1 ? "s" : "");
	fprintf(stdout, "\tfor a rate of %lf messages/second,\n",
		MsgsSent / (float)TotTime);
	fprintf(stdout, "\tand %lf bytes/second,\n",
		(MsgSize * MsgsSent) / (float)TotTime);
	fprintf(stdout, "\tincluding a %d-second processing delay per message.\n\n",
		Delay / 1000);

	/*
	 *	Clean Up
	 *	========
	 */

	XipcLogout();

	return(0);
}

/****** END OF MOMPROD.C ********************************************/

