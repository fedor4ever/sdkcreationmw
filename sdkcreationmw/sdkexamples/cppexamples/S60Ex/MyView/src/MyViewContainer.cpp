/*
* Copyright (c) 2002 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Container of CMyViewView view, does all the drawing for CMyViewView
*
*/


// INCLUDE FILES
#include "MyViewContainer.h"

#include <eiklabel.h>  // for example label control

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMyViewContainer::ConstructL(const TRect& aRect)
// Two phased constructor
// ---------------------------------------------------------
//
void CMyViewContainer::ConstructL(const TRect& aRect)
    {
    CreateWindowL();

    iLabel = new (ELeave) CEikLabel;
    iLabel->SetContainerWindowL( *this );
    iLabel->SetTextL( _L("MyView 1\n\nSelect local view\nswitching from menu") );

    SetRect(aRect);
    ActivateL();
    }

// Destructor
CMyViewContainer::~CMyViewContainer()
    {
    delete iLabel;
    }

// ---------------------------------------------------------
// CMyViewContainer::SizeChanged()
// Called by framework when the view size is changed
// ---------------------------------------------------------
//
void CMyViewContainer::SizeChanged()
    {
    // Control resize code
    iLabel->SetExtent( TPoint(10,10), iLabel->MinimumSize() );
    }

// ---------------------------------------------------------
// CMyViewContainer::CountComponentControls() const
// ---------------------------------------------------------
//
TInt CMyViewContainer::CountComponentControls() const
    {
    return 1; // return nbr of controls inside this container
    }

// ---------------------------------------------------------
// CMyViewContainer::ComponentControl(TInt aIndex) const
// ---------------------------------------------------------
//
CCoeControl* CMyViewContainer::ComponentControl(TInt aIndex) const
    {
    switch ( aIndex )
        {
        case 0:
            return iLabel;
        default:
            return NULL;
        }
    }

// ---------------------------------------------------------
// CMyViewContainer::Draw(const TRect& aRect) const
// ---------------------------------------------------------
//
void CMyViewContainer::Draw(const TRect& aRect) const
    {
    CWindowGc& gc = SystemGc();
    gc.SetPenStyle(CGraphicsContext::ENullPen);
    gc.SetBrushColor(KRgbBlue);
    gc.SetBrushStyle(CGraphicsContext::ESolidBrush);
    gc.DrawRect(aRect);
    }

// ---------------------------------------------------------
// CMyViewContainer::HandleControlEventL(
//     CCoeControl* aControl,TCoeEvent aEventType)
// ---------------------------------------------------------
//
void CMyViewContainer::HandleControlEventL(
    CCoeControl* /*aControl*/,TCoeEvent /*aEventType*/)
    {
    // TODO: Add your control event handler code here
    }

// End of File  
