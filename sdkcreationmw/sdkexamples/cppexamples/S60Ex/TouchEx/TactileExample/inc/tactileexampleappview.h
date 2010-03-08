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
* Description:  Declares the container controller for the Tactile Example
*
*/


#ifndef __TACTILEEXAMPLEAPPVIEW_H__
#define __TACTILEEXAMPLEAPPVIEW_H__

// INCLUDES
#include <coecntrl.h>
#include "tactileexamplesquare.h"
#include "tactileexamplecircle.h"

// FORWARD DECLARATIONS
class CAknButton;
class MTouchFeedback;

// CLASS DECLARATION
/**
* CTactileExampleAppView application view class.
*
*/
class CTactileExampleAppView : public CCoeControl, MCoeControlObserver
    {
public: // New methods

    /**
     * NewL.
     * Two-phased constructor.
     * Create a CTactileExampleAppView object, which will draw itself to aRect.
     * @param aRect The rectangle this view will be drawn to.
     * @return a pointer to the created instance of CTactileExampleAppView.
     */
    static CTactileExampleAppView* NewL( const TRect& aRect );

    /**
     * NewLC.
     * Two-phased constructor.
     * Create a CTactileExampleAppView object, which will draw itself
     * to aRect.
     * @param aRect Rectangle this view will be drawn to.
     * @return A pointer to the created instance of CTactileExampleAppView.
     */
    static CTactileExampleAppView* NewLC( const TRect& aRect );

    /**
     * ~CTactileExampleAppView
     * Virtual Destructor.
     */
    virtual ~CTactileExampleAppView();



// from base class CCoeControl

    /**
     * From CoeControl.
     * @return Nbr of controls inside this container.
     */
    TInt CountComponentControls() const;

    /**
     * From CCoeControl.
     * @param Index number of the control.
     * @return Pointer of controls inside this container.
     */
    CCoeControl* ComponentControl( TInt aIndex ) const;

    /**
     * From CCoeControl.
     * Draw this CTactileExampleAppView to the screen.
     * @param aRect the rectangle of this view that needs updating
     */
    void Draw( const TRect& aRect ) const;

    /**
     * From CoeControl.
     * Called by framework when the view size is changed.
     */
    void SizeChanged();

    /**
     * From CCoeControl.
     * @param aPointerEvent The pointer event.
     */
    void HandlePointerEventL( const TPointerEvent& aPointerEvent );

    /**
     * From CCoeControl.
     * @param aType The type of resources that have changed.
     */
    void HandleResourceChange( TInt aType );


// from base class MCoeControlObserver

    /**
     * From MCoeControlObserver.
     * Handles an event of type aEventType
     * @param aControl Pointer to component control
     * @param aEventType Event code
     */
    void HandleControlEventL( CCoeControl* aControl, TCoeEvent aEventType );



private:

    /**
     * CTactileExampleAppView.
     * C++ default constructor.
     */
    CTactileExampleAppView();

    /**
     * ConstructL

     * 2nd phase constructor.
     * Perform the second phase construction of a
     * CTactileExampleAppView object.
     * @param aRect The rectangle this view will be drawn to.
     */
    void ConstructL( const TRect& aRect );

    /**
     * SetObjects
     * Sets the circle and square objects to default position
     */
    void SetObjects();

    /**
     * Move
     * Moves the control snapping to imaginary grid lines
     * @param aControl Control which is moved.
     * @param aPoint Point where control is moved to.
     */
    void Move( CCoeControl* aControl, TPoint aPoint );

    /**
     * ObjectDimension
     * Calculates object's side length related to screen size
     */
    void ObjectDimension();



private: // data

    /**
     * Offset from the object's top left corner to the point of touch
     */
    TPoint  iDraggingOffset;

    /**
     * Area where dragging is possible
     */
    TRect  iDragArea;

    /**
     * Flag indicating circle is dragged
     */
    TBool  iDragCircle;

    /**
     * Flag indicating square is dragged
     */
    TBool  iDragSquare;

    /**
     * Size of objects related to screen size
     */
    TInt  iObjectDimension;

    /**
     * Circle object
     * Own.
     */
    CTactileExampleCircle* iCircle;

    /**
     * Square object
     * Own.
     */
    CTactileExampleSquare* iSquare;

    /**
     * Reset button
     * Own.
     */
    CAknButton*  iButton;

    /**
     * Pointer to touch feedback instance
     * Not own.
     */
    MTouchFeedback* iFeedback;
    };

#endif // __TACTILEEXAMPLEAPPVIEW_H__

// End of File
