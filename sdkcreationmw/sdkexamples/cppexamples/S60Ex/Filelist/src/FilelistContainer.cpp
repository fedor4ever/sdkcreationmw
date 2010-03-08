/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
#include <AknLists.h> // for AknListBoxLayouts
#include <Avkon.hrh>
#include <AknIconArray.h> // for CArrayPtr
#include <AknIconUtils.h>
#include <AknNoteWrappers.h> // for CAknErrorNote
#include <AknUtils.h> 
#include <barsread.h> // for TResourceReader
#include <gulicon.h> // for CGulIcon
#include <f32file.h>
#include <aknnavide.h> //NaviPane Decorator
#include <GDI.H>

#include <Filelist.rsg>
#include <Filelist.mbg>
#include "FilelistContainer.h"
#include "FilelistAppUi.h"
#include "Filelist.hrh"


// ================= MEMBER FUNCTIONS ==========================================

// -----------------------------------------------------------------------------
// CFilelistContainer::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CFilelistContainer* CFilelistContainer::NewL( const TRect& aRect )
    {
    CFilelistContainer* self = CFilelistContainer::NewLC( aRect );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CFilelistContainer::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CFilelistContainer* CFilelistContainer::NewLC( const TRect& aRect )
    {
    CFilelistContainer* self = new ( ELeave ) CFilelistContainer();
    CleanupStack::PushL( self );
    self->ConstructL( aRect );
    return self;
    }
    
// -----------------------------------------------------------------------------
// CFilelistContainer::~CFilelistContainer()
// Destructor
// -----------------------------------------------------------------------------
//
CFilelistContainer::~CFilelistContainer()
    {
    iSession.Close();
    delete iGrid;    
    delete iFilelist;
    delete iNaviDecorator;
    }

// -----------------------------------------------------------------------------
// CFilelistContainer::SetGridInitialParameters()
// Sets initial parameters for grid
// -----------------------------------------------------------------------------
//
void CFilelistContainer::SetGridInitialParametersL()
    {
    TSize mpSize;     // main pane size
    AknLayoutUtils::LayoutMetricsSize( AknLayoutUtils::EMainPane, mpSize );

    iNumOfItems = iFilelist->Count(); 
    
    iNumOfColumns = KGridColumns;
    iNumOfRows = KGridRows;
    
    // Determinate cell size
    iSizeOfCell.iWidth = mpSize.iWidth / iNumOfColumns;
    iSizeOfCell.iHeight = mpSize.iHeight / iNumOfRows;
    
    // Determinate scrolling type
    iVerticalScrollingType = CAknGridView::EScrollIncrementLineAndLoops;    
    iHorizontalScrollingType = CAknGridView::EScrollIncrementLineAndLoops;

      
    iVerticalOrientation = EFalse;
    iLeftToRight = ETrue;
    iTopToBottom = ETrue;
    
    // Set grid layout
    iGrid->SetLayoutL( iVerticalOrientation, 
        iLeftToRight, iTopToBottom, 
        iNumOfColumns, iNumOfRows,  
        iSizeOfCell );

    // Set scrolling type
    iGrid->SetPrimaryScrollingType( iVerticalScrollingType );
    iGrid->SetSecondaryScrollingType( iHorizontalScrollingType );
    
    // Set current index in grid
     iGrid->SetCurrentDataIndex( 0 );
    }

// -----------------------------------------------------------------------------
// CFilelistContainer::CreateGridResourceL()
// Prepares resources for the grid
// -----------------------------------------------------------------------------
//
void CFilelistContainer::CreateGridResourceL()
    {
    // Load graphics
    LoadGraphicsL();

    // Create data
    AddDataL();
    }

// -----------------------------------------------------------------------------
// CFilelistContainer::LoadGraphicsL()
// Loads icon resource
// -----------------------------------------------------------------------------
//
void CFilelistContainer::LoadGraphicsL()
    {    
    // Create icon array with granularity of 1 icon
    CArrayPtr< CGulIcon >* icons = new(ELeave) CAknIconArray( 1 );
    CleanupStack::PushL( icons );

	// Creates bitmap and mask for an icon.
    AknIconUtils::CreateIconL( iBitmap, 
            iMask, 
            KIconName, 
            EMbmFilelistQgn_menu_filelist, 
            EMbmFilelistQgn_menu_filelist_mask );

    // Determinate icon size
    TSize iconSize;
    iconSize.iWidth = iSizeOfCell.iWidth / 2;
    iconSize.iHeight = iSizeOfCell.iHeight / 2;

	//Initializes the icon to the given size.
	//Note that this call sets the sizes of both bitmap and mask 
    AknIconUtils::SetSize( iBitmap, iconSize );
    
    // Append the icon to icon array
    icons->AppendL( CGulIcon::NewL( iBitmap, iMask ) );
    
    // Attach icon array to grid's item drawer
    iGrid->ItemDrawer()->FormattedCellData()->SetIconArrayL( icons );

    CleanupStack::Pop( icons );    
    }

// -----------------------------------------------------------------------------
// CFilelistContainer::AddDataL()
// Adds filenames into the grid.
// -----------------------------------------------------------------------------
//
void CFilelistContainer::AddDataL()
    {
    
    MDesCArray* array = iGrid->Model()->ItemTextArray();
    CDesCArray* cArray = ( CDesCArray* )array;
	
	// The number of items in the grid.
    TInt numberOfData = iFilelist->Count();
    
    // Delete data from array
    if ( iGrid->GridModel()->NumberOfData() > 0 )
    {
        cArray->Delete( 0, iGrid->GridModel()->NumberOfData() );
    }
    iGrid->HandleItemRemovalL();

    // Create text which is added to the grid.
    TBuf< KLengthOfItemResourceString > cellText;
    TBuf< KLengthOfItemResourceString > cellTextFormat;

    iCoeEnv->ReadResourceL( cellTextFormat, R_FILELIST_FORMATSTRING );

    // Add the item to the last index
    for ( TInt loop = 0; loop < numberOfData; loop++ )
        {
        cellText.Format( cellTextFormat, &(*iFilelist)[loop].iName );
        cArray->AppendL( cellText );
        }

    // Inform list box that data was added.
    iGrid->HandleItemAdditionL();
    }

// -----------------------------------------------------------------------------
// CFilelistContainer::ApplySelGridGraphicStyle()
// Applies the layout to cells in the grid
// -----------------------------------------------------------------------------
//
void CFilelistContainer::ApplySelGridGraphicStyle()
    {
    AknListBoxLayouts::SetupStandardGrid( *iGrid );

    // Layout of the graphic
    AknListBoxLayouts::SetupFormGfxCell(
       *iGrid,              // Reference to grid control
       iGrid->ItemDrawer(),// Pointer to the item drawer
       0,                   // Column index
       0,                   // Left position
       0,                   // Top position
       0,                   // Right - unused
       0,                   // Bottom - unused
       iSizeOfCell.iWidth, // Width
       iSizeOfCell.iHeight/2,// Height
       TPoint(0,0),         // Start position
       TPoint(iSizeOfCell.iWidth, iSizeOfCell.iHeight/2));     // End position
     
    const CFont* fontText = AknLayoutUtils::FontFromId(
                    EAknLogicalFontPrimaryFont, NULL );
                    
    // Deretminate the baseline for the text
    TInt baseline = iSizeOfCell.iHeight - fontText->DescentInPixels() - 1;

    // Layout of text
    AknListBoxLayouts::SetupFormTextCell(
       *iGrid,                          // Reference to grid
       iGrid->ItemDrawer(),            // Pointer to the item drawer
       1,                           // Column index
       fontText,                       // Font
       215,                         // Color (215 = black)
       0,                           // Left margin
       0,                           // Right margin - unused
       baseline,                    // Baseline
       iSizeOfCell.iWidth,            // Text width
       CGraphicsContext::ECenter,    // Text alignment
       TPoint(0, iSizeOfCell.iHeight/2), // Start position
       TPoint(iSizeOfCell.iWidth, 
               iSizeOfCell.iHeight));    // End position
               
    //Update icon size
	SetIconSizes( iGrid->ItemDrawer()->FormattedCellData()->IconArray() );
    }


// -----------------------------------------------------------------------------
// CFilelistContainer::SetIconSizes()
// Sets size of the icon in iconarray.
// -----------------------------------------------------------------------------
//
void CFilelistContainer::SetIconSizes( CArrayPtr<CGulIcon>* aIconArray )
    {
    TSize iconSize;
    iconSize.iWidth = iSizeOfCell.iWidth / 2;
    iconSize.iHeight = iSizeOfCell.iHeight / 2;

    // note  AknIconUtils::SetSize sets both mask and bitmap size.
    // regardless which is given.
    AknIconUtils::SetSize( (*aIconArray)[0]->Bitmap(), iconSize );
    }
    
// -----------------------------------------------------------------------------
// CFilelistContainer::CreateFilelist()
// Gets filename from private folder.
// -----------------------------------------------------------------------------
//
void CFilelistContainer::CreateFilelist()
    {
    // List must be empty
    if ( iFilelist )
        {
        delete iFilelist;
        iFilelist=NULL;
        }        

    TBuf< KPathNameLength > dPath;
    TBuf< KPathNameLength > dPathF;

    // Get application's private path
    iSession.PrivatePath( dPath );

    // Set actual folder path
    if ( FolderID() == EPrivateFolder2 )
        {
    	dPathF.Format( KPrivateFolderPath2, &dPath );
        }
    else
        {
    	dPathF.Format( KPrivateFolderPath1, &dPath );
        }  

    // Get the file list, sorted by name
    User::LeaveIfError( iSession.GetDir( 
            dPathF,
            KEntryAttMaskSupported,
            ESortByName,
            iFilelist ) );
    }

// -----------------------------------------------------------------------------
// CFilelistContainer::ChangePrivateFolderL()
// Changes private folder to another one
// -----------------------------------------------------------------------------
//
void CFilelistContainer::ChangePrivateFolderL( TInt aDestFolder )
    {
    // Change folder ID
    SetFolderID( aDestFolder );
    
    // Read filenames from folder
    CreateFilelist();
    
    // Add text to grid
    AddDataL();
    
    // Set text to navi pane
    SetNaviPaneTextL();
    
    iGrid->DrawNow();
    }
    
// -----------------------------------------------------------------------------
// CFilelistContainer::ChangeProtectedFolderL()
// Protected folder
// -----------------------------------------------------------------------------
//
void CFilelistContainer::ChangeProtectedFolderL()
    {    
    CDir* protFolder;
    
    //Expected value is KErrPermissionDenied=(-46)
    TInt err = iSession.GetDir( 
                KProtectedFolderPath,
                KEntryAttMaskSupported,
                ESortByName,
                protFolder ) ;
                
    if ( err != KErrNone )
    	{    
        ShowErrorNoteL( err );
    	}
    delete protFolder;    
    }

// -----------------------------------------------------------------------------
// CFilelistContainer::ShowErrorNoteL()
// Shows error note
// -----------------------------------------------------------------------------
//
void CFilelistContainer::ShowErrorNoteL( TInt aErr )
    {
    TBuf< KErrorNoteLength > errorBuf;
    // Create CAknErrorNote instance
    CAknErrorNote* dialog = new ( ELeave ) CAknErrorNote();
    
    //Allocate TBuf with constant length.
    TBuf< KErrorNoteLength > errNote;

     // Reads a resource into a descriptor.
    iCoeEnv->ReadResourceL( errNote, R_FILELIST_ERRORNOTE_TEXT );
    errorBuf.Format( errNote, aErr );

    // Show the Dialog 
    dialog->ExecuteLD( errorBuf );    
    }  

// -----------------------------------------------------------------------------
// CFilelistContainer::HandleListBoxEventL
// Called by the framework whenever a grid event occurs for which this container
// is an observer.
// @param aListBoxEvent The type of event which occured
// -----------------------------------------------------------------------------    
void CFilelistContainer::HandleListBoxEventL( CEikListBox* /*aListBox*/,
		TListBoxEvent /*aListBoxEvent*/ )
    {
    // no implementation required 
    }

// ---------------------------------------------------------
// CFilelistContainer::HandleResourceChange()
// Called by framework when layout is changed.
// ---------------------------------------------------------
//
/*void CFilelistContainer::HandleResourceChange( TInt aType )
    {
    CCoeControl::HandleResourceChange( aType );

    // ADDED FOR SCALABLE UI SUPPORT
    // *****************************
    if ( aType==KEikDynamicLayoutVariantSwitch )
        {
        TRect rect;
        AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, rect );
        SetRect( rect );
        }
    }
*/	
// -----------------------------------------------------------------------------
// CFilelistContainer::ConstructL(const TRect& aRect)
// Symbian two phased constructor
// -----------------------------------------------------------------------------
//
void CFilelistContainer::ConstructL( const TRect& aRect )
    {
    CreateWindowL();

	iSession.Connect();
	
    SetFolderID( EPrivateFolder1 );

    // Create CAknGrid instance:  
    iGrid = new(ELeave) CAknGrid;

    //Set the container window for grid control
    iGrid->SetContainerWindowL( *this );

    //Create a model and attach it to grid control: 
    iGridM = new(ELeave) CAknGridM;
    iGrid->SetModel( iGridM );    
    
    //Call ConstructL for the grid object: 
    TInt gridFlags = EAknListBoxSelectionGrid;
    iGrid->ConstructL( this, gridFlags);
   
    // Gets filename from folder
    CreateFilelist();
    
    // Set initial parameters for the grid
    SetGridInitialParametersL();    
    	
    //Sets text to empty grid
    SetEmptyTextL();
     
    // Create resouce for grid
    CreateGridResourceL();
    
    // Set observer
    iGrid->SetListBoxObserver( this );
    
    //Set navi pane text from resource
    SetNaviPaneTextL();

    // Set the windows size
    SetRect( aRect );    
    
    // Activate the window, which makes it ready to be drawn
    ActivateL();   
    }   
     
// -----------------------------------------------------------------------------
// CFilelistContainer::SizeChanged()
// Called by framework when the view size is changed.
// -----------------------------------------------------------------------------
//
void CFilelistContainer::SizeChanged()
    {         
    if ( iGrid )
        {
        // Set initial parameters for the grid
        TRAP_IGNORE( SetGridInitialParametersL() );
    
	    //Apply the layout to cells in the grid
	 	ApplySelGridGraphicStyle();   

        iGrid->SetRect( Rect() );
        }
    }

// -----------------------------------------------------------------------------
// CFilelistContainer::Draw()
// Draws the display.
// -----------------------------------------------------------------------------
//
void CFilelistContainer::Draw( const TRect& /*aRect*/ ) const
    {
    // no implementation required 
    }

// -----------------------------------------------------------------------------
// CFilelistContainer::CountComponentControls()
// Called by the framework in compound controls    
// @return The number of controls in this CSimpleGridContainer
// -----------------------------------------------------------------------------
//
TInt CFilelistContainer::CountComponentControls() const
    {
    return 1; // return number of controls inside this container
    }

// -----------------------------------------------------------------------------
// CFilelistContainer::ComponentControl
// Called by the framework in compound controls    
// @param The index of the control to return
// @return The control for aIndex
// -----------------------------------------------------------------------------
//
CCoeControl* CFilelistContainer::ComponentControl(TInt aIndex) const
    {
    switch ( aIndex )
        {
        case 0:
            return iGrid;
        default:
            return NULL;
        }
    }

// -----------------------------------------------------------------------------
// CFilelistContainer::OfferKeyEventL
// Called by the framework whenever a key event occurs.    
// @param aType the type of Key event which occurred, e.g. key up, key down
// @return TKeyResponse EKeyWasNotConsumed if the key was not processed,
// EKeyWasConsumed if it was 
// -----------------------------------------------------------------------------
TKeyResponse CFilelistContainer::OfferKeyEventL( const TKeyEvent& aKeyEvent,
        TEventCode aType )
    {
    if ( iGrid )
        {
        return iGrid->OfferKeyEventL ( aKeyEvent, aType );
        }
    else
        {
        return EKeyWasNotConsumed;    
        }
    }

// -----------------------------------------------------------------------------
// CFilelistContainer::SetFolderID()
// Sets folderID to point to desired folder
// -----------------------------------------------------------------------------
//
void CFilelistContainer::SetFolderID( TInt aFolderID )
    {
    iFolderID = aFolderID;
    }
    
// -----------------------------------------------------------------------------
// FilelistContainer::FolderID()
// Gets folder ID
// -----------------------------------------------------------------------------
//
TInt CFilelistContainer::FolderID() const
    {
    return iFolderID;
    }

// -----------------------------------------------------------------------------
// CFilelistContainer::SetEmptyText()
// Sets text to empty grid
// -----------------------------------------------------------------------------
//
void CFilelistContainer::SetEmptyTextL()
    {
    TBuf< KEmptyGridText > emptyTxt;
    iCoeEnv->ReadResourceL( emptyTxt, R_FILELIST_EMPTYTEXT );
    iGrid->SetEmptyGridTextL( emptyTxt );
    }

// -----------------------------------------------------------------------------
// CFilelistContainer::SetNaviPaneTextL()
// Changes text to navi pane
// -----------------------------------------------------------------------------    
void CFilelistContainer::SetNaviPaneTextL()
    {
    TResourceReader spText; 
	
	// Create navipane pointer
	if ( !iNaviPane )
		{
		CEikStatusPane *sp = 
		            ( ( CAknAppUi* )iEikonEnv->EikAppUi() )->StatusPane();
		// Fetch pointer to the default navi pane control
		iNaviPane = ( CAknNavigationControlContainer * )
		            sp->ControlL( TUid::Uid( EEikStatusPaneUidNavi ) );
		}
    
    if ( iNaviDecorator )
        {
        delete iNaviDecorator;
        iNaviDecorator = NULL;        
        }
        
    // Read text from resource
    if ( FolderID() == EPrivateFolder1 )
        {
        iCoeEnv->CreateResourceReaderLC( spText, R_FILELIST_NAVI_PANE_TEXT1 );
        }
    else
        {
        iCoeEnv->CreateResourceReaderLC( spText, R_FILELIST_NAVI_PANE_TEXT2 );
        }
        
    // Set text to navi pane
    iNaviDecorator = iNaviPane->CreateNavigationLabelL( spText );
    CleanupStack::PopAndDestroy(); // Pushed by CreateResourceReaderLC
    iNaviPane->PushL( *iNaviDecorator );
    }

// End of File  

