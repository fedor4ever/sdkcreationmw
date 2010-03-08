/*
* Copyright (c) 2006 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef RTEAPPUI_H
#define RTEAPPUI_H

// INCLUDES
#include <eikapp.h>
#include <eikdoc.h>
#include <e32std.h>
#include <coeccntx.h>
#include <aknappui.h>

// FORWARD DECLARATIONS
class CRTEContainer;


// CLASS DECLARATION

/**
* Application UI class.
* Provides support for EIKON control architecture
*
*/
class CRTEAppUi : public CAknAppUi
    {
    public: // // Constructors and destructor

        /**
        * EPOC default constructor.
        */
        void ConstructL();

        /**
        * Destructor.
        */
        ~CRTEAppUi();

        /**
        * HandleResourceChangeL()
        * Called by framework when layout is changed.
        * @param aType the type of resources that have changed
        */
        void HandleResourceChangeL( TInt aType );

    private:
        /**
        * From CEikAppUi, takes care of command handling.
        * @param aCommand command to be handled
        */
        void HandleCommandL(TInt aCommand);

    private: //Data
        CRTEContainer* iAppContainer;
    };

#endif

// End of File
