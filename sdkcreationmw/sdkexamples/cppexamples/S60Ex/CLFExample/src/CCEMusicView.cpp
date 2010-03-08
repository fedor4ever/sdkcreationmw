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
#include "CCEMusicView.h"

#include "CCEAppUi.h"
#include "CCEOperationObserver.h"
#include "CCEMusicContainer.h"
#include "CCECustomSorter.h"
#include "CCESoundPlayer.h"
#include "CCEWaitNote.h"
#include "CLFExample.hrh"
#include "CLFExampleConsts.h"

#include <akntitle.h>
#include <f32file.h>
#include <stringloader.h>
#include <aknnotewrappers.h>
#include <barsread.h>
#include <barsc.h>
#include <ContentListingFactory.h>
#include <MCLFContentListingEngine.h>
#include <MCLFItemListModel.h>
#include <MCLFModifiableItem.h>
#include <MCLFSortingStyle.h>
#include <CLFContentListing.hrh>
#include <CLFExample.rsg>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCEMusicView::CCEMusicView
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCEMusicView::CCEMusicView( MCLFContentListingEngine* aEngine,
                            MCLFItemListModel* aModel,
                            CCEOperationObserver* aObserver )
    : iEngine( aEngine ),
      iModel( aModel ),
      iObserver( aObserver )
    {
    }

// -----------------------------------------------------------------------------
// CCEMusicView::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCEMusicView::ConstructL()
    {
    BaseConstructL( R_CE_MUSIC_VIEW );

    // Load no-tag-info list item from the resource. It will be used for
    // items missing artist or title tag info (file name is displayed).
    iNoTagInfoItem = StringLoader::LoadL( R_CE_NO_TAG_INFO_LIST_ITEM );

    // Create sound player for playing audio files
    iSoundPlayer = CCESoundPlayer::NewL();

    // Create a wait note
    iWaitNote = CCEWaitNote::NewL();
    }

// -----------------------------------------------------------------------------
// CCEMusicView::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCEMusicView* CCEMusicView::NewL( MCLFContentListingEngine* aEngine,
                                  MCLFItemListModel* aModel,
                                  CCEOperationObserver* aObserver )
    {
    CCEMusicView* self = CCEMusicView::NewLC( aEngine, aModel, aObserver );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CCEMusicView::NewLC
// Two-phased constructor, pointer is left to the CleanupStack.
// -----------------------------------------------------------------------------
//
CCEMusicView* CCEMusicView::NewLC( MCLFContentListingEngine* aEngine,
                                   MCLFItemListModel* aModel,
                                   CCEOperationObserver* aObserver )
    {
    CCEMusicView* self =
        new (ELeave) CCEMusicView( aEngine, aModel, aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// Destructor
CCEMusicView::~CCEMusicView()
    {
    if( iObserver )
        {
        iObserver->RemoveMusicView();
        }
    if( iContainer )
        {
        AppUi()->RemoveFromStack( iContainer );
        delete iContainer;
        }

    delete iWaitNote;
    delete iSecondarySortingStyle;
    delete iSortingStyle;
    delete iCustomSorter;
    delete iSoundPlayer;
    delete iNoTagInfoItem;
    }

// -----------------------------------------------------------------------------
// CCEMusicView::HandleCommandL
// Handle user's command.
// -----------------------------------------------------------------------------
//
void CCEMusicView::HandleCommandL( TInt aCommand )
    {
    switch( aCommand )
        {
        case ECECmdSelect:
            {
            // Start playback if the player is not playing
            if( iSoundPlayer->PlayerState() != ECEPlaying )
                {
                HandleCommandL( ECECmdPlay );
                }

            // Stop current playback if the player is playing.
            // After stopping, play the file that is currently selected.
            else if( iSoundPlayer->PlayerState() == ECEPlaying )
                {
                HandleCommandL( ECECmdStop );
                }

            break;
            }

        case ECECmdPlay:
            {
            // Which sound file is selected
            TInt selectedFile( iContainer->CurrentItemIndex() );

            // Check that there was song files in the list
            if( selectedFile >= 0 )
                {
                // Get path and file name of the sound file
                TPtrC fileNameAndPath;
                TInt error( iModel->Item( selectedFile ).GetField(
                    ECLFFieldIdFileNameAndPath, fileNameAndPath ) );
                User::LeaveIfError( error );

                // Opens the file and starts playback
                iSoundPlayer->StartPlaybackL( fileNameAndPath );
                }
            break;
            }

        case ECECmdStop:
            {
            // Stop playback
            iSoundPlayer->StopPlayback();
            break;
            }

        case ECECmdSort:
            {
            // Set sorting options
            SetSortingL();
            break;
            }

        case ECECmdRename:
            {
            // Rename selected file
            TInt selectedFile( iContainer->CurrentItemIndex() );
            if( selectedFile >= 0 )
                {
                RenameMusicFileL( iContainer->CurrentItemIndex() );
                }
            break;
            }

        case EAknSoftkeyBack:
            {
            // Remove the changed item observer
            if( iObserver )
                {
                iObserver->RemoveMusicView();
                }

            AppUi()->ActivateLocalViewL( KCEGroupViewId );
            break;
            }

        case EAknCmdExit:
            {
            // Remove the changed item observer
            if( iObserver )
                {
                iObserver->RemoveMusicView();
                }

            // FALLTHROUGH
            }


        default:
            {
            AppUi()->HandleCommandL( aCommand );
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CCEMusicView::Id
// Get Id of the view.
// -----------------------------------------------------------------------------
//
TUid CCEMusicView::Id() const
    {
    return KCEMusicViewId;
    }

// -----------------------------------------------------------------------------
// CCEMusicView::DoActivateL
// Activate the view.
// -----------------------------------------------------------------------------
//
void CCEMusicView::DoActivateL(const TVwsViewId& /*aPrevViewId*/,
                                     TUid /*aCustomMessageId*/,
                               const TDesC8& /*aCustomMessage*/)
    {
    // Create the container for Music view
    if( !iContainer )
        {
        iContainer = new (ELeave) CCEMusicContainer( this );
        iContainer->SetMopParent( this );
        iContainer->ConstructL();
        }

    AppUi()->AddToViewStackL( *this, iContainer );

    // Generate and populate the list box
    PopulateListBoxL();

    iObserver->SetMusicView( this );
    }

// -----------------------------------------------------------------------------
// CCEMusicView::DoDeactivate
// Deactivate the view.
// -----------------------------------------------------------------------------
//
void CCEMusicView::DoDeactivate()
    {
    if( iContainer )
        {
        AppUi()->RemoveFromStack( iContainer );
        }

    delete iContainer;
    iContainer = NULL;
    }

// -----------------------------------------------------------------------------
// CCEMusicView::HandleClientRectChange
// Handle change of rect.
// -----------------------------------------------------------------------------
//
void CCEMusicView::HandleClientRectChange()
    {
    if( iContainer )
        {
        iContainer->SetRect( iAvkonAppUi->ClientRect() );
        }
    }

// -----------------------------------------------------------------------------
// CCEMusicView::DynInitMenuPaneL
// Dim items of the menu pane.
// -----------------------------------------------------------------------------
//
void CCEMusicView::DynInitMenuPaneL( TInt aResourceId,
                                     CEikMenuPane* aMenuPane )
	{
	// Dim items from the options-menu
	if( aResourceId == R_CE_MUSIC_VIEW_MENUPANE )
		{
        // Dim "stop" if the player is not playing
		if( iSoundPlayer->PlayerState() != ECEPlaying )
			{
			aMenuPane->SetItemDimmed( ECECmdStop, ETrue );
			}

		// Dim "play" if the player is playing, or if there are
		// no music files in the list box
		else if( iSoundPlayer->PlayerState() == ECEPlaying )
			{
			aMenuPane->SetItemDimmed( ECECmdPlay, ETrue );
			}

	    // Dim "play", "sort" and "rename" if there are
	    // no music files in the list box
	    if ( iContainer->CurrentItemIndex() < 0 )
	        {
			aMenuPane->SetItemDimmed( ECECmdPlay, ETrue );
			aMenuPane->SetItemDimmed( ECECmdSort, ETrue );
			aMenuPane->SetItemDimmed( ECECmdRename, ETrue );
	        }
		}
	}

// -----------------------------------------------------------------------------
// CCEMusicView::ItemsChangedL
// Changed item observer will call this function when some item has changed.
// -----------------------------------------------------------------------------
//
void CCEMusicView::ItemsChangedL()
    {
    // Refresh the model and wait for refresh to complete while displaying
    // a wait note. After refreshing, re-generate the list box.
    iObserver->PrepareForRefresh( iWait );

    iWaitNote->OpenWaitNoteL( R_CE_WAIT_NOTE );

    iModel->RefreshL();

    iWait.Start();
    PopulateListBoxL();

    iWaitNote->CloseWaitNoteL();
    }

// -----------------------------------------------------------------------------
// CCEMusicView::PopulateListBoxL
// List all files from the model to the list box.
// -----------------------------------------------------------------------------
//
void CCEMusicView::PopulateListBoxL()
    {
    // Reset the list box
    iContainer->ResetListBoxL();

    // Add items from model to the list box
    TInt numberOfItems( iModel->ItemCount() );
    for( TInt i = 0; i < numberOfItems; ++i )
        {
        // Get the item
        const MCLFItem& myItem = iModel->Item( i );

        // Get title, artist and album tag info of the item
        TPtrC songTitle;
        TInt errorTitle( myItem.GetField( ECLFFieldIdSongName, songTitle ) );
        TPtrC songArtist;
        TInt errorArtist( myItem.GetField( ECLFFieldIdArtist, songArtist ) );
        TPtrC songAlbum;
        TInt errorAlbum( myItem.GetField( ECLFFieldIdAlbum, songAlbum ) );

        // Create text array for creating the list box item
        CDesCArray* textItems =
            new (ELeave) CDesCArraySeg( KCEArrayGranularity );
        CleanupStack::PushL( textItems );

        // If title and artist tags were found
        if( errorTitle == KErrNone && errorArtist == KErrNone )
            {
            textItems->AppendL( songTitle );
            textItems->AppendL( songArtist );

            // If an album tag was found too
            if( errorAlbum == KErrNone )
                {
                textItems->AppendL( songAlbum );
                }
            }

        // If title or artist tag is not found, the file name is used
        else
            {
            TPtrC songFileName;
            TInt errorFileName( iModel->Item( i ).GetField(
                ECLFFieldIdFileName, songFileName ) );
            User::LeaveIfError( errorFileName );

            textItems->AppendL( songFileName );
            textItems->AppendL( *iNoTagInfoItem );
            }

        // Add item to the list box
        iContainer->AppendListBoxItemL( textItems );

        CleanupStack::PopAndDestroy( textItems );
        }

    // Select the topmost item of the list box to current
    iContainer->SetCurrentItemIndex( 0 );
    }

// -----------------------------------------------------------------------------
// CCEMusicView::RenameMusicFileL
// Renames music file.
// -----------------------------------------------------------------------------
//
void CCEMusicView::RenameMusicFileL( const TInt aFileIndex )
    {
    // Get file name of the sound file
    TPtrC fileName;
    TInt errorName( iModel->Item( aFileIndex ).GetField(
        ECLFFieldIdFileNameAndPath, fileName ) );
    User::LeaveIfError( errorName );

    // Query new file name
    TFileName newFileName = fileName;
    CAknTextQueryDialog* dlg = CAknTextQueryDialog::NewL( newFileName );

    if( dlg->ExecuteLD( R_CE_RENAME_QUERY ) )
        {
        // Open connection to the file system and rename the file
        RFs fs;
        User::LeaveIfError( fs.Connect() );
        CleanupClosePushL( fs );
        User::LeaveIfError( fs.Rename( fileName, newFileName ) );
        CleanupStack::PopAndDestroy(); // fs

        // Request the content listing engine to refresh its file listing.
        // Calling method UpdateItemsL after renaming a file, will cause
        // changed item event to be generated.
        iEngine->UpdateItemsL();
        }
    }

// -----------------------------------------------------------------------------
// CCEMusicView::SetSortingL
// Set sorting options.
// -----------------------------------------------------------------------------
//
void CCEMusicView::SetSortingL()
    {
    // Remove old sorting styles and custom sorter if any
    iModel->SetSortingStyle( NULL );
    iModel->SetCustomSorter( NULL );

    delete iSortingStyle;
    iSortingStyle = NULL;

    delete iSecondarySortingStyle;
    iSecondarySortingStyle = NULL;

    delete iCustomSorter;
    iCustomSorter = NULL;

    // Create new sorting style for default sorting
    iSortingStyle = ContentListingFactory::NewSortingStyleLC();
    CleanupStack::Pop(); // iSortingStyle

    // Query for default sorting parameter
    TInt sortBy( 0 );
    CAknListQueryDialog* dlg;
    dlg = new (ELeave) CAknListQueryDialog( &sortBy );
    if( dlg->ExecuteLD( R_CE_DEFAULT_SORTING_PARAM_QUERY ) )
        {
        // Set default sorting parameter
        switch( sortBy )
            {
            case ECESortByAlbum:
                {
                // Sort items by album name
                iSortingStyle->AddFieldL( ECLFFieldIdAlbum );

                // You can add multiple parameters for sorting style:
                // In this case, if the item does not have ECLFFieldIdAlbum
                // then ECLFFieldIdArtist is used to sort the item
                iSortingStyle->AddFieldL( ECLFFieldIdArtist );

                // Fields are string type
                iSortingStyle->SetSortingDataType( ECLFItemDataTypeDesC );
                break;
                }

            case ECESortByArtist:
                {
                // Sort items by artist name
                iSortingStyle->AddFieldL( ECLFFieldIdArtist );

                // Field is string type
                iSortingStyle->SetSortingDataType( ECLFItemDataTypeDesC );
                break;
                }

            case ECESortByTitle:
                {
                // Sort items by song title
                iSortingStyle->AddFieldL( ECLFFieldIdSongName );

                // Field is string type
                iSortingStyle->SetSortingDataType( ECLFItemDataTypeDesC );
                break;
                }

            case ECESortByGenre:
                {
                // Sort items by genre
                iSortingStyle->AddFieldL( ECLFFieldIdGenre );

                // Field is string type
                iSortingStyle->SetSortingDataType( ECLFItemDataTypeDesC );
                break;
                }

            case ECESortByFileDate:
                {
                // Sort items by file date
                iSortingStyle->AddFieldL( ECLFFieldIdFileDate );

                // Field is TTime type
                iSortingStyle->SetSortingDataType( ECLFItemDataTypeTTime );
                break;
                }

            case ECESortByRandom:
                {
                // Create a custom sorter and set it to the model.
                // This custom sorter will randomize the list model.
                iCustomSorter = CCECustomSorter::NewL();
                iModel->SetCustomSorter( iCustomSorter );

                // Refresh the model, wait for the refresh to complete
                // and re-generate the list box
                iModel->RefreshL( ECLFRefreshSorting | ECLFRefreshPostFilter );
                PopulateListBoxL();
                return;
                }

            default:
                {
                return;
                }
            }
        }
    else
        {
        return;
        }

    // Query for sorting order
    TInt sortingOrder( 0 );
    dlg = new (ELeave) CAknListQueryDialog( &sortingOrder );
    if( dlg->ExecuteLD( R_CE_ORDERING_SORTING_PARAM_QUERY ) )
        {
        // Set sorting order
        switch( sortingOrder )
            {
            case ECESortingOrderAscending:
                {
                iSortingStyle->SetOrdering( ECLFOrderingAscending );
                break;
                }

            case ECESortingOrderDescending:
                {
                iSortingStyle->SetOrdering( ECLFOrderingDescending );
                break;
                }

            default:
                {
                return;
                }
            }
        }
    else
        {
        return;
        }

    // Query how to position items that have no tag info
    // for default sorting parameter.
    TInt sortPosition( 0 );
    dlg = new (ELeave) CAknListQueryDialog( &sortPosition );
    if( dlg->ExecuteLD( R_CE_POSITION_SORTING_PARAM_QUERY ) )
        {
        switch( sortPosition )
            {
            // Items are sent to the end of the list
            case ECESortPositionBottom:
                {
                iSortingStyle->SetUndefinedItemPosition(
                    ECLFSortingStyleUndefinedEnd );
                break;
                }

            // Items are sent to the top of the list
            case ECESortPositionTop:
                {
                iSortingStyle->SetUndefinedItemPosition(
                    ECLFSortingStyleUndefinedFirst );
                break;
                }

            default:
                {
                return;
                }
            }
        }
    else
        {
        return;
        }

    // Set the primary sorting style to the model
    iModel->SetSortingStyle( iSortingStyle );

    // Sorting styles can be created from the resource too:
    // Create a resource reader for creating the secondary sorting style
    // from the resource
    TResourceReader resourceReader;
    iCoeEnv->CreateResourceReaderLC(
        resourceReader, R_CE_SECONDARY_SORTING_STYLE );
    iSecondarySortingStyle =
        ContentListingFactory::NewSortingStyleLC( resourceReader );
    CleanupStack::Pop(); // iSecondarySortingStyle

    // Set the secondary sorting style to the model:
    // After sorting music files by default sorting parameter/parameters,
    // sort remaining files; files that did not have any of those
    // parameters. In this case, files that do not have any of the primary
    // sorting parameters, are sorted by file name.
    // You can add multiple secondary sorting styles too.
    iModel->AppendSecondarySortingStyleL( *iSecondarySortingStyle );

    // Refresh the model and re-generate the list box
    iModel->RefreshL( ECLFRefreshSorting | ECLFRefreshPostFilter );
    PopulateListBoxL();

    CleanupStack::PopAndDestroy(); // resourceReader
    }

//  End of File
