#ifndef _h_SrvExtension
#define _h_SrvExtension

/*************************************************************************
* Copyright � 1994-2001 Macromedia, Inc. All Rights Reserved
*
* THE CODE IS PROVIDED TO YOU ON AN "AS IS" BASIS AND "WITH ALL FAULTS,"
* WITHOUT ANY TECHNICAL SUPPORT OR WARRANTY OF ANY KIND FROM MACROMEDIA. YOU
* ASSUME ALL RISKS THAT THE CODE IS SUITABLE OR ACCURATE FOR YOUR NEEDS AND
* YOUR USE OF THE CODE IS AT YOUR OWN DISCRETION AND RISK. MACROMEDIA
* DISCLAIMS ALL EXPRESS AND IMPLIED WARRANTIES FOR CODE INCLUDING, WITHOUT
* LIMITATION, ANY WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR
* PURPOSE. ALSO, THERE IS NO WARRANTY OF NON-INFRINGEMENT, TITLE OR QUIET
* ENJOYMENT.
*
* YOUR USE OF THIS CODE IS SUBJECT TO THE TERMS, CONDITIONS AND RESTRICTIONS
* SET FORTH IN THE CORRESPONDING LICENSE AGREEMENT BETWEEN YOU AND MACROMEDIA.
* Filename:   SrvExtension.h
* Purpose:	  This file contains the member declarations for the CSrvXtn class,
*	          which is the heart of the server extension Xtra.
****************************************************************************/
/*****************************************************************************
*  INCLUDE FILE(S)
*  ---------------
****************************************************************************/

#ifndef _H_moaxtra
	#include "moaxtra.h"
#endif

#include	"SWServerXtra.h"

#ifdef MACINTOSH
	#include <windows.h>
	#include <string.h>
#endif


 /*****************************************************************************
 *	CLASS SPECIFIC DEFINE(S)
 *	-----------------------
 *	Any #define's specific to this class
 ****************************************************************************/
#ifndef UNUSED
	#define UNUSED(x) x
#endif



/*****************************************************************************
 *	CLSID
 *	-----
 *	The CLSID is a GUID that unquely identifies your MOA class.  To generate a
 *	GUID, use the genUID.app utility (Mac) or the GUIDGEN.EXE utility (Win).
 *	The following line will produce a pre-compiler error if not replaced with
 *	a valid CLSID.
 ****************************************************************************/
// guid defined in SrvXtnGUIDs.h
#include "SrvXtnGUIDs.h"


//=======================================================================
// Global routines.
//=======================================================================
PIMoaCalloc	GetCalloc( void );




/*****************************************************************************
 *  CLASS INSTANCE VARIABLES
 *  ------------------------
 *  Class instance variable are variables whose scope is exclusive to the
 *  methods implemented by your MOA class.  Variables necessary for the
 *  implementation of your MOA class should be placed here.
 ****************************************************************************/


EXTERN_BEGIN_DEFINE_CLASS_INSTANCE_VARS(CSrvXtn)

	PISWServerMovie		mMovie;		// Interface to the movie we are a part of
	PISWServer			mServer;	// Interface to the server we are a part of


	// Add your own class member variables here

EXTERN_END_DEFINE_CLASS_INSTANCE_VARS



/*****************************************************************************
 *  CLASS INTERFACE(S)
 *  ------------------
 *
 *  Syntax:
 *  EXTERN_BEGIN_DEFINE_CLASS_INTERFACE(<class-name>, <interface-name>)
 *		EXTERN_DEFINE_METHOD(<return-type>, <method-name>,(<argument-list>))
 *	EXTERN_END_DEFINE_CLASS_INTERFACE
 ****************************************************************************/
EXTERN_BEGIN_DEFINE_CLASS_INTERFACE(CSrvXtn, IMoaSWServerExtension)

	IMOASWSERVEREXTENSION_EXTERNS

EXTERN_END_DEFINE_CLASS_INTERFACE


#endif


