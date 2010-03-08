/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Common definitions between Ecmt Client dll and Server exe
*
*/


#ifndef ECMTCLIENTSERVERCOMMON_H
#define ECMTCLIENTSERVERCOMMON_H

#include <e32cmn.h>

/**
*  @defgroup EcmtClient Client
*  The Client component provides access to the framework functionality for any 
*  Symbian OS application.
*/


// Ecmt server version numbers
const TUint KEcmtServerMajorVersionNumber = 1;
const TUint KEcmtServerMinorVersionNumber = 0;
const TUint KEcmtServerBuildVersionNumber = 1;

// Name of server and semaphore used in starting synchronization
_LIT( KEcmtServerName, "EcmtCore" );
_LIT( KEcmtSemaphoreName, "EcmtServerSemaphore" );

// Definitions for default bearer setting. Needed from Ecmt Server and GuiLauncher
_LIT( KEcmtDefBearerFileName, "c:\\Data\\EcmtBearer.dat" );
_LIT(KEcmtBluetooth, "Bluetooth");
_LIT8( KEcmtBluetooth8, "Bluetooth" );
_LIT8( KEcmtWlan8, "WLAN" );
_LIT8( KEcmtUsb8, "USB" );

// Ecmt server UID
#define KECMT_SERVER_UID 0x10202062

/** Opcodes for server requests
*
*  @ingroup EcmtClient
*/
enum TEcmtServerRequests
    {
    EEcmtNotifyUid,
    EEcmtRequestServerNotification,
    EEcmtCancelRequestServerNotification,
    EEcmtRequestSendText,
    EEcmtRequestSendData
    };


#endif
// End of File
