/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Ecmt server observer interface
*
*/



#ifndef MECMTSERVEROBSERVER_H
#define MECMTSERVEROBSERVER_H

//  INCLUDES
#include <e32std.h>

// CONSTANTS

// Notification messages 
_LIT( KEcmtServerListening, "Listening" );
_LIT( KEcmtServerConnected, "Connected" );
_LIT( KEcmtServerDisconnected, "Disconnected" );
_LIT( KEcmtServerError, "Error" );

// CLASS DECLARATION

/**
 * Ecmt server observer interface.
 * A client application that wishes to receive Ecmt server notification
 * messages such as connection / disconnection indications must implement
 * this interface and then register itself to Ecmt server using
 * REcmt::SetServerObserverL. There is at least one such client in the
 * system - the EcmtGui application. Other clients shoudn't be
 * concerned with the state of Ecmt server.
 *
 * @ingroup EcmtClient
 */
class MEcmtServerObserver
{
public:

    /**
     * Callback method invoked when a notification message arrive from Ecmt
     * server.
     *
     * @param aMsg Notification message from Ecmt server.
     * @param aError Completion code of message reception from server.
     * Either KErrNone if notification message was received succesfully
     * or one of the system wide error codes.
     */
    virtual void HandleEcmtServerNotification(const TDesC& aMsg, TInt aErr) =0;
};

#endif      
           
// End of File
