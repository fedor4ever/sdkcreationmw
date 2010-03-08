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


#ifndef __CCEMUSICVIEW_H__
#define __CCEMUSICVIEW_H__

// INCLUDES
#include <aknview.h>
#include <e32base.h>
#include <e32std.h>

// FORWARD DECLARATIONS
class MCLFContentListingEngine;
class MCLFItemListModel;
class MCLFSortingStyle;

class CCEMusicContainer;
class CCEOperationObserver;
class CCESoundPlayer;
class CCECustomSorter;
class CCEWaitNote;

// CLASS DECLARATION

/**
*  Music files view for CLF Example Application.
*  The view is for displaying and playing music files of selected group.
*/
class CCEMusicView 
    : public CAknView
    {
    public:  // Constructors and destructor

		/**
		 * Creates and returns a new instance of this class.
		 * @param aEngine Pointer to a Content Listing Engine
		 * @param aModel Pointer to a Listing Model
		 * @param aObserver Pointer to a CLF Operation Observer
		 * @return Pointer to the CCEMusicView object
		 */
	    static CCEMusicView* NewL( MCLFContentListingEngine* aEngine, 
	                               MCLFItemListModel* aModel, 
	                               CCEOperationObserver* aObserver );

		/**
		 * Creates and returns a new instance of this class. 
		 * The returned object is left on the cleanup stack.
		 * @param aEngine Pointer to a Content Listing Engine
		 * @param aModel Pointer to a Listing Model
		 * @param aObserver Pointer to a CLF Operation Observer
		 * @return Pointer to the CCEMusicView object
		 */
	    static CCEMusicView* NewLC( MCLFContentListingEngine* aEngine, 
	                                MCLFItemListModel* aModel, 
	                                CCEOperationObserver* aObserver );

        /**
        * Destructor.
        */
        virtual ~CCEMusicView();

    private:    // Constructors
        /**
        * C++ default constructor.
        * @param aEngine Pointer to a Content Listing Engine
        * @param aModel Pointer to a Listing Model
        * @param aObserver Pointer to a CLF Operation Observer
        */
        CCEMusicView( MCLFContentListingEngine* aEngine, 
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

    public:     // New functions
        
        /**
        * Changed item observer will call this function when some item has 
        * changed.
        */
        void ItemsChangedL();

        /**
        * List all files from the list model to the list box.
        */
        void PopulateListBoxL();

    private:    // New functions

        /**
        * Renames selected music file.
        * @param aFileIndex Index number of the selected file to be renamed
        */
        void RenameMusicFileL( const TInt aFileIndex );
        
        /**
        * Query and set sorting options.
        */
        void SetSortingL();
        
    private:    // Data
        
        // Container of music view (owned)
        CCEMusicContainer* iContainer;

        // Content listing engine (not owned)
        MCLFContentListingEngine* iEngine;

        // Content listing model (not owned)
        MCLFItemListModel* iModel;
        
        // Operation observer for CLF operations (not owned)
        CCEOperationObserver* iObserver;
       
        // Sound player for playing audio files (owned) 
        CCESoundPlayer* iSoundPlayer;
        
        // Wait note dialog for indicating refreshing process of the List Model
        // (owned)
        CCEWaitNote* iWaitNote;
        
        // Default sorting style for sorting the model (owned)
        MCLFSortingStyle* iSortingStyle;
        
        // Secondary sorting style for sorting the model (owned)
        MCLFSortingStyle* iSecondarySortingStyle;
        
        // Custom sorting style for randomizing the model (owned)
        CCECustomSorter* iCustomSorter;

        // List item for items that have no tag info (owned)
        HBufC* iNoTagInfoItem;
        
        // For waiting the ansychronous refresh operation to complete
        CActiveSchedulerWait iWait;
        
    };

#endif // __CCEMUSICVIEW_H__

// End of File
