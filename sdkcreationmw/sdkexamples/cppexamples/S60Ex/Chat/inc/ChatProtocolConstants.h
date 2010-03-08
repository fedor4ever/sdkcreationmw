/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef CHATPROTOCOLCONSTANTS_H
#define CHATPROTOCOLCONSTANTS_H

// INCLUDES
#include <bt_sock.h> 

const TInt KRfcommChannel = 1;
const TInt KServiceClass = 0x1101;    //  SerialPort

const TUid KUidChatApp = { 0x10005B8B };

_LIT( KStrRFCOMM, "RFCOMM" );

const TInt KListeningQueSize = 1;
const TInt KChatTextBufLength = 128;
const TInt KDefaultPortNumber = 5005;
const TInt KMaxServerNameLength = 256;
const TInt KMaxMarkLen = 2;

const TInt KChatTimeOut = 20000000; // 20 seconds time-out

#endif //CHATPROTOCOLCONSTANTS_H

// End of File
