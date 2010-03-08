/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declares custom control class "Ball".
*
*/


#ifndef C_HAPTICSEXAMPLEBALL_H
#define C_HAPTICSEXAMPLEBALL_H

#include <coecntrl.h>

/**
* CHapticsExampleBall control class.
*
*/
class CHapticsExampleBall : public CCoeControl, MCoeControlHitTest
    {
public:

    /**
     * First phase of two-phased constructor.
     * @return Instance of CHapticsExampleBall.
     */
    static CHapticsExampleBall* NewL();

    /**
     * Virtual destructor.
     */
    virtual ~CHapticsExampleBall();

public:
    /**
     * From CCoeControl.
     * Draw this CHapticsExampleBall control to the screen.
     * @param aRect the rectangle of this control that needs updating
     */
    void Draw( const TRect& aRect ) const;

public:
    /**
     * From MCoeControlHitTest.
     * Tests whether a pointer event occurred inside the control's hit region
     * @param aPoint The position of the pointer event
     * @param aControl ignored
     * @return true if point inside controls area
     */
    TBool HitRegionContains( const TPoint& aPoint, 
                             const CCoeControl& aControl ) const;


private:

    /** 
     * Constructor
     */
    CHapticsExampleBall();

    /**
     * Second phase constructor.
     */
    void ConstructL();
    };

#endif // C_HAPTICSEXAMPLEBALL_H

// End of File
