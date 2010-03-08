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


// INCLUDE FILES
#include <eikclb.h>   // For listbox
#include "AknExQueryBaseListEventHandler.h"

// ================= MEMBER FUNCTIONS =========================================

// ----------------------------------------------------------------------------
// CAknExQueryBaseListEventHandler::CAknExQueryBaseListEventHandler()
// Default constructor.
// ----------------------------------------------------------------------------
//
CAknExQueryBaseListEventHandler::CAknExQueryBaseListEventHandler( 
                                            CAknExQueryContainer* aContainer )
: iContainer( aContainer )
    {
    }

// ----------------------------------------------------------------------------
// CAknExQueryBaseListEventHandler::~CAknExQueryBaseListEventHandler()
// Destructor.
// ----------------------------------------------------------------------------
//
CAknExQueryBaseListEventHandler::~CAknExQueryBaseListEventHandler()
    {
    }

// ----------------------------------------------------------------------------
// CAknExQueryBaseListEventHandler::HandleListBoxEventL( 
//     CEikListBox* TListBoxEvent,
//     TListBoxEvent aEventType )
// ----------------------------------------------------------------------------
//
void CAknExQueryBaseListEventHandler::HandleListBoxEventL( 
    CEikListBox* aListBox,
    TListBoxEvent aEventType )
    {
    if ( ( aEventType == MEikListBoxObserver::EEventEnterKeyPressed ) ||
         ( aEventType == MEikListBoxObserver::EEventItemClicked ) )
        {
        HandleSelectedListItemL( aListBox->CurrentItemIndex() );
        }
    }

// End of File
