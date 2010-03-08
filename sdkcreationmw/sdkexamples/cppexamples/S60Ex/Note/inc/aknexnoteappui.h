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


#ifndef __AKNEXNOTEAPPUI_H__
#define __AKNEXNOTEAPPUI_H__


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
class CAknExNoteAppUi : public CAknViewAppUi
    {
    public: // Constructors and destructor

        /**
        * CAknExNoteAppUi.
        * C++ default constructor.
        */
        CAknExNoteAppUi();

        /**
        * ConstructL
        * 2nd phase constructor.
        */
        void ConstructL();

        /**
        * ~CAknExNoteAppUi
        * Virtual Destructor.
        */
        virtual ~CAknExNoteAppUi();

    private: // From CEikAppUi

        /**
        * HandleCommandL
        * Handles the commands.
        * @param aCommand command to be handled
        */
        void HandleCommandL( TInt aCommand );

        /**
        * From CEikAppUi, HandleResourceChangeL.
        * Handles resource changes.
        * @param aType Resource change type to be handled
        */
        void HandleResourceChangeL( TInt aType );

    };

#endif  //__AKNEXNOTEAPPUI_H__

// End of File
