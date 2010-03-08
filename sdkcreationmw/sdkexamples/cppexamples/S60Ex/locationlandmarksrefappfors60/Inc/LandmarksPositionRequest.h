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


#ifndef __LANDMARKS_POSITION_REQUEST_H__
#define __LANDMARKS_POSITION_REQUEST_H__

// INCLUDES
#include <e32base.h>
#include <aknprogressdialog.h>
#include <lbs.h>

// FORWARD DECLARATIONS
class MLandmarksOperationObserver;
class CAknWaitDialog;

// CLASS DECLARATION

/**
*  Provides an interface for retrieving the current location.
*  It also manages a floating progress ball for indicating the progress of the 
*  location retrieval.
*/
class CLandmarksPositionRequest: public CActive, public MProgressDialogCallback
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        *
        * @param aAppName the name of this application
        * @param aObserver observer to callback when locations are retrieved
        */
        static CLandmarksPositionRequest* NewL(
            const TDesC& aAppName,
            MLandmarksOperationObserver* aObserver);

        /**
        * Destructor.
        */
        ~CLandmarksPositionRequest();

    public: // New Functions

        /**
        * Starts the fetching of a location. In the first place the default 
        * proxy is used for retrieving a location. If fetching default location
        * fails the last known location is fetched. If fetching last known 
        * location fails, an error code is returned to the observer.
        *
        * @param aMessage the message to present in the progress dialog.
        * @param aPositionInfo contains the fetched location
        */
        void FetchPositionInfoL(
            const TDesC& aMessage,
            TPositionInfo& aPositionInfo);

        /**
        * Dismisses the wait note.
        */
        void DismissWaitNote();

    protected: // from CActive

        /**
        * Implements cancellation of an outstanding request.
        */
        void DoCancel();

        /**
        * Handles an active object’s request completion event.
        */
        void RunL();

        /**
        * Handles a leave occurring in the request completion event 
        * handler RunL().
        *
        * @return KErrNone
        */
        TInt RunError(TInt aError);

    public: // from MProgressDialogCallback

        //! Inherited from MProgressDialogCallback
        void DialogDismissedL(TInt aButtonId);

    private:

        /**
        * C++ constructor.
        *
        * @param aObserver observer to callback when locations are retrieved
        */
        CLandmarksPositionRequest(
            MLandmarksOperationObserver* aObserver);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL(const TDesC& aAppName);

        /**
        * Executes the next step of the location retrieval.
        */
        void ExecuteNextStep();

        /**
        * Notifies the observer that the location retrieval is complete.
        *
        * @param aError the status of the location retrieval
        */
        void NotifyOperationCompleteL(TInt aError);

        /**
        * Initializes the class to be ready for location acquisition.
        */
        void InitializeL();

        /**
        * Informs user about location request failure.
        */
        void ShowErrorL(TInt aError);

    private: // Data types

        /**
        * TState defines the different states this object may enter.
        */
        enum TState 
            {
            EIdle,
            EAcquiringDefaultLocation,
            EAcquiringLastKnownLocation,
            EDismissingDialog
            };

    private: // Data

        //! Contains the location
        TPositionInfo* iPositionInfo;

        //! The observer to callback when location retieval is ready
        MLandmarksOperationObserver* iObserver;

        //! Reference to the Location Server session
        RPositionServer iLocationServer;

        //! Reference to the Location Server subsession
        RPositioner iPositioner;

        //! The current state this object has entered
        TState iState;

        //! The progress bar displayed when active
        CAknWaitDialog* iWaitNote;

        //! The name of this application
        HBufC* iAppName;

    };

#endif // __LANDMARKS_POSITION_REQUEST_H__

// End of File

