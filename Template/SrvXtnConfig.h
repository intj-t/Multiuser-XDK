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
 *
 * 	File:		SrvXtnConfig.h
 *
 *	Purpose:	Set flags for build configurations	
****************************************************************************/

#pragma once
#if	!defined(_h_SrvXtnConfig_)
#define	_h_SrvXtnConfig_

#include	"stdio.h"

// Set flags for build configurations
#if defined( RELEASE )

	// Release builds - all should be off !
	#undef DEBUG
	#undef	__STL_DEBUG

#else
	// Development builds
	#ifndef DEBUG
		#define	DEBUG				1
	#endif

	#define	__STL_DEBUG				1
#endif


//--------------------------------------------------------------------------
// Configurations for Macintosh builds:
//--------------------------------------------------------------------------
#if defined( MAC )
#undef OLDROUTINENAMES

#if defined(__MC68K__)
#error No 68K builds !
#endif

// Include Metrowerks Mac headers
#include "MacHeaders.c"

#endif		// Mac


//--------------------------------------------------------------------------
// Configurations for Windows builds:
//--------------------------------------------------------------------------
#if WIN

// DAS:  we get some very, very long names with the STL
#pragma warning (disable : 4786)

const double _PI = 3.14159265359;
const double PI = 3.14159265359;

#endif		// Win


//--------------------------------------------------------------------------
// Cross-platform include files that are generally useful...
//--------------------------------------------------------------------------
#include	"Assert.h"
#include	"OperatorNew.h"

#endif		// _h_SrvXtnConfig_

