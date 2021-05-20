/**************************************************************************** 	
	This program shows the use of XIPC ASYNC option.

						NOTE: 

	In this example each thread calls XipcAsyncEventHandler directly.
	XipcAsyncEventHandler is a BLOCKING function call. 

	If you do not want to block - call select/poll and when data is ready for
    reading call XipcAsyncEventHandler. 
*****************************************************************************/

#include <pthread.h>
#include <stdio.h>
#include <errno.h>

#include "xipc.h"

#ifndef MSGSZ
#define MSGSZ 32
#endif

#define NUMMSG 200

extern char	*getenv();

VOID *consumer(void *Arg);
VOID *producer(void *Arg);
VOID Callback_1(ASYNCRESULT *AcbPtr);

void main(int argc, char **argv)
{
	int	ret;
	int	loopcnt;
	int	*status;
	
	pthread_t consumer_thread;
	pthread_t producer_thread;
	pthread_attr_t thread_attr;

	loopcnt = 2000; /* default */
	if (argc > 1)
		loopcnt = atoi(argv[1]);

	if (!getenv("XIPC")) 
	{
		fprintf(stderr, "Error: Environment variable XIPC not set\n");
		exit(1);
	}

	pthread_attr_init(&thread_attr);
	pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_JOINABLE);

	ret = pthread_create(&consumer_thread,
						&thread_attr,
						consumer, 
						(void *)&loopcnt);
	if (ret != 0)
	{
		perror("pthread_create");
		exit(1);
	}

	ret = pthread_create(&producer_thread,
						 &thread_attr,
						 producer,
						 (void *)&loopcnt);
	if (ret != 0)
	{
		perror("pthread_create");
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
	int pid;
	int msgnum;
	char *config;
	pthread_t tid;
	char msg[MSGSZ+1]; 

	XINT qid;
	XINT uid;
	XINT prio;
	XINT retqid;
	XINT msgcnt;
	QIDLIST	qidlist;

	pid = getpid();
	tid = pthread_self();

	config = getenv("XIPC");

	uid = XipcLogin(config, "producer");
	if (uid <= 0)
	{
		fprintf(stderr, "XipcLogin failed. %s\n", XipcError(uid));
		pthread_exit(&uid);
	}

	fprintf(stderr, "producer... pid = %d tid = %d XipcLogin id = %d\n",
			pid, tid, uid);
	fflush(stderr);

	qid = QueCreate("demoq", NUMMSG, NUMMSG * MSGSZ);
	if (qid < 0)
	{
		if (qid == QUE_ER_DUPLICATE)
			qid = QueAccess("demoq");
		else
		{
			ret = qid;
			fprintf(stderr, "QueCreate failed. %s\n", XipcError(qid));
			fflush(stderr);
			pthread_exit(&ret);
		}
	}

	QueListBuild(qidlist, qid, QUE_EOL);

	msgcnt = *((int *)argcnt);
	for (msgnum = 0; msgnum < msgcnt; ++msgnum)
	{
		prio = 100;

		ret = QueSend(QUE_Q_ANY, qidlist, msg, MSGSZ, prio, &retqid, QUE_WAIT);
		if (ret < 0)
		{
			fprintf(stderr, "QueSend failed. %s\n", XipcError(ret));
			fflush(stderr);
			break;
		}

		XipcSleep(1000);
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
	int fd;
	int pid;
	int msgnum;
	char *config;
	char Envbuf[256];
	char buf[MSGSZ+1];

	pthread_t tid;
	ASYNCRESULT Acb;

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

	/* change default xipc async notification */
	sprintf(Envbuf, "XIPCASYNCIO=1");
	putenv(Envbuf);

	uid = XipcLogin(config, "consumer");
	if (uid <= 0)
	{
		fprintf(stderr, "XipcLogin failed. %s\n", XipcError(uid));
		pthread_exit(&ret);
	}

	fd = XipcAsyncIoDescriptor();
	if (fd < 0)
	{
		fprintf(stderr, "AsyncIoDescriptor errno = %d ret %d\n", errno, fd);
		fflush(stderr);
		pthread_exit(&ret);
	}

	fprintf(stderr, "consumer... pid = %d tid = %d XipcLogin id = %d\n",
			pid, tid, uid);
	fflush(stderr);

	qid = QueCreate("demoq", NUMMSG, NUMMSG * MSGSZ);
	if (qid < 0)
	{
		if (qid == QUE_ER_DUPLICATE)
			qid = QueAccess("demoq");
		else
		{
			ret = qid;
			fprintf(stderr, "QueCreate failed. %s\n", XipcError(qid));
			pthread_exit(&ret);
		}
	}
	
	QueListBuild(qidlist, QUE_M_PREQ(qid, 100), QUE_EOL);

	msgcnt = *((int *)argcnt);
	for (msgnum = 0; msgnum < msgcnt; ++msgnum)
	{
		ret = QueReceive(QUE_Q_EA, qidlist, buf, MSGSZ, &prio,
						&retqid, QUE_CALLBACK(Callback_1, &Acb));
		if (ret < 0) 
		{
			if (ret == QUE_ER_ASYNC)
			{
				ret = XipcAsyncEventHandler();
				if (ret < 0)
				{
					fprintf(stderr, "XipcAsyncEventHandler errno = %d\n", ret);
					fflush(stderr);
				}
				continue;
			}
			else 
			{
				fprintf(stderr, "QueRecv failed. %s\n", XipcError(ret));
				fflush(stderr);
				break;
			}
		}
	}

	QueInfoQue(qid, &qinfo);

	ret = XipcLogout();
	if (ret < 0)
	{
		fprintf(stderr, "XipcLogout failed. %s\n", XipcError(ret));
		fflush(stderr);
	}

	fprintf(stderr, "Msgs #in = %d #out = %d\n", qinfo.CountIn, qinfo.CountOut);
	fflush(stderr);

	pthread_exit(&ret);
}

VOID
Callback_1(Aeb)
ASYNCRESULT	*Aeb;
{
	fprintf(stderr, "thread: enter callback\n");
	fflush(stderr);

	return;
}
