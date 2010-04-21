/*
* Copyright (c) 2006 - 2007 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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
#include <aknutils.h> 		// Fonts
#include <w32std.h>			// key codes and events

#include "AudioStreamView.h"


CAudioStreamView* CAudioStreamView::NewL(const TRect & aRect, 
										 CAudioStreamEngine* aEngine)
	{
	CAudioStreamView* self = NewLC(aRect, aEngine);
	CleanupStack::Pop(self);
	return self;
	}

CAudioStreamView* CAudioStreamView::NewLC(const TRect & aRect, 
										  CAudioStreamEngine* aEngine)
	{
	CAudioStreamView* self = new (ELeave) CAudioStreamView();
	CleanupStack::PushL(self);
	self->ConstructL(aRect, aEngine);
	return self;
	}

// ----------------------------------------------------------------------------
// CAudioStreamView::ConstructL(const TRect& aRect, 
//								  CAudioStreamEngine* aEngine)
//
// Standard EPOC 2nd phase constructor
// ----------------------------------------------------------------------------
void CAudioStreamView::ConstructL(const TRect& aRect, 
								  CAudioStreamEngine* aEngine)
	{
	iEngine = aEngine;
	// create new window,
    CreateWindowL();
	// create label to display status to user
    iLabel = new (ELeave) CEikLabel;
    iLabel->SetContainerWindowL( *this );
	
		
	    iLabel->SetFont( LatinBold12() );      
	
    // set window size
    SetRect(aRect);

	_LIT(KInfo, "Audio streaming example\n");
    iLabel->SetTextL( KInfo );

    // activate window
    ActivateL();    
	}

// ----------------------------------------------------------------------------
// CAudioStreamView::CAudioStreamView()
//
// constructor
// ----------------------------------------------------------------------------
CAudioStreamView::CAudioStreamView():
    iLabel(NULL)
	{
	}

// ----------------------------------------------------------------------------
// CAudioStreamView::~CAudioStreamView()
//
// destructor
// ----------------------------------------------------------------------------
CAudioStreamView::~CAudioStreamView()
	{
    delete iLabel;
    iLabel=NULL;
	}

// ----------------------------------------------------------------------------
// CAudioStreamView::SizeChanged()
//
// called by framework when the view size is changed
// ----------------------------------------------------------------------------
void CAudioStreamView::SizeChanged()
	{
    iLabel->SetRect( Rect() );
	}

// ---------------------------------------------------------
// CAudioStreamView::HandleResourceChange()
// Called by framework when resource is changed.
// ---------------------------------------------------------
//
void CAudioStreamView::HandleResourceChange(TInt aType)
    {
    CCoeControl::HandleResourceChange(aType);

    // ADDED FOR SCALABLE UI SUPPORT
    // *****************************
    if ( aType==KEikDynamicLayoutVariantSwitch )
        {
        TRect rect;
        AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, rect);
        SetRect(rect);
        }
    }


// ----------------------------------------------------------------------------
// CAudioStreamView::CountComponentControls() const
// ----------------------------------------------------------------------------
TInt CAudioStreamView::CountComponentControls() const
	{
    return 1;	// return number of controls inside this container
	}

// ----------------------------------------------------------------------------
// CAudioStreamView::ComponentControl(TInt aIndex) const
// ----------------------------------------------------------------------------
CCoeControl* CAudioStreamView::ComponentControl(TInt aIndex) const
	{
    switch ( aIndex )
		{
        case 0:
            return iLabel;
        default:
            return NULL;
	    }
	}


// ----------------------------------------------------------------------------
// CAudioStreamView::Draw(const TRect& aRect) const
// ----------------------------------------------------------------------------
void CAudioStreamView::Draw( const TRect& aRect ) const
	{
    CWindowGc& gc = SystemGc();   
    gc.SetPenStyle( CGraphicsContext::ENullPen );
    gc.SetBrushColor( KRgbWhite );
    gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
    gc.DrawRect( aRect );
	}


// ----------------------------------------------------------------------------
// CAudioStreamView::HandleControlEventL(
//     CCoeControl* aControl,TCoeEvent aEventType)
// ----------------------------------------------------------------------------
void CAudioStreamView::HandleControlEventL(
    CCoeControl* /*aControl*/,TCoeEvent /*aEventType*/)
	{
	}


// ----------------------------------------------------------------------------
// CAudioStreamView::ShowMessageL(const TDesC& aMsg)
//
// displays status messages to user
// ----------------------------------------------------------------------------
void CAudioStreamView::ShowMessageL(const TDesC& aMsg)
	{
	iLabel->SetTextL(aMsg);
	DrawNow();
	}


// End of File  


