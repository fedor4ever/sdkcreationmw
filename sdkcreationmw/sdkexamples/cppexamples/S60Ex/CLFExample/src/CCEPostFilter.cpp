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



// INCLUDE FILES
#include "CCEPostFilter.h"

#include "CLFExample.hrh"

#include <MCLFContentListingEngine.h>
#include <ContentListingFactory.h>
#include <MCLFItem.h>
#include <CLFContentListing.hrh>
#include <CLFExample.rsg>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCEPostFilter::CCEPostFilter
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCEPostFilter::CCEPostFilter( const TCLFDefaultFieldId aFilterCriteria )
    : iFilterCriteria( aFilterCriteria )
    {
    }

// -----------------------------------------------------------------------------
// CCEPostFilter::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCEPostFilter::ConstructL( const TDesC& aFilterParameter )
    {
    iFilterParameter = aFilterParameter.AllocL();
    }

// -----------------------------------------------------------------------------
// CCEPostFilter::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCEPostFilter* CCEPostFilter::NewL(
                            const TCLFDefaultFieldId aFilterCriteria,
	                        const TDesC& aFilterParameter )
    {
    CCEPostFilter* self =
        CCEPostFilter::NewLC( aFilterCriteria, aFilterParameter );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CCEPostFilter::NewLC
// Two-phased constructor, pointer is left to the CleanupStack.
// -----------------------------------------------------------------------------
//
CCEPostFilter* CCEPostFilter::NewLC(
                            const TCLFDefaultFieldId aFilterCriteria,
	                        const TDesC& aFilterParameter )
    {
    CCEPostFilter* self = new (ELeave) CCEPostFilter( aFilterCriteria );
    CleanupStack::PushL( self );
    self->ConstructL( aFilterParameter );
    return self;
    }

// Destructor
CCEPostFilter::~CCEPostFilter()
    {
    delete iFilterParameter;
    }

// -----------------------------------------------------------------------------
// CCEPostFilter::FilterItemsL
// Method for filtering the source list.
// -----------------------------------------------------------------------------
//
void CCEPostFilter::FilterItemsL( const TArray<MCLFItem*>& aItemList,
                                  RPointerArray<MCLFItem>& aFilteredItemList )
    {
    // Process all items in the item list
    TInt count( aItemList.Count() );
    for( TInt i = 0; i < count; ++i )
        {
        // Get value of the field defined in iFilterCriteria.
        // For example, if iFilterCriteria contains ECLFFieldIdAlbum,
        // name of the album is stored to fieldValue variable.
        TPtrC fieldValue;
        MCLFItem* currentItem = aItemList[i];
        TInt error( currentItem->GetField(
            iFilterCriteria, fieldValue ) );

        // Add the music file to the filtered list,
        // if value of of the field is same as iFilterParameter.
        // For example, if it is from the desired album.
        if( error == KErrNone )
            {
            if( fieldValue == iFilterParameter->Des() )
                {
                aFilteredItemList.AppendL( currentItem );
                }
            }
        }
    }

//  End of File
