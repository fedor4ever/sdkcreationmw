/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implements CommonDialogsEx container.
*
*/


// INCLUDE FILES
#include "CommonDialogsExContainer.h"

#include <eiklabel.h>  // for example label control
#include <aknlists.h>

_LIT( KCommonDialogsExTitle, "Example View" );
_LIT( KCommonDialogsExGuide, "Press Options to start" );
// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CCommonDialogsExContainer::ConstructL(const TRect& aRect)
// EPOC two phased constructor
// ---------------------------------------------------------
//
void CCommonDialogsExContainer::ConstructL(const TRect& aRect)
    {
    CreateWindowL();
    
    iLabel = new (ELeave) CEikLabel;
    iLabel->SetContainerWindowL( *this );
    iLabel->SetTextL( KCommonDialogsExTitle );

    iToDoLabel = new (ELeave) CEikLabel;
    iToDoLabel->SetContainerWindowL( *this );
    iToDoLabel->SetTextL( KCommonDialogsExGuide );


    SetRect(aRect);
    ActivateL();
    }

// Destructor
CCommonDialogsExContainer::~CCommonDialogsExContainer()
    {
    delete iLabel;
    delete iToDoLabel;
    }

// ---------------------------------------------------------
// CCommonDialogsExContainer::SizeChanged()
// Called by framework when the view size is changed
// ---------------------------------------------------------
//
void CCommonDialogsExContainer::SizeChanged()
    {
    iLabel->SetExtent( TPoint(10,10), iLabel->MinimumSize() );
    iToDoLabel->SetExtent( TPoint(10,90), iToDoLabel->MinimumSize() );
    }

// ---------------------------------------------------------
// CCommonDialogsExContainer::CountComponentControls() const
// ---------------------------------------------------------
//
TInt CCommonDialogsExContainer::CountComponentControls() const
    {
    return 2; // return nbr of controls inside this container
    }

// ---------------------------------------------------------
// CCommonDialogsExContainer::ComponentControl(TInt aIndex) const
// ---------------------------------------------------------
//
CCoeControl* CCommonDialogsExContainer::ComponentControl(TInt aIndex) const
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
// CCommonDialogsExContainer::Draw(const TRect& aRect) const
// ---------------------------------------------------------
//
void CCommonDialogsExContainer::Draw(const TRect& aRect) const
    {
    CWindowGc& gc = SystemGc();
    gc.SetPenStyle(CGraphicsContext::ENullPen);
    gc.SetBrushColor(KRgbGray);
    gc.SetBrushStyle(CGraphicsContext::ESolidBrush);
    gc.DrawRect(aRect);
    }

// ---------------------------------------------------------
// CCommonDialogsExContainer::HandleControlEventL(
//     CCoeControl* aControl,TCoeEvent aEventType)
// ---------------------------------------------------------
//
void CCommonDialogsExContainer::HandleControlEventL(
    CCoeControl* /*aControl*/,TCoeEvent /*aEventType*/)
    {

    }

// ---------------------------------------------------------
// CCommonDialogsExContainer::ShowTextL
// Show dynamic text;
// ---------------------------------------------------------
//
void CCommonDialogsExContainer::ShowTextL(
    const TDesC& aText, const TDesC& aTextSub)
    {
    iLabel->SetTextL(aText);
    iToDoLabel->SetTextL(aTextSub);
    SizeChanged();
    DrawDeferred();
    }

// ----------------------------------------------------------------------------
// void CAknAtSetPageContainer::HandleResourceChange(TInt)
// Makes dynamic resolution change smoother
// ----------------------------------------------------------------------------
//
void CCommonDialogsExContainer::HandleResourceChange(TInt aType)
    {
    if (aType == KEikDynamicLayoutVariantSwitch)
        {
        SetRect( iAvkonAppUi->ClientRect());
        }
    CCoeControl::HandleResourceChange( aType );
    }

// End of File
