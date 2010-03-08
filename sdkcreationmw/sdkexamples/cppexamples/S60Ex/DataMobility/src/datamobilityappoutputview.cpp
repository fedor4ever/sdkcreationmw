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
* Description:  CDataMobilityAppOutputView implementation.
*
*/


// INCLUDE FILES
#include  <eikmenub.h>
#include  <aknviewappui.h>
#include  <akntitle.h>
#include  <AknUtils.h>
#include  <barsread.h>  //TResourceReader
#include  <datamobility.rsg>
#include  "datamobilityappoutputview.h"
#include  "datamobilityappoutputcontainer.h"
#include  "datamobilityappui.h"

#include  "datamobility.hrh"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDataMobilityAppOutputView::NewL
//
// Symbian OS Two-phased constructor.
// -----------------------------------------------------------------------------
//
CDataMobilityAppOutputView* CDataMobilityAppOutputView::NewL()
    {
    CDataMobilityAppOutputView* self =
        new ( ELeave ) CDataMobilityAppOutputView();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CDataMobilityAppOutputView::ConstructL
//
// Symbian OS two-phased constructor.
// ----------------------------------------------------------------------------
//
void CDataMobilityAppOutputView::ConstructL()
    {
    BaseConstructL( R_APPUI_OUTPUTVIEW );
    }

// ----------------------------------------------------------------------------
// CDataMobilityAppOutputView::~CDataMobilityAppOutputView
//
// Destructor.
// ----------------------------------------------------------------------------
//
CDataMobilityAppOutputView::~CDataMobilityAppOutputView()
    {
    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
        }

    delete iNaviDecorator;
    }

// ----------------------------------------------------------------------------
// CDataMobilityAppOutputView::Id
//
// Returns view´s id.
// ----------------------------------------------------------------------------
//
TUid CDataMobilityAppOutputView::Id() const
    {
    return TUid::Uid( EOutputViewId );
    }

// ----------------------------------------------------------------------------
// CDataMobilityAppOutputView::HandleCommandL
//
// Handles a command.
// ----------------------------------------------------------------------------
//
void CDataMobilityAppOutputView::HandleCommandL( TInt aCommand )
    {
    switch ( aCommand )
        {
        case EAknSoftkeyBack:
            {
            AppUi()->HandleCommandL( EAppUIGoBack );
            break;
            }
        default:
            {
            AppUi()->HandleCommandL( aCommand );
            break;
            }
        }

    }

// ----------------------------------------------------------------------------
// CDataMobilityAppOutputView::DoActivateL
//
// Initializes view when activated.
// ----------------------------------------------------------------------------
//
void CDataMobilityAppOutputView::DoActivateL(
   const TVwsViewId& /*aPrevViewId*/, TUid aCustomMessageId,
   const TDesC8& /*aCustomMessage*/ )
    {
    if ( !iContainer )
        {
        iContainer = new ( ELeave ) CDataMobilityAppOutputContainer;
        iContainer->SetMopParent( this );

        iContainer->ConstructL( ClientRect(), aCustomMessageId.iUid );

        AppUi()->AddToStackL( *this, iContainer );
        }

    CEikStatusPane* statusPane = iEikonEnv->AppUiFactory()->StatusPane();
    CAknNavigationControlContainer* navigation =
        static_cast<CAknNavigationControlContainer*>
            ( statusPane->ControlL( TUid::Uid( EEikStatusPaneUidNavi ) ) );

    TResourceReader reader;
    iCoeEnv->CreateResourceReaderLC( reader, R_NAVITITLE_OUTPUTVIEW );
    iNaviDecorator = navigation->CreateNavigationLabelL( reader );
    CleanupStack::PopAndDestroy(); // resource reader

    navigation->PushL( *iNaviDecorator );
    }

// ----------------------------------------------------------------------------
// CDataMobilityAppOutputView::DoDeactivate
//
// Deactivates view.
// ----------------------------------------------------------------------------
//
void CDataMobilityAppOutputView::DoDeactivate()
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
