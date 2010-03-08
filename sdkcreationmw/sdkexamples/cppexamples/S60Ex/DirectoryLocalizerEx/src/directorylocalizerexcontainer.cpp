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


#include "directorylocalizerexcontainer.h"
#include <directorylocalizerex.rsg>
#include <aknnotewrappers.h>
#include <COEMAIN.H>
#include <barsread.h>
#include <f32file.h>
#include <e32des8.h>

#define KAknAtListGray TRgb(0xaaaaaa)


_LIT( KDLTestExistsDir, "c:\\example\\" );
_LIT( strNote, "Not Localized" );

// ============================== CONSTANTS ==================================
const TInt KMaxString = 20;

// =========================== MEMBER FUNCTIONS ==============================

// ---------------------------------------------------------------------------
// C++ default Constructor
// ---------------------------------------------------------------------------
//
CDirectoryLocalizerExContainer::CDirectoryLocalizerExContainer()
    {
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CDirectoryLocalizerExContainer::~CDirectoryLocalizerExContainer()
    {
    ResetControl();
    }

// ---------------------------------------------------------------------------
// Symbian 2nd Constructor
// ---------------------------------------------------------------------------
//
void CDirectoryLocalizerExContainer::ConstructL( const TRect& aRect )
    {
    CreateWindowL();
    SetRect( aRect );
    ActivateL();
    }

// ----------------------------------------------------------------------------
// CDirectoryLocalizerContainer::Draw
// Fills the window's rectangle.
// ----------------------------------------------------------------------------
//
void CDirectoryLocalizerExContainer::Draw( const TRect& aRect ) const
    {
    CWindowGc& gc = SystemGc();
    gc.SetPenStyle( CGraphicsContext::ENullPen );
    gc.SetBrushColor( KAknAtListGray );
    gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
    gc.DrawRect( aRect );
    }

// ---------------------------------------------------------------------------
// CDirectoryLocalizerContainer::CountComponentControls
// ---------------------------------------------------------------------------
//
TInt CDirectoryLocalizerExContainer::CountComponentControls() const
    {
    if ( iControl )
        {
        return 1;
        }
    else
        {
        return 0;
        }
    }

// ---------------------------------------------------------------------------
// CDirectoryLocalizerContainer::ComponentControl
// ---------------------------------------------------------------------------
//
CCoeControl* CDirectoryLocalizerExContainer::ComponentControl( TInt ) const
    {
    return iControl;
    }

// ---------------------------------------------------------------------------
// CDirectoryLocalizerContainer::SetControlL
// ---------------------------------------------------------------------------
//
void CDirectoryLocalizerExContainer::SetControlL( CCoeControl* aControl )
    {
    iControl = aControl;
    if ( iControl )
        {
        // You can change the position and size
        iControl->SetExtent( Rect().iTl, Rect().Size() );
        iControl->ActivateL();
        DrawNow();
        }
    }

// ---------------------------------------------------------------------------
// CDirectoryLocalizerContainer::ResetControl
// ---------------------------------------------------------------------------
//
void CDirectoryLocalizerExContainer::ResetControl()
    {
    delete iControl;
    iControl = NULL;
    }
// ---------------------------------------------------------------------------
// CDirectoryLocalizerContainer::SetupFromResRdL
// ---------------------------------------------------------------------------
//

void CDirectoryLocalizerExContainer::SetupFromResRdL()
    {
    TResourceReader reader;
    CCoeEnv::Static()->CreateResourceReaderLC( reader,
                            R_EXAMPLE_LOCALIZER_ENTRIES );
    CDirectoryLocalizer*  localizer = CDirectoryLocalizer::NewL( reader );
    CleanupStack::PushL(localizer);  // localizer

    localizer->SetFullPath( KDLTestExistsDir );
    TBool success = localizer->IsLocalized();

    CAknInformationNote *note = new ( ELeave ) CAknInformationNote();
    if( success )
        {
        const TPtrC strNote = localizer->LocalizedName();
        note->ExecuteLD( strNote );
        }
    else
        {
        note->ExecuteLD( strNote );
        }

    CleanupStack::PopAndDestroy();  // localizer
    CleanupStack::PopAndDestroy();  // reader
    }
// ---------------------------------------------------------------------------
// CDirectoryLocalizerContainer::SetupFromResIdL
// ---------------------------------------------------------------------------
//
void CDirectoryLocalizerExContainer::SetupFromResIdL()
    {
    // construct with default data
    CDirectoryLocalizer* localizer = CDirectoryLocalizer
                                         ::NewL( R_EXAMPLE_LOCALIZER_ENTRIES );
    CleanupStack::PushL(localizer);  // localizer

    localizer->SetFullPath( KDLTestExistsDir );
    TBool success = localizer->IsLocalized();

    CAknInformationNote *note = new ( ELeave ) CAknInformationNote();
    if( success )
        {
        const TPtrC strNote = localizer->LocalizedName();
        note->ExecuteLD( strNote );
        }
    else
        {
        note->ExecuteLD( strNote );
        }

    CleanupStack::PopAndDestroy();  // localizer
    }
// ---------------------------------------------------------------------------
// CDirectoryLocalizerContainer::LocalizedFromResIdL
// ---------------------------------------------------------------------------
//

void CDirectoryLocalizerExContainer::LocalizedFromResIdL()
    {
    // construct with default data
    CDirectoryLocalizer* localizer = CDirectoryLocalizer::NewL();
    CleanupStack::PushL(localizer);  // localizer

    localizer->AddFromResourceL( R_EXAMPLE_LOCALIZER_ENTRIES );
    localizer->SetFullPath( KDLTestExistsDir );
    TBool success = localizer->IsLocalized();

    CAknInformationNote *note = new ( ELeave ) CAknInformationNote();
    if( success )
        {
        const TPtrC strNote = localizer->LocalizedName();
        note->ExecuteLD( strNote );
        }
    else
        {
        note->ExecuteLD( strNote );
        }

    CleanupStack::PopAndDestroy();  // localizer
    }

// ---------------------------------------------------------------------------
// CDirectoryLocalizerContainer::GetExtraDataL
// ---------------------------------------------------------------------------
//
void CDirectoryLocalizerExContainer::GetExtraDataL()
    {
    // construct with default data
    CDirectoryLocalizer* localizer = CDirectoryLocalizer::NewL();
    CleanupStack::PushL(localizer);  // localizer

    localizer->AddFromResourceL( R_EXAMPLE_LOCALIZER_ENTRIES );
    localizer->SetFullPath( KDLTestExistsDir );
    TBool success = localizer->IsLocalized();

    CAknInformationNote *note = new ( ELeave ) CAknInformationNote();
    if( success )
        {
        const TPtrC strNote = localizer->ExtraData();
        note->ExecuteLD( strNote );
        }
    else
        {
        note->ExecuteLD( strNote );
        }

    CleanupStack::PopAndDestroy();  // localizer
    }
// ---------------------------------------------------------------------------
// CDirectoryLocalizerContainer::GetIconIndexL
// ---------------------------------------------------------------------------
//
void CDirectoryLocalizerExContainer::GetIconIndexL()
    {
    // construct with default data
    CDirectoryLocalizer* localizer = CDirectoryLocalizer::NewL();
    CleanupStack::PushL(localizer);  // localizer

    localizer->AddFromResourceL( R_EXAMPLE_LOCALIZER_ENTRIES );
    localizer->SetFullPath( KDLTestExistsDir );
    TBool success = localizer->IsLocalized();

    CAknInformationNote *note = new ( ELeave ) CAknInformationNote();
    if( success )
        {
        TInt iconIndex = localizer->Icon();
        TBuf<KMaxString> strIndex;
        strIndex.AppendNum( iconIndex );
        note->ExecuteLD( strIndex );
        }
    else
        {
        note->ExecuteLD( strNote );
        }

    CleanupStack::PopAndDestroy();  // localizer
    }
