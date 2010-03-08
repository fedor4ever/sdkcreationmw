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



#ifndef __STYLUSPOPUPMENUEX_APPUI_H__
#define __STYLUSPOPUPMENUEX_APPUI_H__

// INCLUDES
#include <aknappui.h>
#include <eikmobs.h>
// FORWARD DECLARATIONS
class CStylusPopupMenuExContainer;

// CLASS DECLARATION
/**
 * CStylusPopupMenuExAppUi application UI class.
 * Interacts with the user through the UI and request message processing
 * from the handler class
 */
class CStylusPopupMenuExAppUi : public CAknAppUi
    {
public:

   /**
    * ConstructL.
    * 2nd phase constructor.
    */
    void ConstructL();

   /**
    * CStylusPopupMenuExAppUi.
    * C++ default constructor. This needs to be public due to
    * the way the framework constructs the AppUi
    */
    CStylusPopupMenuExAppUi();

   /**
    * ~CStylusPopupMenuExAppUi.
    * Virtual Destructor.
    */
    virtual ~CStylusPopupMenuExAppUi();

public: // from CEikAppui

   /**
    * From CEikAppUi, HandleCommandL.
    * Takes care of command handling.
    * @param aCommand Command to be handled.
    */
    void HandleCommandL( TInt aCommand );
    
    void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane *aMenuPane);
private: // data

   /*
    * Pointer to the container
    * own
    */
    CStylusPopupMenuExContainer* iAppContainer;

    /**
    * From CEikAppUi, HandleResourceChangeL.
    * Handles resource changes.
    * @param aType Resource change type to be handled
    */
    //void HandleResourceChangeL( TInt aType );    
        
    };
    
#endif // __STYLUSPOPUPMENUEX_APPUI_H__

