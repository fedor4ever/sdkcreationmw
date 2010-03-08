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
#include <AknsUtils.h>
#include <avkon.hrh>
#include <eiklabel.h>
#include "LmUiRefAppUi.h"
#include <lmuirefapp.rsg>
#include "lmuirefapp.hrh"
#include "LmUiRefAppView.h"
#include "LmUiRefAppContainer.h"

_LIT(KLmUiRefApp, "Landmarks UI Reference");
_LIT(KTextApp, "Application!");

const TInt KLable1X(10);
const TInt KLable1Y(50);
const TInt KLable2X(60);
const TInt KLable2Y(80);

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------------------------
// CLmUiRefAppContainer::NewL
// ----------------------------------------------------------------------------
//
CLmUiRefAppContainer* CLmUiRefAppContainer::NewL(
        MObjectProvider* aParent,
	const TRect& aRect )
    {
    CLmUiRefAppContainer* self =
    new( ELeave ) CLmUiRefAppContainer();
    CleanupStack::PushL( self );
    self->SetMopParent( aParent );
    self->ConstructL( aRect );
    CleanupStack::Pop(self ); // self
    return self;
    }

// ----------------------------------------------------------------------------
// CLmUiRefAppContainer::ConstructL 
// ----------------------------------------------------------------------------
void CLmUiRefAppContainer::ConstructL(const TRect& aRect)
    {
    CreateWindowL();
    
    iLabel = new (ELeave) CEikLabel;
    iLabel->SetContainerWindowL(*this);    
    iLabel->SetTextL(KLmUiRefApp);
    iToDoLabel = new (ELeave) CEikLabel;
    iToDoLabel->SetContainerWindowL( *this );        
    iToDoLabel->SetTextL(KTextApp);
    SetRect(aRect);
    ActivateL();
    }

// ----------------------------------------------------------------------------
// CLmUiRefAppContainer::CLmUiRefAppContainer() 
// ----------------------------------------------------------------------------
CLmUiRefAppContainer::CLmUiRefAppContainer()
    {
    }

// ----------------------------------------------------------------------------
// CLmUiRefAppContainer::~CLmUiRefAppContainer() 
// ----------------------------------------------------------------------------
CLmUiRefAppContainer::~CLmUiRefAppContainer()
    {
    delete iLabel;
    delete iToDoLabel;
    }

// ---------------------------------------------------------
// CLmUiRefAppContainer::SizeChanged() 
// Called by framework when the view size is changed
// ---------------------------------------------------------
//
void CLmUiRefAppContainer::SizeChanged()
    {
    // TODO: Add here control resize code etc.    
    iLabel->SetExtent(TPoint(KLable1X, KLable1Y), iLabel->MinimumSize());
    iToDoLabel->SetExtent(TPoint(KLable2X, KLable2Y), iToDoLabel->MinimumSize());     
    }

// ---------------------------------------------------------
// CLmUiRefAppContainer::CountComponentControls() const 
// return number of controls inside this container
// ---------------------------------------------------------
//
TInt CLmUiRefAppContainer::CountComponentControls() const
    {
    return 2; // return nbr of controls inside this container
    }

// ---------------------------------------------------------
// CLmUiRefAppContainer::ComponentControl(TInt aIndex) const 
// return control inside the container
// ---------------------------------------------------------
//
CCoeControl* CLmUiRefAppContainer::ComponentControl(TInt aIndex) const
    {
    switch ( aIndex )
        {
        case 0:
            return iLabel;
        case 1:
            return iToDoLabel;
        default:
            return NULL;
        }
    }

// ---------------------------------------------------------
//  CLmUiRefAppContainer::Draw(Trect& aRect) 
// ---------------------------------------------------------
//
void CLmUiRefAppContainer::Draw(const TRect& /*aRect*/) const
    {
    // Get the standard graphics context 
    CWindowGc& gc = SystemGc();   
    // TODO: Add your drawing code here
    // example code...
    gc.SetPenStyle(CGraphicsContext::ENullPen);
    gc.SetBrushColor(KRgbGray);
    gc.SetBrushStyle(CGraphicsContext::ESolidBrush);
    TRect rect = Rect();
    gc.DrawRect(rect);
    }

// End of File  
