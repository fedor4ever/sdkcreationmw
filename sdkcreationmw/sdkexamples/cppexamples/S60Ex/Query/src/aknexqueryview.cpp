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
#include <aknviewappui.h>
#include <akntitle.h>
#include <avkon.hrh>
#include <AknExQuery.rsg>
#include "AknExQueryView.h"
#include "AknExQueryContainer.h"
#include "AknExQuery.hrh"

// ================= MEMBER FUNCTIONS =========================================

// ------------------------------------------------------------------------------
// CAknExQueryView::ConstructL()
// Symbian 2nd phase constructor can leave.
// ------------------------------------------------------------------------------
//
void CAknExQueryView::ConstructL()
    {
    BaseConstructL( R_AKNEXQUERY_VIEW );
    }

// ----------------------------------------------------------------------------
// CAknExQueryView::CAknExQueryView()
// Overload constructor.
// ----------------------------------------------------------------------------
//
CAknExQueryView::CAknExQueryView()
: iCurrentOutline( EAknExQueryNoOutline )
    {
    }

// ----------------------------------------------------------------------------
// CAknExQueryView::~CAknExQueryView()
// Destructor.
// ----------------------------------------------------------------------------
//
CAknExQueryView::~CAknExQueryView()
    {
    if ( iContainer )
        {
        AppUi()->RemoveFromStack( iContainer );
        }
    delete iContainer;
    }

// ----------------------------------------------------------------------------
// CAknExQueryView::DisplayNextOutlineL()
// Display of NextOutline.
// ----------------------------------------------------------------------------
//
void CAknExQueryView::DisplayNextOutlineL()
    {
    switch( iCurrentOutline )
        {
        case EAknExQueryNoOutline:
            HandleCommandL( EAknExQueryOutline1 );
            break;
        case EAknExQueryOutline1:
            HandleCommandL( EAknExQueryOutline4 );
            break;
        case EAknExQueryOutline4:
            HandleCommandL( EAknExQueryOutline8 );
            break;
        case EAknExQueryOutline8:
            HandleCommandL( EAknExQueryOutline10 );
            break;
        case EAknExQueryOutline10:
            HandleCommandL( EAknExQueryOutline16 );
            break;
        case EAknExQueryOutline16:
            HandleCommandL( EAknExQueryOutline44 );
            break;
        case EAknExQueryOutline44:
            HandleCommandL( EAknExQueryOutline48 );
            break;
        case EAknExQueryOutline48:
            HandleCommandL( EAknExQueryOutline52 );
            break;
        case EAknExQueryOutline52:
            HandleCommandL( EAknExQueryOutline54 );
            break;
        case EAknExQueryOutline54:
            HandleCommandL( EAknExQueryOutline61 );
            break;
        case EAknExQueryOutline61:
            HandleCommandL( EAknExQueryNoOutline );
            break;
        default:
            break;
        }
    }

// ----------------------------------------------------------------------------
// CAknExQueryView::Id()
// Returns ID of View.
// ----------------------------------------------------------------------------
//
TUid CAknExQueryView::Id() const
    {
    return KViewId;
    }

// ----------------------------------------------------------------------------
// CAknExQueryView::HandleCommandL( TInt aCommand )
// Handles the commands.
// ----------------------------------------------------------------------------
//
void CAknExQueryView::HandleCommandL( TInt aCommand )
    {
    if ( !iContainer )
        {
        return;
        }

    iContainer->SetTextToLabelL( R_AKNEXQUERY_OPERATE_MESSAGE );
    switch ( aCommand )
        {
        case EAknExQueryNoOutline:
            SetTitlePaneL( aCommand );
            iContainer->SetTextToLabelL( R_AKNEXQUERY_TITLE_MESSAGE );
            break;
        case EAknExQueryOutline1:
            SetTitlePaneL( aCommand );
            iContainer->CreateLabelL( R_AKNEXQUERY_OUTLINE1 );
            iContainer->ShowDataQueryL( R_AKNEXQUERY_DATA_QUERY );
            break;
        case EAknExQueryOutline4:
            SetTitlePaneL( aCommand );
            iContainer->CreateLabelL( R_AKNEXQUERY_OUTLINE4 );
            iContainer->ShowDataQueryL( 
                R_AKNEXQUERY_DATA_QUERY, 
                R_AKNEXQUERY_DATA_QUERY_INITIAL_TEXT );
            break;
        case EAknExQueryOutline8:
            SetTitlePaneL( aCommand );
            iContainer->CreateLabelL( R_AKNEXQUERY_OUTLINE8 );
            iContainer->ShowDataQueryL( R_AKNEXQUERY_DATA_QUERY );
            break;
       case EAknExQueryOutline10:
            SetTitlePaneL( aCommand );
            iContainer->CreateLabelL( R_AKNEXQUERY_OUTLINE10 );
            iContainer->ShowConfirmationQueryL( 
                R_AKNEXQUERY_CONFIRMATION_QUERY );
            break;
        case EAknExQueryOutline16:
            SetTitlePaneL( aCommand );
            iContainer->CreateLabelL( R_AKNEXQUERY_OUTLINE16 );
            iContainer->ShowConfirmationQueryL( 
                R_AKNEXQUERY_CONFIRMATION_QUERY_OTHER_SOFTKEYS );
            break;
        case EAknExQueryOutline44:
            SetTitlePaneL( aCommand );
            iContainer->PopoutListBoxL();
            break;
        case EAknExQueryOutline48:
            SetTitlePaneL( aCommand );
            iContainer->CreateLabelL( R_AKNEXQUERY_OUTLINE48 );
            iContainer->ShowListQueryL( 
                R_AKNEXQUERY_LIST_SINGLE_PANE );
            break;
        case EAknExQueryOutline52:
            SetTitlePaneL( aCommand );
            iContainer->CreateLabelL( R_AKNEXQUERY_OUTLINE52 );
            iContainer->ShowListQueryL( 
                R_AKNEXQUERY_LIST_SINGLE_GRAPHIC_HEADING_PANE );
            break;
        case EAknExQueryOutline54:
            SetTitlePaneL( aCommand );
            iContainer->CreateLabelL( R_AKNEXQUERY_OUTLINE54 );
            iContainer->ShowListQueryL( R_AKNEXQUERY_MULTI_SELECTION_QUERY );
            break;
        case EAknExQueryOutline61:
            SetTitlePaneL( aCommand );
            iContainer->CreateLabelL( R_AKNEXQUERY_OUTLINE61 );
            iContainer->ShowGlobalListQueryL();
            break;
        case EAknExQueryCbaCmdNext:
            DisplayNextOutlineL();
            break;
        case EAknSoftkeyBack:
            AppUi()->ProcessCommandL( EAknCmdExit );
            break;
        default:
            SetTitlePaneL( EAknExQueryNoOutline );
            iContainer->SetTextToLabelL( R_AKNEXQUERY_TITLE_MESSAGE );
            break;
        }
    }

// ----------------------------------------------------------------------------
// CAknExQueryView::SetTitlePane( TInt aOutlineId )
// Sets text of title pane. 
// ----------------------------------------------------------------------------
//
void CAknExQueryView::SetTitlePaneL( TInt aOutlineId )
    {
    TInt titleId( KAknExQueryInitId );
    TBool defaultFlg( EFalse );

    iContainer->DeleteLabelL();
    iContainer->DeleteListBoxL();
    iCurrentOutline = aOutlineId; // Set current display outlinenumber

    switch ( aOutlineId )
        {
        case EAknExQueryNoOutline:
            defaultFlg = ETrue;
            break;
        case EAknExQueryOutline1:
            titleId = R_AKNEXQUERY_OUTLINE1;
            break;
        case EAknExQueryOutline4:
            titleId = R_AKNEXQUERY_OUTLINE4;
            break;
        case EAknExQueryOutline8:
            titleId = R_AKNEXQUERY_OUTLINE8;
            break;
        case EAknExQueryOutline10:
            titleId = R_AKNEXQUERY_OUTLINE10;
            break;
        case EAknExQueryOutline16:
            titleId = R_AKNEXQUERY_OUTLINE16;
            break;
        case EAknExQueryOutline44:
            titleId = R_AKNEXQUERY_OUTLINE44;
            break;
        case EAknExQueryOutline48:
            titleId = R_AKNEXQUERY_OUTLINE48;
            break;
        case EAknExQueryOutline52:
            titleId = R_AKNEXQUERY_OUTLINE52;
            break;
        case EAknExQueryOutline54:
            titleId = R_AKNEXQUERY_OUTLINE54;
            break;
        case EAknExQueryOutline61:
            titleId = R_AKNEXQUERY_OUTLINE61;
            break;
        default:
            defaultFlg = ETrue;
            break;
        }

    // Fetches pointer to the default title pane control.
    CAknTitlePane* titlePane = 
        static_cast<CAknTitlePane*>
            ( StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );

    if ( !defaultFlg )
        {
        // Makes and sets text whitch is used title pane.
        TBuf<KAknExQueryTitleBufLength> MessageText( KAknExQueryBufferEmpty );
        iCoeEnv->ReadResource( MessageText, titleId );
        titlePane->SetTextL( MessageText );
        }
    else
        {
        titlePane->SetTextToDefaultL();
        }
    }

// ----------------------------------------------------------------------------
// CAknExQueryView::DoActivateL()
// Creates container class object.
// ----------------------------------------------------------------------------
//
void CAknExQueryView::DoActivateL( 
    const TVwsViewId& /*aPrevViewId*/,
    TUid /*aCustomMessageId*/,
    const TDesC8& /*aCustomMessage*/ )
    {
   if ( !iContainer )
        {
        iContainer = new ( ELeave ) CAknExQueryContainer( this );
        iContainer->SetMopParent( this );
        iContainer->ConstructL( ClientRect() );
        AppUi()->AddToStackL( *this, iContainer );
        }
    
    }

// ----------------------------------------------------------------------------
// CAknExQueryView::DoDeactivate()
// Deletes container class object.
// ----------------------------------------------------------------------------
//
void CAknExQueryView::DoDeactivate()
    {
    if ( iContainer )
        {
        AppUi()->RemoveFromStack( iContainer );
        delete iContainer;
        iContainer = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CAknExQueryView::HandleViewRectChange()
// Handles Clients Rect
// -----------------------------------------------------------------------------
//
void CAknExQueryView::HandleViewRectChange()
    {
    if ( iContainer )
        {
        iContainer->SetRect( ClientRect() );
        }
    }

// End of File
