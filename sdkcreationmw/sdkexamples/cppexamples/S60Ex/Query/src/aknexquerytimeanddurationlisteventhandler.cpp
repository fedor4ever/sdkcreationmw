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
#include "AknExQuery.hrh"
#include "AknExQueryContainer.h"
#include "AknExQueryTimeAndDurationListEventHandler.h"
#include <AknExQuery.rsg>

// ================= MEMBER FUNCTIONS =========================================

// ----------------------------------------------------------------------------
// CAknExQueryTimeAndDurationListEventHandler::
//     CAknExQueryTimeAndDurationListEventHandler()
// Default constructor.
// ----------------------------------------------------------------------------
//
CAknExQueryTimeAndDurationListEventHandler::
                                    CAknExQueryTimeAndDurationListEventHandler( 
                                            CAknExQueryContainer* aContainer )
: CAknExQueryBaseListEventHandler( aContainer )
    {
    }

// ----------------------------------------------------------------------------
// CAknExQueryTimeAndDurationListEventHandler::
//     ~CAknExQueryTimeAndDurationListEventHandler()
// Destructor.
// ----------------------------------------------------------------------------
//
CAknExQueryTimeAndDurationListEventHandler::
    ~CAknExQueryTimeAndDurationListEventHandler()
    {
    }

// ----------------------------------------------------------------------------
// CAknExQueryTimeAndDurationListEventHandler::HandleSelectedListItemL()
// ----------------------------------------------------------------------------
//
void CAknExQueryTimeAndDurationListEventHandler::HandleSelectedListItemL( 
                                                                TInt aCommand )
    {
    switch ( aCommand )
        {
        case EAknExQueryListBoxCmdData1:
           iContainer->ShowMultiLineTimeAndDurationLayoutL( 
                R_AKNEXQUERY_MULTI_LINE_DATE_AND_DURATION_LAYOUT,
                R_AKNEXQUERY_DATE_INITIAL_VALUE );
            break;
        case EAknExQueryListBoxCmdData2:
            iContainer->ShowMultiLineTimeAndDurationLayoutL( 
                R_AKNEXQUERY_MULTI_LINE_TIME_AND_DURATION_LAYOUT,
                R_AKNEXQUERY_TIME_INITIAL_VALUE );
            break;
        default:
            break;
        }
    }

// End of File
