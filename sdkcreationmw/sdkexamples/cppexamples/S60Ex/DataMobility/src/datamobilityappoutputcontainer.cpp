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
* Description:  CDataMobilityAppOutputContainer implementation.
*
*/


// INCLUDE FILES
#include <AknUtils.h>
#include "datamobilityappoutputcontainer.h"
#include "datamobility.hrh"

#include "standardapplication.h"
#include "applicationusingsnap.h"
#include "alrapplication.h"
#include "applicationtriggeringconndlg.h"
#include "alrapplicationusingnopref.h"
#include "applicationusinguisettings.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDataMobilityAppOutputContainer::ConstructL
//
// Symbian OS two phased constructor.
// -----------------------------------------------------------------------------
//
void CDataMobilityAppOutputContainer::ConstructL( const TRect& aRect,
                                                  TInt aObjectToStart )
    {
    CreateWindowL();

    iLinesToPrint = new ( ELeave ) CDesCArrayFlat( 8 );

    iAllowedToPrint = ETrue;

    iApplication = NULL;

    switch( aObjectToStart )
        {
        case EAppUIGoToStandardApplication:
            {
            CStandardApplication* standardApplication =
                CStandardApplication::NewL( *this );
            CleanupStack::PushL( standardApplication );

            standardApplication->StartL();

            CleanupStack::Pop( standardApplication );

            iApplication = standardApplication;
            break;
            }
        case EAppUIGoToApplicationUsingSnap:
            {
            CApplicationUsingSNAP* applicationUsingSNAP =
                CApplicationUsingSNAP::NewL( *this );

            CleanupStack::PushL( applicationUsingSNAP );

            applicationUsingSNAP->StartL();

            CleanupStack::Pop( applicationUsingSNAP );

            iApplication = applicationUsingSNAP;
            break;
            }
        case EAppUIGoToTriggeringConnectionDialog:
            {
            CApplicationTriggeringConnDlg* applicationTriggeringConnDlg =
                CApplicationTriggeringConnDlg::NewL( *this );

            CleanupStack::PushL( applicationTriggeringConnDlg );

            applicationTriggeringConnDlg->StartL();

            CleanupStack::Pop( applicationTriggeringConnDlg );

            iApplication = applicationTriggeringConnDlg;
            break;
            }
        case EAppUIGoToALRApplication:
            {
            CALRApplication* alrApplication = CALRApplication::NewL( *this );

            CleanupStack::PushL( alrApplication );

            alrApplication->StartL();

            CleanupStack::Pop( alrApplication );

            iApplication = alrApplication;
            break;
            }
        case EAppUIGoToALRApplicationUsingDefaultConnection:
            {
            CALRApplicationUsingNoPref* alrApplicationUsingNoPref =
                CALRApplicationUsingNoPref::NewL( *this );

            CleanupStack::PushL( alrApplicationUsingNoPref );

            alrApplicationUsingNoPref->StartL();

            CleanupStack::Pop( alrApplicationUsingNoPref );

            iApplication = alrApplicationUsingNoPref;
            break;
            }
        case EAppUIGoToUsingConnectionSetting:
            {
            CApplicationUsingUiSettings* applicationUsingUiSettings =
                CApplicationUsingUiSettings::NewL( *this );

            CleanupStack::PushL( applicationUsingUiSettings );

            applicationUsingUiSettings->StartL();

            CleanupStack::Pop( applicationUsingUiSettings );

            iApplication = applicationUsingUiSettings;
            break;
            }
        default:
            break;
        }


    SetRect( aRect );
    ActivateL();
    }

// ----------------------------------------------------------------------------
// CDataMobilityAppOutputContainer::~CDataMobilityAppOutputContainer
//
// Destructor.
// ----------------------------------------------------------------------------
//
CDataMobilityAppOutputContainer::~CDataMobilityAppOutputContainer()
    {
    iAllowedToPrint = EFalse;
    delete iApplication;
    delete iLinesToPrint;
    }

// ----------------------------------------------------------------------------
// CDataMobilityAppOutputContainer::Draw
//
// Draw a control, called by window server.
// ----------------------------------------------------------------------------
//
void CDataMobilityAppOutputContainer::Draw( const TRect& aRect ) const
    {
    // Get the standard graphics context
    CWindowGc& gc = SystemGc();

    // Clears the screen
    gc.Clear( aRect );

    // draw all the texts to the screen
    gc.UseFont( AknLayoutUtils::FontFromId( EAknLogicalFontSecondaryFont ) );

    gc.SetPenColor( KRgbBlack );

    for ( TInt i( 0 ); i < iLinesToPrint->Count(); ++i )
        {
        gc.DrawText( iLinesToPrint->MdcaPoint( i ),
                     TPoint( KTextXMargin, KTextMinY+
                                 ( i*KTextDistanceBetweenLines ) ) );
        }

    gc.DiscardFont();
    }

// ----------------------------------------------------------------------------
// CDataMobilityAppOutputContainer::Print
//
// Print one line of text in the output view.
// ----------------------------------------------------------------------------
//
void CDataMobilityAppOutputContainer::Print( const TDesC& aLine )
    {
    if ( iAllowedToPrint )
        {
        TRAP_IGNORE( iLinesToPrint->AppendL( aLine ) )
        DrawNow();
        }
    }

// ---------------------------------------------------------
// CDataMobilityAppOutputContainer::HandleResourceChange()
//
// Called by framework when layout is changed.
// ---------------------------------------------------------
//
void CDataMobilityAppOutputContainer::HandleResourceChange( TInt aType )
    {
    CCoeControl::HandleResourceChange( aType );

    // ADDED FOR SCALABLE UI SUPPORT
    // *****************************
    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        TRect rect;
        AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, rect );
        SetRect( rect );
        }
    }

// End of File
