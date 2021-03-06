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
****************************************************************************/

#if defined( WINDOWS )
#include "SrvXtnPrecompile.h"
#endif

#include "SrvExtension.h"
#include "SrvXtnUtils.h"
#include "SrvXtnDefs.h"


#include "SrvExtension.h"
#include "SrvXtnReg.h"
#include "Assert.h"
#include "Logger.h"
#include "MovieLogData.h"
#include "mmtypes.h"
#include "MarsErrors.h"


/************************************************************************
*	Symbols used by the xtra
************************************************************************/
static char * kLogStartTimeSym = 	"logStartTime";
static char * kLogEndTimeSym = 		"logEndTime";
static char * kServerStartTimeSym = "serverStartTime";
static char * kMovieLogDataSym = 	"movieLogData";
static char * kTotalUsersSym = 		"totalUsers";
static char * kMaxUsersSym = 		"maxUsers";
static char * kTimeConnected = 		"timeConnected";
static char * kResponseTimeSym =		"responseTime";
static char * kResponseTimeNumMsgsSym = "numberOfMessages";



//=======================================================================
//	Static functions
//=======================================================================
static MovieLogData *	FindMovieInLogDataList( CSrvXtn_IMoaSWServerExtension FAR * This,
												const BString & movieID );

static void				CreateNewMovieLogData( 	CSrvXtn_IMoaSWServerExtension * This,
												PISWServerMovie newMovie,
												const BString & movieID );

static long				CreateLogReport( 	CSrvXtn_IMoaSWServerExtension * This,
											BString & outReport );

static long				CreateLogInfoPropList( CSrvXtn_IMoaSWServerExtension * This,
											PISWServerContent content );

static MoaError			CheckPermission( CSrvXtn_IMoaSWServerExtension * This,
											PISWServerUser sender,
											long cmdNumber );

static long 			CalculateSeconds( long firstTimeStamp, long secondTimeStamp );

static void				WriteLogFileToDisk( CSrvXtn_IMoaSWServerExtension * This );



//=======================================================================
//	HandleServerIdle
//		Respond to server event
//=======================================================================
void	HandleServerIdle( CSrvXtn_IMoaSWServerExtension * This )
{
	if ( This->pObj->mIdleCounter < 1000 )
	{	// Ignore most calls
		This->pObj->mIdleCounter++;
	}
	else
	{	// Check to see if the day has changed
		This->pObj->mIdleCounter= 0;
		long curDay = GetServerDay( This );
		if ( curDay != This->pObj->mLastLogDay )
		{	// It's a brand new day !  Time to write the log file
			This->pObj->mLastLogDay = curDay;
			WriteLogFileToDisk( This );
		}
	}
}



//=======================================================================
//	LogMovieCreated
//		Respond to server event
//=======================================================================
long	LogMovieCreated( CSrvXtn_IMoaSWServerExtension * This,
							PISWServerMovie movie )
{
	MoaError err = kMoaErr_NoErr;

	// Get the movie ID
	BString movieID;
	AssertNotNull_( movie );
	movie->GetSetting( "movieID", (PMoaChar) movieID, movieID.MaxLength() );

	// Make certain we have an entry for this movie in our list
	bool foundIt = false;
	if ( This->pObj->mMovieDataList != NULL )
	{
		MovieLogDataList::iterator	iter = This->pObj->mMovieDataList->begin();
		while ( iter != This->pObj->mMovieDataList->end() )
		{
			if( movieID.Matches( (*iter).GetMovieID() ) )
			{
				foundIt = true;
				break;
			}
			iter++;
		}
	}

	if ( foundIt == false )
	{	// Not found, so create and add it to the list
		CreateNewMovieLogData( This, movie, movieID );
	}

	return err;
}



//=======================================================================
//	LogMovieDeleted
//		Respond to server event
//=======================================================================
long	LogMovieDeleted( CSrvXtn_IMoaSWServerExtension * This,
							PISWServerMovie movie )
{
	MoaError err = kMoaErr_NoErr;

	// Currently doesn't do anything - we leave data in the log info until it's written
	// In the future, may want to record time, etc.

	return err;
}


//=======================================================================
//	LogUserLogon
//		Respond to server event
//=======================================================================
long	LogUserLogon( CSrvXtn_IMoaSWServerExtension * This,
							PISWServerUser user )
{
	MoaError err = kMoaErr_NoErr;

	// Get the movie the user is logged on to
	PISWServerMovie movie = (PISWServerMovie) user->GetUsersMovie();
	AssertNotNull_( movie );

	// Get the movie ID
	BString movieID;
	movie->GetSetting( "movieID", (PMoaChar) movieID, movieID.MaxLength() );

	// Find this movie in our list
	MovieLogData * curEntry = FindMovieInLogDataList( This, movieID );
	AssertNotNull_( curEntry );
	if ( curEntry != NULL )
	{	// Add to the user count
		curEntry->IncrementTotalUsers();

		// See if this is a new max value
		MoaLong numUsers = 0;
		movie->GetSetting( "numUsers", (PMoaChar) &numUsers, sizeof( numUsers ) );
		long curMaxUsers = curEntry->GetMaxUsers();
		if ( numUsers > curMaxUsers )
		{
			curEntry->SetMaxUsers( numUsers );
		}
	}

	// Save the timestamp the user logged on at
	long curTimeStamp = 0;
	This->pObj->mServer->GetSetting( "timeStamp", (PMoaChar) &curTimeStamp, sizeof( curTimeStamp ) );
	user->SetData( "logonTimestamp", (PMoaChar) &curTimeStamp, sizeof( curTimeStamp ) );

	return err;
}



//=======================================================================
//	LogUserLogoff
//		Respond to server event
//=======================================================================
long	LogUserLogoff( CSrvXtn_IMoaSWServerExtension * This,
							PISWServerUser user )
{
	MoaError err = kMoaErr_NoErr;

	// Get the movie the user is logged on to
	PISWServerMovie movie = (PISWServerMovie) user->GetUsersMovie();
	AssertNotNull_( movie );

	// Get the movie ID
	BString movieID;
	movie->GetSetting( "movieID", (PMoaChar) movieID, movieID.MaxLength() );

	// Find this movie in our list
	MovieLogData * curEntry = FindMovieInLogDataList( This, movieID );
	AssertNotNull_( curEntry );
	if ( curEntry != NULL )
	{	// Adjust the hang time
		// Get the timestamp the user logged on at
		long curTimeStamp = 0;
		This->pObj->mServer->GetSetting( "timeStamp", (PMoaChar) &curTimeStamp, sizeof( curTimeStamp ) );

		// Get the timestamp the user logged on at
		long startTimeStamp = 0;
		user->GetData( "logonTimestamp", (PMoaChar) &startTimeStamp, sizeof( startTimeStamp ) );

		long secHangTime = CalculateSeconds( startTimeStamp, curTimeStamp );
		curEntry->IncrementTotalHangTime( secHangTime / 60.0 );
	}


	return err;
}




//=======================================================================
//	DoGetCurrentLog
//		Respond to user command to retrieve the current log state
//=======================================================================
long DoGetCurrentLog( CSrvXtn_IMoaSWServerExtension * This,
													ConstPMoaChar subject,
													ConstPMoaChar senderID,
													ConstPMoaChar recipientID,
													PISWServerContent msgContents,
													PISWServerUser sender,
													PISWServerMovie movie )
{
	MoaError err = kMoaErr_NoErr;
	MoaError sendErr = kMoaErr_NoErr;

	try
	{
		long contentPos = 0;

		if ( subject == NULL || senderID == NULL || msgContents == NULL || sender == NULL || movie == NULL )
		{
			err = kMoaErr_BadParam;
		}
		else
		{
			// Save original content position
			msgContents->GetPosition( &contentPos );

			// Check permission
			err = CheckPermission( This, sender, kCmdLoggerGetCurrentLog );

			long	returnMsgError = kMoaErr_NoErr;

			// Create the content for the return message
			PISWServerContent content = (PISWServerContent) This->pObj->mServer->CreateContent();
			if ( content == NULL )
			{
				err = kMoaErr_InternalError;
			}
			else if ( err == kMoaErr_NoErr )
			{
				// Create the log info to send back
				BString	logReport;
				CreateLogReport( This, logReport );

				// Send the report back as the message contents
				err = content->WriteValue( kMoaMmValueType_String, logReport.Length(), (char *) logReport );

				// Now send the reply back
				if ( err == kMoaErr_NoErr )
				{	// Send the return message
					sendErr = sender->SendMessage( senderID, returnMsgError, subject, recipientID, false, content );
				}
				else
				{	// On error, clear the content buffer.  The return message gets sent below.
					content->Reset();
				}

				// Release the content interface
				content->Release();
			}	// content != NULL
		}

		// If there's an error code, send back a message
		if ( err != kMoaErr_NoErr && sendErr == kMoaErr_NoErr )
		{
			// Restore the content position pointer first
			msgContents->SetPosition( contentPos );

			sendErr = sender->SendMessage( senderID, err, subject, recipientID, false, msgContents );
		}
	}
	catch(...)
	{
		err = kMoaErr_InternalError;
	}

	// Return no error here since we've passed the error return back to the sender.
	return kMoaErr_NoErr;
}



//=======================================================================
//	DoGetCurrentLogData
//		Server command to retrieve the log data as a prop list
//=======================================================================
long DoGetCurrentLogData( CSrvXtn_IMoaSWServerExtension * This,
						ConstPMoaChar subject,
						ConstPMoaChar senderID,
						ConstPMoaChar recipientID,
						PISWServerContent msgContents,
						PISWServerUser sender,
						PISWServerMovie movie )
{
	MoaError err = kMoaErr_NoErr;
	MoaError sendErr = kMoaErr_NoErr;

	try
	{
		long contentPos = 0;

		if ( subject == NULL || senderID == NULL || msgContents == NULL || sender == NULL || movie == NULL )
		{
			err = kMoaErr_BadParam;
		}
		else
		{
			// Save original content position
			msgContents->GetPosition( &contentPos );

			// Check permission
			err = CheckPermission( This, sender, kCmdLoggerGetCurrentLogData );

			long	returnMsgError = kMoaErr_NoErr;

			// Create the content for the return message
			PISWServerContent content = (PISWServerContent) This->pObj->mServer->CreateContent();
			if ( content == NULL )
			{
				err = kMoaErr_InternalError;
			}
			else if ( err == kMoaErr_NoErr )
			{
				// Create the log info to send back
				err = CreateLogInfoPropList( This, content );

				// Now send the reply back
				if ( err == kMoaErr_NoErr )
				{	// Send the return message
					sendErr = sender->SendMessage( senderID, returnMsgError, subject, recipientID, false, content );
				}
				else
				{	// On error, clear the content buffer.  The return message gets sent below.
					content->Reset();
				}

				// Release the content interface
				content->Release();
			}	// content != NULL
		}

		// If there's an error code, send back a message
		if ( err != kMoaErr_NoErr && sendErr == kMoaErr_NoErr )
		{
			// Restore the content position pointer first
			msgContents->SetPosition( contentPos );

			sendErr = sender->SendMessage( senderID, err, subject, recipientID, false, msgContents );
		}
	}
	catch(...)
	{
		err = kMoaErr_InternalError;
	}

	// Return no error here since we've passed the error return back to the sender.
	return kMoaErr_NoErr;
}



//=======================================================================
//	DoClearLogData
//		Server command to clear the current log data
//=======================================================================
long DoClearLogData( CSrvXtn_IMoaSWServerExtension * This,
						ConstPMoaChar subject,
						ConstPMoaChar senderID,
						ConstPMoaChar recipientID,
						PISWServerContent msgContents,
						PISWServerUser sender,
						PISWServerMovie movie )
{
	MoaError err = kMoaErr_NoErr;
	MoaError sendErr = kMoaErr_NoErr;

	try
	{
		long contentPos = 0;

		if ( subject == NULL || senderID == NULL || msgContents == NULL || sender == NULL || movie == NULL )
		{
			err = kMoaErr_BadParam;
		}
		else
		{
			// Save original content position
			msgContents->GetPosition( &contentPos );

			// Check permission
			err = CheckPermission( This, sender, kCmdLoggerClearLogData );

			// This could be modified in the future to get a list of movie IDs from the incoming
			// message and clear only that data.

			// Clear the data and send back a message
			if ( err == kMoaErr_NoErr )
			{	// Clear data
				SyncLogData( This );

				// Send the return message
				sendErr = sender->SendMessage( senderID, kMoaErr_NoErr, subject, recipientID, false, msgContents );
			}
		}

		// If there's an error code, send back a message
		if ( err != kMoaErr_NoErr && sendErr == kMoaErr_NoErr )
		{
			// Restore the content position pointer first
			msgContents->SetPosition( contentPos );

			sendErr = sender->SendMessage( senderID, err, subject, recipientID, false, msgContents );
		}
	}
	catch(...)
	{
		err = kMoaErr_InternalError;
	}

	// Return no error here since we've passed the error return back to the sender.
	return kMoaErr_NoErr;
}



//=======================================================================
//	DoGetResponseTime
//		Server command to get the current server response time
//=======================================================================
extern long DoGetResponseTime( CSrvXtn_IMoaSWServerExtension * This,
													ConstPMoaChar subject,
													ConstPMoaChar senderID,
													ConstPMoaChar recipientID,
													PISWServerContent msgContents,
													PISWServerUser sender,
													PISWServerMovie movie )
{
	MoaError err = kMoaErr_NoErr;
	MoaError sendErr = kMoaErr_NoErr;

	try
	{
		if ( subject == NULL || senderID == NULL || msgContents == NULL || sender == NULL || movie == NULL )
		{
			err = kMoaErr_BadParam;
		}
		else
		{
			// Check permission
			err = CheckPermission( This, sender, kCmdLoggerGetResponseTime );
			if ( err == kMoaErr_NoErr )
			{	// Get the response time and number of messages.


				long avgMs = 0;
				long numMsgs = 0;

				err = This->pObj->mServer->GetSetting( "responseTime",  (PMoaChar) &avgMs, sizeof(avgMs) );
				if ( err == kMoaErr_NoErr )
				{
					err = This->pObj->mServer->GetSetting( "responseTimeNumMsgs",  (PMoaChar) &numMsgs, sizeof(numMsgs) );
				}

				// Make contents to send back
				PISWServerContent content = (PISWServerContent) This->pObj->mServer->CreateContent();
				if ( content == NULL )
				{
					err = kMoaErr_InternalError;
				}
				else if ( err == kMoaErr_NoErr )
				{	// Make a prop list and return it.
					ThrowIfError_( content->WriteValue( kMoaMmValueType_PropList, 2, NULL ) );

					ThrowIfError_( content->WriteValue( kMoaMmValueType_Symbol, 0, kResponseTimeSym ) );
					ThrowIfError_( content->WriteValue( kMoaMmValueType_Integer, sizeof(avgMs) , &avgMs ) );

					ThrowIfError_( content->WriteValue( kMoaMmValueType_Symbol, 0, kResponseTimeNumMsgsSym ) );
					ThrowIfError_( content->WriteValue( kMoaMmValueType_Integer, sizeof(numMsgs), &numMsgs ) );

					// Send the return message
					sendErr = sender->SendMessage( senderID, kMoaErr_NoErr, subject, recipientID, false, content );

					// Release the content interface
					content->Release();
				}	// content != NULL
			}
		}

		// If there's an error code, send back a message
		if ( err != kMoaErr_NoErr && sendErr == kMoaErr_NoErr )
		{
			sendErr = sender->SendMessage( senderID, err, subject, recipientID, false, msgContents );
		}
	}
	catch(...)
	{
		err = kMoaErr_InternalError;
	}

	// Return no error here since we've passed the error return back to the sender.
	return kMoaErr_NoErr;
}




//=======================================================================
//	DoSwitchResponseTime
//		Server command to turn response time checking on or off
//=======================================================================
extern long DoSwitchResponseTime( CSrvXtn_IMoaSWServerExtension * This,
													ConstPMoaChar subject,
													ConstPMoaChar senderID,
													ConstPMoaChar recipientID,
													PISWServerContent msgContents,
													PISWServerUser sender,
													PISWServerMovie movie )
{
	MoaError err = kMoaErr_NoErr;
	MoaError sendErr = kMoaErr_NoErr;

	try
	{
		long contentPos = 0;

		if ( subject == NULL || senderID == NULL || msgContents == NULL || sender == NULL || movie == NULL )
		{
			err = kMoaErr_BadParam;
		}
		else
		{
			// Save original content position
			msgContents->GetPosition( &contentPos );

			// Check permission
			err = CheckPermission( This, sender, kCmdLoggerGetSwitchResponseTime );

			// Get the parameter, change setting and send back a message
			if ( err == kMoaErr_NoErr )
			{
				long valueType = 0;
				err = msgContents->GetValueInfo( &valueType, NULL, NULL );
				if ( err == kMoaErr_NoErr )
				{
					// Read the numeric value for a boolean flag
					long intFlag = 0;
					if ( valueType == kMoaMmValueType_Integer )
					{	// Have one time to read
						err = msgContents->ReadValue( (char *) &intFlag );
						if ( err == kMoaErr_NoErr )
						{
							// Change the setting
							err = This->pObj->mServer->ChangeSetting( "responseTimeChecks",  (PMoaChar) &intFlag, sizeof(intFlag) );
						}
					}
					else	// Not an integer
					{
						err = COMMERR_BAD_PARAMETER;
					}
				}

				// Send the return message
				sendErr = sender->SendMessage( senderID, kMoaErr_NoErr, subject, recipientID, false, msgContents );
			}
		}

		// If there's an error code, send back a message
		if ( err != kMoaErr_NoErr && sendErr == kMoaErr_NoErr )
		{
			// Restore the content position pointer first
			msgContents->SetPosition( contentPos );

			sendErr = sender->SendMessage( senderID, err, subject, recipientID, false, msgContents );
		}
	}
	catch(...)
	{
		err = kMoaErr_InternalError;
	}

	// Return no error here since we've passed the error return back to the sender.
	return kMoaErr_NoErr;
}








//=======================================================================
//	SyncLogData
//		Set log info to match the current state
//=======================================================================
long SyncLogData( CSrvXtn_IMoaSWServerExtension FAR * This )
{
	MoaError err = kMoaErr_NoErr;

	try
	{
		// For existing movies, clear the tempData which we'll use as a marker
		if ( This->pObj->mMovieDataList == NULL )
		{
			IMMEMTAG( "CreateNewMovieLogData new MovieLogDataList" );
			This->pObj->mMovieDataList = new MovieLogDataList;
		}

		ThrowIfNull_( This->pObj->mMovieDataList );

		MovieLogDataList::iterator	iter = This->pObj->mMovieDataList->begin();
		while ( iter != This->pObj->mMovieDataList->end() )
		{
			(*iter).SetTempData( 0 );
			iter++;
		}

		// Now scan the existing movies and make sure we have an entry for them
		if ( This->pObj->mServer != NULL )
		{
			MoaLong numMovies = 0;
			This->pObj->mServer->GetSetting( "numMovies", (PMoaChar) &numMovies, sizeof( numMovies ) );

			// Loop for all movies and set the event notifications
			MoaLong curMovieIndex = 0;
			while ( curMovieIndex < numMovies )
			{
				PISWServerMovie curMovie = (PISWServerMovie) This->pObj->mServer->GetMovieAt( curMovieIndex );
				if ( curMovie != NULL )
				{
					// Get the movie ID
					BString curMovieID;
					curMovie->GetSetting( "movieID", (PMoaChar) curMovieID, curMovieID.MaxLength() );

					// Find the movie in our log list
					MovieLogData * curEntry = FindMovieInLogDataList( This, curMovieID );
					if ( curEntry == NULL )
					{	// Not found, so add it to the list
						CreateNewMovieLogData( This, curMovie, curMovieID );
						curEntry = FindMovieInLogDataList( This, curMovieID );
					}

					// Set the temp data so we won't delete it later
					if ( curEntry != NULL )
					{
						curEntry->SetTempData( 1 );
					}
				}

				curMovieIndex++;
			}	// while ( curMovieIndex < numMovies )
		}

		// Scan the movie data list and delete the movies that are no longer in use
		if ( This->pObj->mMovieDataList != NULL )
		{
			MovieLogDataList::iterator	iter = This->pObj->mMovieDataList->begin();
			while ( iter != This->pObj->mMovieDataList->end() )
			{
				if ( (*iter).GetTempData() == 0 )
				{	// Movie doesn't exist any more, so delete it
					This->pObj->mMovieDataList->erase( iter );
				}
				else
				{	// Movie does exist, so go on to the next one
					iter++;
				}
			}
		}

		// Set the log start time value
		GetServerTimeString( This, This->pObj->mLogStartTime );


	}
	catch(...)
	{
		err = kMoaErr_OutOfMem;
	}

	return err;
}



//=======================================================================
//	CreateNewMovieLogData
//		Create a new movie data object and set it's values
//=======================================================================
static void CreateNewMovieLogData( CSrvXtn_IMoaSWServerExtension * This,
											PISWServerMovie newMovie,
											const BString & movieID )
{
	try
	{
		// Set all the initial values
		MovieLogData	newData;
		newData.SetMovieID( movieID );

		AssertNotNull_( This->pObj->mMovieDataList );

		IMMEMTAG( "CreateNewMovieLogData push_back" );
		This->pObj->mMovieDataList->push_back( newData );
	}
	catch(...)
	{
	}
}



//=======================================================================
//	FindMovieInLogDataList
//		Return the matching movie log data entry, NULL if not found
//=======================================================================
static MovieLogData *	FindMovieInLogDataList( CSrvXtn_IMoaSWServerExtension FAR * This,
												const BString & movieID )
{
	MovieLogData * curEntry = NULL;

	if ( This->pObj->mMovieDataList != NULL )
	{
		MovieLogDataList::iterator	iter = This->pObj->mMovieDataList->begin();
		while ( iter != This->pObj->mMovieDataList->end() )
		{
			if( movieID.Matches( (*iter).GetMovieID() ) )
			{
				curEntry = &(*iter);
				break;
			}
			iter++;
		}
	}

	return curEntry;
}


//=======================================================================
//	CreateLogInfoPropList
//		Get log info as a prop list
//=======================================================================
static long	CreateLogInfoPropList( CSrvXtn_IMoaSWServerExtension * This,
											PISWServerContent content )
{
	MoaError err = kMoaErr_NoErr;

	try
	{
		BString temp;
		long	x;
		double	d;

		// Start a prop list with N entires, one for each group.
		ThrowIfError_( content->WriteValue( kMoaMmValueType_PropList, 4, NULL ) );

		ThrowIfError_( content->WriteValue( kMoaMmValueType_Symbol, 0, kLogStartTimeSym ) );
		ThrowIfError_( content->WriteValue( kMoaMmValueType_String, 0, (char *) This->pObj->mLogStartTime ) );

		GetServerTimeString( This, temp );
		ThrowIfError_( content->WriteValue( kMoaMmValueType_Symbol, 0, kLogEndTimeSym ) );
		ThrowIfError_( content->WriteValue( kMoaMmValueType_String, 0, (char *) temp ) );

		ThrowIfError_( content->WriteValue( kMoaMmValueType_Symbol, 0, kServerStartTimeSym ) );
		ThrowIfError_( content->WriteValue( kMoaMmValueType_String, 0, (char *) This->pObj->mServerStartupTime ) );

		long movieListSize = 0;
		if ( This->pObj->mMovieDataList != NULL )
		{
			movieListSize = This->pObj->mMovieDataList->size();
		}
		ThrowIfError_( content->WriteValue( kMoaMmValueType_Symbol, 0, kMovieLogDataSym ) );
		ThrowIfError_( content->WriteValue( kMoaMmValueType_PropList, movieListSize, NULL ) );

		if ( This->pObj->mMovieDataList != NULL )
		{
			MovieLogDataList::iterator	iter = This->pObj->mMovieDataList->begin();
			while ( iter != This->pObj->mMovieDataList->end() )
			{
				temp = (*iter).GetMovieID();
				ThrowIfError_( content->WriteValue( kMoaMmValueType_String, 0, (char *) temp ) );
				ThrowIfError_( content->WriteValue( kMoaMmValueType_PropList, 3, NULL ) );

				x = (*iter).GetTotalUsers();
				ThrowIfError_( content->WriteValue( kMoaMmValueType_Symbol, 0, kTotalUsersSym ) );
				ThrowIfError_( content->WriteValue( kMoaMmValueType_Integer, 0, (char *) &x ) );

				x = (*iter).GetMaxUsers();
				ThrowIfError_( content->WriteValue( kMoaMmValueType_Symbol, 0, kMaxUsersSym ) );
				ThrowIfError_( content->WriteValue( kMoaMmValueType_Integer, 0, (char *) &x ) );

				d = (*iter).GetTotalHangTime();
				d = d / (*iter).GetTotalUsers();
				ThrowIfError_( content->WriteValue( kMoaMmValueType_Symbol, 0, kTimeConnected ) );
				ThrowIfError_( content->WriteValue( kMoaMmValueType_Float, 0, (char *) &d ) );

				iter++;
			}
		}
	}
	catch(...)
	{
		err = kMoaErr_OutOfMem;
	}

	return err;
}


//=======================================================================
//	CreateLogReport
//		Main routine to write the current log info
//=======================================================================
static long	CreateLogReport( 	CSrvXtn_IMoaSWServerExtension * This,
							BString & outReport )
{
	MoaError err = kMoaErr_NoErr;

	try
	{
		BString temp;
		BString	curLine;
		curLine.AllocateEnoughStringSpace( 256 );

		outReport = curLine;
		outReport.AllocateEnoughStringSpace( 2000 );

		// Start the header
		outReport = "Shockwave Multiuser Server Log Report\n";
		outReport = "=====================================\n";
		outReport += '\n';

		outReport += "Log start time:      ";
		outReport += This->pObj->mLogStartTime;
		outReport += '\n';

		outReport += "Log end time:        ";
		GetServerTimeString( This, temp );
		outReport += temp;
		outReport += '\n';

		outReport += '\n';

		outReport += "Server startup time: ";
		outReport += This->pObj->mServerStartupTime;
		outReport += '\n';

		outReport += '\n';

		outReport += "Number of movies:    ";
		long numMovies = 0;
		if ( This->pObj->mMovieDataList != NULL )
		{
			numMovies = This->pObj->mMovieDataList->size();
		}
		temp.SetToInteger( numMovies );
		outReport += temp;
		outReport += '\n';

		outReport += "Number of movies:    ";

		if ( This->pObj->mMovieDataList != NULL )
		{
			outReport += '\n';
			MovieLogDataList::iterator	iter = This->pObj->mMovieDataList->begin();
			while ( iter != This->pObj->mMovieDataList->end() )
			{
				outReport += "Movie ID:               ";
				outReport += (*iter).GetMovieID();
				outReport += '\n';

				outReport += "Total users:            ";
				temp.SetToInteger( (*iter).GetTotalUsers() );
				outReport += temp;
				outReport += '\n';

				outReport += "Maximum users:          ";
				temp.SetToInteger( (*iter).GetMaxUsers() );
				outReport += temp;
				outReport += '\n';

				outReport += "Average time connected: ";
				double avgTime = (*iter).GetTotalHangTime();
				avgTime = avgTime / (*iter).GetTotalUsers();
				temp.SetToDouble( avgTime, 0, 2 );
				outReport += temp;
				outReport += '\n';

				outReport += '\n';

				iter++;
			}
		}
	}
	catch(...)
	{
		outReport = "Unable to create report";
	}

	return err;
}




//=======================================================================
//	WriteLogFileToDisk
//		Write the log file to disk
//=======================================================================
static void				WriteLogFileToDisk( CSrvXtn_IMoaSWServerExtension * This )
{
	BString temp;

	GetServerTimeString( This, temp );		// Get server date
	temp.Delete( 8, 100 );					// Trim to YY/MM/DD
	temp.ReplaceAll( "/", "-" );			// Replace slashes with dashes so no crashes (Unix!)

	// Munge the full log file path name
	BString logFileName = This->pObj->mLogFilePath;
	logFileName += "SWServerUsage";
	logFileName += temp;
	logFileName += ".txt";

	// Open the file
	FILE * logFile = fopen( logFileName, "w" );

	if ( logFile == NULL )
	{	// Show an error message if we can't open it
		BString errMsg;
		errMsg = "*** Unable to open server log file: \"<1>\"\n";
		//errMsg.LoadFromResources( kMsgLogBadFileOpen );		// "Invalid configuration command for the Logger xtra: \"<1>\"\n"
		errMsg.Replace1st( BString( "<1>" ), logFileName );
		DisplayMessage( This, errMsg );
	}
	else
	{	// Create the log data and write to disk
		BString logReport;
		long err = CreateLogReport( This, logReport );

		if ( err == kMoaErr_NoErr )
		{
			long numWritten = fwrite( (const char *) logReport, 1, logReport.Length(), logFile );
			Assert_( numWritten == logReport.Length() );
		}

		fclose( logFile );
	}
}




//=======================================================================
//	GetServerTimeString
//		Return the current time string89
//=======================================================================
void			GetServerTimeString( CSrvXtn_IMoaSWServerExtension * This,
										BString & outString )
{
	AssertNotNull_( This->pObj->mServer );
	This->pObj->mServer->GetSetting( "currentTime", (PMoaChar) outString, outString.MaxLength() );
	outString.Delete( 19, 10 );		// Get rid of the milliseconds
}



//=======================================================================
//	GetServerDay
//		Return the current day as an integer
//=======================================================================
long			GetServerDay( CSrvXtn_IMoaSWServerExtension * This )
{
	BString temp;
	GetServerTimeString( This, temp );	// Returns YYYY/MM/DD hh:mm:ss

	temp.Delete( 0, 8 );
	temp.Delete( 2, 100 );
	long curDay = temp.Integer();

	return curDay;
}


//=======================================================================
//	CheckPermission
//		Checks the command permission.
//=======================================================================
static MoaError		CheckPermission( CSrvXtn_IMoaSWServerExtension * This,
													PISWServerUser sender,
													long cmdNumber )
{
	MoaError err = kMoaErr_NoErr;

	Assert_( sender != NULL );
	long	userLevel = 0;
	err = sender->GetSetting( "userLevel", (PMoaChar) &userLevel, sizeof(long) );
	if ( err == kMoaErr_NoErr )
	{
		Assert_( cmdNumber >= 0 && cmdNumber < kNumPermissions );
		if ( userLevel < This->pObj->mPermissions[ cmdNumber ] )
		{
			err = COMMERR_SERV_NOT_PERMITTED;
		}
	}

	return err;
}



//=======================================================================
//	CalculateSeconds
//		Calculates the difference betweeen two timeStamp values.
//		The second must be later than the first.   This takes in
//		account the rollover that happens on the server every 28 days.
//=======================================================================
static long CalculateSeconds( long firstTimeStamp, long secondTimeStamp )
{
	long diff = 0;
	if ( secondTimeStamp >= firstTimeStamp )
	{	// Normal case
		diff = secondTimeStamp - firstTimeStamp;
	}
	else
	{	// Deal with server wrap-around
		unsigned long temp = ((unsigned long) secondTimeStamp) + 0x80000000;
		diff = (long) (temp - firstTimeStamp);
	}

	return diff;
}



//=======================================================================
//	DisplayMessage
//		Static helper routine
//=======================================================================
void	DisplayMessage( CSrvXtn_IMoaSWServerExtension FAR * This, const char * message )
{
	// This xtra can be loaded at either the movie or server level
	if( This->pObj->mMovie != NULL )
	{
		This->pObj->mMovie->DisplayMessage( message );
	}
	else if ( This->pObj->mServer != NULL )
	{
		This->pObj->mServer->DisplayMessage( message );
	}
}



#if defined( DEBUG )
//=======================================================================
//	__stl_error_report
//		Debug function called from STL
//=======================================================================
__STL_BEGIN_NAMESPACE
void __stl_error_report(const char* file, int line, const char* diagnostic )
{
	Assert_( 0 );
	file;
	line;
	diagnostic;
}
__STL_END_NAMESPACE
#endif 	// DEBUG

