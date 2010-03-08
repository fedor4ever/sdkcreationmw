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

#ifndef __T_MSGSESSIONVIEW_H__
#define __T_MSGSESSIONVIEW_H__

//  Include Files

#include <coecntrl.h>       // CCoeControl
#include <coeccntx.h>       // MCoeControlBrushContext

class CTest;

//  Class Definitions

class CT_MsgSessionView : public CCoeControl, public MCoeControlBrushContext
    {
public:     
    // new functions
    CT_MsgSessionView();
    void ConstructL(const TRect& aClientRect, CTest *aTest);
    ~CT_MsgSessionView();

public:     
    // from CCoeControl
    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
    TSize MinimumSize();

protected:  
    // from CCoeControl
    void FocusChanged(TDrawNow aDrawNow);
    void SizeChanged();
    void PositionChanged();
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl(TInt aIndex) const;

private:    
    // from CCoeControl
    void Draw(const TRect& aRect) const;

private:    
    // data
    //
    // add controls here
    //
    };


#endif  // __T_MSGSESSIONVIEW_H__

// End of file
