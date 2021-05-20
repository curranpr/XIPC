/********************************************************** MOMSRVR.C
**									   (MomSys Subsystem - Sample)
**
**
**	Name:	MOMSRVR.C
**	----
**
**	Title:	MomSys Server - Sample Program
**	-----
**
**	Description:
**	-----------
**	This file contains a server program that illustrates the 
**		server side of the MomSys Client/Server paradigm. 
**		See "momclnt.c" for the client side of the paradigm.
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
#include <ctype.h>
#include <string.h>
#ifdef __STDC__
#include <stdlib.h>
#else
#include <malloc.h>
#endif
#include "xipc.h"

/*********************************************************************
** Defines
*********************************************************************/
#define DEF_MSG_SIZE	512
#define DEF_NUM_MSGS	100

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
**	Name:	MomSrvr (main)
**	----
*********************************************************************/

INT XCDECL
main(ArgC, ArgV)
INT ArgC;
CHAR **ArgV;
{

	CHAR		*PgmName = "momsrvr";
	XINT		MsgSize;
	XINT		NumMsgs;
	CHAR		*LocalInstance = NULL;
	CHAR		*ServerAppQue  = NULL;
	XINT		ServerAQid;
	XINT		ReplyQ;
	MOM_MSGID	RequestMsgId;
	CHAR		*Buffer;
	XINT		BufferLen;
	XINT		MsgsRecvd;
	XINT		i;
	XINT		RetCode;
	int 		c;

	/*
	 *	Usage
	 *	=====
	 */

	if (ArgC < 5)
	{
		fprintf(stderr,
			"usage: %s <MsgSize> <# Msgs> <Local Inst> <ServerQue Name>\n",
			ArgV[0]);
		exit (1);
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

	ServerAppQue  = ArgV[4];

	/*
	 *	Login
	 *	=====
	 */

	RetCode = XipcLogin(LocalInstance, PgmName);
	if (RetCode < 0)
	{
		fprintf(stderr, "%s: XipcLogin failed, RetCode = %d: %s\n",
			PgmName, RetCode, XipcError(RetCode));
		if (RetCode == XIPC_ER_SYSERR)
			PrintSysError();
		exit(1);
	}

	/*
	 *	Create/Access Server AppQue
	 *	===========================
	 */
	if ((RetCode = MomCreate(ServerAppQue, MOM_APPQUEUE_DISK_REGISTER)) < 0)
	{
		if (RetCode != MOM_ER_DUPLICATE)
		{
			fprintf(stderr, "%s: MomCreate failed, RetCode = %d: %s\n",
				PgmName, RetCode, XipcError(RetCode));
			if (RetCode == XIPC_ER_SYSERR)
				PrintSysError();
			XipcLogout();
			exit(1);
		}

		/*
		 * ServerAppQue already exists; try to Access it.
		 */ 

		if ((RetCode = MomAccess(ServerAppQue)) < 0)
		{
			fprintf(stderr, "%s: MomAccess failed, RetCode = %d: %s\n",
				PgmName, RetCode, XipcError(RetCode));
			if (RetCode == XIPC_ER_SYSERR)
				PrintSysError();
			XipcLogout();
			exit(2);
		}
	}
	ServerAQid = RetCode;


	/*
	 *	Allocate buffer
	 *	===============
	 */

	Buffer = (CHAR *)malloc(MsgSize);
	if (Buffer == NULL)
	{
		fprintf(stderr, "%s: malloc failed.\n", PgmName);
		XipcLogout();
		exit(6);
	}
	BufferLen = MsgSize;



	/*
	 *	Process client messages in loop
	 *	===============================
	 */
	for (MsgsRecvd = 0; MsgsRecvd < NumMsgs; MsgsRecvd++)
	{
		/*
		 * Read the first message on server queue.
		 * MOM_TRUNCATE protects us from too big
		 * messages.
		 */

		RetCode = MomReceive(ServerAQid,
							Buffer,
							MOM_TRUNCATE(BufferLen),
							MOM_MESSAGE_FIRST,
							&ReplyQ,
							&RequestMsgId,
							NULL,
							MOM_WAIT);

		if (RetCode < 0)
		{
			fprintf(stderr, "%s: MomReceive failed, RetCode = %d: %s\n",
				PgmName, RetCode, XipcError(RetCode));
			if (RetCode == XIPC_ER_SYSERR)
				PrintSysError();
			XipcLogout();
			exit(1);
		}

		/*
		 * Process the message: 
		 * We simply change the case of the 
		 * received text message.
		 * *************************************
		 * Insert your own server code here... *
		 * *************************************
		 */

		for (i = 0; i < BufferLen; i++)
		{
			c = (CHAR)Buffer[i];
			if (isalpha(c))
			{
				if (islower(c))
					Buffer[i] = toupper(c);
				else
					Buffer[i] = tolower(c);
			}
		}


		/*
		 * Send a Response message
		 * The MOM_REPLYTO parameter guarantees correlation
		 * of response messages as they arrive back at the client.
		 * See "momclnt.c" for the client side of this correlation.
		 */

		RetCode = MomSend(ReplyQ,
						Buffer,
						strlen(Buffer) + 1,
						MOM_PRIORITY_NORMAL,
						MOM_TRACK_SHIPPED,
						MOM_REPLY_NONE,
						NULL,
						MOM_REPLYTO(RequestMsgId),
						MOM_WAIT);

		if (RetCode < 0)
		{
			fprintf(stderr, "%s: MomSend failed, RetCode = %d: %s\n",
				PgmName, RetCode, XipcError(RetCode));
			if (RetCode == XIPC_ER_SYSERR)
				PrintSysError();
			XipcLogout();
			exit(1);
		}

	} /* end loop. */

	XipcLogout();

	return(0);
}

/*
 ********* END ********************************************** MOMSRVR.C
 */
