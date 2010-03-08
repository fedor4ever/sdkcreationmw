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
#include "CCEGroupView.h"

#include "CCEAppUi.h"
#include "CCEOperationObserver.h"
#include "CCEGroupContainer.h"
#include "CCECustomGrouper.h"
#include "CCEPostFilter.h"
#include "CCEWaitNote.h"
#include "CLFExample.hrh"
#include "CLFExampleConsts.h"

#include <akntitle.h>
#include <stringloader.h>
#include <aknnotewrappers.h>
#include <ContentListingFactory.h>
#include <MCLFItem.h>
#include <MCLFItemListModel.h>
#include <CLFContentListing.hrh>
#include <CLFExample.rsg>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCEGroupView::CCEGroupView
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCEGroupView::CCEGroupView( MCLFContentListingEngine* aEngine,
                            MCLFItemListModel* aModel,
                            CCEOperationObserver* aObserver )
    : iEngine( aEngine ),
      iModel( aModel ),
      iObserver( aObserver ),
      iGrouping( ECENoGrouping )
    {
    }

// -----------------------------------------------------------------------------
// CCEGroupView::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCEGroupView::ConstructL()
    {
    BaseConstructL( R_CE_GROUP_VIEW );

    // Create a wait note
    iWaitNote = CCEWaitNote::NewL();
    }

// -----------------------------------------------------------------------------
// CCEGroupView::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCEGroupView* CCEGroupView::NewL( MCLFContentListingEngine* aEngine,
                                  MCLFItemListModel* aModel,
                                  CCEOperationObserver* aObserver )
    {
    CCEGroupView* self =
        CCEGroupView::NewLC( aEngine, aModel, aObserver );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CCEGroupView::NewLC
// Two-phased constructor, pointer is left to the CleanupStack.
// -----------------------------------------------------------------------------
//
CCEGroupView* CCEGroupView::NewLC( MCLFContentListingEngine* aEngine,
                                   MCLFItemListModel* aModel,
                                   CCEOperationObserver* aObserver )
    {
    CCEGroupView* self =
        new (ELeave) CCEGroupView( aEngine, aModel, aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// Destructor
CCEGroupView::~CCEGroupView()
    {
    delete iWaitNote;
    delete iArtistGrouper;
    delete iPostFilter;

    if( iContainer )
        {
        AppUi()->RemoveFromStack( iContainer );
        delete iContainer;
        }
    }

// -----------------------------------------------------------------------------
// CCEGroupView::HandleCommandL
// Handle user's commands.
// -----------------------------------------------------------------------------
//
void CCEGroupView::HandleCommandL( TInt aCommand )
    {
    // Diplay a wait note if user tries to do something while refresh operation
    // of the list model has not finished yet.
    if( iObserver->IsRefreshOngoing() )
        {
        // View the wait note
        iWaitNote->OpenWaitNoteL( R_CE_WAIT_NOTE );

        // Wait for the refresh operation to complete. Operation Observer will
        // stop this wait when the refresh operation has compeleted.
        iWait.Start();

        // Close the wait note
        iWaitNote->CloseWaitNoteL();
        }

    switch( aCommand )
        {
        case ECECmdGroupByAllMusicFiles:
            {
            // Reset grouping state
            iGrouping = ECENoGrouping;

            // Group items in the list model
            GroupListItemsL( ECEGroupedByAll );

            // Reset the list box
            iContainer->ResetListBoxL();

            if( iModel->ItemCount() <= 0 )
                {
                NoMusicFilesFoundL();
                break;
                }

            // Load "all files" list item from the resource and add it to
            // the list box if there are music files in the model
            HBufC* defaultAllFilesItem = StringLoader::LoadLC(
                R_CE_GROUP_ALL_FILES_ITEM );
            iContainer->AppendListBoxItemL( *defaultAllFilesItem );
            CleanupStack::PopAndDestroy( defaultAllFilesItem );

            // Focus the topmost item of the list box
            iContainer->SetCurrentItemIndex( 0 );
            break;
            }

        case ECECmdGroupByAlbum:
            {
            // Reset the grouping state
            iGrouping = ECENoGrouping;

            // Group items in the list model
            GroupListItemsL( ECEGroupedByAlbum );

            if( iModel->ItemCount() <= 0 )
                {
                NoMusicFilesFoundL();
                }

            // Re-generate the list box with album names
            PopulateListBoxL( ECLFFieldIdAlbum );

            // Focus the topmost item of the list box
            iContainer->SetCurrentItemIndex( 0 );
            break;
            }

        case ECECmdGroupByArtist:
            {
            // Reset the grouping state
            iGrouping = ECENoGrouping;

            // Group items in the list model
            GroupListItemsL( ECEGroupedByArtist );

            // Remove custom grouper from the model
            iModel->SetCustomGrouper( NULL );

            if( iModel->ItemCount() <= 0 )
                {
                NoMusicFilesFoundL();
                }

            // Re-generate the list box with artist names
            PopulateListBoxL( ECLFFieldIdArtist );

            // Select the topmost item from the list box
            iContainer->SetCurrentItemIndex( 0 );
            break;
            }

        case ECECmdSelect:
        case ECECmdOpen:
            {
            // Prepare to get all music files to the model
            iModel->SetGroupingStyle( ECLFNoGrouping );
            iModel->SetCustomGrouper( NULL );
            iModel->SetPostFilter( NULL );

            // Delete old post filter if any
            if( iPostFilter )
                {
                delete iPostFilter;
                iPostFilter = NULL;
                }

            // Check the grouping state of the application
            // The list is grouped by ..
            // .. all files
            if( iGrouping == ECEGroupedByAll )
                {
                // Do nothing, because the model has been prepared to get
                // all music files on the refresh.
                }

            // .. albums
            else if( iGrouping == ECEGroupedByAlbum )
                {
                // Which album is selected
                TInt selectedAlbum( iContainer->CurrentItemIndex() );

                // Get name of the album
                TPtrC nameOfAlbum;
                TInt error( iModel->Item( selectedAlbum ).GetField(
                    ECLFFieldIdAlbum, nameOfAlbum ) );
                User::LeaveIfError( error );

                // Create and activate a post filter for album filtering
                // so that the model will contain only sound files
                // from selected album
                iPostFilter =
                    CCEPostFilter::NewL( ECLFFieldIdAlbum, nameOfAlbum );
                iModel->SetPostFilter( iPostFilter );
                }

            // .. artists
            else if( iGrouping == ECEGroupedByArtist )
                {
                // Which artist is selected
                TInt selectedArtist( iContainer->CurrentItemIndex() );

                // Get name of the artist
                TPtrC nameOfArtist;
                TInt error( iModel->Item( selectedArtist ).GetField(
                    ECLFFieldIdArtist, nameOfArtist ) );
                User::LeaveIfError( error );

                // Create and activate a post filter for album filtering
                // so that the model will contain only sound files
                // from selected artist
                iPostFilter =
                    CCEPostFilter::NewL( ECLFFieldIdArtist, nameOfArtist );
                iModel->SetPostFilter( iPostFilter );
                }

            // .. files have not been grouped yet
            else
                {
                return;
                }

            // Refresh the model and activate the music files view
            iModel->RefreshL( ECLFRefreshAll );
            AppUi()->ActivateLocalViewL( KCEMusicViewId );
            break;
            }

        default:
            {
            AppUi()->HandleCommandL( aCommand );
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CCEGroupView::Id
// Get Id of the view.
// -----------------------------------------------------------------------------
//
TUid CCEGroupView::Id() const
    {
    return KCEGroupViewId;
    }

// -----------------------------------------------------------------------------
// CCEGroupView::DoActivateL
// Activate the view.
// -----------------------------------------------------------------------------
//
void CCEGroupView::DoActivateL(const TVwsViewId& /*aPrevViewId*/,
                                     TUid /*aCustomMessageId*/,
                               const TDesC8& /*aCustomMessage*/)
    {
    // Create a container and refresh the list model for Group view in the
    // first activate
    if( !iContainer )
        {
        iContainer = new (ELeave) CCEGroupContainer( this );
        iContainer->SetMopParent( this );
        iContainer->ConstructL();

        // Send reference of CActiveSchedulerWait to the CLF Operation Observer
        iObserver->PrepareForRefresh( iWait );

        // Call refresh to get all music files from the file system to the model
        iModel->RefreshL();

		// At this point, other initialization operations can be done as the
		// list model refresh is an asychronous operation.
        }

    AppUi()->AddToViewStackL( *this, iContainer );

    // Group files, if they were grouped when the view was deactivated
    if( iGrouping != ECENoGrouping )
        {
        GroupListItemsL( iGrouping );
        }
    }

// -----------------------------------------------------------------------------
// CCEGroupView::DoDeactivate
// Deactivate the view.
// -----------------------------------------------------------------------------
//
void CCEGroupView::DoDeactivate()
    {
    if( iContainer )
        {
        AppUi()->RemoveFromStack( iContainer );
        }
    }

// -----------------------------------------------------------------------------
// CCEGroupView::HandleClientRectChange
// Handle change of the client rect.
// -----------------------------------------------------------------------------
//
void CCEGroupView::HandleClientRectChange()
    {
    if( iContainer )
        {
        iContainer->SetRect( iAvkonAppUi->ClientRect() );
        }
    }

// -----------------------------------------------------------------------------
// CCEGroupView::DynInitMenuPaneL
// Dim items of the menu pane.
// -----------------------------------------------------------------------------
//
void CCEGroupView::DynInitMenuPaneL( TInt aResourceId,
                                     CEikMenuPane* aMenuPane )
	{
	// Dim open-option from the options-menu
	// if there are no items in the list box
	if( aResourceId == R_CE_GROUP_VIEW_MENUPANE )
		{
		if( iContainer->CurrentItemIndex() < 0 )
			{
			aMenuPane->SetItemDimmed( ECECmdOpen, ETrue );
			}
		}
	}

// -----------------------------------------------------------------------------
// CCEGroupView::GroupListItemsL
// Group items in the list model if they were grouped when the view
// was deactivated.
// -----------------------------------------------------------------------------
//
void CCEGroupView::GroupListItemsL( const TCEGrouping aGroupingState )
    {
    // Remove existing post filter and sorting style from the model
    iModel->SetPostFilter( NULL );
    iModel->SetSortingStyle( NULL );

    switch( aGroupingState )
        {
        case ECENoGrouping:
            {
            break;
            }

        case ECEGroupedByAll:
            {
            // Remove grouping
            iModel->SetGroupingStyle( ECLFNoGrouping );
            break;
            }

        case ECEGroupedByAlbum:
            {
            // Set album grouping
            iModel->SetGroupingStyle( ECLFMusicAlbumGrouping );
            break;
            }

        case ECEGroupedByArtist:
            {
            // Delete old and create new custom grouper for artist grouping
            if( iArtistGrouper )
                {
                delete iArtistGrouper;
                iArtistGrouper = NULL;
                }

            iArtistGrouper = CCECustomGrouper::NewL( ECLFFieldIdArtist );
            iModel->SetCustomGrouper( iArtistGrouper );
            break;
            }
        }

    // Refresh the model
    iModel->RefreshL( ECLFRefreshAll );

    // Set new grouping state if items were found
    if( iModel->ItemCount() > 0 )
        {
        iGrouping = aGroupingState;
        }
    else
        {
        iGrouping = ECENoGrouping;
        }
    }

// -----------------------------------------------------------------------------
// CCEGroupView::NoMusicFilesFoundL
// Show user a warning note that no music files were found.
// -----------------------------------------------------------------------------
//
void CCEGroupView::NoMusicFilesFoundL()
    {
    HBufC* noteText = StringLoader::LoadLC( R_CE_NO_MUSIC_FILES_FOUND );
    CAknWarningNote* note = new (ELeave) CAknWarningNote;
    note->ExecuteLD( *noteText );
    CleanupStack::PopAndDestroy( noteText );
    }

// -----------------------------------------------------------------------------
// CCEGroupView::PopulateListBoxL
// List all files from the model to the list box.
// -----------------------------------------------------------------------------
//
void CCEGroupView::PopulateListBoxL( const TCLFDefaultFieldId aFieldId )
    {
    // Reset the list box
    iContainer->ResetListBoxL();

    // Add all items to the list box
    TInt countItems( iModel->ItemCount() );
    for( TInt i = 0; i < countItems; ++i )
        {
        // Get value of requested field and add item to the list box
        TPtrC fieldValue;
        TInt error( iModel->Item( i ).GetField( aFieldId, fieldValue ) );

        if( error == KErrNone )
            {
            iContainer->AppendListBoxItemL( fieldValue );
            }
        }
    }

//  End of File
