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


#ifndef __AKNEXPOPFIELDAPPUI_H__
#define __AKNEXPOPFIELDAPPUI_H__

// INCLUDES
#include <aknviewappui.h>
#include "aknexpopfieldview.h"


// CLASS DECLARATION

/**
* Application UI class.
* Provides support for the following features:
* - EIKON control architecture
* - view architecture
* - status pane
*/
class CAknExPopFieldAppUi : public CAknViewAppUi
    {
    public: // Constructors and destructor

        /**
        * Default constructor.
        */
        CAknExPopFieldAppUi();

        /**
        * ConstructL.
        * 2nd phase constructor.
        */
        void ConstructL();

        /**
        * Destructor.
        */
        virtual ~CAknExPopFieldAppUi();

    private: // From CEikAppUi

        /**
        * From CEikAppUi, HandleCommandL.
        * Handles the commands.
        * @param aCommand Command to be handled
        */
        void HandleCommandL( TInt aCommand );
                
        /**
        * From CEikAppUi, HandleResourceChangeL.
        * Handles resource changes.
        * @param aType Resource change type to be handled
        */
        void HandleResourceChangeL( TInt aType );
        
    private: // Data

        /**
        * iView
        * Owned by CAknExPopFieldAppUi object.
        */
        CAknExPopFieldView* iView;


    };

#endif  //__AKNEXPOPFIELDAPPUI_H__

// End of File
