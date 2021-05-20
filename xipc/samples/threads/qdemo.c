/********************************************************** qdemo.c
**
**
**	Name:	qdemo.c
**	----
**
**	Title:	Queue System Test (POSIX Draft 7)
**	-----
**
**	Description:
**	-----------
**
**	Last Revision:
**	-------------
**	$Revision:	$
**	$Date:	August 1991$
**	$Author:	
**
**	Revision History:
**	----------------
**
**	Rev   Date		By	 Description
**	---   --------	---  -----------
**
**
**	Copyright (C) Momentum Software Corporation, 1989-95
**
*********************************************************************/

/*********************************************************************
** header files
*********************************************************************/

#include <stdio.h>
#include <pthread.h>

#include "xipc.h"

#ifndef MSGSZ
#define MSGSZ 32
#endif

#ifndef NUMMSG
#define NUMMSG 10000
#endif

#define NUMTHREADS	2

extern char	*getenv();

void *producer(void *);
void *consumer(void *);

void main(int argc, char **argv)
{
	int ret;
	int loopcnt;
	int *status;

	pthread_attr_t ThreadAttr;
	pthread_t producer_thread;
	pthread_t consumer_thread;

	loopcnt = 5000; /* default */
	if (argc > 1)
		loopcnt = atoi(argv[1]);

	if (!getenv("XIPC")) 
	{
		fprintf(stderr, "Error: Environment variable XIPC not set\n");
		exit(1);
	}

	pthread_attr_init(&ThreadAttr);
	pthread_attr_setdetachstate(&ThreadAttr, PTHREAD_CREATE_JOINABLE);

	ret = pthread_create(&consumer_thread, 
					&ThreadAttr, consumer, (void *)&loopcnt);
	if (ret != 0) 
	{
		fprintf(stderr, "Error: Consumer thread not created.\n");
		exit(1);
	}

	ret = pthread_create(&producer_thread, 
					&ThreadAttr, producer, &loopcnt);
	if (ret != 0) 
	{
		fprintf(stderr, "Error: Producer thread not created.\n");
		exit(1);
	}

	pthread_join(producer_thread, (void **) &status); 
	pthread_join(consumer_thread, (void **) &status);

	exit(0);
}

void *
producer(void *argcnt)
{
	int ret;
	int msgnum;
	int pid;
	pthread_t tid;
	char msg[MSGSZ+1]; 
	char *config;

	XINT qid;
	XINT uid;
	XINT prio;
	XINT retqid;
	XINT msgcnt;
	QIDLIST	qidlist;

	pid = getpid();
	tid = pthread_self();

	config = getenv("XIPC");

	uid = XipcLogin(config, "sender");
	if (uid <= 0) 
	{
		fprintf(stderr, "sndr: XipcLogin failed. %s. RetCode = %d\n",
			XipcError(uid), uid);
		pthread_exit(&uid);
	}

	fprintf(stderr, "producer... pid = %d tid = %d uid = %d\n", pid, tid, uid);
	fflush(stderr);

	qid = QueCreate("qdemo", NUMMSG, NUMMSG * MSGSZ);
	if (qid < 0)
	{
		if (qid == QUE_ER_DUPLICATE)
			qid = QueAccess("qdemo");
		else
		{
			ret = qid;
			fprintf(stderr, "QueCreate failed. %s\n", XipcError(qid));
			fflush(stderr);
			pthread_exit(&ret);
		}
	}

	QueListBuild(qidlist, qid, QUE_EOL);

	prio = 1234;

	msgcnt = *((int *)argcnt);
	for (msgnum = 0; msgnum < msgcnt; ++msgnum) 
	{
		sprintf(msg, "packet #%d", msgnum);

		ret = QueSend(QUE_Q_ANY, qidlist, msg, MSGSZ, prio, &retqid, QUE_WAIT);
		if (ret < 0) 
		{
			fprintf(stderr, "QueSend failed. %s\n", XipcError(ret));
			fflush(stderr);
			break;
		}
	}

	ret = XipcLogout();
	if (ret < 0) 
	{
		fprintf(stderr, "XipcLogout failed. %s\n", XipcError(ret));
		fflush(stderr);
	}

	pthread_exit(&ret);
}

void *
consumer(void *argcnt)
{
	int ret;
	int msgnum;
	int pid;
	char buf[MSGSZ+1];
	char *config;
	pthread_t tid;

	XINT prio;
	XINT qid;
	XINT uid;
	XINT retqid;
	XINT msgcnt;
	QIDLIST	qidlist;
	QUEINFOQUE qinfo;

	pid = getpid();
	tid = pthread_self();

	config = getenv("XIPC");

	uid = XipcLogin(config, "receiver");
	if (uid <= 0) 
	{
		fprintf(stderr, "rcvr: XipcLogin failed. %s RetCode = %d\n",
			XipcError(uid), uid);
		pthread_exit(&uid);
	}

	fprintf(stderr, "consumer... pid = %d tid = %d uid = %d\n", pid, tid, uid);
	fflush(stderr);

	qid = QueCreate("qdemo", NUMMSG, NUMMSG * MSGSZ);
	if (qid < 0)
	{
		if (qid == QUE_ER_DUPLICATE)
			qid = QueAccess("qdemo");
		else
		{
			ret = qid;
			fprintf(stderr, "QueCreate failed. %s\n", XipcError(qid));
			pthread_exit(&ret);
		}
	}
	
	QueListBuild(qidlist, QUE_M_EA(qid), QUE_EOL);

	msgcnt = *((int *)argcnt);
	for (msgnum = 0; msgnum < msgcnt; ++msgnum) 
	{
		ret = QueReceive(QUE_Q_EA, qidlist, buf,
						MSGSZ, &prio, &retqid, QUE_WAIT);
		if (ret < 0) 
		{
			fprintf(stderr, "QueRecv failed. %s\n", XipcError(ret));
			fflush(stderr);
			break;
		}
	}

	QueInfoQue(qid, &qinfo);

	ret = XipcLogout();
	if (ret < 0) 
	{
		fprintf(stderr, "XipcLogout failed. %s\n", XipcError(ret));
		fflush(stderr);
	}

	fprintf(stderr,"Msgs #in = %d #out = %d\n", qinfo.CountIn, qinfo.CountOut);
	fflush(stderr);

	pthread_exit(&ret);
}
