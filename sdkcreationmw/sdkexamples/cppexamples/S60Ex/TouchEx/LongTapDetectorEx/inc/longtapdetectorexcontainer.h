/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __BUTTONEX_CONTAINER_H__
#define __BUTTONEX_CONTAINER_H__

#include <coecntrl.h>
#include <aknlongtapdetector.h>

/**
 * Container class
 */

class CLongTapDetectorExContainer : public CCoeControl, 
                           public MAknLongTapDetectorCallBack
    {
    
public: // constructor and destructor
    
   /*
    * C++ default constructor 
    */
    CLongTapDetectorExContainer();
    
   /*
    * Destructor 
    */
    virtual ~CLongTapDetectorExContainer();
    
   /*
    * Symbian 2nd constructor 
    * aParam aRect The rect for LongTapDetector example
    */
    void ConstructL( const TRect& aRect );
    
public: // from CCoeControl
    
   /*
    * From CCoeControl, Draw
    * Draws the application area 
    * @param aRect Region of the control to be drawn
    */
    void Draw( const TRect& aRect ) const;

   /*
    * From CCoeControl, CountcomponentControls
    * Returns the number of controls 
    * @return Number of controls
    */
    TInt CountComponentControls() const;
    
   /*
    * From CCoeControl, ComponentControl
    * Returns pointer to a particular component
    * @param aIndex Index of the control
    * @return Pointer to component control identified by index
    */
    CCoeControl* ComponentControl( TInt aIndex ) const;
    
   /* 
    * From CCoeControl, HandleResourceChange
    * Responds to changes to the size and position of the contents of this control.
    */ 
    void SizeChanged();
    
   /* 
    * From CCoeControl, HandleResourceChange
    * This function gets called whenever a pointer event occurs.
    * @param aEvent The pointer event.
    */
    void HandleResourceChange( TInt aType );

    /* 
    * From CCoeControl, HandlePointerEventL
    * Handles a change to the control's resources.
    * @param aType TA message UID value
    */
    void HandlePointerEventL( const TPointerEvent& aEvent );
public: // from MAknLongTapDetectorCallBack
    void HandleLongTapEventL( const TPoint& aPenEventLocation, 
                                      const TPoint& aPenEventScreenLocation );
        
private: // data

   /*
    * Label for showing messages
    */
    CEikLabel* iLabel; // Own
    
   /*
    * Background context 
    */
    CAknsBasicBackgroundControlContext* iBgContext;  // Own

    CAknLongTapDetector* iLongTapDetector;
    
    TBool iLongTapCallbackReceived;
    };

#endif // __BUTTONEX_CONTAINER_H__