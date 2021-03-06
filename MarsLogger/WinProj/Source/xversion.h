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
*	Name: xversion.h
*
* 	Purpose: This file contains version information for the Xtra.
*
****************************************************************************/

#define	VER_FILETYPE				VFT_DLL

/////////////////////////////////////////////////////////////////////////////
//
// PER-PRODUCT CUSTOMIZATION
//
//    Edit the items in this section for each product.
//
/////////////////////////////////////////////////////////////////////////////

#define	VER_PRODUCTNAME				"Shockwave Multiuser Server Logger Xtra"

/////////////////////////////////////////////////////////////////////////////
//
// PER-RELEASE CUSTOMIZATION
//
//    Edit the items in this section for each release.
//
/////////////////////////////////////////////////////////////////////////////

#define	VER_MAJORVERSION			 1
#define	VER_MAJORVERSION_STRING		"1"
#define	VER_MINORVERSION			 0
#define	VER_MINORVERSION_STRING		"0"
#define	VER_BUGFIXVERSION			 0
#define	VER_BUGFIXVERSION_STRING	"0"


// Include the common Mars version file.
#include	"CommonVersion.h"

/////////////////////////////////////////////////////////////////////////////
//
// PER-COMPONENT CUSTOMIZATION
//
//    Edit the items in this section for each executable or library built.
//
/////////////////////////////////////////////////////////////////////////////

#define	VER_COMPONENTNAME			"Server Logger Xtra"

#ifdef	WIN16
	#error There is no Win16 build !
#else
	#define	VER_WINFILENAME			"ServerLogger.x32"
	#define	VER_MINMODULENAME		"ServerLogger"
#endif


