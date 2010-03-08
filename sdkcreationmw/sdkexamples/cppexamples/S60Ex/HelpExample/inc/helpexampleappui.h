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


#ifndef __HELPEXAMPLE_APPUI_H__
#define __HELPEXAMPLE_APPUI_H__

// INCLUDES
#include <aknappui.h>

// FORWARD DECLARATION
class CHelpExampleAppView;

/**
* CHelpExampleAppUi
* An instance of class CHelpExampleAppUi is the UserInterface part of the 
* AVKON application framework for the HelpExample example application
*/

class CHelpExampleAppUi : public CAknAppUi
    {
        public: // Constructors and destructor.

            /**
            * ConstructL()
            * Perform the second phase construction of a CHelpExampleAppUi 
            * object this needs to be public due to the way the framework 
            * constructs the AppUi
            */
            void ConstructL();

            /**
            * CHelpExampleAppUi()
            * Perform the first phase of two phase construction.
            * This needs to be public due to the way the framework 
            * constructs the AppUi 
            */
            CHelpExampleAppUi();

            /**
            * ~CHelpExampleAppUi()
            * Destroy the object
            */
            virtual ~CHelpExampleAppUi();


        public: // from CAknAppUi

            /**
            * HandleCommandL()
            * Handle user menu selections
            * @param aCommand The enumerated code for the option selected
            */
            void HandleCommandL( TInt aCommand );

        private: // from CAknAppUi

            /**
            * HelpContextL()
            * Return the help context for this application
            * @return A pointer to the help context
            */
            CArrayFix<TCoeHelpContext>* HelpContextL() const;

        private:    // data

            /** 
            * iAppView 
            * The application view 
            * Owned by CHelpExampleAppUi
            */
            CHelpExampleAppView* iAppView;
    };


#endif // __HELPEXAMPLE_APPUI_H__

// End of File
