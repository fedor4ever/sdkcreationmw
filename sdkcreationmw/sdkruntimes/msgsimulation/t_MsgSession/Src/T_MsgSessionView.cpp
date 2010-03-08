/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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


//  Include Files

#include "T_MsgSessionView.h"
#include <T_MsgSession.rsg>               // resource ids
#include <eikenv.h>                 // CEikonEnv
#include "TestStates.h"

/****************************************************************************
*   CT_MsgSessionView
*****************************************************************************
*
*
*
****************************************************************************/
CT_MsgSessionView::CT_MsgSessionView()
    {
    }

void CT_MsgSessionView::ConstructL(const TRect& aClientRect, CTest *aTest)
    {
    CreateWindowL();
    Window().SetShadowDisabled(ETrue);
    SetRect(aClientRect);

    // white background using MCoeControlBrushContext
    iContext = this;
    iBrushStyle = CGraphicsContext::ESolidBrush;
    iBrushColor = KRgbWhite;
    SetBlank();

    //
    // create controls here
    //

    SetFocus(ETrue);
    ActivateL();
    }

CT_MsgSessionView::~CT_MsgSessionView()
    {
    //
    // delete controls here
    //
    }

TKeyResponse CT_MsgSessionView::OfferKeyEventL(const TKeyEvent& /*aKeyEvent*/
                                       , TEventCode /*aType*/)
    {
    //
    // key event processing goes here
    //
    return EKeyWasNotConsumed;
    }

TSize CT_MsgSessionView::MinimumSize()
    {
    //
    // set minumun size for the view
    //
    return Size();
    }

void CT_MsgSessionView::FocusChanged(TDrawNow /*aDrawNow*/)
    {
    //
    // called when application gains or loses focus
    //
    }

void CT_MsgSessionView::SizeChanged()
    {
    //
    // called when view size is changed, that is when SetExtent(), SetSize(),
    // SetRect(), SetCornerAndSize(), or SetExtentToWholeScreen() are called
    //
    }

void CT_MsgSessionView::PositionChanged()
    {
    //
    // called when view position is changed
    //
    }

TInt CT_MsgSessionView::CountComponentControls() const
    {
    return 0;               // return the number of controls
    }

CCoeControl* CT_MsgSessionView::ComponentControl(TInt aIndex) const
    {
    switch (aIndex)
        {
    case 0:             // return 1st control
        break;

    default:
        break;
        }
    return NULL;            // no such control
    }

void CT_MsgSessionView::Draw(const TRect& /*aRect*/) const
    {
    CWindowGc &gc = SystemGc();     // get graphics context
    gc.SetPenStyle(CGraphicsContext::ESolidPen);
    //
    // add code here
    //
    }
// End of file
