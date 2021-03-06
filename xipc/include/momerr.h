/************************************************************ MOMERR.H
**					   (MomSys - Message Oriented Middleware System)
**
**	Name:	MOMERR.H
**	----
**
**	Title:	MOM System Error Codes
**	-----
**
**	Description:
**	-----------
**	This file contains definitions for the MomSys error codes.
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

#ifndef MOMERR_H
#define MOMERR_H MOMERR_H

#include "xipcerr.h"

/*********************************************************************
** success return code
*********************************************************************/

#define MOM_SUCCESS 	XIPC_SUCCESS
#define MOM_FAILURE 	XIPC_FAILURE

/*********************************************************************
** XIPC error codes
*********************************************************************/

#define MOM_ER_NOACCESS 		XIPC_ER_NOACCESS
#define MOM_ER_NOTLOGGEDIN		XIPC_ER_NOTLOGGEDIN
#define MOM_ER_NOSUBSYSTEM		XIPC_ER_NOSUBSYSTEM
#define MOM_ER_NOASYNC			XIPC_ER_NOASYNC
#define MOM_ER_ISFROZEN 		XIPC_ER_ISFROZEN
#define MOM_ER_NOTFROZEN		XIPC_ER_NOTFROZEN
#define MOM_ER_ISMASKED 		XIPC_ER_ISMASKED
#define MOM_ER_NOTMASKED		XIPC_ER_NOTMASKED
#define MOM_ER_BADCONFIGNAME	XIPC_ER_BADCONFIGNAME
#define MOM_ER_BADCONFIGFILE	XIPC_ER_BADCONFIGFILE
#define MOM_ER_BADCONFIGINFO	XIPC_ER_BADCONFIGINFO
#define MOM_ER_BADLISTOFFSET	XIPC_ER_BADLISTOFFSET
#define MOM_ER_BADUSERNAME		XIPC_ER_BADUSERNAME
#define MOM_ER_BADFILENAME		XIPC_ER_BADFILENAME
#define MOM_ER_BADUID			XIPC_ER_BADUID
#define MOM_ER_BADVAL			XIPC_ER_BADVAL
#define MOM_ER_BADOPTION		XIPC_ER_BADOPTION
#define MOM_ER_BADBLOCKOPT		XIPC_ER_BADBLOCKOPT
#define MOM_ER_DUPLICATE		XIPC_ER_DUPLICATE
#define MOM_ER_NOTFOUND 		XIPC_ER_NOTFOUND
#define MOM_ER_NOWAIT			XIPC_ER_NOWAIT
#define MOM_ER_DESTROYED		XIPC_ER_DESTROYED
#define MOM_ER_NOTLOCAL 		XIPC_ER_NOTLOCAL
#define MOM_ER_NOMORE			XIPC_ER_NOMORE
#define MOM_ER_BLOCKCANCELED	XIPC_ER_BLOCKCANCELED
#define MOM_ER_NOTSUPPORTED 	XIPC_ER_NOTSUPPORTED
#define MOM_ER_FAILCANWAIT		XIPC_ER_FAILCANWAIT
#define MOM_ER_FAILRESET		XIPC_ER_FAILRESET
#define MOM_ER_FAILACQRESP		XIPC_ER_FAILACQRESP
#define MOM_ER_FAILTIMEOUT		XIPC_ER_FAILTIMEOUT
#define MOM_ER_FAILWAKEUP		XIPC_ER_FAILWAKEUP
#define MOM_ER_BADTRIGGERCODE	XIPC_ER_BADTRIGGERCODE
#define MOM_ER_TRIGGERNOTEXIST	XIPC_ER_TRIGGERNOTEXIST
#define MOM_ER_ALREADY_OWNED	XIPC_ER_ALREADY_OWNED
#define MOM_ER_NOTOWNER 		XIPC_ER_NOTOWNER
#define MOM_ER_NOTOWNED 		XIPC_ER_NOTOWNED
#define MOM_ER_BADIDENTFILE 	XIPC_ER_BADIDENTFILE
#define MOM_ER_BADTEMPFILE		XIPC_ER_BADTEMPFILE
#define MOM_ER_INSTACTIVE		XIPC_ER_INSTACTIVE
#define MOM_ER_INSTNOTACTIVE	XIPC_ER_INSTNOTACTIVE
#define MOM_ER_ASYNC			XIPC_ER_ASYNC
#define MOM_ER_ASYNCABORT		XIPC_ER_ASYNCABORT
#define MOM_ER_TIMEOUT			XIPC_ER_TIMEOUT
#define MOM_ER_INTERRUPT		XIPC_ER_INTERRUPT
#define MOM_ER_SYSERR			XIPC_ER_SYSERR
#define MOM_ER_FAILCRITON		XIPC_ER_FAILCRITON
#define MOM_ER_FAILCRITOFF		XIPC_ER_FAILCRITOFF
#define MOM_ER_FAILACQTRACESTEP XIPC_ER_FAILACQTRACESTEP
#define MOM_ER_FAILACQTRACEDONE XIPC_ER_FAILACQTRACEDONE
#define MOM_ER_FAILRELTRACESTEP XIPC_ER_FAILRELTRACESTEP
#define MOM_ER_FAILRELTRACEDONE XIPC_ER_FAILRELTRACEDONE
#define MOM_ER_MEMORY			XIPC_ER_MEMORY

/*********************************************************************
** MomSys error codes
*********************************************************************/

#define MOM_ER_BADAQID			MOM_ER_BASECODE
#define MOM_ER_REMOTE_QUEUE 	(MOM_ER_BASECODE-(XINT)1)
#define MOM_ER_LOCAL_QUEUE		(MOM_ER_BASECODE-(XINT)2)
#define MOM_ER_BADAPPQUENAME	(MOM_ER_BASECODE-(XINT)3)
#define MOM_ER_NOTEMPTY 		(MOM_ER_BASECODE-(XINT)4)
#define MOM_ER_WAITEDON 		(MOM_ER_BASECODE-(XINT)5)
#define MOM_ER_BADLENGTH		(MOM_ER_BASECODE-(XINT)6)
#define MOM_ER_BADBUFFER		(MOM_ER_BASECODE-(XINT)7)
#define MOM_ER_TOOBIG			(MOM_ER_BASECODE-(XINT)8)
#define MOM_ER_SERVER			(MOM_ER_BASECODE-(XINT)9)
#define MOM_ER_BADMSGSPEC		(MOM_ER_BASECODE-(XINT)10)
#define MOM_ER_BLOCKED_RECEIVE	(MOM_ER_BASECODE-(XINT)11)	  /* Internal */
#define MOM_ER_NOMSG			(MOM_ER_BASECODE-(XINT)12)	  /* Internal */
#define MOM_ER_NOREPLYQUEUE 	(MOM_ER_BASECODE-(XINT)13)
#define MOM_ER_BADATTRBLOCK 	(MOM_ER_BASECODE-(XINT)14)
#define MOM_ER_BADATTR			(MOM_ER_BASECODE-(XINT)15)
#define MOM_ER_BADKEY			(MOM_ER_BASECODE-(XINT)16)
#define MOM_ER_INTERNAL 		(MOM_ER_BASECODE-(XINT)17)
#define MOM_ER_NONETWORK		(MOM_ER_BASECODE-(XINT)18)
#define MOM_ER_OUTOFSEQUENCE	(MOM_ER_BASECODE-(XINT)19)	  /* Internal */
#define MOM_ER_QUEUEFULL		(MOM_ER_BASECODE-(XINT)20)	  /* Internal */
#define MOM_ER_DBERROR			(MOM_ER_BASECODE-(XINT)21)	  /* Internal */
#define MOM_ER_DBFULL			(MOM_ER_BASECODE-(XINT)22)	  /* Internal */
#define MOM_ER_DBDUPLICATE		(MOM_ER_BASECODE-(XINT)23)	  /* Internal */
#define MOM_ER_DBNOTFOUND		(MOM_ER_BASECODE-(XINT)24)	  /* Internal */
#define MOM_ER_OWN_ASYNC_ABORTED (MOM_ER_BASECODE-(XINT)25)   /* Internal */
#define MOM_ER_BADPRIORITY		(MOM_ER_BASECODE-(XINT)26)
#define MOM_ER_RESENDDATA		(MOM_ER_BASECODE-(XINT)27)	  /* Internal */
#define MOM_ER_DBNOTEXTSPACE	(MOM_ER_BASECODE-(XINT)29)	  /* Internal */
#define MOM_ER_BADLINKID		(MOM_ER_BASECODE-(XINT)30)
#define MOM_ER_MRIINITFAILED	(MOM_ER_BASECODE-(XINT)31)
#define MOM_ER_MROINITFAILED	(MOM_ER_BASECODE-(XINT)32)
#define MOM_ER_SSSINITFAILED	(MOM_ER_BASECODE-(XINT)33)
#define MOM_ER_CATINITFAILED	(MOM_ER_BASECODE-(XINT)34)
#define MOM_ER_BADREPLYAQID 	(MOM_ER_BASECODE-(XINT)35)
#define MOM_ER_NODATABASE		(MOM_ER_BASECODE-(XINT)36)
#define MOM_ER_NOCATALOG		(MOM_ER_BASECODE-(XINT)37)
#define MOM_ER_MSG_ALREADY_RECEIVED (MOM_ER_BASECODE-(XINT)38)
#define MOM_ER_UNNAMEDCAT		(MOM_ER_BASECODE-(XINT)39)
#define MOM_ER_REGPRIVATE		(MOM_ER_BASECODE-(XINT)40)

#define MOM_ER_CAPACITY_USER		(MOM_ER_BASECODE-(XINT)41)
#define MOM_ER_CAPACITY_LOCALQUEUE	(MOM_ER_BASECODE-(XINT)42)
#define MOM_ER_CAPACITY_REMOTEQUEUE (MOM_ER_BASECODE-(XINT)43)
#define MOM_ER_CAPACITY_CS			(MOM_ER_BASECODE-(XINT)44)
#define MOM_ER_CAPACITY_INSTANCE	(MOM_ER_BASECODE-(XINT)45)
#define MOM_ER_CAPACITY_ASYNC_USER	(MOM_ER_BASECODE-(XINT)46)
#define MOM_ER_CAPACITY_MR			(MOM_ER_BASECODE-(XINT)47)
#define MOM_ER_CAPACITY_INTERNAL	(MOM_ER_BASECODE-(XINT)48)
#define MOM_ER_CAPACITY_REQUEST 	(MOM_ER_BASECODE-(XINT)49)
#define MOM_ER_CAPACITY_QUEUE_LIFE 	(MOM_ER_BASECODE-(XINT)50)

#define MOM_ER_CATERROR			(MOM_ER_BASECODE-(XINT)59)
#define MOM_ER_CATUNREACHABLE	(MOM_ER_BASECODE-(XINT)60)
#define MOM_ER_MSGEXPIRED		(MOM_ER_BASECODE-(XINT)61)
#define MOM_ER_MSGRETIRED		(MOM_ER_BASECODE-(XINT)62)
#define MOM_ER_UNNAMEDINST		(MOM_ER_BASECODE-(XINT)63)
#define MOM_ER_BADCOMMANDOPTION (MOM_ER_BASECODE-(XINT)64)
#define MOM_ER_APPQUEINACTIVE	(MOM_ER_BASECODE-(XINT)65)
#define MOM_ER_APPQUEACTIVE 	(MOM_ER_BASECODE-(XINT)66)
#define MOM_ER_DBINCONSISTENT	(MOM_ER_BASECODE-(XINT)67)
#define MOM_ER_CLKINITFAILED	(MOM_ER_BASECODE-(XINT)68)
#define MOM_ER_CATREGISTRATION	(MOM_ER_BASECODE-(XINT)69)
#define MOM_ER_NOTACCESSED		(MOM_ER_BASECODE-(XINT)70)
#define MOM_ER_MSGQDESTROYED 	(MOM_ER_BASECODE-(XINT)71)

/*********************************************************************
** MomSys MRI error codes
*********************************************************************/
#endif /* MOMERR_H */


/*
** END ****************************************************** MOMERR.H
*/
