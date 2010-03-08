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



#ifndef __CCECUSTOMSORTER_H__
#define __CCECUSTOMSORTER_H__

//  INCLUDES
#include <e32base.h>
#include <e32std.h>
#include <MCLFCustomSorter.h>

// FORWARD DECLARATIONS
class MCLFItem;

// CLASS DECLARATION

/**
*  CLF Custom Sorter.
*  Example of Custom Sorter that can be used to sort files in the list model. 
*  With Custom Sorters you can create sorting functionality that is not 
*  supported in Content Listing Framework by default. This Custom Sorter 
*  will randomize the list model.
*/
class CCECustomSorter
    : public CBase,
      public MCLFCustomSorter
    {
    public:  // Constructors and destructor

		/**
		 * Creates and returns a new instance of this class.
		 * @return Pointer to the CCECustomSorter object
		 */
	    static CCECustomSorter* NewL();

        /**
        * Destructor.
        */
        virtual ~CCECustomSorter();

    private:    // Constructors
        /**
        * C++ default constructor.
        */
        CCECustomSorter();

    protected:  // Functions from base classes

        /**
        * From MCLFCustomSorter. Method for sorting Content Listing
        * Framework items. Will get called when sorting operation is executed
        * by calling refresh for the list model.
        * @since Series 60 3.0
        * @param aItemArray item array
        */

        void SortItemsL( RPointerArray<MCLFItem>& aItemArray );

    public:    // New functions

        /**
        * Compare two items for ordering the list model. In this case the 
        * list model will be randomized.
        * @param aFirst First item to compare
        * @param aSecond Second item to compare
        * @return Random number
        */
        static TInt Compare( const MCLFItem& aFirst,
                             const MCLFItem& aSecond );

    private:    // Data

    };

#endif // __CCECUSTOMSORTER_H__

// End of File
