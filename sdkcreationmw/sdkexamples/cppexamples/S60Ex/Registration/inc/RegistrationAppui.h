/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef REGISTRATIONAPPUI_H
#define REGISTRATIONAPPUI_H

// INCLUDE FILES
#include <aknviewappui.h>
#include <avkon.hrh>
#include <aknnotewrappers.h>

#include <Registration.rsg>
#include "Registration.hrh"

// FORWARD DECLARATIONS
class CRegistrationView;

// CONSTANTS
const TInt KMaxNoteLength = 64;

// CLASS DECLARATION

/**
* Application UI class.
* Provides support for the following features:
* - EIKON control architecture
*/
class CRegistrationAppUi : public CAknViewAppUi
    {
    public: // Constructors and destructor

        /**
        * Symbian default constructor.
        */      
        void ConstructL();

        /**
        * Destructor.
        */      
        ~CRegistrationAppUi();
        
    public: // New functions

    public: // Functions from base classes

    private: // From MEikMenuObserver
        /**
        * This function is called by the EIKON framework just before 
        * it displays a menu pane.
        */
        void DynInitMenuPaneL( TInt aResourceId,CEikMenuPane* aMenuPane );

    private: // From CEikAppUi
        /**
        * Takes care of command handling.
        * @param aCommand command to be handled
        */
        void HandleCommandL( TInt aCommand );
        
        /**
        * Called by framework when layout is changed.
        * @param aType Type of resources that have changed
        */
        virtual void HandleResourceChangeL( TInt aType );

        /**
        * Handles key events.
        * @param aKeyEvent Event to handled
        * @param aType Type of the key event
        * @return Response code (EKeyWasConsumed, EKeyWasNotConsumed)
        */
        TKeyResponse HandleKeyEventL(
            const TKeyEvent& aKeyEvent,TEventCode aType );     

    private: // Data   
    
    CRegistrationView*          iAppView;
    TBuf<KMaxNoteLength>        iNote;
  
    };

#endif // REGISTRATIONAPPUI_H

// End of File
