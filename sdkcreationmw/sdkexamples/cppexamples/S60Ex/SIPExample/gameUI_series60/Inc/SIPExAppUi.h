/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __CSIPEXAPPUI_H__
#define __CSIPEXAPPUI_H__

//  INCLUDES

#include "SIPExGameObserver.h"
#include "SIPExGameConstants.h"
#include "SIPExGameEngine.h"
#include "SIPExStateViewNotifier.h"
#include <aknviewappui.h> // View AppUi adds View handling to AppUi
#include <akntabgrp.h>          // For handling tabs
#include <aknnavide.h>

// FORWARD DECLARATIONS
class CSIPExEngine;
class CSIPExGameContainer;
class CSIPExLogContainer;

// CLASS DECLARATION
/**
* Application user interface class.
* User interface part of the CKON application framework 
* for the SIPEx game.
*/
class CSIPExAppUi 
:   public CAknViewAppUi, 
    public MSIPExGameObserver,
    public MSIPExStateViewNotifier   
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor. This needs to be public due to the way 
        * the framework constructs the AppUi.
        */
        CSIPExAppUi();

        /**
        * 2nd phase constructor. This needs to be public due to the way 
        * the framework constructs the AppUi 
        */
        void ConstructL();
        
        /**
        * Destructor.
        */
        virtual ~CSIPExAppUi();

    public: // Functions from base classes

        /**
        * From CEikAppUi, handles key events.
        * @param aKeyEvent Event to handled.
        * @param aType Type of the key event. 
        * @return Response code (EKeyWasConsumed, EKeyWasNotConsumed). 
        */
        TKeyResponse HandleKeyEventL( const TKeyEvent& aKeyEvent,TEventCode aType );
            
        /**
        * From CEikAppUi handles the user interaction.
        * @param aCommand Id of the initiated command        
        */
        void HandleCommandL( TInt aCommand );   


        // From MSIPExGameObserver
        void GameStateChanged( CSIPExEngine::TEngineState aState );
        TBool AcceptInvitationL( const TDesC& aFrom );
        void  IMReceivedL( const TDesC8& aFrom, const TDesC8& aMsg );
        
        // From MSIPExStateViewNotifier
        void ShowStatusInfoL( const TDesC& aText );
        void ShowInfoL( const TDesC& aText );
        void ShowInfoL( const TDesC8& aText );
        
    public:
    
        /**
        * Return current state of the engine
        */
        const CSIPExEngine::TEngineState EngineState();
        
        /**
        * Return true if currently active view is the GameView, false otherwise
        */
        TBool GameViewActive();
        
        /**
        * Return true if currently active view is the LogView, false otherwise
        */
        TBool LogViewActive();
        
        /**
        * Return true if currently active view is matching with aViewUid
        */
        TBool IsViewActive( TUid aViewUid );
        
        /**
        * Return pointer to game container, ownership is not transferred
        */
        CSIPExGameContainer* GameContainer();
        
        /**
        * Return pointer to log container, ownership is not transferred
        */
        CSIPExLogContainer* LogContainer();
        

    private: // New functions
    
        
    private:    // Data
    
        // Not owned: pointer to the application engine.
        CSIPExEngine* iEngine;
        // The address field. The app remembers the last address.
        TBuf< KMaxSIPAddressLength > iAddress;
        // The state of the game.
        CSIPExEngine::TEngineState iState;
        
        CAknNavigationControlContainer* iNaviPane;
        CAknTabGroup* iTabGroup;
        CAknNavigationDecorator* iDecoratedTabGroup;
    };

#endif      // __CSIPEXAPPUI_H__
            
// End of File
