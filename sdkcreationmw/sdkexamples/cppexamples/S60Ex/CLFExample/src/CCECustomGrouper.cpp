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
#include "CCECustomGrouper.h"

#include "CLFExample.hrh"
#include "CLFExampleConsts.h"

#include <MCLFContentListingEngine.h>
#include <ContentListingFactory.h>
#include <MCLFModifiableItem.h>
#include <CLFContentListing.hrh>
#include <CLFExample.rsg>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCECustomGrouper::CCECustomGrouper
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCECustomGrouper::CCECustomGrouper( const TCLFDefaultFieldId aGroupingCriteria )
    : iGroupingCriteria( aGroupingCriteria )
    {
    }

// -----------------------------------------------------------------------------
// CCECustomGrouper::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCECustomGrouper* CCECustomGrouper::NewL( TCLFDefaultFieldId aGroupingCriteria )
    {
    CCECustomGrouper* self =
        new (ELeave) CCECustomGrouper( aGroupingCriteria );
    return self;
    }

// Destructor
CCECustomGrouper::~CCECustomGrouper()
    {
    iModItems.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CCECustomGrouper::GroupItemsL
// Method for grouping Content Listing Framework items.
// -----------------------------------------------------------------------------
//
void CCECustomGrouper::GroupItemsL( const TArray<MCLFItem*>& aSourceList,
                                    RPointerArray<MCLFItem>& aGroupedList )
    {
    // Delete old modifiable items (groups) is any
    iModItems.ResetAndDestroy();

    // Process all items of the source list
    TInt count( aSourceList.Count() );
    for( TInt i = 0; i < count; ++i )
        {
        // Get value of the field in iGroupingCriteria.
        // For example, if iFilterCriteria contains ECLFFieldIdArtist,
        // name of the artist is stored to fieldValue variable.
        TPtrC fieldValue;
        MCLFItem* currentItem = aSourceList[i];
        TInt error( currentItem->GetField(
            iGroupingCriteria, fieldValue ) );

        if( error == KErrNone )
            {
            // Check if the group is already added to the list
            if( !GroupAlreadyExists( fieldValue ) )
                {
                // Create a new modifiable item and add a field
                // name and value of the field.
                // For example, if grouping criteria is ECLFFieldIdArtist,
                // the field is added to the item with name of the artist.
                MCLFModifiableItem* modItem =
                    ContentListingFactory::NewModifiableItemLC();
                modItem->AddFieldL( iGroupingCriteria, fieldValue );

                // Add the item to the iModItems array to keep track,
                // which groups are already in the list (for example, which
                // groups are already in the list). The iModItems array
                // also deletes these items, when they are no longer needed.
                iModItems.AppendL( modItem ); // ownership transferred

                CleanupStack::Pop(); // modItem

                // Add the item to the model
                aGroupedList.AppendL( modItem );
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CCECustomGrouper::GroupAlreadyExists
// Checks if group has been added to the model already.
// -----------------------------------------------------------------------------
//
TBool CCECustomGrouper::GroupAlreadyExists( const TDesC& aGroupName )
    {
    TInt count( iModItems.Count() );
    for( TInt i = 0; i < count; ++i )
        {
        // Get value of the field in iGroupingCriteria.
        // For example, if iGroupingCriteria contains ECLFFieldIdArtist,
        // name of the artist is stored to fieldValue variable.
        TPtrC fieldValue;
        MCLFItem* currentItem = iModItems[i];
        TInt error( currentItem->GetField(
            iGroupingCriteria, fieldValue ) );

        if( error == KErrNone )
            {
            if( fieldValue == aGroupName )
                {
                // Group is in the list
                return ETrue;
                }
            }
        }

    // Group is not in the list
    return EFalse;
    }


//  End of File
