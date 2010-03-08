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



#ifndef __CCEPOSTFILTER_H__
#define __CCEPOSTFILTER_H__

//  INCLUDES
#include <e32base.h>
#include <e32std.h>
#include <MCLFPostFilter.h>
#include <CLFContentListing.hrh>

// FORWARD DECLARATIONS
class MCLFItem;

// CLASS DECLARATION

/**
*  CLF Post Filter.
*  Example of Post Filter that can be used to filter items from the
*  list model. With post filter your can select, which kind of items are
*  excluded from the list model. For example, you can include only music 
*  files of some specific artist and remove all other items.
*/
class CCEPostFilter 
    : public CBase,
      public MCLFPostFilter
    {
    public:  // Constructors and destructor

		/**
		 * Creates and returns a new instance of this class.
         * @param aFilterCriteria Filtering criteria (for example 
         *        ECLFFielIdAlbum or ECLFFieldIdArtist)
         * @param aFilterParameter Filtering parameter (for example 
         *        name of album or artist)
		 * @return Pointer to the CCEPostFilter object
		 */
	    static CCEPostFilter* NewL( const TCLFDefaultFieldId aFilterCriteria,
	                                const TDesC& aFilterParameter );

		/**
		 * Creates and returns a new instance of this class. Pointer is
		 * left to the CleanupStack.
         * @param aFilterCriteria Filtering criteria (for example 
         *        ECLFFielIdAlbum or ECLFFieldIdArtist)
         * @param aFilterParameter Filtering parameter (for example 
         *        name of album or artist)
		 * @return Pointer to the CCEPostFilter object
		 */
	    static CCEPostFilter* NewLC( const TCLFDefaultFieldId aFilterCriteria,
	                                 const TDesC& aFilterParameter );

        /**
        * Destructor.
        */
        virtual ~CCEPostFilter();

    private:    // Constructors
        /**
        * C++ default constructor.
        * @param aFilterCriteria Filtering criteria
        */
        CCEPostFilter( const TCLFDefaultFieldId aFilterCriteria );

        /**
        * Symbian 2nd phase constructor.
        */
        void ConstructL( const TDesC& aFilterParameter );

    protected:  // Functions from base classes

        /**
        * From MCLFPostFilter. Method for filtering the source list. Will get 
        * called when post filtering operation is executed by calling refresh 
        * for the list model.
        * @since Series 60 3.0
        * @param aItemList Source item list
        * @param aFilteredItemList Filtered/destination item list
        */
        void FilterItemsL( const TArray<MCLFItem*>& aItemList,
                           RPointerArray<MCLFItem>& aFilteredItemList );

    private:    // Data
    
        // Parameter for filtering (owned)
        HBufC* iFilterParameter;

        // Criteria for filtering
        TCLFDefaultFieldId iFilterCriteria;

    };

#endif // __CCEPOSTFILTER_H__

// End of File
