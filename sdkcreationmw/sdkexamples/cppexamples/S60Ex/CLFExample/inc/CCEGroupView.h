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



#ifndef __CCEGROUPVIEW_H__
#define __CCEGROUPVIEW_H__

//  INCLUDES
#include <aknview.h>
#include <e32base.h>
#include <e32std.h>
#include <babackup.h>
#include <CLFContentListing.hrh>

#include "CLFExample.hrh"

// FORWARD DECLARATIONS
class MCLFContentListingEngine;
class MCLFItemListModel;

class CCEGroupContainer;
class CCEOperationObserver;
class CCECustomGrouper;
class CCEPostFilter;
class CCEWaitNote;

// CLASS DECLARATION

/**
*  Group view for CLF Example Application.
*  The view is for displaying groups of music files.
*/
class CCEGroupView
    : public CAknView
    {
    public:  // Constructors and destructor

		/**
		 * Creates and returns a new instance of this class.
		 * @param aEngine Pointer to a Content Listing Engine
		 * @param aModel Pointer to a Listing Model
		 * @param aObserver Pointer to a CLF Operation Observer
		 * @return Pointer to the CCEGroupView object
		 */
	    static CCEGroupView* NewL( MCLFContentListingEngine* aEngine,
	                               MCLFItemListModel* aModel,
	                               CCEOperationObserver* aObserver );

		/**
		 * Creates and returns a new instance of this class.
		 * The returned object is left on the cleanup stack.
		 * @param aEngine Pointer to a Content Listing Engine
		 * @param aModel Pointer to a Listing Model
		 * @param aObserver Pointer to a CLF Operation Observer
		 * @return Pointer to the CCEGroupView object
		 */
	    static CCEGroupView* NewLC( MCLFContentListingEngine* aEngine,
	                                MCLFItemListModel* aModel,
	                                CCEOperationObserver* aObserver );

        /**
        * Destructor.
        */
        virtual ~CCEGroupView();

    private:    // Constructors
        /**
        * C++ default constructor.
        * @param aEngine Pointer to a Content Listing Engine
        * @param aModel Pointer to a Listing Model
        * @param aObserver Pointer to a CLF Operation Observer
        */
        CCEGroupView( MCLFContentListingEngine* aEngine,
                      MCLFItemListModel* aModel,
                      CCEOperationObserver* aObserver );

        /**
        * Symbian 2nd phase constructor.
        */
        void ConstructL();

    public:     // Functions from base classes

        /**
        * From CAknView. Handle user's commands.
        */
        void HandleCommandL( TInt aCommand );

        /**
        * From CAknView. Get Id of the view.
        */
        TUid Id() const;

        /**
        * From CAknView. Activate the view.
        */
   		void DoActivateL(const TVwsViewId& aPrevViewId,
                               TUid aCustomMessageId,
                         const TDesC8& aCustomMessage);

        /**
        * From CAknView. Deactivate the view.
        */
        void DoDeactivate();

        /**
        * From CAknView. Handle change of rect.
        */
        void HandleClientRectChange();

        /**
        * From CAknView. Dim items of the menu pane.
        */
		void DynInitMenuPaneL( TInt aResourceId,
		                       CEikMenuPane* aMenuPane );

    private:    // New functions

        /**
        * Group items in the list model if they were grouped when the view
        * was deactivated. For example, files are grouped when you switch
        * back from music view to group view.
        * @param aGroupingState State of grouping
        */
        void GroupListItemsL( const TCEGrouping aGroupingState );

        /**
        * Show user a warning note that no music files were found for selected
        * grouping option.
        */
        void NoMusicFilesFoundL();

        /**
        * List all files from the list model to the list box.
        * @param aFieldId Field to view in the list box. Album name is shown 
        *        in album grouping and artist name is shown in the artist
        *        grouping.
        */
        void PopulateListBoxL( const TCLFDefaultFieldId aFieldId );

    private:    // Data

        // Container of group view (owned)
        CCEGroupContainer* iContainer;

        // Content listing engine (not owned)
        MCLFContentListingEngine* iEngine;

        // Content listing model (not owned)
        MCLFItemListModel* iModel;

        // Operation observer (not owned)
        CCEOperationObserver* iObserver;

        // Custom grouper for artist grouping (owned)
        CCECustomGrouper* iArtistGrouper;

        // Post filter for filtering by artist and album (owned)
        CCEPostFilter* iPostFilter;

        // Wait note dialog for indicating refreshing process of the List Model
        // (owned)
        CCEWaitNote* iWaitNote;

        // For waiting the asynchronous refresh operation to complete
        CActiveSchedulerWait iWait;

        // Grouping state that is currently selected
        TCEGrouping iGrouping;
    };

#endif // __CCEGROUPVIEW_H__

// End of File
