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


#ifndef ISVTELCALLAPPAPPUI_H
#define ISVTELCALLAPPAPPUI_H

// INCLUDES
#include <aknviewappui.h>

// CLASS DECLARATION
/**
* Application UI class.
* Provides support for the following features:
* - EIKON control architecture
* - view architecture
* - status pane
*/
class CIsvTelCallAppUi : public CAknViewAppUi
    {
    public: // Constructors and destructor

        /**
        * Default constructor.
        */
        CIsvTelCallAppUi();

        /**
        * EPOC constructor.
        */
        void ConstructL();

        /**
        * Destructor.
        */
        virtual ~CIsvTelCallAppUi();

    private: // From CEikAppUi
        
        /**
        * From CEikAppUi, ProcessCommandL.
        * Processes the commands.
        * @param aCommand Command to be handled
        */
        void ProcessCommandL( TInt aCommand );

        /**
        * From CEikAppUi, HandleCommandL.
        * Handles the commands.
        * @param aCommand Command to be handled
        */
        void HandleCommandL( TInt aCommand );

    private: //Data
        TUid iViewId;

    };

#endif

// End of File