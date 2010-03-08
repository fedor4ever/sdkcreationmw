/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CDataMobilityAppMenuView implementation.
*
*/


// INCLUDE FILES
#include  <aknviewappui.h>
#include  <barsread.h>  //TResourceReader
#include  <datamobility.rsg>
#include  "datamobilityappmenuview.h"
#include  "datamobilityappmenucontainer.h"
#include  "datamobility.hrh"
#include  "datamobilityappui.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDataMobilityAppMenuView::NewL
//
// Symbian OS Two-phased constructor.
// -----------------------------------------------------------------------------
//
CDataMobilityAppMenuView* CDataMobilityAppMenuView::NewL()
    {
    CDataMobilityAppMenuView* self =
        new ( ELeave ) CDataMobilityAppMenuView();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CDataMobilityAppMenuView::ConstructL
//
// Symbian OS two-phased constructor
// -----------------------------------------------------------------------------
//
void CDataMobilityAppMenuView::ConstructL()
    {
    BaseConstructL( R_APPUI_MENUVIEW );
    }

// -----------------------------------------------------------------------------
// CDataMobilityAppMenuView::~CDataMobilityAppMenuView
//
// Destructor
// -----------------------------------------------------------------------------
//
CDataMobilityAppMenuView::~CDataMobilityAppMenuView()
    {
    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
        }

    delete iNaviDecorator;
    }

// -----------------------------------------------------------------------------
// TUid CDataMobilityAppMenuView::Id
//
// Returns view´s id.
// -----------------------------------------------------------------------------
//
TUid CDataMobilityAppMenuView::Id() const
    {
    return TUid::Uid( EMenuViewId );
    }

// -----------------------------------------------------------------------------
// CDataMobilityAppMenuView::HandleCommandL
//
// Handles a command.
// -----------------------------------------------------------------------------
//
void CDataMobilityAppMenuView::HandleCommandL( TInt aCommand )
    {
    switch ( aCommand )
        {
        case EAknSoftkeyBack:
            {
            AppUi()->HandleCommandL( EEikCmdExit );
            break;
            }
        case EAknCmdOpen:
            {
            TInt activeLine = iContainer->GetActiveLine();
            iContainer->HandleSelectedListItemL( activeLine );
            }
            break;
        default:
            {
            AppUi()->HandleCommandL( aCommand );
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CDataMobilityAppMenuView::DoActivateL
//
// Initializes view when activated.
// -----------------------------------------------------------------------------
//
void CDataMobilityAppMenuView::DoActivateL(
   const TVwsViewId& /*aPrevViewId*/, TUid /*aCustomMessageId*/,
   const TDesC8& /*aCustomMessage*/ )
    {
    if ( !iContainer )
        {
        iContainer = new ( ELeave ) CDataMobilityAppMenuContainer;
        iContainer->SetMopParent( this );
        iContainer->ConstructL( ClientRect() );
        AppUi()->AddToStackL( *this, iContainer );
        }

    CEikStatusPane* statusPane = iEikonEnv->AppUiFactory()->StatusPane();
    CAknNavigationControlContainer* navigation =
        static_cast<CAknNavigationControlContainer*>
            ( statusPane->ControlL( TUid::Uid( EEikStatusPaneUidNavi ) ) );

    TResourceReader reader;
    iCoeEnv->CreateResourceReaderLC( reader, R_NAVITITLE_MENUVIEW );
    iNaviDecorator = navigation->CreateNavigationLabelL( reader );
    CleanupStack::PopAndDestroy(); // resource reader

    navigation->PushL( *iNaviDecorator );
   }

// -----------------------------------------------------------------------------
// CAppUIView::DoDeactivate
//
// Deactivates view.
// -----------------------------------------------------------------------------
//
void CDataMobilityAppMenuView::DoDeactivate()
    {
    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
        iContainer = NULL;
        }
    delete iNaviDecorator;
    iNaviDecorator = NULL;
    }

// End of File
