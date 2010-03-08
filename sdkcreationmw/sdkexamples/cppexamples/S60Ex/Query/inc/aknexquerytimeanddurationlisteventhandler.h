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


#ifndef __AKNEXQUERYTIMEANDDURATIONLISTEVENTHANDLER_H__
#define __AKNEXQUERYTIMEANDDURATIONLISTEVENTHANDLER_H__

// INCLUDES
#include "AknExQueryBaseListEventHandler.h"

// FORWARD DECLARATIONS
class CAknExQueryContainer;

// CLASS DECLARATION

/**
* CAknExQueryTimeAndDurationListEventHandler class.
*/
class CAknExQueryTimeAndDurationListEventHandler :
    public CAknExQueryBaseListEventHandler
    {
    public: // Constructors and destructor

        /**
        * CAknExQueryTimeAndDurationListEventHandler
        * Constructor.
        * @param aContainer Pointer to Container class object.
        */
        CAknExQueryTimeAndDurationListEventHandler( 
            CAknExQueryContainer* aContainer );

        /**
        * ~CAknExQueryTimeAndDurationListEventHandler
        * Destructor.
        */
        virtual ~CAknExQueryTimeAndDurationListEventHandler();

    private: // From CAknExQueryBaseListEventHandler

        /**
        * HandleSelectedListItemL
        * From CAknExQueryBaseListEventHandler, HandlerSelectedListItem.
        * Handles the event which list item selected.
        * @param aCommand Selected item index
        */
        void HandleSelectedListItemL( TInt aCommand );

    };

#endif // __AKNEXQUERYTIMEANDDURATIONLISTEVENTHANDLER_H__

// End of File
