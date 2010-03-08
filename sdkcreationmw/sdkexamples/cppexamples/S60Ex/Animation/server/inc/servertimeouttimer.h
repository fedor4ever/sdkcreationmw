/*
* Copyright (c) 2004 - 2006 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __SERVERTIMEOUTTIMER_H__
#define __SERVERTIMEOUTTIMER_H__

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class MTimeoutNotify;


// CLASS DECLARATION

/**
* CTimeOutTimer
* An instance of CTimeoutTimer is a timer for scheduling animation
*/
class CTimeOutTimer : public CTimer
    {
    public: // Constructors and destructor

        /**
        * NewL.
        * Two-phased constructor.
        * Create a CTimeOutTimer object using two phase construction,
        * and return a pointer to the created object.
        * @param aPriority the priority to use for the active object
        * @param aTimeOutNotify the observer to notify
        * @return A pointer to the created instance of CTimeOutTimer.
        */
        static CTimeOutTimer* NewL( const TInt aPriority, 
                                    MTimeoutNotify& aTimeOutNotify );

        /**
        * NewLC.
        * Two-phased constructor.
        * Create a CTimeOutTimer object using two phase construction,
        * and return a pointer to the created object.
        * @param aPriority the priority to use for the active object
        * @param aTimeOutNotify the observer to notify
        * @return A pointer to the created instance of CTimeOutTimer.
        */
        static CTimeOutTimer* NewLC( const TInt aPriority, 
                                     MTimeoutNotify& aTimeOutNotify );

        /**
        * ~CTimeOutTimer
        * Virtual Destructor.
        */
        virtual ~CTimeOutTimer();

    public: // From CTimer

        /**
        * RunL
        * Handle the event when the timer expires.
        */
        void RunL();

    protected: // Constructor

        /**
        * CTimeOutTimer
        * Perform the first phase of two phase construction.
        * @param aPriority the priority to use for the active object
        * @param aTimeOutNotify the observer to notify
        */
        CTimeOutTimer( const TInt aPriority, MTimeoutNotify& aTimeOutNotify );

    protected: // Constructor

        /**
        * ConstructL
        * Perform the second phase of two phase construction.
        */
        void ConstructL();

    private: // Data

        /**
        * iNotify
        * Reference to object implementing the notify interface.
        * Called on timeout.
        */
        MTimeoutNotify& iNotify;
    };

#endif //__SERVERTIMEOUTTIMER_H__

// End of File
