/************************************************************ QUEERR.H
**					   (QueSys - Queue System)
**
**	Name:	QUEERR.H
**	----
**
**	Title:	Queue System Error Codes
**	-----
**
**	Description:
**	-----------
**	This file contains definitions for the QueSys error codes.
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

#ifndef QUEERR_H
#define QUEERR_H QUEERR_H

#include "xipcerr.h"

/*********************************************************************
** success return code
*********************************************************************/

#define QUE_SUCCESS 	XIPC_SUCCESS

/*********************************************************************
** XIPC error codes
*********************************************************************/

#define QUE_ER_NOACCESS 	XIPC_ER_NOACCESS
#define QUE_ER_NOTLOGGEDIN	XIPC_ER_NOTLOGGEDIN
#define QUE_ER_CAPACITY 	XIPC_ER_CAPACITY
#define QUE_ER_NOSUBSYSTEM	XIPC_ER_NOSUBSYSTEM
#define QUE_ER_NOASYNC		XIPC_ER_NOASYNC
#define QUE_ER_ISFROZEN 	XIPC_ER_ISFROZEN
#define QUE_ER_NOTFROZEN	XIPC_ER_NOTFROZEN
#define QUE_ER_ISMASKED 	XIPC_ER_ISMASKED
#define QUE_ER_NOTMASKED	XIPC_ER_NOTMASKED
#define QUE_ER_BADCONFIGNAME	XIPC_ER_BADCONFIGNAME
#define QUE_ER_BADCONFIGFILE	XIPC_ER_BADCONFIGFILE
#define QUE_ER_BADCONFIGINFO	XIPC_ER_BADCONFIGINFO
#define QUE_ER_BADLISTOFFSET	XIPC_ER_BADLISTOFFSET
#define QUE_ER_BADUSERNAME		XIPC_ER_BADUSERNAME
#define QUE_ER_BADFILENAME		XIPC_ER_BADFILENAME
#define QUE_ER_BADUID			XIPC_ER_BADUID
#define QUE_ER_BADVAL			XIPC_ER_BADVAL
#define QUE_ER_BADOPTION		XIPC_ER_BADOPTION
#define QUE_ER_BADBLOCKOPT		XIPC_ER_BADBLOCKOPT
#define QUE_ER_DUPLICATE		XIPC_ER_DUPLICATE
#define QUE_ER_NOTFOUND 		XIPC_ER_NOTFOUND
#define QUE_ER_NOWAIT			XIPC_ER_NOWAIT
#define QUE_ER_DESTROYED		XIPC_ER_DESTROYED
#define QUE_ER_NOTLOCAL 		XIPC_ER_NOTLOCAL
#define QUE_ER_NOMORE			XIPC_ER_NOMORE
#define QUE_ER_BLOCKCANCELED	XIPC_ER_BLOCKCANCELED
#define QUE_ER_FAILCANWAIT		XIPC_ER_FAILCANWAIT
#define QUE_ER_FAILRESET		XIPC_ER_FAILRESET
#define QUE_ER_FAILACQRESP		XIPC_ER_FAILACQRESP
#define QUE_ER_FAILTIMEOUT		XIPC_ER_FAILTIMEOUT
#define QUE_ER_FAILWAKEUP		XIPC_ER_FAILWAKEUP
#define QUE_ER_BADTRIGGERCODE	XIPC_ER_BADTRIGGERCODE
#define QUE_ER_TRIGGERNOTEXIST	XIPC_ER_TRIGGERNOTEXIST
#define QUE_ER_STARTCFG 		XIPC_ER_STARTCFG
#define QUE_ER_BADIDENTFILE 	XIPC_ER_BADIDENTFILE
#define QUE_ER_BADTEMPFILE		XIPC_ER_BADTEMPFILE
#define QUE_ER_STOPIDS			XIPC_ER_STOPIDS
#define QUE_ER_INSTACTIVE		XIPC_ER_INSTACTIVE
#define QUE_ER_INSTNOTACTIVE	XIPC_ER_INSTNOTACTIVE
#define QUE_ER_ASYNC			XIPC_ER_ASYNC
#define QUE_ER_ASYNCABORT		XIPC_ER_ASYNCABORT
#define QUE_ER_TIMEOUT			XIPC_ER_TIMEOUT
#define QUE_ER_INTERRUPT		XIPC_ER_INTERRUPT
#define QUE_ER_SYSERR			XIPC_ER_SYSERR
#define QUE_ER_FAILCRITON		XIPC_ER_FAILCRITON
#define QUE_ER_FAILCRITOFF		XIPC_ER_FAILCRITOFF
#define QUE_ER_FAILACQTRACESTEP XIPC_ER_FAILACQTRACESTEP
#define QUE_ER_FAILACQTRACEDONE XIPC_ER_FAILACQTRACEDONE
#define QUE_ER_FAILRELTRACESTEP XIPC_ER_FAILRELTRACESTEP
#define QUE_ER_FAILRELTRACEDONE XIPC_ER_FAILRELTRACEDONE
#define QUE_ER_MEMORY			XIPC_ER_MEMORY
#define QUE_ER_BADBUFFER		XIPC_ER_BADBUFFER

/*********************************************************************
** QueSys error codes
*********************************************************************/

#define QUE_ER_BLOCKEDWRITE 	(QUE_ER_BASECODE-(XINT)1)
#define QUE_ER_BLOCKEDPUT		(QUE_ER_BASECODE-(XINT)2)
#define QUE_ER_BLOCKEDGET		(QUE_ER_BASECODE-(XINT)3)
#define QUE_ER_BADSID			(QUE_ER_BASECODE-(XINT)10)
#define QUE_ER_BADQUENAME		(QUE_ER_BASECODE-(XINT)11)
#define QUE_ER_BADQID			(QUE_ER_BASECODE-(XINT)12)
#define QUE_ER_BADQIDLIST		(QUE_ER_BASECODE-(XINT)13)
#define QUE_ER_BADLENGTH		(QUE_ER_BASECODE-(XINT)14)
#define QUE_ER_BADLIMIT 		(QUE_ER_BASECODE-(XINT)15)
#define QUE_ER_BADPRIORITY		(QUE_ER_BASECODE-(XINT)16)
#define QUE_ER_BADTEXT			(QUE_ER_BASECODE-(XINT)17)
#define QUE_ER_BADMSGSELECTCODE (QUE_ER_BASECODE-(XINT)18)
#define QUE_ER_BADQUESELECTCODE (QUE_ER_BASECODE-(XINT)19)
#define QUE_ER_BADDIRECTION 	(QUE_ER_BASECODE-(XINT)20)
#define QUE_ER_PURGED			(QUE_ER_BASECODE-(XINT)21)
#define QUE_ER_NOTEMPTY 		(QUE_ER_BASECODE-(XINT)22)
#define QUE_ER_WAITEDON 		(QUE_ER_BASECODE-(XINT)23)
#define QUE_ER_MSGHDRREMOVED	(QUE_ER_BASECODE-(XINT)24)
#define QUE_ER_ENDOFQUEUE		(QUE_ER_BASECODE-(XINT)25)
#define QUE_ER_MSGHDRNOTREMOVED (QUE_ER_BASECODE-(XINT)26)
#define QUE_ER_TEXTFULL 		(QUE_ER_BASECODE-(XINT)27)
#define QUE_ER_TOOBIG			(QUE_ER_BASECODE-(XINT)31)
#define QUE_ER_UNGETFAILED		(QUE_ER_BASECODE-(XINT)32)
#define QUE_ER_MEMPOOL			(QUE_ER_BASECODE-(XINT)35)

#define QUE_ER_CAPACITY_USER	(QUE_ER_BASECODE-(XINT)41)
#define QUE_ER_CAPACITY_HEADER	(QUE_ER_BASECODE-(XINT)42)
#define QUE_ER_CAPACITY_NODE	(QUE_ER_BASECODE-(XINT)43)
#define QUE_ER_CAPACITY_TABLE	(QUE_ER_BASECODE-(XINT)44)
#define QUE_ER_CAPACITY_ASYNC_USER	(QUE_ER_BASECODE-(XINT)45)

#define QUE_ER_FAILSTART		(QUE_ER_BASECODE-(XINT)51)
#define QUE_ER_FAILSTOP 		(QUE_ER_BASECODE-(XINT)52)
#define QUE_ER_GHOSTSTART		(QUE_ER_BASECODE-(XINT)53)
#define QUE_ER_GHOSTSTOP		(QUE_ER_BASECODE-(XINT)54)
#define QUE_ER_NOSECCFG 		(QUE_ER_BASECODE-(XINT)55)
#define QUE_ER_NOSECIDS 		(QUE_ER_BASECODE-(XINT)56)

#define QUE_ER_RECEIVEBURSTSYNC 	(QUE_ER_BASECODE-(XINT)61)
#define QUE_ER_RECEIVEBURSTSTOP 	(QUE_ER_BASECODE-(XINT)62)
#define QUE_ER_NOTINSENDBURST		(QUE_ER_BASECODE-(XINT)63)
#define QUE_ER_NOTINRECEIVEBURST	(QUE_ER_BASECODE-(XINT)64)
#define QUE_ER_INSENDBURST			(QUE_ER_BASECODE-(XINT)65)
#define QUE_ER_INRECEIVEBURST		(QUE_ER_BASECODE-(XINT)66)
#define QUE_ER_BADERROROPT			(QUE_ER_BASECODE-(XINT)67)
#define QUE_ER_BADQCL				(QUE_ER_BASECODE-(XINT)68)
#define QUE_ER_BADCREDITVALUE		(QUE_ER_BASECODE-(XINT)69)
#define QUE_ER_BADCALLBACKOPT		(QUE_ER_BASECODE-(XINT)70)
#define QUE_ER_BADREADAHEAD 		(QUE_ER_BASECODE-(XINT)71)
#define QUE_ER_BADFLOWCONTROL		(QUE_ER_BASECODE-(XINT)72)
#define QUE_ER_FLOWCONTROL			(QUE_ER_BASECODE-(XINT)73)
#define QUE_ER_BADSYNCMODE			(QUE_ER_BASECODE-(XINT)74)
#define QUE_ER_MSGHDRDUPLICATED 	(QUE_ER_BASECODE-(XINT)75)

#endif /* QUEERR_H */

/*
** END ****************************************************** QUEERR.H
*/
