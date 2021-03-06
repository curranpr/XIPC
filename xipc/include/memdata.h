/*********************************************************** MEMDATA.H
**											  (MemSys - Memory System)
**
**	Name:	MEMDATA.H
**	----
**
**	Title:	MemSys Data Definition Header File
**	-----
**
**	Description:
**	-----------
**	This file generates either actual declarations or 'extern'
**	defintions of the data used by the Memory System.
**	Actual declarations are generated by including this file
**	with the preprocessor name 'MEM_DATA_ALLOC' defined.
**	'extern' definitions  are generated by including this file
**	with the preprocessor name 'MEM_DATA_ALLOC' un-defined.
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

/*********************************************************************
** Includes
*********************************************************************/

#include "mempubd.h"

/*********************************************************************
** determine if actual declarations or 'extern' defintions
*********************************************************************/

#ifdef	EXTERN
#undef	EXTERN
#endif

#ifdef	MEM_DATA_ALLOC
#define EXTERN	GLOBALDEF
#else
#define EXTERN	GLOBALREF DATAIMP
#endif

/*********************************************************************
** global variables - public and private
*********************************************************************/


#ifdef	C_PLUSPLUS
extern	"C" {
#endif

EXTERN SECTION GLOBALDATA MemEol
#ifdef	MEM_DATA_ALLOC
=
{
	-1L, -1L, -1L
}
#endif
;

#ifdef	C_PLUSPLUS
}
#endif


EXTERN SECTION GLOBALDATA MemAllHeld[1]
#ifdef	MEM_DATA_ALLOC
=
{
	MEM_ALL, 0L, 0L
}
#endif
;

/*
** END ***************************************************** MEMDATA.H
*/
