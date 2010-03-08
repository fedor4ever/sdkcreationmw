/*
* Copyright (c) 2002 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declares UI class for application
*
*/


#ifndef MYVIEWAPPUI_H
#define MYVIEWAPPUI_H

// INCLUDES
#include <eikapp.h>
#include <eikdoc.h>
#include <e32std.h>
#include <coeccntx.h>
#include <aknviewappui.h>       // View AppUi adds View handling to AppUi
#include <akntabgrp.h>          // For handling tabs
#include <aknnavide.h>

// FORWARD DECLARATIONS
class CMyViewView;
class CMyViewView2;  
// CLASS DECLARATION

/**
* Application UI class. CAknViewAppUi derives from CAknAppUi and has view handling methods added.
* Provides support for the following features:
* - EIKON control architecture
* - view architecture
* - status pane
* 
*/
class CMyViewAppUi : public CAknViewAppUi
    {
    public: // // Constructors and destructor

        /**
        * EPOC default constructor.
        */      
        void ConstructL();

        /**
        * Destructor.
        */      
        ~CMyViewAppUi();
        
    private:
        // From MEikMenuObserver
        // With this, we can dynamically manipulate menu items.
        void DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane);

    private:
        /**
        * From CEikAppUi, takes care of command handling.
        * @param aCommand command to be handled
        */
        void HandleCommandL(TInt aCommand);

        /**
        * From CEikAppUi, handles key events.
        * @param aKeyEvent Event to handled.
        * @param aType Type of the key event. 
        * @return Response code (EKeyWasConsumed, EKeyWasNotConsumed). 
        */
        virtual TKeyResponse HandleKeyEventL(
            const TKeyEvent& aKeyEvent,TEventCode aType);

        void HandleForegroundEventL(TBool aForeground);

	    /**
	    * From CEikAppUi, handles key events.
	    * @param aType The type of resources that have changed
	    */    
	    virtual void HandleResourceChangeL( TInt aType );  

    private: //Data
        CAknNavigationControlContainer* iNaviPane;
        CAknTabGroup*                   iTabGroup;
        CAknNavigationDecorator*        iDecoratedTabGroup;
		TUid 							iViewId1;
		TUid 							iViewId2;
    };

#endif

// End of File
