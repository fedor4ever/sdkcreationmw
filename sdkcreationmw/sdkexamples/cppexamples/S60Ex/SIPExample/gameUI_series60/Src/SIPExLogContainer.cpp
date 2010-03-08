/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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
#include "SIPExLogContainer.h"
#include "SIPExGameEngine.h"
#include "SIPExDoc.h"

#include <SIPEx.rsg>

#include <akndef.h>
#include <eikenv.h>
#include <eikgted.h> // CGlobalTextEditor
#include <txtglobl.h> // CGlobalText
#include <barsread.h>

#include <AknsDrawUtils.h>
#include <AknsBasicBackgroundControlContext.h>

const TInt KComponentCount( 1 );
const TInt KEditorMaxLength( 1024 );
const TInt KEditorDistanceFromScreenEdge( 8 );
const TInt KEditorBorderGrow( 2 );
const TInt KFontColorIndexBlue( 200 );

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPExGameContainer::CSIPExGameContainer
// C++ default constructor
// -----------------------------------------------------------------------------
//
CSIPExLogContainer::CSIPExLogContainer( CSIPExEngine& aEngine )
    : iEngine( aEngine )
    {
    // no implementation required
    }
    
// -----------------------------------------------------------------------------
// CSIPExLogContainer::ConstructL
// EPOC two phased constructor
// -----------------------------------------------------------------------------
//
void CSIPExLogContainer::ConstructL( const TRect& aRect )
    {
    CreateWindowL();
    
    iBgContext = 
	    CAknsBasicBackgroundControlContext::NewL( KAknsIIDQsnBgAreaMain, 
	                                              aRect, 
	                                              ETrue );
    
    // Create editor in which info texts are shown (log window)
    //
    iEditor = new ( ELeave ) CEikGlobalTextEditor();
    
    TInt flags( EEikEdwinNoAutoSelection |
                EEikEdwinJustAutoCurEnd |
                EEikEdwinLineCursor |
                EEikEdwinNoHorizScrolling );
      
    TRect editorRect( aRect );
    editorRect.Shrink( KEditorDistanceFromScreenEdge, 
                       KEditorDistanceFromScreenEdge );
                                 
    iEditor->ConstructL( this, 
                         NumOfFittingEditorLines( editorRect ), 
                         KEditorMaxLength, 
                         flags, 
                         EGulFontControlAll, 
                         EGulAllFonts );
                         
    iEditor->SetContainerWindowL( *this );
    

    	
    AknLayoutUtils::LayoutEdwin( iEditor, 
                                 editorRect, 
                                 GetEditorLayout( editorRect ) );
    
    iEditor->CreateScrollBarFrameL();
    iEditor->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );
    

    SetRect( aRect );
    
    iEditor->ActivateL();
           
    ShowInfoL();
        
    ActivateL();
    }

// -----------------------------------------------------------------------------
// CSIPExLogContainer::~CSIPExLogContainer
// Destructor
// -----------------------------------------------------------------------------
//
CSIPExLogContainer::~CSIPExLogContainer()
    {
    delete iEditor;
    delete iBgContext;
    }

// -----------------------------------------------------------------------------
// CSIPExLogContainer::SizeChanged
// Called by framework when the view size is changed
// -----------------------------------------------------------------------------
//
void CSIPExLogContainer::SizeChanged()
    {
    TRect editorRect( Rect() );
    editorRect.Shrink( KEditorDistanceFromScreenEdge, 
                       KEditorDistanceFromScreenEdge );
    
    AknLayoutUtils::LayoutEdwin( iEditor, 
                                 editorRect, 
                                 GetEditorLayout( editorRect ) );
    }

// -----------------------------------------------------------------------------
// CSIPExLogContainer::CountComponentControls
// -----------------------------------------------------------------------------
//
TInt CSIPExLogContainer::CountComponentControls() const
    {
    return KComponentCount; // return nbr of controls inside this container
    }

// -----------------------------------------------------------------------------
// CSIPExLogContainer::ComponentControl
// -----------------------------------------------------------------------------
//
CCoeControl* CSIPExLogContainer::ComponentControl( TInt aIndex ) const
    {
    switch( aIndex )
        {
        case 0:
            return iEditor;
        default:
            {
            _LIT( KPanicCntx, "SIP Example" );
            __ASSERT_ALWAYS( EFalse, User::Panic( KPanicCntx(), KErrArgument ) );
            }
        }
    return NULL;
    }

// -----------------------------------------------------------------------------
// CSIPExLogContainer::Draw
// -----------------------------------------------------------------------------
//
void CSIPExLogContainer::Draw( const TRect& aRect ) const
    {
    CWindowGc& gc = SystemGc();

    gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
    gc.SetBrushColor( KRgbWhite );
    gc.Clear();
    
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    MAknsControlContext* cc = AknsDrawUtils::ControlContext( this );
    
    AknsDrawUtils::Background( skin, cc, this, gc, aRect );
    
    // Draw editor border
    gc.SetBrushStyle( CGraphicsContext::ENullBrush );
    gc.SetPenStyle( CGraphicsContext::ESolidPen );
    gc.SetPenColor( TRgb( 174, 186, 227 ) );

    TRect editorRect( iEditor->Rect() );
    editorRect.Grow( KEditorBorderGrow, KEditorBorderGrow );                   
    gc.DrawRect( editorRect );  
    }

// -----------------------------------------------------------------------------
// CSIPExLogContainer::HandleControlEventL(
//     CCoeControl* aControl,TCoeEvent aEventType)
// -----------------------------------------------------------------------------
//
void CSIPExLogContainer::HandleControlEventL(
    CCoeControl* /*aControl*/,TCoeEvent /*aEventType*/ )
    {
    // Add your control event handler code here
    }
    
// -----------------------------------------------------------------------------
// CSIPExLogContainer::OfferKeyEventL
// From CCoeControl.
// -----------------------------------------------------------------------------
//
TKeyResponse CSIPExLogContainer::OfferKeyEventL( 
    const TKeyEvent& aKeyEvent, 
    TEventCode aType )
    {
    if( aType == EEventKeyUp )
        {
        switch( aKeyEvent.iScanCode )
            {
            case EStdKeyUpArrow:
                iEditor->MoveCursorL( TCursorPosition::EFPageUp, EFalse );
                return EKeyWasConsumed;
                
            case EStdKeyDownArrow:
                iEditor->MoveCursorL( TCursorPosition::EFPageDown, EFalse );
                return EKeyWasConsumed;
          
            default:
                return EKeyWasNotConsumed;
            }
        }

    return EKeyWasNotConsumed;
    }
   
// -----------------------------------------------------------------------------
// CSIPExLogContainer::HandleResourceChange
// From CCoeControl.
// -----------------------------------------------------------------------------
// 
void CSIPExLogContainer::HandleResourceChange( TInt aType )
	{
	CCoeControl::HandleResourceChange( aType );
	
    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        TRect mainPaneRect;
        AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, 
                                           mainPaneRect );                                          
        if ( iBgContext )
            {
            iBgContext->SetRect( mainPaneRect );
            }
            
        SetRect( mainPaneRect );
        DrawNow();
        }
	}
	
// -----------------------------------------------------------------------------
// CSIPExLogContainer::MopSupplyObject
// From CCoeControl.
// -----------------------------------------------------------------------------
// 	
TTypeUid::Ptr CSIPExLogContainer::MopSupplyObject( TTypeUid aId )
    {
    if ( aId.iUid == MAknsControlContext::ETypeId ) 
        { 
        return MAknsControlContext::SupplyMopObject( aId, iBgContext );
        }
    return CCoeControl::MopSupplyObject( aId );
    }

// -----------------------------------------------------------------------------
// CSIPExLogView::ShowInfoL
// This is called from AppUi when info is received from engine
// -----------------------------------------------------------------------------
//   
void CSIPExLogContainer::ShowInfoL()
    {
    // Read info text from document and set it to editor.
    // Current way is not the most optimal.
    CEikDocument* doc = 
        static_cast< CEikAppUi* >( CEikonEnv::Static()->AppUi() )->Document();
    
    CGlobalText* globalText = iEditor->GlobalText();
    globalText->Reset();
    globalText->InsertL( 0, static_cast< CSIPExDoc* >( doc )->InfoText() );

    TCharFormat charFormat;
    TCharFormatMask charMask;
    iEditor->ApplyCharFormatL( charFormat, charMask );
    
    iEditor->MoveCursorL( TCursorPosition::EFPageDown, EFalse );
    
    DrawDeferred();
    }

// -----------------------------------------------------------------------------
// CSIPExLogView::GetEditorLAyout
//
// -----------------------------------------------------------------------------
//   
AknLayoutUtils::SAknLayoutTextMultiline CSIPExLogContainer::GetEditorLayout( 
    const TRect& aRect ) const
    {
    /*
    SAknLayoutTextMultiline struct:
    
    @param iFont      font id, ELatinPlain12, ELatinBold12, ELatinBold13 etc..
	@param iC         colour index, 0..255
	@param iL         left margin
	@param iR         right margin
	@param iB         Baseline from top of the parent rectangle
	@param iW         text width in pixels
	@param iJ         justification. ELayoutAlignNone; ELayoutAlignCenter;
	                  ELayoutAlignLeft; ELayoutAlignRight; ELayoutAlignBidi
	                  
	@param iNumberOfLinesShown
	@param iNextLineB baseline of 2nd line for multi-line labels/editors
	*/
	TInt fontHeight( FontHeightInPixels() );
	TInt fontTotalHeight( FontTotalHeightInPixels() );

	AknLayoutUtils::SAknLayoutTextMultiline editorLayout;
	editorLayout.iFont = EAknLogicalFontSecondaryFont;
    editorLayout.iC = KFontColorIndexBlue;
    editorLayout.iL = KEditorDistanceFromScreenEdge;
    editorLayout.iR = KEditorDistanceFromScreenEdge;
    editorLayout.iB = KEditorDistanceFromScreenEdge + fontHeight;
    editorLayout.iW = aRect.Width() - editorLayout.iL - editorLayout.iR;
    editorLayout.iJ = ELayoutAlignLeft;
    editorLayout.iNumberOfLinesShown = NumOfFittingEditorLines( aRect );
    editorLayout.iNextLineB = editorLayout.iB + fontTotalHeight;
    return editorLayout;
    }

// -----------------------------------------------------------------------------
// CSIPExLogView::FontHeightInPixels
//
// -----------------------------------------------------------------------------
//  
TInt CSIPExLogContainer::FontHeightInPixels() const
    {
    const CFont* font = AknLayoutUtils::FontFromId( EAknLogicalFontSecondaryFont );
    
   	_LIT( KPanicCntx, "SIP Example" );
	__ASSERT_ALWAYS( font, User::Panic( KPanicCntx(), KErrNotFound ) );
	
	return  font->HeightInPixels();
    }
    
// -----------------------------------------------------------------------------
// CSIPExLogView::FontTotalHeightInPixels
//
// -----------------------------------------------------------------------------
//  
TInt CSIPExLogContainer::FontTotalHeightInPixels() const
    {
    const CFont* font = AknLayoutUtils::FontFromId( EAknLogicalFontSecondaryFont );
    
   	_LIT( KPanicCntx, "SIP Example" );
	__ASSERT_ALWAYS( font, User::Panic( KPanicCntx(), KErrNotFound ) );
	
	return  ( font->HeightInPixels() + 
              font->AscentInPixels() + 
              font->DescentInPixels() );
    }
// -----------------------------------------------------------------------------
// CSIPExLogView::NumOfFittingLines
//
// -----------------------------------------------------------------------------
//     
TInt CSIPExLogContainer::NumOfFittingEditorLines( const TRect& aRect ) const
    {
	return aRect.Height() / FontTotalHeightInPixels();
    }
    
// End of File  
