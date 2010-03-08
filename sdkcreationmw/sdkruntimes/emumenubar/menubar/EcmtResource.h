/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/
#ifndef ECMTRESOURCE_H
#define ECMTRESOURCE_H

#include <winuser.h>

#define IDM_MENU                    100
#define IDM_OPEN                    101
#define IDM_EXIT                    102
#define IDM_DIAGNOSTICS             103
//#define IDM_CANVAS                104
#define IDM_STARTUPMODE							104 //As IDM_CANVAS option is not in use, i am using the ID here.
//And if i use new number say 121,i have to declare the item two times in menu(EcmtMenubar.rc),
//then only it is getting displayed in menu.So just using existed one.

#define IDM_EVENT_GENERATION        105
#define IDM_UTILITIES               106
#define IDM_PREFERENCES             107
#define IDM_RELEASE_NOTES           108
#define IDM_USERS_GUIDE             109
#define IDM_INSTALLATION_GUIDE      110
#define IDM_JAVA_DOC                111
#define IDM_ABOUT                   112
#define IDM_REGISTER_NOW            113
#define IDM_LOAD                    114
#define IDM_SWITCH_CONFIG           115
#define IDM_LINK1                   116
#define IDM_LINK2                   117
#define IDM_LINK3                   118
#define IDM_LINK4                   119
#define IDM_LINK5                   120
#define ECMTMENUBAR_LINK_COUNT      6

#define IDB_ABOUT_LEFT              200
#define IDB_ABOUT_TOP               201
#define IDB_ABOUT_GRADIENT          202
#define IDB_JAVA_POWERED            203

#define IDS_WAIT_WINDOW             301

#define IDR_ACCELERATOR             400
#define IDD_ABOUT                   401
#define IDD_LOAD                    402
#define IDR_ABOUT_RTF               403

#define IDC_STATIC                  -1
#define IDC_ABOUT_IMAGE             500
#define IDC_PRODUCT_NAME            501
#define IDC_PRODUCT_KEY             502
#define IDC_ABOUT_TEXT_SCROLL       503
#define IDC_ABOUT_TEXT_NO_SCROLL    504
#define IDC_REGISTER                505
#define IDC_URL                     506
#define IDC_BROWSE                  507
#define IDC_ABOUT_RICHTEXT          508
//#define INTERNAL_RELEASE            1
#endif // ECMTRESOURCE_H
