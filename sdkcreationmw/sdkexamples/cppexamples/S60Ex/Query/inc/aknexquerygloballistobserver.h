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


#ifndef __AKNEXQUERYGLOBALLISTOBSERVER_H__
#define __AKNEXQUERYGLOBALLISTOBSERVER_H__

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <AknGlobalListQuery.h>
//#include "AknExQueryContainer.h"

// FORWARD DECLARATIONS
class CAknExQueryContainer;

// CLASS DECLARATION

/**
* CAknExQueryGlobalListObserver class
*/
class CAknExQueryGlobalListObserver:
    public CActive
    {
    public:  // Constructors and destructor
        /**
        * NewL
        * Two-phased constructor.
        */
        static CAknExQueryGlobalListObserver* NewL( 
                                            CAknExQueryContainer* aContainer );

        /**
        * ~CAknExQueryGlobalListObserver
        * Destructor.
        */
        virtual ~CAknExQueryGlobalListObserver();

    public:     // New function
        /**
        * Start
        * Start the activator.
        */
        void Start();

        /*
        * SetSelectItemFlag
        * Set iSelectItemFlag.
        */
        void SetSelectItemFlag();

        /*
        * UnsetSelectItemFlag
        * Unset iSelectItemFlag.
        */
        void UnsetSelectItemFlag();

    private:    // New function
        /**
        * CAknExQueryGlobalListObserver
        * C++ default constructor.
        */
        CAknExQueryGlobalListObserver( CAknExQueryContainer* aContainer );

        /**
        * ConstructL
        * Symbian default constructor.
        */
        void ConstructL();

    private:    // From CActive
        /**
        * RunL
        * From CActive, handle an event.
        */
        void RunL();

        /**
        * DoCancel
        * From CActive, implement the Cancel protocol.
        */
        void DoCancel();
        
    private: // Data
        /**
        * iContainer, container class.
        * Does not own.
        */
        CAknExQueryContainer* iContainer;

        /**
        * iSelectItemFlag
        */
        TBool                 iSelectItemFlag;
    };

#endif // __AKNEXQUERYGLOBALLISTOBSERVER_H__

// End of File
