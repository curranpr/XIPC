/************************************************************* MMCOS.H
**
**  Name:       MMCOS.H (UNIX)
**  ----
**
**  Title:      Momentum Operating-System Dependent Common Header File
**  -----
**
**  Description:
**  -----------
**      This file contains operating-system definition that are 
**		common to all platforms.
**
**  Last Revision:
**  -------------
**  $Revision:  $
**  $Date:      $
**  $Author:    $
**
**  Revision History:
**  ----------------
**
**  Rev   Date      By   Description
**  ---   --------  ---  -----------
**
**
**  Copyright (C) Momentum Software Corporation, 1989
**
*********************************************************************/

#ifndef MMCOS_H
#define MMCOS_H MMCOS_H

/* function prototype specification */
#ifdef __cplusplus
#define	C_PLUSPLUS	
#endif

/* ANSI-C function prototype specification */

#if defined(__STDC__) || defined(__cplusplus)
#define ANSI_PROTOTYPES
#define	C_PROTOARGS(args)	args
#else  /* not __STDC__ and not __cplusplus */
#define	C_PROTOARGS(args)	()
#undef ANSI_PROTOTYPES
#endif

/*********************************************************************
** macros
*********************************************************************/

#define CDECL
#define XCDECL
#define XAPI
#define GLOBAL
#define GLOBALDATA

#define GLOBALDEF
#define GLOBALREF		extern

#define GLOBALIMP
#define DATAIMP

#define APIENTRY
#define FAR

#ifdef	VOID
#undef	VOID
#endif

#define VOID void

#define MEDINIT


/*********************************************************************
** data types
*********************************************************************/

#ifndef ANY
#define ANY void
#endif

typedef	void	XANY;

typedef int 			XINT;

typedef long 			XLONG;
typedef long 			XHANDLE;

typedef 		 char	CHAR;
typedef 		 int	INT;
typedef unsigned int	LBITS;
typedef unsigned int	BITS;
typedef unsigned int	FLAG;
typedef unsigned int    UXINT;

typedef int XTID;
typedef int XPID;
typedef VOID (GLOBAL XIPCSIGHANDLER)(INT);

#endif /* MMCOS_H */

/*
** END ******************************************************* MMCOS.H
*/
