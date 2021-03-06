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
*   Filename:  MarsErrors.h
*
*	Purpose: Class to do basic server chores
*****************************************************************************/

#ifndef MARSERRORS_H
#define MARSERRORS_H
//  These are the globally defined error values.  There must be an entry for
//	each one in gCommErrorTable, defined in globdata.cpp.

// We've been allocated the range 0x14a0 - 0x150f for MoaErrors.  0x14a0 is used
// in BException, the rest are used here.   That gives us 95 possiblities here.

#include	"moatypes.h"

#define		kMarsErrorBase		0x14a1
enum
{
	COMMERR_NO_ERROR = 0,

	COMMERR_UNKNOWN_ERROR 				= MAKE_MOAERR( kMarsErrorBase +  0 ),
	COMMERR_INVALID_MOVIEID 			= MAKE_MOAERR( kMarsErrorBase +  1 ),
	COMMERR_INVALID_USERID 				= MAKE_MOAERR( kMarsErrorBase +  2 ),
	COMMERR_INVALID_PASSWORD 			= MAKE_MOAERR( kMarsErrorBase +  3 ),
	COMMERR_DATALOST 					= MAKE_MOAERR( kMarsErrorBase +  4 ),
	COMMERR_BAD_SERVER_NAME 			= MAKE_MOAERR( kMarsErrorBase +  5 ),
	COMMERR_SERVER_OR_MOVIE_FULL 		= MAKE_MOAERR( kMarsErrorBase +  6 ),
	COMMERR_BAD_PARAMETER 				= MAKE_MOAERR( kMarsErrorBase +  7 ),
	COMMERR_NOSOCKETS 					= MAKE_MOAERR( kMarsErrorBase +  8 ),
	COMMERR_NOCONNECTION 				= MAKE_MOAERR( kMarsErrorBase +  9 ),
	COMMERR_NOWAITINGMESSAGE 			= MAKE_MOAERR( kMarsErrorBase + 10 ),
	COMMERR_BAD_CONNECTION_ID 			= MAKE_MOAERR( kMarsErrorBase + 11 ),
	COMMERR_WRONG_NUMBER_OF_PARAMS 		= MAKE_MOAERR( kMarsErrorBase + 12 ),
	COMMERR_SOMETHING_FUNKY 			= MAKE_MOAERR( kMarsErrorBase + 13 ),
	COMMERR_CONNECTION_REFUSED 			= MAKE_MOAERR( kMarsErrorBase + 14 ),
	COMMERR_MESSAGE_BUFFER_FULL 		= MAKE_MOAERR( kMarsErrorBase + 15 ),

	COMMERR_INVALID_MESSAGE_FORMAT 		= MAKE_MOAERR( kMarsErrorBase + 16 ),
	COMMERR_INVALID_MESSAGE_LENGTH 		= MAKE_MOAERR( kMarsErrorBase + 17 ),
	COMMERR_MESSAGE_MISSING 			= MAKE_MOAERR( kMarsErrorBase + 18 ),

	// Server based errors
	COMMERR_SERV_INIT_FAILED 			= MAKE_MOAERR( kMarsErrorBase + 19 ),
	COMMERR_SERV_SEND_FAILED 			= MAKE_MOAERR( kMarsErrorBase + 20 ),
	COMMERR_SERV_CLOSE_FAILED 			= MAKE_MOAERR( kMarsErrorBase + 21 ),
	COMMERR_SERV_CONNECTION_DUPLICATE 	= MAKE_MOAERR( kMarsErrorBase + 22 ),
	COMMERR_SERV_BAD_NUMBER_RECIPIENTS 	= MAKE_MOAERR( kMarsErrorBase + 23 ),
	COMMERR_SERV_BAD_RECIPIENT 			= MAKE_MOAERR( kMarsErrorBase + 24 ),
	COMMERR_SERV_BAD_MESSAGE 			= MAKE_MOAERR( kMarsErrorBase + 25 ),
	COMMERR_SERV_INTERNAL_ERROR 		= MAKE_MOAERR( kMarsErrorBase + 26 ),
	COMMERR_SERV_CANT_JOIN_GROUP 		= MAKE_MOAERR( kMarsErrorBase + 27 ),
	COMMERR_SERV_ERROR_LEAVING_GROUP 	= MAKE_MOAERR( kMarsErrorBase + 28 ),
	COMMERR_SERV_BAD_GROUP_NAME 		= MAKE_MOAERR( kMarsErrorBase + 29 ),
	COMMERR_SERV_BAD_SERVER_COMMAND 	= MAKE_MOAERR( kMarsErrorBase + 30 ),
	COMMERR_SERV_NOT_PERMITTED		 	= MAKE_MOAERR( kMarsErrorBase + 31 ),
	COMMERR_SERV_BAD_DATABASE 			= MAKE_MOAERR( kMarsErrorBase + 32 ),
	COMMERR_SERV_BAD_INIT_FILE 			= MAKE_MOAERR( kMarsErrorBase + 33 ),
	COMMERR_SERV_CANT_WRITE_DATABASE 	= MAKE_MOAERR( kMarsErrorBase + 34 ),
	COMMERR_SERV_CANT_READ_DATABASE 	= MAKE_MOAERR( kMarsErrorBase + 35 ),
	COMMERR_SERV_BAD_ID_FOR_DATABASE 	= MAKE_MOAERR( kMarsErrorBase + 36 ),
	COMMERR_SERV_CANT_ADD_USER 			= MAKE_MOAERR( kMarsErrorBase + 37 ),
	COMMERR_SERV_DATABASE_LOCK 			= MAKE_MOAERR( kMarsErrorBase + 38 ),
	COMMERR_SERV_DATA_RECORD_NOT_UNIQUE = MAKE_MOAERR( kMarsErrorBase + 39 ),
	COMMERR_SERV_NO_CURRENT_RECORD 		= MAKE_MOAERR( kMarsErrorBase + 40 ),
	COMMERR_SERV_RECORD_DOESNT_EXIST 	= MAKE_MOAERR( kMarsErrorBase + 41 ),
	COMMERR_SERV_BOFEOF 				= MAKE_MOAERR( kMarsErrorBase + 42 ),
	COMMERR_SERV_SEEK_FAILED 			= MAKE_MOAERR( kMarsErrorBase + 43 ),
	COMMERR_SERV_NO_TAG_SELECTED 		= MAKE_MOAERR( kMarsErrorBase + 44 ),
	COMMERR_SERV_NO_CURRENT_DATABASE 	= MAKE_MOAERR( kMarsErrorBase + 45 ),
	COMMERR_SERV_CANT_FIND_CFG_FILE 	= MAKE_MOAERR( kMarsErrorBase + 46 ),
	COMMERR_SERV_RECORD_NOT_LOCKED 		= MAKE_MOAERR( kMarsErrorBase + 47 ),
	COMMERR_UNSAFE						= MAKE_MOAERR( kMarsErrorBase + 48 ),

	// New ones added with Pathfinder
	COMMERR_DATA_NOT_FOUND				= MAKE_MOAERR( kMarsErrorBase + 49 ),
	COMMERR_CONTENT_HAS_ERROR_INFO		= MAKE_MOAERR( kMarsErrorBase + 50 ),
	COMMERR_CONCURRENCY_EXCEPTION		= MAKE_MOAERR( kMarsErrorBase + 51 ),

	// New errors added with Tron
	COMMERR_UDP_SOCKET_ERROR			= MAKE_MOAERR( kMarsErrorBase + 52 ),

	// Adjust kCommErrLastValidError below when adding new values

	// Don't go beyond 95, see note above
	COMMERR_LAST_ERROR_PLUS_ONE			= MAKE_MOAERR( kMarsErrorBase + 96 )
};

#define	kCommErrLastValidError			COMMERR_UDP_SOCKET_ERROR


#endif		// MARSERRORS_H
