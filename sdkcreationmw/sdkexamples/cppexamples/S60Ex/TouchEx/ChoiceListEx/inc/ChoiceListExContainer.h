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


#ifndef __CHOICELISTEX_CONTAINER_H__
#define __CHOICELISTEX_CONTAINER_H__

#include <coecntrl.h>
#include <aknchoicelist.h>


/**
 * Container class
 */

class CChoiceListExContainer : public CCoeControl, 
                               public MCoeControlObserver
    {
    
public: // constructor and destructor
    
   /*
    * C++ default constructor 
    */
    CChoiceListExContainer();
    
   /*
    * Destructor 
    */
    virtual ~CChoiceListExContainer();
    
   /*
    * Symbian 2nd constructor 
    * aParam aRect The rect for ChoiceListEx
    */
    void ConstructL( const TRect& aRect );
    
public: // new functions

   /*
    * Creates a default choice list
    */
    void CreateDefaultChoiceListL();    

   /*
    * Creates a choice list with button
    */
    void CreateButtonChoiceListL();
    
   /*
    * Creates an empty choice list and adds
    * items to it from a resource
    */
    void CreateResourceChoiceListL();
    
   /*
    * Opens the choice list 
    */
    void ShowListL();
    
   /*
    * Closes the choice list
    */
    void HideListL();
    
   /*
    * Adds an item to a choice list
    */
    void AddItemL();
    
   /*
    * Inserts an item to a choice list
    */
    void InsertItemL();
    
   /*
    * Removes an item from a choice list
    */
    void RemoveItemL();
    
   /*
    * Selects an item from a choice list
    */
    void SelectItemL();
       
   /*
    * Changes choice list flag settings ( positioning )
    * @Param aFlag New position flag of choice list popup
    */
    void ChangeListFlagsL( CAknChoiceList::TAknChoiceListFlags aFlag );
    
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
    TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,TEventCode aType );

   /* 
    * From CCoeControl, HandleResourceChange
    * Handles a change to the control's resources.
    * @param aType TA message UID value
    */
    void HandleResourceChange( TInt aType );
   
   /* 
    * From CCoeControl, HandleResourceChange
    * Responds to changes to the size and position of the contents of this control.
    */ 
    void SizeChanged();
    
public: // from MCoeControlObserver

   /*
    * From MCoeControlObserver, HandleControlEventL
    * Handles control events from choice list
    * @param aControl The control that sent the event
    * @param aEventType The event type
    */
    void HandleControlEventL( CCoeControl* aControl, TCoeEvent aEventType );

private: // Help functions

   /*
    * Calculates array resource's size
    * @param aResourceId Id of resource
    * @return Number of items in array
    */
    TInt ArraySizeL( TInt aResourceId );
         
private: // data

   /*
    * Label for showing messages
    */
    CEikLabel* iLabel; // Own
    
   /*
    *  Choice list
    */ 
    CAknChoiceList* iChoiceList; // Own

   /*
    * Background context 
    */
    CAknsBasicBackgroundControlContext* iBgContext;  // Own

   /*
    * Amount of items in choice list
    */
    TInt iArraySize;
    
   /*  
    * Selected item index
    */
    TInt iSelection;
    
    };

#endif // __CHOICELISTEX_CONTAINER_H__