/********************************************************** MOMCLNT.C
**									   (MomSys Sample Program)
**
**
**	Name:	MOMCLNT.C
**	----
**
**	Title:	MomSys Client Test Program
**	-----
**
**	Description:
**	-----------
**	This file contains a server program that illustrates the 
**		client side of the MomSys Client/Server paradigm. 
**		See "momsrvr.c" for the server side of the paradigm.
**
**	This sample is meant to be simple and straight-forward in 
**		demonstrating how to use MomSys for client/server
**		communication. 
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
#include <string.h>
#ifdef __STDC__
#include <stdlib.h>
#else
#include <malloc.h>
#endif
#include "xipc.h"

/*********************************************************************
** Defined Constants
*********************************************************************/

#define DEF_MSG_SIZE	512
#define DEF_NUM_MSGS	100
#define DEF_CLIENT_QUEUE	MOM_PRIVATE

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
**
**	Name:	MomClnt (main)
**	----
*********************************************************************/

INT XCDECL
main(ArgC, ArgV)
INT ArgC;
CHAR **ArgV;
{

	CHAR		*PgmName = "momclnt";
	CHAR		*LocalInstance = NULL;
	CHAR		*ServerAQ = NULL;
	CHAR		*ReplyAQ = NULL;
	XINT		ServerAQid;
	XINT		NumMsgs;
	XINT		MsgSize;
	XINT		MsgsSent;
	CHAR		*Buffer;
	XINT		BufferLen;
	MOM_MSGID	RequestMsgId;
	XINT		ReplyAQid;
	XINT		RetCode;
	XINT		i;
	XINT		STime, FTime, TotTime;

	/*
	 *	Usage
	 *	=====
	 */

	if (ArgC < 6)
	{
		fprintf(stderr,
			"usage: %s <MsgSize> <# Msgs> <LocalInstance> <Reply-AQ> <Server-AQ>\n",
			ArgV[0]);
		exit(1);
	}

	/*
	 *	Set Parameters
	 *	==============
	 */

	MsgSize = atol(ArgV[1]);
	if (MsgSize < 0)
		MsgSize = DEF_MSG_SIZE;

	NumMsgs = atol(ArgV[2]);
	if (NumMsgs < 0)
		NumMsgs = DEF_NUM_MSGS;

	LocalInstance = ArgV[3];

	ReplyAQ  = ArgV[4];
	if (*ReplyAQ == '-')
		ReplyAQ = DEF_CLIENT_QUEUE;

	ServerAQ  = ArgV[5];


	/*
	 *	Login
	 *	=====
	 */

	RetCode = XipcLogin(LocalInstance, PgmName);
	if (RetCode < 0)
	{
		fprintf(stderr, "%s: XipcLogin failed, error %d: %s.\n",
			PgmName, RetCode, XipcError(RetCode));
		if (RetCode == XIPC_ER_SYSERR)
			PrintSysError();	
		exit(1);
	}


	/*
	 *	Create Client Reply AppQueue
	 *	============================
	 */

	RetCode = MomCreate(ReplyAQ, MOM_APPQUEUE_DISK);
	if (RetCode == MOM_ER_DUPLICATE)
		RetCode = MomAccess(ReplyAQ);
	if (RetCode < 0)
	{
		fprintf(stderr, "%s: MomCreate failed, error %d: %s.\n",
			PgmName, RetCode, XipcError(RetCode));
		if (RetCode == XIPC_ER_SYSERR)
			PrintSysError();
		XipcLogout();
		exit(1);
	}
	ReplyAQid = RetCode;


	/*
	 *	Access Server AppQueue
	 *	======================
	 */

	RetCode = MomAccess(ServerAQ);
	if (RetCode < 0)
	{
		fprintf(stderr, "%s: MomAccess failed, error %d: %s.\n",
			PgmName, RetCode, XipcError(RetCode));
		if (RetCode == XIPC_ER_SYSERR)
			PrintSysError();
		XipcLogout();
		exit(1);
	}
	ServerAQid = RetCode;


	/*
	 *	Allocate Buffer.
	 * 	Populate it with test alphabetic data.
	 *	======================================
	 */

	 Buffer = (CHAR *)malloc(MsgSize);
	 if (Buffer == NULL)
	 {
		fprintf(stderr, "%s: malloc failed.\n", PgmName);
		XipcLogout();
		exit(1);
	 }
	 BufferLen = MsgSize;

	 for (i = 0; i < BufferLen - 1; i++)
	 {
		Buffer[i] = 'A' + (CHAR)(i % 26);
	 }
	 Buffer[BufferLen - 1] = '\0'; /* Make it a string */


	/*
	 *	Execute Loop
	 *	============
	 */

	STime = time(NULL);

	for (MsgsSent = 0; MsgsSent < NumMsgs; MsgsSent++)
	{

		/*
		 *	MomSend
		 *	=======
		 */
		RetCode = MomSend(ServerAQid,
						Buffer,
						BufferLen,
						MOM_PRIORITY_NORMAL,
						MOM_TRACK_SHIPPED,
						ReplyAQid,			/* Tells server where
											 * to send response. 
										 	 */
						&RequestMsgId,		/* Used in upcoming
											 * MomReceive() call
											 * to select response
											 * message for this
											 * inquiry.
											 */
						MOM_WAIT);


		if (RetCode < 0)
		{
			fprintf(stderr, "%s: MomSend failed, error %d: %s.\n",
				PgmName, RetCode, XipcError(RetCode));
			if (RetCode == XIPC_ER_SYSERR)
				PrintSysError();	
			break;
		}

		/*
		 *	MomReceive
		 *	==========
		 */
		RetCode = MomReceive(ReplyAQid,
						Buffer,
						BufferLen,
						MOM_MESSAGE_REPLYTO(RequestMsgId),
						NULL,
						NULL,
						NULL,
						MOM_WAIT);

		if (RetCode < 0)
		{
			fprintf(stderr, "%s: MomReceive failed, error %d: %s.\n",
				PgmName, RetCode, XipcError(RetCode));
			if (RetCode == XIPC_ER_SYSERR)
				PrintSysError();
			break;
		}

	} /* end of loop */


	/*
	 *	Print Statistics
	 *	================
	 */

	FTime = time(NULL);
	TotTime = FTime - STime;
	if (TotTime == 0)
		TotTime = 1;

	fprintf(stdout, "%s: Performed %d inquiry/response exchanges in %d seconds,\n",
		PgmName, MsgsSent, TotTime);
	fprintf(stdout, "\tfor a rate of %ld exchanges/second.\n",
		MsgsSent / TotTime);

	/*
	 *	Clean Up
	 */

	XipcLogout();

	return(0);
}
/*
 * ***** END OF MOMCLNT.C *******************************************
 */
