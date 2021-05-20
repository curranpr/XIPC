/********************************************************** XIPCPUXD.H
**			 (XIPC - Extended Interprocess Communications Faclilities)
**
**	Name:		XIPCPUXD.H
**	----
**
**	Title:		System Dependent Header File used internally.
**
**	Description:
**	-----------
**	This file contains extended standard definitions(used by xipc internally)
**  some MACROS are used by the sample programs distributed with the release. 
**  This file might not be distributed with the future releases. 
**
**  IMPORTANT NOTE: XIPC USERS SHOULD NOT USE THIS FILE UNDER ANY CONDITION.
**
**	Last Revision:
**	-------------
**	$Revision:	$
**	$Date:		$
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

#ifndef XIPCPUXD_H
#define XIPCPUXD_H XIPCPUXD_H

/*********************************************************************
** macros
*********************************************************************/

/* wait options */

#define XIPC_IPC_WAIT				0
#define XIPC_IPC_NOWAIT 			IPC_NOWAIT

/* IPC options */

#define XIPC_IPC_PRIVATE		IPC_PRIVATE
#define XIPC_SIG_ATTN			SIGINT
#define XIPC_SIG_QUIT			SIGQUIT
#define XIPC_SIG_TERM			SIGTERM
#define XIPC_SIG_USER			SIGUSR1
#define XIPC_SIG_PIPE			SIGPIPE
#define XIPC_SIG_URGENT 		SIGURG
#define XIPC_SIG_TIMER			SIGALRM

/* signal handler actions */

#define XIPC_SIGA_KILL			0
#define XIPC_SIGA_IGNORE		1
#define XIPC_SIGA_ACCEPT		2
#define XIPC_SIGA_ERROR 		3
#define XIPC_SIGA_ACKNOWLEDGE	4

/* return codes */

#define ERROR_SEM_IS_SET		99

/* file permissions */

#define XIPC_PERM_RW			0666

/*********************************************************************
** macros
*********************************************************************/

#define XIPCSIGFUNC_PROTO(Trap) \
VOID GLOBAL \
Trap()

#define XIPCSIGFUNC_DEFINE(Trap) \
VOID GLOBAL \
Trap(SigNum) \
XINT SigNum;

#define XIPC_NET_ABORT_OPERATION	{}

#endif /* XIPCPUXD_H */

/*
** END **************************************************** XIPCPUXD.H
*/
