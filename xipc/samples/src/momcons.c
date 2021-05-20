/********************************************************** MOMCONS.C
**									   (MomSys Sample Program)
**
**	Name:	MOMCONS.C
**	----
**
**	Title:	MomSys Consumer - Sample Program
**	-----
**
**	Description:
**	-----------
**	This file contains a consumer program that illustrates the 
**		consumer side of the MomSys Producer/Consumer paradigm. 
**		See "momprod.c" for the producer side of the paradigm.
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
#define PRINT_FREQ	500

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

	XINT		MsgSize;
	XINT		NumMsgs;
	CHAR		*LocalInstance = NULL;
	CHAR		*AppQName = NULL;
	CHAR		*ProgramName = "momcons";
	XINT		AppQid;
	XINT		Delay;
	XINT		PrintFreq;
	CHAR		*Buffer;
	XINT		BufferLen;
	XINT		MsgsRecvd;
	XINT		RetCode;
	XINT		STime, FTime, TotTime;
	XINT		i;


	/*
	 *	Usage
	 *	=====
	 */

	if (ArgC < 6)
	{
		printf("Usage: %s <MsgSize> <# Msgs> <LocalInstance> <AppQName> <DelayMsec> [print-freq]\n", ArgV[0]);
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
	if (NumMsgs < 0)	/* Allow <# Msgs> == 0 for infinite loop. */
		NumMsgs = 1;

	LocalInstance = ArgV[3];

	AppQName = ArgV[4];

	Delay = atol(ArgV[5]);
	if (Delay < 0)
		Delay = DELAY_MSECS;

	if (ArgC >= 7)
	{
		PrintFreq = atol(ArgV[6]);
		if (PrintFreq < 0)
			PrintFreq = PRINT_FREQ;	
	}
	else
		PrintFreq = PRINT_FREQ;	


	/*
	 *	Allocate buffer (plus string-terminating null char)
	 *	===============
	 */

	Buffer = (CHAR *)malloc(MsgSize + 1);
	if (Buffer == NULL)
	{	
		fprintf(stderr, "%s: malloc failed, exiting.\n", ProgramName);
		exit(1);
	}
	BufferLen = MsgSize + 1;
		
	/*
	 *	Login to XIPC instance.
	 *	=======================
	 */

	RetCode = XipcLogin(LocalInstance, ProgramName);
	if (RetCode < 0)
	{
		fprintf(stderr, "XipcLogin(%s, %s) failed, error %d: %s.\n",
			LocalInstance, ProgramName, RetCode, XipcError(RetCode));
		if (RetCode == XIPC_ER_SYSERR)
			PrintSysError();	
		exit(1);
	}

	/*
	 *	Create AppQueue & Register it in XIPC Catalog
	 *	=============================================
	 *	App-Queue is created and registered. If messages have already
	 * 	been sent towards the app-queue (via producer) those messages 
	 * 	are now automatically forwarded to the consumer. 
	 * 	(App-queue is accessed if it already exists.)
	 */

	RetCode = MomCreate(AppQName, MOM_APPQUEUE_DISK_REGISTER);
	if (RetCode < 0)
	{
		if (RetCode != MOM_ER_DUPLICATE)
		{
			fprintf(stderr, "%s: MomCreate failed, error %d: %s; exiting.\n",
				ProgramName, RetCode, XipcError(RetCode));
			if (RetCode == XIPC_ER_SYSERR)
				PrintSysError();
			XipcLogout();
			exit(1);
		}

		RetCode = MomAccess(AppQName);
		if (RetCode < 0)
		{
			fprintf(stderr, "%s: MomAccess failed, error %d: %s.\n",
				ProgramName, RetCode, XipcError(RetCode));
			if (RetCode == XIPC_ER_SYSERR)
				PrintSysError();
			XipcLogout();
			exit(1);
		}
	}
	AppQid= RetCode;


	/*
	 *	MomReceive in a loop
	 *	====================
	 *		If <# Msgs> was specified as 0, we will loop forever.
	 */

	STime = time(NULL);

	for (MsgsRecvd = 0; MsgsRecvd < NumMsgs; MsgsRecvd++)
	{

		/*
		 *	MomReceive
		 *	==========
		 */

		RetCode = MomReceive(
							AppQid,
							Buffer,
							BufferLen,
							MOM_MESSAGE_FIRST,
							NULL,	/* No reply-aqid */
							NULL,	/* No MsgId */
							NULL,	/* No InfoMsg */
							MOM_WAIT);
		if (RetCode < 0)
		{
			fprintf(stderr, "%s: MomReceive failed, error %d: %s.\n", 
				ProgramName, RetCode, XipcError(RetCode));
			if (RetCode == XIPC_ER_SYSERR)
				PrintSysError();
			break;
		}

		/*
		 *	Print Message 
		 *	=============
		 */

		if ((MsgsRecvd % PrintFreq) == 0)
		{
			Buffer[RetCode] = '\0';
			fprintf(stdout, "%s: Received message # %d: '%s'\n", 
					ProgramName, MsgsRecvd, Buffer);
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

	fprintf(stdout, "%s: Received %d %d-byte messages in %d seconds,\n", 
		ProgramName, MsgsRecvd, MsgSize, TotTime);
	fprintf(stdout, "\tfor a rate of %lf messages/second,\n",
		MsgsRecvd / (float)TotTime);
	fprintf(stdout, "\tand %lf bytes/second.\n",
		(MsgSize * MsgsRecvd) / (float)TotTime);
	fprintf(stdout, "\tincluding a %d-second delay for processing each message.\n\n",
		Delay / 1000);

	/*
	 *	Clean Up
	 *	========
	 */

	XipcLogout();

	return(0);
}

/****** END OF MOMCONS.C ********************************************/

