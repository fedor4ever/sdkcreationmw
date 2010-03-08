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



#ifndef __CCSASYNCREQUESTHANDLER_H__
#define __CCSASYNCREQUESTHANDLER_H__

// INCLUDE FILES
#include <e32base.h>
#include "TimeServerSession.h"

// FORWARD DECLARATIONS
class MAsyncTimeObserver;

// CLASS DECLARATION
/**
* CCSAsyncRequestHandler
*  An instance of the request handler object for the ClientServerAsync
*  example application
*/
class CCSAsyncRequestHandler : public CActive
    {
    public: // Constructors and destructors
        /**
        * NewL.
        * Two-phased constructor.
        * Creates a CCSAsyncRequestHandler object using two phase construction,
        * and return a pointer to the created object.
        * @param aObserver The object to be used to
        *                  handle updates from the server.
        * @return A pointer to the created instance of CCSAsyncRequestHandler.
        */
        static CCSAsyncRequestHandler* NewL( MAsyncTimeObserver& aObserver );

        /**
        * NewLC.
        * Two-phased constructor.
        * Creates a CCSAsyncRequestHandler object using two phase construction,
        * and return a pointer to the created object.
        * @param aObserver The object to be used to
        *                  handle updates from the server.
        * @return A pointer to the created instance of CCSAsyncRequestHandler.
        */
        static CCSAsyncRequestHandler* NewLC( MAsyncTimeObserver& aObserver );

        /**
        * ~CCSAsyncRequestHandler.
        * Destructor.
        * Destroys the object and release all memory objects.
        */
        virtual ~CCSAsyncRequestHandler();

    public: // New functions

        /**
        * RequestTime.
        * Sends a request to the server for an update to the time.
        */
        void RequestTime();

        /**
        * CancelRequest.
        * Cancels an outstanding request.
        */
        void CancelRequest() ;

        /**
        * Time.
        * Gets a copy of the last time received from the server.
        * @return The time.
        */
        TTime Time() const;

    protected: // Functions from base classes

        /**
        * From CActive, RunL.
        * Callback function.
        * Invoked to handle responses from the server.
        */
        void RunL();

        /**
        * From CActive, DoCancel.
        * Cancels any outstanding operation.
        */
        void DoCancel();

    private: // Constructors and destructors

        /**
        * CCSAsyncRequestHandler.
        * Performs the first phase of two phase construction.
        * @param aObserver The object to be used to
        *                  handle updates from the server.
        */
        CCSAsyncRequestHandler( MAsyncTimeObserver& aObserver );

        /**
        * ConstructL.
        * Performs the second phase construction of a
        * CCSAsyncRequestHandler object.
        */
        void ConstructL();

    private: // Data

        /**
        * iSession, the time server session.
        */
        RTimeServerSession iSession;

        /**
        * iTime, passed to the session to store the time.
        */
        TTime iTime;

        /**
        * iObserver, observer which handles updates from the server.
        */
        MAsyncTimeObserver& iObserver;
    };


#endif //__CCSASYNCREQUESTHANDLER_H__

// End of File