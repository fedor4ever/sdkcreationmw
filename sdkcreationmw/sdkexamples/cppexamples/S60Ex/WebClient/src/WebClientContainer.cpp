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
#include <coemain.h>
#include <eikenv.h>
#include <txtrich.h>
#include <eiklabel.h>
#include <WebClient.rsg>
#include "WebClient.pan"
#include "WebClientContainer.h"
#include "WebClientAppUi.h"
#include "WebClientEngine.h"
#include "WebClient.hrh"

// ----------------------------------------------------------------------------
// CWebClientContainer::NewL()
// Creates instance of CWebClientContainer.
// ----------------------------------------------------------------------------
CWebClientContainer* CWebClientContainer::NewL( const TRect& aRect )
    {
    CWebClientContainer* self = CWebClientContainer::NewLC( aRect );
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CWebClientContainer::NewLC()
// Creates instance of CWebClientContainer.
// ----------------------------------------------------------------------------
CWebClientContainer* CWebClientContainer::NewLC( const TRect& aRect )
    {
    CWebClientContainer* self = new (ELeave) CWebClientContainer;
    CleanupStack::PushL( self );
    self->ConstructL( aRect );
    return self;
    }

// ----------------------------------------------------------------------------
// CWebClientContainer::CWebClientContainer()
// First phase construction.
// ----------------------------------------------------------------------------
CWebClientContainer::CWebClientContainer()
    {
    // no implementation required
    }

// ----------------------------------------------------------------------------
// CWebClientContainer::ConstructL()
// Second phase construction.
// ----------------------------------------------------------------------------
//
void CWebClientContainer::ConstructL( const TRect& aRect )
    {
    // Create a window for this application view
    CreateWindowL();

    iLabel = new (ELeave) CEikLabel;
    iLabel->SetContainerWindowL( *this );
    iLabel->SetTextL( KHttpPrefix );

    // Create header window
    iHeaderWindow = new (ELeave) CEikRichTextEditor();
    iHeaderWindow->SetContainerWindowL( *this );
    iHeaderWindow->ConstructL( this, KNumberOfLines, KTextLimit, 
        EEikEdwinReadOnly, EGulFontControlAll, EGulNoSymbolFonts );
    iHeaderWindow->CreateScrollBarFrameL()->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff,
        CEikScrollBarFrame::EOn );
  
    // Create source window
    iSourceWindow = new (ELeave) CEikRichTextEditor();
    iSourceWindow->SetContainerWindowL( *this );
    iSourceWindow->ConstructL( this, KNumberOfLines, KTextLimit, 
        EEikEdwinReadOnly, EGulFontControlAll, EGulNoSymbolFonts );
    iSourceWindow->CreateScrollBarFrameL()->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff,
        CEikScrollBarFrame::EOn );

    // Create status window
    iStatusWindow = new (ELeave) CEikRichTextEditor();
    iStatusWindow->SetContainerWindowL( *this );
    iStatusWindow->ConstructL( this, KNumberOfLines, KTextLimit, 
        EEikEdwinReadOnly, EGulFontControlAll, EGulNoSymbolFonts );
    iStatusWindow->CreateScrollBarFrameL()->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff,
        CEikScrollBarFrame::EOn );
    iStatusWindow->SetFocus( ETrue );

    // Set the main window size
    SetRect( aRect );

    // Activate the window, which makes it ready to be drawn
    ActivateL();
    }

// ----------------------------------------------------------------------------
// CWebClientContainer::~CWebClientContainer()
// Destructor.
// ----------------------------------------------------------------------------
//
CWebClientContainer::~CWebClientContainer()
    {
    if ( iLabel )
        {
        delete iLabel;
        iLabel = NULL;
        }
    if ( iHeaderWindow )
        {
        delete iHeaderWindow;
        iHeaderWindow = NULL;
        }
    if ( iSourceWindow )
        {
        delete iSourceWindow;
        iSourceWindow = NULL;
        }
    if ( iStatusWindow )
        {
        delete iStatusWindow;
        iStatusWindow = NULL;
        }
    }

// ---------------------------------------------------------
// CWebClientContainer::SizeChanged()
// Called by framework when the view size is changed
// ---------------------------------------------------------
//
void CWebClientContainer::SizeChanged()
    {
    TInt pointY = 0;
    TRect rect = Rect();
    TInt width = rect.Width();
    TInt height = rect.Height() - 4;    // leave space for thin lines between windows
    TSize labelSize = iLabel->MinimumSize();
    TInt dataWindowHeigth = (( height - labelSize.iHeight - KStatusWindowHeight ) / 2 );
    labelSize.iWidth = width;

    iLabel->SetExtent( TPoint( 0,pointY ), labelSize );
    pointY = pointY + labelSize.iHeight + 1;
    iHeaderWindow->SetExtent( TPoint( 0,pointY ),
        TSize( width - KScrollBarWidth, dataWindowHeigth ));
    pointY = pointY + dataWindowHeigth + 1;
    iSourceWindow->SetExtent( TPoint( 0,pointY ),
        TSize( width - KScrollBarWidth, dataWindowHeigth ));
    pointY = pointY + dataWindowHeigth + 1;
    iStatusWindow->SetExtent( TPoint( 0,pointY ),
        TSize( width - KScrollBarWidth, KStatusWindowHeight ));

    DrawNow();
    }

// ----------------------------------------------------------------------------
// CWebClientContainer::Draw()
// Draw this application's view to the screen
// ----------------------------------------------------------------------------
//
void CWebClientContainer::Draw( const TRect& aRect ) const
    {
    CWindowGc& gc = SystemGc();

    gc.SetPenStyle( CGraphicsContext::ENullPen );
    gc.SetBrushColor( KRgbGray );
    gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
    gc.DrawRect( aRect );
    }

// ----------------------------------------------------------------------------
// CWebClientContainer::CountComponentControls()
// Returns number of controls in this compound control.
// ----------------------------------------------------------------------------
//
TInt CWebClientContainer::CountComponentControls() const
    {
    return 4;   // Label and 3 windows
    }

// ----------------------------------------------------------------------------
// CWebClientContainer::ComponentControl()
// Returns pointer to control with index aIndex.
// ----------------------------------------------------------------------------
//
CCoeControl* CWebClientContainer::ComponentControl( TInt aIndex ) const
    {
    switch( aIndex ) 
        {
        case 0:
            return iLabel;
        case 1:
            return iHeaderWindow;
        case 2:
            return iSourceWindow;
        case 3:
            return iStatusWindow;
        default:
            Panic( EClientView );
            return 0;
        }
    }

// ----------------------------------------------------------------------------
// CWebClientContainer::HandleControlEventL()
// Handles control events.
// ----------------------------------------------------------------------------
//
void CWebClientContainer::HandleControlEventL( CCoeControl* /* aControl */, 
                                               TCoeEvent    /* aEventType */ )
    {
    // no implementation required
    }

// ----------------------------------------------------------------------------
// CWebClientContainer::ResetL()
// Set label text and reset contents of text windows.
// ----------------------------------------------------------------------------
//
void CWebClientContainer::ResetL( const TDesC& aText )
    {
    iLabel->SetTextL( aText );
    iLabel->DrawNow();

    iHeaderWindow->Text()->Reset();
    iHeaderWindow->HandleTextChangedL();
    iHeaderWindow->SetCursorPosL( 0, EFalse );

    iSourceWindow->Text()->Reset();
    iSourceWindow->HandleTextChangedL();
    iSourceWindow->SetCursorPosL( 0, EFalse );

    iStatusWindow->Text()->Reset();
    iStatusWindow->HandleTextChangedL();
    iStatusWindow->SetCursorPosL( 0, EFalse );

    iHeaderWindow->SetFocus( EFalse );
    iSourceWindow->SetFocus( EFalse );
    iStatusWindow->SetFocus( ETrue );
    }

// ----------------------------------------------------------------------------
// CWebClientContainer::HandleCommandL()
// Called from CWebClientAppUi::HandleCommandL()
// ----------------------------------------------------------------------------
//
void CWebClientContainer::HandleCommandL( TInt aCommand )
    {
    switch( aCommand )
        {
        case EWebClientCmdHeader:
            {
            // Change focus to Header window and set cursor visible
            iHeaderWindow->SetFocus( ETrue );
            iSourceWindow->SetFocus( EFalse );
            iStatusWindow->SetFocus( EFalse );
            iHeaderWindow->MoveCursorL( TCursorPosition::EFNoMovement, EFalse );
            }
            break;
        case EWebClientCmdSource:
            {
            // Change focus to Source window and set cursor visible
            iSourceWindow->SetFocus( ETrue );
            iHeaderWindow->SetFocus( EFalse );
            iStatusWindow->SetFocus( EFalse );
            iSourceWindow->MoveCursorL( TCursorPosition::EFNoMovement, EFalse );
            }
            break;
        case EWebClientCmdStatus:
            {
            // Change focus to Status window and set cursor visible
            iStatusWindow->SetFocus( ETrue );
            iHeaderWindow->SetFocus( EFalse );
            iSourceWindow->SetFocus( EFalse );
            iStatusWindow->MoveCursorL( TCursorPosition::EFNoMovement, EFalse );
            }
            break;
        default:
            break;
        }
    }

// ----------------------------------------------------------------------------
// CWebClientContainer::OfferKeyEventL()
// Handles key events.
// ----------------------------------------------------------------------------
//
TKeyResponse CWebClientContainer::OfferKeyEventL( const TKeyEvent& aKeyEvent, 
                                                  TEventCode aType )
    {
    // Redirect keyevents to controls
    if ( iHeaderWindow ) 
        {
        if ( iHeaderWindow->IsFocused() )
            return iHeaderWindow->OfferKeyEventL( aKeyEvent, aType );
        }

    if ( iSourceWindow ) 
        {
        if ( iSourceWindow->IsFocused() )
            return iSourceWindow->OfferKeyEventL( aKeyEvent, aType );
        }

    if ( iStatusWindow ) 
        {
        if ( iStatusWindow->IsFocused() )
            return iStatusWindow->OfferKeyEventL( aKeyEvent, aType );
        }

    return EKeyWasNotConsumed;
    }

// ----------------------------------------------------------------------------
// CWebClientContainer::AddToHeaderWindowL()
// Add a single line of text to the header window.
// ----------------------------------------------------------------------------
//
void CWebClientContainer::AddToHeaderWindowL( const TDesC& aText )
    {
    // Append aText to header window with line break
    CRichText* text = iHeaderWindow->RichText();
    text->InsertL( text->DocumentLength(), aText );
    text->InsertL( text->DocumentLength(), CEditableText::ELineBreak );
    iHeaderWindow->HandleTextChangedL();
    }

// ----------------------------------------------------------------------------
// CWebClientContainer::AddToSourceWindowL()
// Add text to Sourcewindow. No formatting is provided.
// ----------------------------------------------------------------------------
//
void CWebClientContainer::AddToSourceWindowL( const TDesC8& aText )
    {
    // Convert 8-bit aText to 16-bit and append it to end of Source window
    HBufC* tempBuf = HBufC::NewL( aText.Length() );
    CleanupStack::PushL( tempBuf );
    tempBuf->Des().Copy( aText );
    CRichText* text = iSourceWindow->RichText();
    text->InsertL( text->DocumentLength(), *tempBuf );
    iSourceWindow->HandleTextChangedL();
    CleanupStack::PopAndDestroy( tempBuf );
    }

// ----------------------------------------------------------------------------
// CWebClientContainer::AddToStatusWindowL()
// Add a single line of text to the status window.
// ----------------------------------------------------------------------------
//
void CWebClientContainer::AddToStatusWindowL( const TDesC& aText )
    {
    // Append aText to status window with line break
    CRichText* text = iStatusWindow->RichText();
    text->InsertL( text->DocumentLength(), aText );
    text->InsertL( text->DocumentLength(), CEditableText::ELineBreak );
    iStatusWindow->HandleTextChangedL();
    iStatusWindow->MoveCursorL( TCursorPosition::EFLineDown, EFalse );
    }

// ----------------------------------------------------------------------------
// CWebClientContainer::ClientEvent()
// Called by CWebClientEngine to notify events to user
// ----------------------------------------------------------------------------
//
void CWebClientContainer::ClientEvent( const TDesC& aEventDescription )
    {
    TRAPD( err, AddToStatusWindowL( aEventDescription ));
    if( err )
        Panic( EClientView );
    }

// ----------------------------------------------------------------------------
// CWebClientContainer::ClientHeaderReceived()
// Called by CWebClientEngine when HTTP header received
// ----------------------------------------------------------------------------
//
void CWebClientContainer::ClientHeaderReceived( const TDesC& aHeaderData ) 
    {
    TRAPD( err, AddToHeaderWindowL( aHeaderData ));
    if( err )
        Panic( EClientView );
    }

// ----------------------------------------------------------------------------
// CWebClientContainer::ClientBodyReceived()
// Called by CWebClientEngine when part of response body received
// ----------------------------------------------------------------------------
//
void CWebClientContainer::ClientBodyReceived( const TDesC8& aBodyData ) 
    {
    TRAPD( err, AddToSourceWindowL( aBodyData ));
    if( err )
        Panic( EClientView );
    }
