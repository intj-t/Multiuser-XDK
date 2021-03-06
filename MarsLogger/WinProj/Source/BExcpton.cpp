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
*	Name: BExcpton.cpp
*
*  Purpose:	Exception classes and error codes defined here
*
****************************************************************************/

#if defined( WINDOWS )
#include "CPrecompile.h"
#endif

#include	"BExcpton.h"
#include	"BString.h"

#if ( SERVER )
#include	"ServerFunc.h"
#endif

#if defined( DEBUG )
long	SException::mDebugOptions = SException::kExceptionsAllOff;
#endif		// DEBUG

//++------------------------------------------------------------------------------
// SException::SException constructors
//++------------------------------------------------------------------------------
#if	defined( DEBUG )
SException::SException( MoaError code, char *aFileName, int aLineNumber )
		: mErrorCode(code), mFileName(aFileName), mLineNumber(aLineNumber)
{
	if ( mDebugOptions )
	{
		BString	msg( "Exception thrown at " );

		msg += aFileName;
		msg += " at line ";

		BString	lineStr;
		lineStr.SetToInteger( mLineNumber );
		msg += lineStr;

		msg += "\n";

		#if ( SERVER )
		if ( mDebugOptions & kExceptionsShowMsgOnScreen )
		{
			ServerFunc::DisplayMsg( msg );
		}
		#endif

		if ( mDebugOptions & kExceptionsStopInDebugger )
		{
			DebugBreakStr_( msg );
		}
	}
}
#else	// Release version
SException::SException( long code ) : mErrorCode(code)
{
}
#endif

