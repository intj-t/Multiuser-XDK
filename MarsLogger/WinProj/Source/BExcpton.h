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
*	Name:  BExcpton.h
*	Purpose:	Exception classes and error codes defined here
****************************************************************************/


#pragma once
#if	!defined(__BExcpton__)
#define __BExcpton__


#include	"moatypes.h"

//++------------------------------------------------------------------------------
// Setup DebugBreak_ macros - used to force drop into debugger.
//++------------------------------------------------------------------------------
#undef DebugBreak_
#if defined( DEBUG )
	#if defined ( MACINTOSH )
		#ifdef	powerc
			#define DebugBreak_		SysBreak()
		#else
			#define DebugBreak_		Debugger()
		#endif
	#elif defined ( WINDOWS )
		#define DebugBreak_		DebugBreak()
	#endif
#else
	#define		DebugBreak_
#endif


// DebugStr_( string ) - used to dump a string to debugger, doesn't break.
// Not implemented for Mac
#undef DebugStr_
#if defined( DEBUG )
	#if defined( WINDOWS )
		#define	DebugStr_(s)	OutputDebugString(s)
	#else
		#define	DebugStr_(s)
	#endif
#else
	#define	DebugStr_(s)
#endif

// DebugBreakStr_( string ) - used to dump a string to debugger and break.
#undef DebugBreakStr_
#if defined( DEBUG )

	#if defined ( MACINTOSH )
		#include <string.h>
		#define DebugBreakStr_(s)	{ Str255 tempDebugBrkStr;					\
									  strcpy( (char *) tempDebugBrkStr, s );	\
									  c2pstr( (char *) &tempDebugBrkStr );		\
									  DebugStr( tempDebugBrkStr ); }
	#elif defined ( WINDOWS )
		// Warning - this is two statements, so be careful around if (...) DebugBreakStr_() usage
		#define DebugBreakStr_(s)	{ OutputDebugString(s); DebugBreak(); }
		// was DebugBreak()
	#endif
#else
	#define		DebugBreakStr_(s)
#endif


//++------------------------------------------------------------------------------
//	class SException
//++------------------------------------------------------------------------------
class SException
{
	public:
		#if defined( DEBUG )
		SException( MoaError code, char *aFileName, int aLineNumber );
		#else	// Release version
		SException( long code );
		#endif

		MoaError			GetErrorCode() const {	return mErrorCode; };

		#if defined( DEBUG )
		enum	// Bits for exception debugging options
		{
			kExceptionsAllOff			= 0,
			kExceptionsShowMsgOnScreen	= 1,	// Display the exception in some way
			kExceptionsStopInDebugger	= 2		// Stop in debugger
		};

		static long			GetDebugOptions( void )				{ return mDebugOptions;			}
		static void			SetDebugOptions( long newOptions )	{ mDebugOptions = newOptions;	}
		#endif	// DEBUG

	protected:
		MoaError			mErrorCode;

		//#if defined ( MACINTOSH )
		//static ExceptionHandler   mLowLevelExHandler;
		//#endif	// MACINTOSH

		#if defined( DEBUG )
		char				*mFileName;
		int					mLineNumber;

		static long			mDebugOptions;
		#endif 	// DEBUG
};






// Setup Throw_ macro

// das note - the reason we use exceptionErrorTemp is so that we don't
// get double function calls in cases where we do
// something like ThrowIfError_( myFunction() )

// Did an experiment on Windows to see if making exceptionErrorTemp a global
// would reduce code size.  It actually increased code size by about 2%

extern	long	exceptionErrorTemp;

#undef Throw_
#if defined( DEBUG )
	#define Throw_( err_ ) 				\
	{ long exceptionErrorTemp = err_; 	\
	throw SException(exceptionErrorTemp, __FILE__, __LINE__); }
#else
	#define Throw_( err_ ) 				\
	{ long exceptionErrorTemp = err_; 	\
	throw SException(exceptionErrorTemp); }
#endif

// Setup ThrowIfError_ macro
#undef ThrowIfError_
#if defined( DEBUG )
	#define ThrowIfError_( err_ ) 		\
	{ long exceptionErrorTemp = err_; 	\
	if ( exceptionErrorTemp != 0 )		\
		throw SException( exceptionErrorTemp, __FILE__, __LINE__ ); }
#else
	#define ThrowIfError_( err_ )		\
	{ long exceptionErrorTemp = err_; 	\
	if ( exceptionErrorTemp != 0 )		\
		throw SException( exceptionErrorTemp ); }
#endif


#define		kMoaErr_ThrowIfNull				MAKE_MOAERR( 0x14a0 )	/* ThrowIfNull_ triggered */


// Setup ThrowIfNull_ macro
#undef ThrowIfNull_
#if defined( DEBUG )
	#define ThrowIfNull_( test )					\
	if ( test == NULL )								\
		throw SException( kMoaErr_ThrowIfNull, __FILE__, __LINE__ )
#else
	#define ThrowIfNull_( test )					\
	if ( test == NULL )								\
		throw SException( kMoaErr_ThrowIfNull )
#endif


// Setup ThrowIfFalse_ macro
#undef ThrowIfFalse_
#if defined( DEBUG )
	#define ThrowIfFalse_( test )					\
	if ( !test )									\
		throw SException( kMoaErr_ThrowIfNull, __FILE__, __LINE__ )
#else
	#define ThrowIfFalse_( test )					\
	if ( !test )									\
		throw SException( kMoaErr_ThrowIfNull )
#endif

#endif // __BExcpton__


