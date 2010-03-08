/*
* Copyright (c) 2004 - 2006 Nokia Corporation and/or its subsidiary(-ies).
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
#include <eikenv.h>
#include <eiklabel.h> // For label control
#include <eikclb.h>   // For listbox
#include <eiktxlbm.h> // For listbox model
#include <eikclbd.h>  // For listbox data
#include <aknlists.h> // For list pane
#include <aknpopup.h>
#include <aknpopupheadingpane.h>
#include <akniconarray.h>
#include <AknMessageQueryDialog.h> // for global message query
#include <aknglobalconfirmationquery.h> // for global message query
#include <StringLoader.h>
#include <avkon.hrh>
#include <avkon.rsg>
#include <AKNEXQUERY.mbg>
#include <AknListQueryDialog.h>
#include <AknExQuery.rsg>
#include "AknExQuery.hrh"
#include "AknExQueryView.h"
#include "AknExQueryContainer.h"
#include "AknExQueryTimeAndDurationListEventHandler.h"

// ================= MEMBER FUNCTIONS =========================================

// ----------------------------------------------------------------------------
// CAknExQueryContainer::CAknExQueryContainer()
// Overload constructor.
//-----------------------------------------------------------------------------
//
CAknExQueryContainer::CAknExQueryContainer( CAknExQueryView* aView )
: iView( aView )
    {
    }

// ----------------------------------------------------------------------------
// CAknExQueryContainer::ConstructL()
// two phased constructor
//-----------------------------------------------------------------------------
//
void CAknExQueryContainer::ConstructL( const TRect& aRect )
    {
    // Creates window.
    CreateWindowL();

    // Creates label object.
    if ( !iLabel )
        {
        iLabel = new ( ELeave ) CEikLabel;
        iLabel->SetContainerWindowL( *this );
        if (AknLayoutUtils::Variant() == EApacVariant)
		    {
			iLabel->SetFont( ApacPlain16() );
		    }
		else
		    {
			iLabel->SetFont( LatinBold19() );
		    }

        SetTextToLabelL( R_AKNEXQUERY_TITLE_MESSAGE );
        }

    SetRect( aRect ); // Sets rectangle of frame.
    ActivateL(); // Activates the window. ( Ready to draw )
    }

// ----------------------------------------------------------------------------
// CAknExQueryContainer::~CAknExQueryContainer()
// Destructor.
// ----------------------------------------------------------------------------
//
CAknExQueryContainer::~CAknExQueryContainer()
    {
    delete iLabel;
    delete iLabelResultMessage;
    delete iListBox;
    delete iHandler;

    delete iGlobalListQuery;
    delete iGlobalListObserver;
    }

// ----------------------------------------------------------------------------
// CAknExQueryContainer::ShowConfirmationQueryL()
// Display of Confirmation Query.
// ----------------------------------------------------------------------------
//
void CAknExQueryContainer::ShowConfirmationQueryL(
                             const TInt aResourceId,
                             const TInt aTextResourceId,
                             const TBool aPromptFlag,
                             const TBool aToneFlag,
                             const TBool aHeaderTextAndImageFlag,
                             const CAknQueryDialog::TTone aTone
                             )
    {
    iDisplayDialog = ETrue;
    SetTextToLabelL( R_AKNEXQUERY_OPERATE_MESSAGE );
    CAknQueryDialog* dlg;

    if ( aHeaderTextAndImageFlag )
        {
        dlg = CAknQueryDialog::NewL( aTone );
        dlg->PrepareLC( aResourceId );
        TBuf<KAknExQueryTextBufLength> prompt;
        iCoeEnv->ReadResource( prompt, R_AKNEXQUERY_NUMQUERY_WITHHEAD_HEAD );
        dlg->QueryHeading()->SetTextL( prompt );
        SetIconL( dlg, EMbmAknexqueryAnchor_id, EMbmAknexqueryAnchor_id_mask );
        dlg->RunLD();
        }
    else
        {
        if ( !aTextResourceId )
            {
            dlg = CAknQueryDialog::NewL( aTone );
            }
        else
            {
            TBuf<KAknExQueryTextBufLength> prompt;
            iCoeEnv->ReadResource( prompt, aTextResourceId );

            if ( aPromptFlag )
                {
                dlg = CAknQueryDialog::NewL( aTone );
                CleanupStack::PushL( dlg );
                dlg->SetPromptL( prompt );
                CleanupStack::Pop();
                }
            else if ( aToneFlag )
                {
                dlg = new ( ELeave ) CAknQueryDialog( prompt, aTone );
                }
            else
                {
                dlg = new ( ELeave ) CAknQueryDialog( prompt );
                }
            }
        ShowResultMessageL( dlg->ExecuteLD( aResourceId ) );
        }
    iDisplayDialog = EFalse;
    }

// ----------------------------------------------------------------------------
// CAknExQueryContainer::ShowResultMessageL()
// Display return value of dialog.
// ----------------------------------------------------------------------------
//
void CAknExQueryContainer::ShowResultMessageL( TBool aResult )
    {
    TInt res( R_AKNEXQUERY_NO_OR_CANCEL_MESSAGE );

    if ( aResult )
        {
        res = R_AKNEXQUERY_YES_OR_OK_MESSAGE;
        }

    TBuf<KAknExQueryTextBufLength> messageConfirmationQuery;
    iCoeEnv->ReadResource( messageConfirmationQuery, res );

    ShowMessageL( messageConfirmationQuery );
    }

// ----------------------------------------------------------------------------
// CAknExQueryContainer::ShowDataQueryL()
// Display of Data Query.
// ----------------------------------------------------------------------------
//
void CAknExQueryContainer::ShowDataQueryL(
    const TInt aQueryResourceId,
    const TInt aTextResourceId,
    const TInt aPromptResourceId,
    const TInt aMaxLength,
    const TBool aDisplayFlag )
    {
    iDisplayDialog = ETrue;
    SetTextToLabelL( R_AKNEXQUERY_OPERATE_MESSAGE );

    TBuf<KAknExQueryTextBufLength> textData;
    if ( aTextResourceId )
        {
        iCoeEnv->ReadResource( textData, aTextResourceId );
        }

    CAknTextQueryDialog* dlg;
    if ( !aPromptResourceId )
        {
        dlg = new ( ELeave )CAknTextQueryDialog(
            textData, CAknQueryDialog::ENoTone );
        }
    else
        {
        TBuf<KAknExQueryTextBufLength> prompt;
        iCoeEnv->ReadResource( prompt, aTextResourceId );
        dlg = CAknTextQueryDialog::NewL(
            textData, CAknQueryDialog::ENoTone );
        CleanupStack::PushL( dlg );
        dlg->SetPromptL( prompt );
        CleanupStack::Pop(); // dlg
        }

    if ( aMaxLength )
        {
        dlg->SetMaxLength( aMaxLength );
        }

    TBool answer( dlg->ExecuteLD( aQueryResourceId ) );

    if ( aDisplayFlag && answer )
        {
        iLabelResultMessage->SetTextL( textData );
        }

    iDisplayDialog = EFalse;

    DrawNow();
    }

// ----------------------------------------------------------------------------
// CAknExQueryContainer::ShowListQueryL()
// Display of List Query.
// ----------------------------------------------------------------------------
//
void CAknExQueryContainer::ShowListQueryL( const TInt aResourceId )
    {
    iDisplayDialog = ETrue;
    SetTextToLabelL( R_AKNEXQUERY_OPERATE_MESSAGE );
    CArrayFixFlat<TInt>* indexArray = NULL;
    CAknListQueryDialog* dlg;
    TInt index( 0 );

    if ( aResourceId == R_AKNEXQUERY_MULTI_SELECTION_LIST_QUERY )
        {

        indexArray = new ( ELeave ) CArrayFixFlat<TInt>( 6 );
        CleanupStack::PushL( indexArray );
        dlg = new ( ELeave ) CAknListQueryDialog( indexArray );

        }
    else
        {
        dlg = new ( ELeave ) CAknListQueryDialog( &index );
        }

    TInt answer( dlg->ExecuteLD( aResourceId ) );

    TBuf<KAknExQueryTextBufLength> text;
    if ( aResourceId == R_AKNEXQUERY_LIST_QUERY )
        {
        if ( answer )
            {
            TBuf<KAknExQueryTextBufLength> baseText;
            iCoeEnv->ReadResource( baseText, R_AKNEXQUERY_ITEM_SELECTED );
            text.Format( baseText, index+1 );
            ShowMessageL( text );
            }
        else
            {
            iCoeEnv->ReadResource( text, R_AKNEXQUERY_QUERYDISCARDED );
            ShowMessageL( text );
            }
        }

    if ( indexArray )
        {
        CleanupStack::PopAndDestroy();
        }
    iDisplayDialog = EFalse;
    }


// ----------------------------------------------------------------------------
// CAknExQueryContainer::PopoutListBoxL()
// Create handler.
// ----------------------------------------------------------------------------
//
void CAknExQueryContainer::PopoutListBoxL()
    {
    SetTextToLabelL( R_AKNEXQUERY_OPERATE_MESSAGE );

    CreateListBoxL();
    
    iHandler->HandleListBoxEventL(
            iListBox, MEikListBoxObserver::EEventEnterKeyPressed );
    }

// ----------------------------------------------------------------------------
// CAknExQueryContainer::CreateListBoxL()
// Creates listbox object.
// ----------------------------------------------------------------------------
//
void CAknExQueryContainer::CreateListBoxL()
    {
    DeleteListBoxL();

    TInt resourceId = R_AKNEXQUERY_MULTILINE_TIME_AND_DURATION_LIST_ITEM;
    iHandler = new ( ELeave ) CAknExQueryTimeAndDurationListEventHandler(
                    this );

    TInt flags( CEikListBox::EPopout | CEikListBox::ELeftDownInViewRect );
    iListBox = new ( ELeave ) CEikColumnListBox;
    iListBox->ConstructL( this, flags );

    iListBox->SetListBoxObserver( iHandler );
    iListBox->SetBorder( TGulBorder::EShallowRaised );
    iListBox->CreateScrollBarFrameL( ETrue );
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );

    // Creates list items
    CDesCArray* textArray = iCoeEnv->ReadDesCArrayResourceL( resourceId );
    iListBox->Model()->SetItemTextArray( textArray );
    iListBox->Model()->SetOwnershipType( ELbmOwnsItemArray );

    // Sets pixel values of width.
    TRect rect( TPoint( KAknExQueryListBoxRectPointX,
        KAknExQueryListBoxRectPointY ),
        TSize( KAknExQueryListBoxRectWidth,
        KAknExQueryListBoxRectHeight )  );

    CColumnListBoxData* columnData = iListBox->ItemDrawer()->ColumnData();
    columnData->SetColumnWidthPixelL( KAknExQueryNameColumnIndex,
                                      rect.Width() );
    columnData->SetColumnWidthPixelL( KAknExQueryNameGapColumnIndex,
                                      rect.Width() );

    // Gets current number of list box items.
    TInt numberOfItems( iListBox->Model()->NumberOfItems() );
    // Gets new height of list box.
    TInt height( iListBox->CalcHeightBasedOnNumOfItems( numberOfItems ) );
    // If new height is less than defined height of list box
    // sets new height to list box height.
    if ( height < rect.Height() )
        {
        rect.SetHeight( height );
        }

    iListBox->SetRect( rect ); // Sets rectangle of list box.
    ActivateL();
    }

// ----------------------------------------------------------------------------
// CAknExQueryContainer::DeleteListBoxL()
// Deletes listbox object.
// ----------------------------------------------------------------------------
//
void CAknExQueryContainer::DeleteListBoxL()
    {
    if ( iListBox )
        {
        delete iListBox;
        iListBox = NULL;
        }
    if ( iHandler )
        {
        delete iHandler;
        iHandler = NULL;
        }
    }

// ----------------------------------------------------------------------------
// CAknExQueryContainer::CreateLabelL()
// Creates label object.
// ----------------------------------------------------------------------------
//
void CAknExQueryContainer::CreateLabelL( TInt aResourceId )
    {
    if ( !iLabelResultMessage )
        {
        iLabelResultMessage = new ( ELeave ) CEikLabel;
        iLabelResultMessage->SetContainerWindowL( *this );
        iLabelResultMessage->SetRect( Rect() );
        iLabelResultMessage->SetAlignment( EHCenterVTop );
        iLabelResultMessage->SetPosition( AKNEXQUERY_LABEL2_POSITION );
        if (AknLayoutUtils::Variant() == EApacVariant)
			{
			iLabelResultMessage->SetFont( ApacPlain16() );
			}
		else
			{
			iLabelResultMessage->SetFont( LatinBold19() );
		    }
        }

    TBuf<KAknExQueryTextBufLength> labelText;
    iCoeEnv->ReadResource( labelText, aResourceId );
    iLabelResultMessage->SetTextL( labelText );
    }

// ----------------------------------------------------------------------------
// CAknExQueryContainer::DeleteLabelL()
// Deletes label object.
// ----------------------------------------------------------------------------
//
void CAknExQueryContainer::DeleteLabelL()
    {
    delete iLabelResultMessage;
    iLabelResultMessage = NULL;
    }

// ----------------------------------------------------------------------------
// CAknExQueryContainer::SetTextToLabelL()
// Set Label text.
// ----------------------------------------------------------------------------
//
void CAknExQueryContainer::SetTextToLabelL( TInt aResourceId )
    {
    TBuf<KAknExQueryTextBufLength> messageString;
    iCoeEnv->ReadResource( messageString, aResourceId );
    iLabel->SetTextL( messageString );
    DrawNow();
    }


// ----------------------------------------------------------------------------
// CAknExQueryContainer::ShowMessageL( const TDesC& aMessage )
// Show Message.
// ----------------------------------------------------------------------------
//
void CAknExQueryContainer::ShowMessageL( const TDesC& aMessage )
    {
    iLabelResultMessage->SetTextL( aMessage );
    DrawNow();
    }

// ----------------------------------------------------------------------------
// CAknExQueryContainer::OfferKeyEventL()
// Handles the key events.
// ----------------------------------------------------------------------------
//
TKeyResponse CAknExQueryContainer::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    if ( aType != EEventKey )
        {
        return EKeyWasNotConsumed;
        }
    switch ( aKeyEvent.iCode )
        {
        case EKeySpace:
            if ( iDisplayDialog )
                {
                return EKeyWasNotConsumed;
                }
            iView->DisplayNextOutlineL();
            DrawNow();
            break;
        default:
            if ( !iDisplayDialog && iListBox )
                {
                return iListBox->OfferKeyEventL( aKeyEvent, aType );
                }
            else
                {
                return EKeyWasNotConsumed;
                }
        }
    return EKeyWasConsumed;
    }

// ----------------------------------------------------------------------------
// CAknExQueryContainer::SizeChanged()
// Called by framework when the view size is changed
// ----------------------------------------------------------------------------
//
void CAknExQueryContainer::SizeChanged()
    {
    if ( iLabel )
        {
        iLabel->SetExtent( AKNEXQUERY_LABEL_ORIGIN, Size() );
        iLabel->SetAlignment( EHCenterVCenter );
        }
    }

// ----------------------------------------------------------------------------
// CAknExQueryContainer::CountComponentControls() const
//  Returns number of component.
// ----------------------------------------------------------------------------
//
TInt CAknExQueryContainer::CountComponentControls() const
    {
    TInt count( KAknExQueryCountComponentControls );
    if ( iLabelResultMessage )
        {
        count++;
        }
    if ( iListBox )
        {
        count++;
        }
    return count;
    }

// ----------------------------------------------------------------------------
// CAknExQueryContainer::ComponentControl( TInt aIndex ) const
// Returns pointer to particular component.
// ----------------------------------------------------------------------------
//
CCoeControl* CAknExQueryContainer::ComponentControl( TInt aIndex ) const
    {
    switch ( aIndex )
        {
        case EAknExQueryComponentLabel:
            return iLabel;
        case EAknExQueryComponentLabel2:
            if ( iLabelResultMessage )
                {
                return iLabelResultMessage;
                }
            else
                {
                return iListBox;
                }
        case EAknExQueryComponentListBox:
            return iListBox;
        default:
            return NULL;
        }
    }

// ----------------------------------------------------------------------------
// CAknExQueryContainer::Draw( const TRect& aRect ) const
//
// ----------------------------------------------------------------------------
//
void CAknExQueryContainer::Draw( const TRect& aRect ) const
    {
    CWindowGc& gc = SystemGc();
    gc.SetPenStyle( CGraphicsContext::ENullPen );
    gc.SetBrushColor( KAknExQueryGray );
    gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
    gc.DrawRect( aRect );
    }

// ----------------------------------------------------------------------------
// CAknExQueryContainer::ShowMultiLineTimeAndDurationLayoutL
// Display of MultiLine Time with Duration Query
// ----------------------------------------------------------------------------
void CAknExQueryContainer::ShowMultiLineTimeAndDurationLayoutL(
     const TInt aResourceId,
    const TInt aTopResourceId)
    {
    iDisplayDialog = ETrue;
    SetTextToLabelL( R_AKNEXQUERY_OPERATE_MESSAGE );

    TBuf<KAknExQueryTextBufLength> bufferRes;

    iCoeEnv -> ReadResource(bufferRes, aTopResourceId);

    TTime topDateTime;
    topDateTime.Set(bufferRes);
	topDateTime.HomeTime();

    TTimeIntervalSeconds bottomDuration = 300;	// set timer field to 5 minutes

    CAknMultiLineDataQueryDialog* dlg =
        CAknMultiLineDataQueryDialog::NewL( topDateTime, bottomDuration );

    dlg->ExecuteLD( aResourceId );

    iDisplayDialog = EFalse;
    }

// ----------------------------------------------------------------------------
// CAknExQueryContainer::SetIconL()
// Set Icon dynamically
// ----------------------------------------------------------------------------
void CAknExQueryContainer::SetIconL( CAknQueryDialog* aDlg,
                                    TInt32 aBitmap,
                                    TInt32 aMask )
    {
    CEikImage* image = new ( ELeave ) CEikImage();
    CleanupStack::PushL( image );

    CFbsBitmap* bitmap = iEikonEnv->CreateBitmapL(
        KAknExQueryMbmFilePath, aBitmap );
    CleanupStack::PushL( bitmap );
    CFbsBitmap* mask = iEikonEnv->CreateBitmapL(
        KAknExQueryMbmFilePath, aMask );
    CleanupStack::PushL( mask );

    image->SetBitmap( bitmap );
    image->SetMask( mask );
    CleanupStack::Pop( 2 ); //mask and bitmap

    aDlg->QueryHeading()->SetHeaderImageL( image );
    CleanupStack::PopAndDestroy(); //image
    }

// ----------------------------------------------------------------------------
// CAknExQueryContainer::ShowGlobalListQueryL()
// ----------------------------------------------------------------------------
//
void CAknExQueryContainer::ShowGlobalListQueryL()
    {
    iDisplayDialog = ETrue;
    SetTextToLabelL( R_AKNEXQUERY_OPERATE_MESSAGE );

    CDesCArray* textArray = iCoeEnv->ReadDesCArrayResourceL(
       R_AKNEXQUERY_SUB_POPUP_DATA );

    CleanupStack::PushL( textArray );

    if ( iGlobalListObserver )
        {
        iGlobalListObserver->Cancel();
        delete iGlobalListObserver;
        iGlobalListObserver = NULL;
        }
    iGlobalListObserver = CAknExQueryGlobalListObserver::NewL( this );
    iGlobalListObserver->UnsetSelectItemFlag();

    iGlobalListObserver->Start();

    if ( iGlobalListQuery )
        {
        delete iGlobalListQuery;
        iGlobalListQuery = NULL;
        }
    iGlobalListQuery = CAknGlobalListQuery::NewL();

    iGlobalListObserver->SetSelectItemFlag();
    iGlobalListQuery->ShowListQueryL(
        textArray,
        iGlobalListObserver->iStatus,
        0 );

    CleanupStack::PopAndDestroy(); // textArray
    iDisplayDialog = EFalse;
    }

// End of File
