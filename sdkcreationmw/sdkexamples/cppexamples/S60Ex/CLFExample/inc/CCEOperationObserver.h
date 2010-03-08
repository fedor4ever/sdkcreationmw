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



#ifndef __CCEOPERATIONOBSERVER_H__
#define __CCEOPERATIONOBSERVER_H__

// INCLUDES
#include <e32base.h>
#include <MCLFOperationObserver.h>

// FORWARD DECLARATIONS
class CCEMusicView;

// CLASS DECLARATION

/**
*  CLF Operation Observer.
*  Example of CLF Operation Observer that receives operation events of Content
*  Listing Framework. Operation event is generated when asynchronous refresh
*  operation of List Model is finished.
*/
class CCEOperationObserver
    : public CBase,
      public MCLFOperationObserver
    {
    public:  // Constructors and destructor

        /**
         * Creates and returns a new instance of this class.
         * @return Pointer to the CCEOperationObserver object
         */
        static CCEOperationObserver* NewL();

        /**
        * Destructor.
        */
        virtual ~CCEOperationObserver();

    private:    // Constructors
        /**
        * C++ default constructor.
        */
        CCEOperationObserver();

    protected:  // Functions from base classes

        /**
        * From MCLFOperationObserver. Method to handle list model operation
        * events.
        * @param aOperationEvent List model operation event
        * @param aError System wide error code
        */
        void HandleOperationEventL( TCLFOperationEvent aOperationEvent,
                                    TInt aError );

    public:     // New functions

        /**
        * Check if there is a refresh operation is ongoing.
        * @return ETrue if refresh is not finished,
        *         EFalse if refresh is finished
        */
        TBool IsRefreshOngoing();

        /**
        * Prepare for a refresh operation to start.
        * @param aWait The wait instance to stop when refreshing is completed
        */
        void PrepareForRefresh( CActiveSchedulerWait& aWait );

        /**
        * Set Music view
        * @param aTarget Music view that will be notified about item changes
        */
        void SetMusicView( CCEMusicView* aTarget );

        /**
        * Remove Music view
        */
        void RemoveMusicView();

    private:    // Data

        // For waiting the refresh operation to complete
        CActiveSchedulerWait* iWait;

        // For indicating that refresh operation is started but not finished
        TBool iRefreshOngoing;

        // Notify this target, when items are changed
        // in the file system (not owned)
        CCEMusicView* iTarget;


    };

#endif // __CCEOPERATIONOBSERVER_H__

// End of File
