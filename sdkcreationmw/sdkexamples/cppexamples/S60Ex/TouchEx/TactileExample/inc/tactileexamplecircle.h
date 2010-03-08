/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declares the custom control
*
*/


#ifndef __TACTILEEXAMPLECIRCLE_H__
#define __TACTILEEXAMPLECIRCLE_H__

// INCLUDE FILES
#include <coecntrl.h>

// CLASS DECLARATION
/**
* CTactileExampleCircle control class.
*
*/
class CTactileExampleCircle : public CCoeControl, MCoeControlHitTest
    {
public:

    /**
     * First phase of two-phased constructor
     */
    static CTactileExampleCircle* NewL(
        const TRect& aRect, const CCoeControl* aParent );

    /**
     * Destructor.
     */
    virtual ~CTactileExampleCircle();

    /**
     * Enables/disables tactile feedback for this control
     * @param aEnabled Use ETrue to enable feedback and EFalse
     * to disable feedback
     */
    void EnableFeedback( TBool aEnabled );


// from base class CCoeControl

    /**
     * From CCoeControl.
     * Draw this CTactileExampleCircle control to the screen.
     * @param aRect the rectangle of this control that needs updating
     */
    void Draw( const TRect& aRect ) const;

    /**
     * From CCoeControl.
     * @param aPointerEvent The pointer event.
     */
    void HandlePointerEventL( const TPointerEvent& aPointerEvent );


// from base class MCoeControlHitTest

    /**
     * From MCoeControlHitTest.
     * Tests whether a pointer event occurred inside the control's hit region
     * @param aPoint The position of the pointer event
     * @param aControl
     */
    TBool HitRegionContains( const TPoint& aPoint, const CCoeControl& /*aControl*/ ) const;


private:

    CTactileExampleCircle();

    /**
     * Second phase of constructor.
     */
    void ConstructL( const TRect& aRect, const CCoeControl* aParent );
    };

#endif // __TACTILEEXAMPLECIRCLE_H__

// End of File
