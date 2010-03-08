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


#ifndef __LISTBOXAPPUI_H__
#define __LISTBOXAPPUI_H__


#include <aknviewappui.h>
#include "ViewTemplate.h"

class CListboxNumberContainer;
class CListboxGraphicContainer;
class CListboxSettingView;


/**
* CListboxAppUi application UI class.
* Interacts with the user through the UI and request message processing
* from the handler class
*/
class CListboxAppUi : public CAknViewAppUi
    {
    public: // Constructors and destructor

        /**
        * 2nd phase constructor.
        */
        void ConstructL();

        /**
        * C++ default constructor. This needs to be public due to
        * the way the framework constructs the AppUi
        */
        CListboxAppUi();

        /**
        * Virtual Destructor.
        */
        virtual ~CListboxAppUi();

    private:  // Functions from base classes

        /**
        * From CEikAppUi, HandleCommandL.
        * Takes care of command handling.
        * @param aCommand Command to be handled.
        */
        void HandleCommandL( TInt aCommand );
		
		/**
        * Handles a change to the control's resources.
        * @param aType A message UID value. 
        */
		void HandleResourceChangeL( TInt aType );
        
    private: // Data

        /**
        * The SingleNumberStyle view.
        * Owned.
        */
        CViewTemplate<CListboxNumberContainer>* iSingleNumberStyleView;
        
        /**
        * The SingleGraphicStyle view.
        * Owned.
        */
        CViewTemplate<CListboxGraphicContainer>* iSingleGraphicStyleView;
        
        /**
        * The Setting view.
        * Owned.
        */
        CListboxSettingView* iSettingView;
        
        
    };

#endif // __LISTBOXAPPUI_H__

// End of File

