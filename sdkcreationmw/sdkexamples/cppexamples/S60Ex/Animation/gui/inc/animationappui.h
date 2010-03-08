/*
* Copyright (c) 2004 - 2006 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __ANIMATIONAPPUI_H__
#define __ANIMATIONAPPUI_H__

// INCLUDES
#include <aknappui.h>

#include "clientdll.h"
#include "clientimagecommander.h"

// CONSTANTS
// The location of the animation server changes in the emulator
// and target builds.
#if defined( __WINS__ )
_LIT( KServerDllFilename, "Z:\\system\\apps\\animation\\ANIMATION_SERVER" );
#else
// drive-neutral path (MMC-installations safe)
_LIT( KServerDllFilename, "\\system\\apps\\animation\\ANIMATION_SERVER" );
#endif

// FORWARD DECLARATIONS
class CAnimationAppView;

// CLASS DECLARATION

/**
* CAnimationAppUi application UI class.
* An instance of class CAnimationAppUi is the UserInterface part of the AVKON
* application framework for the Animation example application.
*/
class CAnimationAppUi : public CAknAppUi
    {
    public: // Constructors and destructor

        /**
        * CAnimationAppUi.
        * C++ default constructor. This needs to be public due to
        * the way the framework constructs the AppUi.
        * @param aSession the window server session to use.
        */
        CAnimationAppUi( RWsSession& aSession );

        /**
        * ~CAnimationAppUi.
        * Virtual Destructor.
        */
        virtual ~CAnimationAppUi();

    public:  // Functions from base classes

        /**
        * From CEikAppUi, HandleCommandL.
        * Takes care of command handling.
        * @param aCommand The enumerated code for the option selected.
        */
        void HandleCommandL( TInt aCommand );
        
        /**
        * From CEikAppUi, HandleResourceChangeL.
        * Handles resource changes.
        * @param aType Resource change type to be handled
        */
        void HandleResourceChangeL( TInt aType );

    private: // Constructor

        /**
        * ConstructL.
        * 2nd phase constructor.
        */
        void ConstructL();

    private: // New methods

        /**
        * SetupClientDllL.
        * Setup the animation client Dll.
        */
        void SetupClientDllL();

        /**
        * SetupImageCommanderL.
        * Setup the animation image commander.
        */
        void SetupImageCommanderL();

    private: // Data

        /**
        * iAppView
        * The application view
        * Owned by CAnimationAppUi object.
        */
        CAnimationAppView* iAppView;

        // Do not change the order of these variables, as they are called
        // in the AppUi constructor list in the order listed here

        /**
        * iClientDll
        * Animation client dll.
        */
        RClientDll iClientDll;

        /**
        * iClientCommander
        * Controls an animation image object.
        */
        RImageCommander iClientCommander;
    };

#endif // __ANIMATIONAPPUI_H__

// End of File
