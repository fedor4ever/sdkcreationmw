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
* Description:  Container for second view(CMyViewView2), does all drawing for
*                second view
*
*/



// INCLUDE FILES
#include "MyViewContainer2.h"

#include <eiklabel.h>  // for example label control

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMyViewContainer2::ConstructL(const TRect& aRect)
// EPOC two phased constructor
// ---------------------------------------------------------
//
void CMyViewContainer2::ConstructL(const TRect& aRect)
    {
    CreateWindowL();

    iLabel = new (ELeave) CEikLabel;
    iLabel->SetContainerWindowL( *this );
    iLabel->SetTextL( _L("MyView 2\n\nSelect external view\nactivation from menu") );

    SetRect(aRect);
    ActivateL();
    }

// Destructor
CMyViewContainer2::~CMyViewContainer2()
    {
    delete iLabel;
    }

// ---------------------------------------------------------
// CMyViewContainer2::SizeChanged()
// Called by framework when the view size is changed
// ---------------------------------------------------------
//
void CMyViewContainer2::SizeChanged()
    {
    // TODO: Add here control resize code etc.
    iLabel->SetExtent( TPoint(10,10), iLabel->MinimumSize() );
    }

// ---------------------------------------------------------
// CMyViewContainer2::CountComponentControls() const
// ---------------------------------------------------------
//
TInt CMyViewContainer2::CountComponentControls() const
    {
    return 1; // return nbr of controls inside this container
    }

// ---------------------------------------------------------
// CMyViewContainer2::ComponentControl(TInt aIndex) const
// ---------------------------------------------------------
//
CCoeControl* CMyViewContainer2::ComponentControl(TInt aIndex) const
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
// CMyViewContainer2::Draw(const TRect& aRect) const
// ---------------------------------------------------------
//
void CMyViewContainer2::Draw(const TRect& aRect) const
    {
    CWindowGc& gc = SystemGc();

    gc.SetPenStyle(CGraphicsContext::ENullPen);
    gc.SetBrushColor(KRgbBlue);
    gc.SetBrushStyle(CGraphicsContext::ESolidBrush);
    gc.DrawRect(aRect);
    }

// ---------------------------------------------------------
// CMyViewContainer2::HandleControlEventL(
//     CCoeControl* aControl,TCoeEvent aEventType)
// ---------------------------------------------------------
//
void CMyViewContainer2::HandleControlEventL(
    CCoeControl* /*aControl*/,TCoeEvent /*aEventType*/)
    {
    // Add your control event handler code here
    }

// End of File  
