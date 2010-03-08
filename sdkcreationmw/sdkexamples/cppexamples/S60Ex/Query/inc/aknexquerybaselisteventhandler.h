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


#ifndef __AKNEXQUERYBASELISTEVENTHANDLER_H__
#define __AKNEXQUERYBASELISTEVENTHANDLER_H__

// INCLUDES
#include <e32base.h>
#include <eiklbo.h> // For MEikListBoxObserver

// FORWARD DECLARATIONS
class CAknExQueryContainer;

// CLASS DECLARATION

/**
* CAknExQueryBaseListEventHandler class.
*/
class CAknExQueryBaseListEventHandler :
    public CBase,
    public MEikListBoxObserver
    {
    public: // Constructors and destructor

        /**
        * CAknExQueryBaseListEventHandler
        * Constructor.
        * @param aContainer Pointer to Container class object.
        */
        CAknExQueryBaseListEventHandler( CAknExQueryContainer* aContainer );

        /**
        * ~CAknExQueryBaseListEventHandler
        * Destructor.
        */
        virtual ~CAknExQueryBaseListEventHandler();

    public: // New function

        /**
        * HandleSelectedListItemL
        * Handles events that list item selected.
        * Pure virtual function.
        * @param aCommand Selected list item index
        */
        virtual void HandleSelectedListItemL( TInt aCommand ) = NULL;

    public: // From MEikListBoxObserver

        /**
        * HandleListBoxEventL
        * From MEikListBoxObserver, Handle listbox event
        * Handles the list box events.
        * When list item is selected, HandleSelectedListItemL() is called.
        * @param aListBox Pointer to ListBox object.
        * @param aEventType
        */
        void HandleListBoxEventL( 
            CEikListBox* aListBox,
            TListBoxEvent aEventType );

    protected: //data
        /**
        * iContainer
        * Does not own
        */
        CAknExQueryContainer* iContainer;

    };

#endif // __AKNEXQUERYBASELISTEVENTHANDLER_H__

// End of File
