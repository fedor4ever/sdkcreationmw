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
* Description:  See class description below
*
*/


#ifndef __LANDMARKS_DB_EVENT_HANDLER_H__
#define __LANDMARKS_DB_EVENT_HANDLER_H__

// INCLUDES
#include "LandmarksDbObserver.h"
#include <e32base.h>
#include <epos_landmarks.h>

// FORWARD DECLARATIONS
class CPosLandmarkDatabase;
class MLandmarksDbEventObserver;

// CLASS DECLARATION

/**
*  In the Landmark reference application there is only one instance of 
*  CLandmarksDbEventHandler. It is the only object observing the 
*  default landmark database directly. Other objects that want to observe the 
*  database must register to this object. When an event occurs this object 
*  broadcasts it to all registered observers.
*/
class CLandmarksDbEventHandler : public CActive
    {
    public: // Constructors and destructor

        /**
        * C++ constructor.
        *
        * @param aDb the database to observer
        */
        CLandmarksDbEventHandler(
            CPosLandmarkDatabase& aDb);

        /**
        * Destructor.
        */
        ~CLandmarksDbEventHandler();

    public: // New functions

        /**
        * AddObserverL registers a db observer.
        *
        * @param aObserver the observer to register
        */
        void AddObserverL(MLandmarksDbObserver* aObserver);

    protected: // from CActive

        /**
        * Handles an active object’s request completion event.
        */
        void RunL();

        /**
        * Implements cancellation of an outstanding request.
        */
        void DoCancel();

        /**
        * Handles a leave occurring in the request completion event 
        * handler RunL().
        *
        * @return KErrNone
        */
        TInt RunError(TInt aError);

    private:

        /**
        * Starts observing of db enevs
        */
        void ObserveEvents();

    private:

        //! The database to observe
        CPosLandmarkDatabase& iDb;

        //! A db event to be reported
        TPosLmEvent iEvent;

        //! An array of registered observers
        RPointerArray<MLandmarksDbObserver> iDbObservers;

    };


#endif // __LANDMARKS_DB_EVENT_HANDLER_H__

// End of File

