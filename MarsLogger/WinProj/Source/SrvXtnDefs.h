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
*	Name: SrvXtnDefs.h
*
* 	Purpose: Definitions specific to this Xtra.
*
****************************************************************************/

#ifndef _H_SrvXtnDefs
#define _H_SrvXtnDefs

/*****************************************************************************
 *  XTRA SPECIFIC DEFINE(S)
 *  -----------------------
 *	Any #define's specific to this Xtra.
 *****************************************************************************/

#define		kSystemLoggerGetCurrentLog		"System.Logger.GetCurrentLog"
#define		kSystemLoggerGetCurrentLogData	"System.Logger.GetCurrentLogData"
#define		kSystemLoggerClearLogData		"System.Logger.ClearLogData"

#define		kSystemLoggerGetResponseTime			"System.Logger.GetResponseTime"
#define		kSystemLoggerSwitchResponseTime			"System.Logger.SwitchResponseTime"


// Modify this to set the commands you xtra will respond to.
static char userIDTable[] =
{
	"+ "	kSystemLoggerGetCurrentLog		" \n"
	"+ "	kSystemLoggerGetCurrentLogData	" \n"
	"+ "	kSystemLoggerClearLogData		" \n"

	"+ "	kSystemLoggerGetResponseTime			" \n"
	"+ "	kSystemLoggerSwitchResponseTime
};


// The command numbers correspond to the above list
enum
{
	kCmdLoggerGetCurrentLog = 0,
	kCmdLoggerGetCurrentLogData,
	kCmdLoggerClearLogData,
	kCmdLoggerGetResponseTime,
	kCmdLoggerGetSwitchResponseTime
};



// Modify this to name the xtra
#define		ServerXtraName		"Logger"


#endif
