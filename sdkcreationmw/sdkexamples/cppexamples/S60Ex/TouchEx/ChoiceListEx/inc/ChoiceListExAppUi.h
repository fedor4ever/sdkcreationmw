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



#ifndef __CHOICELISTEX_APPUI_H__
#define __CHOICELISTEX_APPUI_H__

// INCLUDES
#include <aknappui.h>

// FORWARD DECLARATIONS
class CChoiceListExContainer;

// CLASS DECLARATION
/**
 * CChoiceListExAppUi application UI class.
 * Interacts with the user through the UI and request message processing
 * from the handler class
 */
class CChoiceListExAppUi : public CAknAppUi
    {
public:

   /**
    * ConstructL.
    * 2nd phase constructor.
    */
    void ConstructL();

   /**
    * CChoiceListExAppUi.
    * C++ default constructor. This needs to be public due to
    * the way the framework constructs the AppUi
    */
    CChoiceListExAppUi();

   /**
    * ~CChoiceListExAppUi.
    * Virtual Destructor.
    */
    virtual ~CChoiceListExAppUi();

public: // from CEikAppui

   /**
    * From CEikAppUi, HandleCommandL.
    * Takes care of command handling.
    * @param aCommand Command to be handled.
    */
    void HandleCommandL( TInt aCommand );

public: // from MEikMenuObserver

   /*
    * From MEikMenuObserver, DynInitMenuPaneL
    * Called by the framework to update options menu
    * @param aResourceId The ID of the menu pane.
    * @param aMenuPane The menu pane itself.
    */
    void DynInitMenuPaneL( TInt aMenuId, CEikMenuPane* aMenuPane );    
    
private: // data

   /*
    * Pointer to the container
    * own
    */
    CChoiceListExContainer* iAppContainer;
    
   /*
    * Flags for options menu updating
    */
    TBitFlags  iMenuFlags;
        
    };


#endif // __CHOICELISTEX_APPUI_H__

