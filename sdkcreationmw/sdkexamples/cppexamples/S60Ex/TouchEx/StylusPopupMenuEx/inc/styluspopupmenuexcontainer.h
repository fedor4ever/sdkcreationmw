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


#ifndef __STYLUSPOPUPMENUEX_CONTAINER_H__
#define __STYLUSPOPUPMENUEX_CONTAINER_H__

#include <coecntrl.h>
#include <eikmobs.h>
#include <aknstyluspopupmenu.h>

const TPoint KPopupPoint( 50, 50 );
/**
 * Container class
 */

class CStylusPopupMenuExContainer : public CCoeControl, 
                           //public MCoeControlObserver, 
                           public MEikMenuObserver
    {
    
public: // constructor and destructor
    
   /*
    * C++ default constructor 
    */
    CStylusPopupMenuExContainer();
    
   /*
    * Destructor 
    */
    virtual ~CStylusPopupMenuExContainer();
    
   /*
    * Symbian 2nd constructor 
    * aParam aRect The rect for StylusPopupMenu example
    */
    void ConstructL( const TRect& aRect );
    
public: // new functions

    void CreatePopupMenuL( const TPoint &aPosition = KPopupPoint );
    void CreatePopupMenuFromResourceL( const TPoint &aPosition = KPopupPoint );
    void UseResourceForPopupMenuL( TBool aValue );
    //void UseCodeForPopupMenu();
    void UpdateMarkableMenus( CEikMenuPane *aMenuPane );
    
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
    * From CCoeControl, OfferKeyEventL
    * Handles key events from the framework
    * @param aKeyEvent the key event    
    * @param aType The type of key event
    * @return Indicates whether or not the keyevent was used
    */
    //TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,TEventCode aType );
    
   /* 
    * From CCoeControl, HandleResourceChange
    * Responds to changes to the size and position of the contents of this control.
    */ 
    void SizeChanged();
    
   /* 
    * From CCoeControl, HandleResourceChange
    * Handles a change to the control's resources.
    * @param aType TA message UID value
    */
    void HandleResourceChange( TInt aType );
    
    void HandlePointerEventL( const TPointerEvent &aPointerEvent );

public: // from MCoeControlObserver

   /*
    * From MCoeControlObserver, HandleControlEventL
    * Handles control events from StylusPopupMenus
    * @param aControl The control that sent the event
    * @param aEventType The event type
    */
   // void HandleControlEventL( CCoeControl* aControl, TCoeEvent aEventType );

public: // from MEikMenuObserver
    //void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane *aMenuPane);
    void ProcessCommandL(TInt aCommandId);
    void SetEmphasis(CCoeControl* /*aMenuControl*/,TBool /*aEmphasis*/)
        {
        }
private: // Help functions


         
private: // data

   /*
    * Label for showing messages
    */
    CEikLabel* iLabel; // Own
    
   /*
    * Background context 
    */
    CAknsBasicBackgroundControlContext* iBgContext;  // Own

    CAknStylusPopUpMenu* iPopup;
    
    TBool iUseResource;
    };

#endif // __STYLUSPOPUPMENUEX_CONTAINER_H__