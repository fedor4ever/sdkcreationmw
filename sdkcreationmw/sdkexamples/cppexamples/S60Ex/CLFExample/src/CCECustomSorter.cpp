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
#include "CCECustomSorter.h"

#include "CLFExample.hrh"

#include <e32math.h>
#include <MCLFContentListingEngine.h>
#include <ContentListingFactory.h>
#include <MCLFItem.h>
#include <CLFContentListing.hrh>
#include <CLFExample.rsg>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCECustomSorter::CCECustomSorter
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCECustomSorter::CCECustomSorter()
    {
    }

// -----------------------------------------------------------------------------
// CCECustomSorter::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCECustomSorter* CCECustomSorter::NewL()
    {
    CCECustomSorter* self = new (ELeave) CCECustomSorter;
    return self;
    }

// Destructor
CCECustomSorter::~CCECustomSorter()
    {
    }

// -----------------------------------------------------------------------------
// CCECustomSorter::SortItemsL
// Method for sorting Content Listing Framework items.
// -----------------------------------------------------------------------------
//
void CCECustomSorter::SortItemsL( RPointerArray<MCLFItem>& aItemArray )
    {
    // TLinearOrder can be used to sort RPointerArrays. TLinearOrder
    // defines a position for each item by comparing two items at a time.
    // It does the comparison by calling static method
    // CCECustomSorter::Compare. In this case, the Compare method
    // returns a random value and the array will be randomized. The
    // comparison process is started when RPointerArray::Sort method
    // is called.
    TLinearOrder<MCLFItem> sorter( CCECustomSorter::Compare );
    aItemArray.Sort( sorter );
    }

// -----------------------------------------------------------------------------
// CCECustomSorter::Compare
// "Compare" two items for ordering the list model by random.
// -----------------------------------------------------------------------------
//
TInt CCECustomSorter::Compare( const MCLFItem& /*aFirst*/,
                               const MCLFItem& /*aSecond*/ )
    {
    // return a random value
    return Math::Random();
    }

//  End of File
