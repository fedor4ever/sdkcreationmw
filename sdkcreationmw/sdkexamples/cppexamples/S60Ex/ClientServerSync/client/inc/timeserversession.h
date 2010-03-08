/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef __TIMESERVERSESSION_H__
#define __TIMESERVERSESSION_H__

// INCLUDE FILES
#include <e32base.h>

// CONSTANTS
// Number of message slots to reserve for this client server session.
// Since we only communicate synchronously here, we never have any
// outstanding asynchronous requests.
static const TUint KDefaultMessageSlots = 0;

// Server UID
const TUid KServerUid3 = { 0xA00001FC };

_LIT( KTimeServerFilename, "CSSyncServer.EXE" );

#ifdef __WINS__
static const TUint KServerMinHeapSize =  0x1000;  //  4K
static const TUint KServerMaxHeapSize = 0x10000;  // 64K
#endif

// CLASS DECLARATION
/**
* RTimeServerSession
*  This class provides the client-side interface to the server session
*/
class RTimeServerSession : public RSessionBase
    {
    public: // Constructors and destructors

        /**
        * RTimeServerSession.
        * Constructs the object.
        */
        RTimeServerSession();

    public: // New functions

        /**
        * Connect.
        * Connects to the server and create a session.
        * @return Error code.
        */
        TInt Connect();

        /**
        * Version.
        * Gets the version number.
        * @return The version.
        */
        TVersion Version() const;

        /**
        * RequestTime.
        * Issues a request for the time to the server.
        * @param aTime The location for the server to write data into.
        */
        void RequestTime( TTime& aTime ) const;

    };

#endif // __TIMESERVERSESSION_H__


// End of File
