/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef _ECMTGWUI_H_
#define _ECMTGWUI_H_

#include "..\..\..\NmitShared\src\native\wlib\w_lib.h"
#include <process.h>
#include <string.h>

#include "GwEngine.h"
#include "EcmtGwRc.h"

#include <windowsx.h>
#include <commctrl.h>

#ifndef BTNS_SHOWTEXT
#  define BTNS_SHOWTEXT 0x0040 /*  TBSTYLE_EX_MIXEDBUTTONS is required */
#endif /* BTNS_SHOWTEXT */

#ifndef TBSTYLE_EX_MIXEDBUTTONS    
#  define TBSTYLE_EX_MIXEDBUTTONS 0x00000008
#endif /* TBSTYLE_EX_MIXEDBUTTONS */

typedef struct _EcmtGwPort EcmtGwPort;
typedef struct _EcmtGwUI {
    EcmtGateway* engine;    /* the engine */
    HMODULE module;         /* resource module */
    HWND mainWindow;        /* hande to the main window */
    HWND mainDialog;        /* hande to the main window */
    HWND listView;          /* list view control (port list) */
    HWND toolBar;           /* handle to the toolbar */
    HWND refreshAnimation;  /* "Refresh" animation control */
    WNDPROC listWndProc;    /* Wndproc for the port list */
    HIMAGELIST toolBarImg;  /* toolbar images */
    HIMAGELIST toolBarImg2; /* toolbar "inactive" images */
    HICON emptyIcon;        /* Empty icon */
    HICON btPortIcon;       /* Bluetooth icon */
    HICON comPortIcon;      /* COM port icon */
		HICON lanPortIcon;       /* Bluetooth icon */
    HICON otherPortIcon;    /* Unknown port icon */
    int btPortIconIndex;    /* Bluetooth icon index in list view image list */
    int comPortIconIndex;   /* COM port icon index in list view image list */
		int wlanPortIconIndex;   /* COM port icon index in list view image list */
    int otherPortIconIndex; /* Unknown icon index in list view image list */
    Bool txLedOn;           /* TX LED is on */
    Bool rxLedOn;           /* RX LED is on */
    HFONT comPortFont;      /* font for the COM port label */
    HFONT statusFont;       /* font for the status label */
    Queue ports;            /* list of ports */
    const EcmtGwPort* port; /* selected port */
    Char* ecmtManExe;       /* full path to Ecmt Manager executable */
    HANDLE ecmtManHandle;   /* handle to the Ecmt Manager process */
    HWND waitDialog;        /* handle to the wait dialog */
    Char* emulatorHome;     /* full path to the SDK installation */
    Vector ecmtCommands;    /* pending EcmtManager commands */
    Bool refreshInProgress; /* True if port list is being refreshed */
    Bool refreshDone;       /* True if refresh has already completed */
} EcmtGwUI;

struct _EcmtGwPort {
    QEntry  entry;          /* entry in EcmtGwUI::ports */
    EcmtGatewayPort* port;  /* gateway port */
    int iItem;              /* index in the listbox */
};

/* utilities */
void ECMTGW_FormatError(EcmtGwUI* ui, UINT uMsgID, ...);

/* window functions */
extern LRESULT CALLBACK GUI_MainWndProc(HWND, UINT, WPARAM, LPARAM);
extern BOOL CALLBACK GUI_MainDlgProc(HWND, UINT, WPARAM, LPARAM);

/* control ids */
#define ID_TOOLBAR 1
#define ID_REFRESH_ANIMATION 1

/* timer events */
#define TX_LED_OFF_TIMER 2
#define RX_LED_OFF_TIMER 3

#define LED_OFF_TIMEOUT 250 /* ms */

/* Windows messages */
enum EcmtGwMessages {
    WM_ECMTGW_BASE = (WM_USER + 333),
    WM_ECMTGW_SELECT_PORT,              /* WPARAM == EcmtGwPort* */
    WM_ECMTGW_ACTIVATE,                 /* WPARAM == EcmtGatewayPort* */
    WM_ECMTGW_DEACTIVATE,               /* WPARAM == reason */
    WM_ECMTGW_PORT_FOUND,               /* WPARAM == EcmtGatewayPort* */
    WM_ECMTGW_PORT_MODIFIED,            /* WPARAM == EcmtGatewayPort* */
    WM_ECMTGW_PORT_DISAPPEARED,         /* WPARAM == EcmtGatewayPort* */
    WM_ECMTGW_REFRESH_STARTED,          /* no parameters */
    WM_ECMTGW_REFRESH_DONE,             /* no parameters */
    WM_ECMTGW_NEW_CLIENT,               /* no parameters */
    WM_ECMTGW_SEND_PACKET,              /* no parameters */
    WM_ECMTGW_SEND_PACKET_COMPLETE,     /* no parameters */
    WM_ECMTGW_RECEIVE_PACKET,           /* no parameters */
    WM_ECMTGW_RECEIVE_PACKET_COMPLETE,  /* no parameters */
	WM_WLAN_SET						    /* no parameters */
};

#endif /* _ECMTGWUI_H_ */
