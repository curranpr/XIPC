/********************************************************** 
**                                                         
**      Name:  	demo_aio.c 
**      ----
**
**      Description:
**      -----------
**		This program echoes whatever the user types on the console
**		back to the console via an asynchronous callback function
**		registered by the QueReceive operation, using asynchronous
**		I/O descriptors.
**
**      Copyright (C) Momentum Software Corporation, 1997
**
*********************************************************************/

#include <stdio.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <sys/time.h>
#include "xipc.h"

/*********************** DEFINES ***********************/

#define MSGSZ 		256
#define MAXPOLLFD	8
#define MAXLINE 	80

/*********************** TYPEDEFS ***********************/

typedef struct _MsgInfo{
	XINT 		retqid;
	XINT 		msgprio;
	QIDLIST 	recvlist;
	CHAR		msgbuf[MSGSZ];
} MSGINFO;

/*********************** GLOBALS ***********************/

MSGINFO			AsyncMsgInfo;
ASYNCRESULT 	RecvAcb;
QIDLIST 		SendList;
static	char	Envbuf[256];
XINT			MsgPrio;

/*********************** FUNCTION PROTOTYPES ***************/

int		read_input_message (void);
int		getline (CHAR *, XINT);
int 	recv_message_async (MSGINFO	*);
void 	echo_message (ASYNCRESULT *);
void 	ExitApp (void);
void 	interrupt_handler (void);

/***********************************************************/


main(argc, argv)
int argc;
char **argv;
{

	extern	char	 *getenv();
	char 	*configname;
	fd_set	pollfd;
	fd_set	savefd_set;

	XINT 	qid;
	XINT 	uid;
	XINT 	retcode;
	XINT	asyncfd; /* returned by xipc */
	
	/* change default xipc async notification */
	sprintf(Envbuf,"XIPCASYNCIO=1");
	putenv(Envbuf);

	if (argc > 1)
		configname = argv[1];
	else
	{
		if ((configname = getenv("XIPC")) == NULL)
		{
			fprintf(stderr, "Please set the XIPC environment variable\n");
			exit(1);
		}
	}

	signal(SIGINT,interrupt_handler);

	uid = XipcLogin(configname, "foo");
	if (uid <= 0) {
		fprintf(stderr,"XipcLogin failed [%s]\n",XipcError(uid));
		exit(1);
	}
	MsgPrio = uid; /* all my messages */

	/* get file descriptor that XIPC will use to notify */
	asyncfd = XipcAsyncIoDescriptor();

	if (asyncfd < 0) {
		fprintf(stderr,"XipcAsyncIoDescriptor failed \n");
		ExitApp();
	}

	if ((qid = QueAccess("foo")) < 0)
		qid = QueCreate("foo", 10, 10*MSGSZ);

	if (qid < 0 ) {
		fprintf(stderr,"QueCreate/Access failed [%s]\n",XipcError(qid));
		ExitApp();
	}
	QueListBuild(SendList, qid, QUE_EOL);
	QueListBuild(AsyncMsgInfo.recvlist, QUE_M_PREQ(qid,MsgPrio), QUE_EOL);

	recv_message_async(&AsyncMsgInfo);

	FD_ZERO(&pollfd);
	FD_SET(1,&pollfd); /* stdin */
	FD_SET(asyncfd, &pollfd); /* add the async descriptor for XIPC */
	savefd_set = pollfd;
	for (;;)
	{
		int	ret;
		pollfd = savefd_set;
		/* MAXPOLLFD is #defined to 8 (randomly chosen) */
		ret = select(MAXPOLLFD,&pollfd,(fd_set *) NULL, (fd_set *) NULL, NULL);
		if (ret == -1) {
			perror("select");
			ExitApp();
		}
		if (FD_ISSET(1,&pollfd))
			read_input_message();
		if (FD_ISSET(asyncfd,&pollfd)) {
			XipcAsyncEventHandler(); /* handle the async message */
			recv_message_async(&AsyncMsgInfo); /* start another async */
		}
	}
}	


int
read_input_message()
{
	XINT	retcode;
	XINT	retqid;
	char	line[MAXLINE];

	getline(line,MAXLINE);

	retcode = QueSend(QUE_Q_ANY, SendList, line,sizeof(line),MsgPrio,
						&retqid, QUE_WAIT);
	if (retcode < 0) {
		fprintf(stderr,"QueSend failed [%s]\n", XipcError(retcode));
		ExitApp();
	}
	return(retcode);
}

int
getline(buf, maxlen)
char	*buf;
int		maxlen;
{
	int i;
	int nread;
	char ch;

	i = 0;
	while(i < maxlen) {
		nread = read(1,&ch,sizeof(char));
		if (nread == 0) 
			break;
		buf[i++] = ch;
		if (ch == '\n')
			break;
	}
	buf[i] = '\0';
	return (i);
}


int
recv_message_async(MsgInfoPtr)
MSGINFO	*MsgInfoPtr;
{
	XINT	retcode;

	retcode = QueReceive(QUE_Q_EA, MsgInfoPtr->recvlist, MsgInfoPtr->msgbuf, 
				sizeof(MsgInfoPtr->msgbuf), &MsgInfoPtr->msgprio, 
				&MsgInfoPtr->retqid, QUE_CALLBACK(echo_message,&RecvAcb));

	if (retcode < 0 && retcode != QUE_ER_ASYNC) {
		fprintf(stderr,"Async QueReceive failed [%s]\n",XipcError(retcode));
		ExitApp();
	}
	return(retcode);
}

void
echo_message(Acb)
ASYNCRESULT 	*Acb;
{
	if (Acb->Api.QueReceive.RetCode >= 0)
		printf("%s\n",Acb->Api.QueReceive.MsgBuf);
}



void
ExitApp()
{
	XipcLogout();
	fprintf(stderr,"Error: exiting....\n");
	exit(1);
}

void
interrupt_handler()
{
	XipcLogout();
	fprintf(stderr,"exiting on SIGINT....\n");
	exit(0);
}

