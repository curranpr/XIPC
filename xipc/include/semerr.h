/************************************************************ SEMERR.H
**					   (SemSys - Semaphore System)
**
**	Name:	SEMERR.H
**	----
**
**	Title:	Semaphore System Error Codes
**	-----
**
**	Description:
**	-----------
**	This file contains definitions for the SemSys error codes.
**
**	Last Revision:
**	-------------
**	$Revision:	$
**	$Date:	$
**	$Author:	$
**
**	Revision History:
**	----------------
**
**	Rev   Date		By	 Description
**	---   --------	---  -----------
**
**
**	Copyright (C) Momentum Software Corporation, 1989
**
*********************************************************************/

#ifndef SEMERR_H
#define SEMERR_H SEMERR_H

#include "xipcerr.h"

/*********************************************************************
** success return code
*********************************************************************/

#define SEM_SUCCESS 	XIPC_SUCCESS

/*********************************************************************
** XIPC error codes
*********************************************************************/

#define SEM_ER_NOACCESS 		XIPC_ER_NOACCESS
#define SEM_ER_NOTLOGGEDIN		XIPC_ER_NOTLOGGEDIN
#define SEM_ER_CAPACITY 		XIPC_ER_CAPACITY
#define SEM_ER_NOSUBSYSTEM		XIPC_ER_NOSUBSYSTEM
#define SEM_ER_NOASYNC			XIPC_ER_NOASYNC
#define SEM_ER_ISFROZEN 		XIPC_ER_ISFROZEN
#define SEM_ER_NOTFROZEN		XIPC_ER_NOTFROZEN
#define SEM_ER_ISMASKED 		XIPC_ER_ISMASKED
#define SEM_ER_NOTMASKED		XIPC_ER_NOTMASKED
#define SEM_ER_BADCONFIGNAME	XIPC_ER_BADCONFIGNAME
#define SEM_ER_BADCONFIGFILE	XIPC_ER_BADCONFIGFILE
#define SEM_ER_BADCONFIGINFO	XIPC_ER_BADCONFIGINFO
#define SEM_ER_BADLISTOFFSET	XIPC_ER_BADLISTOFFSET
#define SEM_ER_BADUSERNAME		XIPC_ER_BADUSERNAME
#define SEM_ER_BADFILENAME		XIPC_ER_BADFILENAME
#define SEM_ER_BADUID			XIPC_ER_BADUID
#define SEM_ER_BADVAL			XIPC_ER_BADVAL
#define SEM_ER_BADOPTION		XIPC_ER_BADOPTION
#define SEM_ER_BADBLOCKOPT		XIPC_ER_BADBLOCKOPT
#define SEM_ER_DUPLICATE		XIPC_ER_DUPLICATE
#define SEM_ER_NOTFOUND 		XIPC_ER_NOTFOUND
#define SEM_ER_NOWAIT			XIPC_ER_NOWAIT
#define SEM_ER_DESTROYED		XIPC_ER_DESTROYED
#define SEM_ER_NOTLOCAL 		XIPC_ER_NOTLOCAL
#define SEM_ER_NOMORE			XIPC_ER_NOMORE
#define SEM_ER_BLOCKCANCELED	XIPC_ER_BLOCKCANCELED
#define SEM_ER_FAILCANWAIT		XIPC_ER_FAILCANWAIT
#define SEM_ER_FAILRESET		XIPC_ER_FAILRESET
#define SEM_ER_FAILACQRESP		XIPC_ER_FAILACQRESP
#define SEM_ER_FAILTIMEOUT		XIPC_ER_FAILTIMEOUT
#define SEM_ER_FAILWAKEUP		XIPC_ER_FAILWAKEUP
#define SEM_ER_BADTRIGGERCODE	XIPC_ER_BADTRIGGERCODE
#define SEM_ER_TRIGGERNOTEXIST	XIPC_ER_TRIGGERNOTEXIST
#define SEM_ER_BADIDENTFILE 	XIPC_ER_BADIDENTFILE
#define SEM_ER_BADTEMPFILE		XIPC_ER_BADTEMPFILE
#define SEM_ER_INSTACTIVE		XIPC_ER_INSTACTIVE
#define SEM_ER_INSTNOTACTIVE	XIPC_ER_INSTNOTACTIVE
#define SEM_ER_ASYNC			XIPC_ER_ASYNC
#define SEM_ER_ASYNCABORT		XIPC_ER_ASYNCABORT
#define SEM_ER_TIMEOUT			XIPC_ER_TIMEOUT
#define SEM_ER_INTERRUPT		XIPC_ER_INTERRUPT
#define SEM_ER_SYSERR			XIPC_ER_SYSERR
#define SEM_ER_FAILCRITON		XIPC_ER_FAILCRITON
#define SEM_ER_FAILCRITOFF		XIPC_ER_FAILCRITOFF
#define SEM_ER_FAILACQTRACESTEP XIPC_ER_FAILACQTRACESTEP
#define SEM_ER_FAILACQTRACEDONE XIPC_ER_FAILACQTRACEDONE
#define SEM_ER_FAILRELTRACESTEP XIPC_ER_FAILRELTRACESTEP
#define SEM_ER_FAILRELTRACEDONE XIPC_ER_FAILRELTRACEDONE
#define SEM_ER_MEMORY			XIPC_ER_MEMORY

/*********************************************************************
** SemSys error codes
*********************************************************************/

#define SEM_ER_BLOCKED			SEM_ER_BASECODE
#define SEM_ER_BADSEMNAME		(SEM_ER_BASECODE-(XINT)11)
#define SEM_ER_BADSEMVALUE		(SEM_ER_BASECODE-(XINT)12)
#define SEM_ER_BADSIDLIST		(SEM_ER_BASECODE-(XINT)13)
#define SEM_ER_BADSID			(SEM_ER_BASECODE-(XINT)14)
#define SEM_ER_BADCLEAROPT		(SEM_ER_BASECODE-(XINT)15)
#define SEM_ER_BADWAITTYPE		(SEM_ER_BASECODE-(XINT)16)
#define SEM_ER_BADACQUIRETYPE	(SEM_ER_BASECODE-(XINT)17)
#define SEM_ER_INVALIDSID		(SEM_ER_BASECODE-(XINT)18)
#define SEM_ER_SEMBUSY			(SEM_ER_BASECODE-(XINT)21)
#define SEM_ER_SEMNOTHELD		(SEM_ER_BASECODE-(XINT)22)
#define SEM_ER_SEMSET			(SEM_ER_BASECODE-(XINT)23)
#define SEM_ER_SEMCLEAR 		(SEM_ER_BASECODE-(XINT)24)
#define SEM_ER_CANCEL			(SEM_ER_BASECODE-(XINT)31)

#define SEM_ER_CAPACITY_USER	(SEM_ER_BASECODE-(XINT)41)
#define SEM_ER_CAPACITY_NODE	(SEM_ER_BASECODE-(XINT)42)
#define SEM_ER_CAPACITY_TABLE	(SEM_ER_BASECODE-(XINT)43)
#define SEM_ER_CAPACITY_ASYNC_USER	(SEM_ER_BASECODE-(XINT)44)

#define SEM_ER_FAILSTART		(SEM_ER_BASECODE-(XINT)51)
#define SEM_ER_FAILSTOP 		(SEM_ER_BASECODE-(XINT)52)
#define SEM_ER_GHOSTSTART		(SEM_ER_BASECODE-(XINT)53)
#define SEM_ER_GHOSTSTOP		(SEM_ER_BASECODE-(XINT)54)
#define SEM_ER_NOSECCFG 		(SEM_ER_BASECODE-(XINT)55)
#define SEM_ER_NOSECIDS 		(SEM_ER_BASECODE-(XINT)56)

#endif /* SEMERR_H */

/*
** END ****************************************************** SEMERR.H
*/