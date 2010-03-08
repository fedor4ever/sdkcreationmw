/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Application UI class, CAddressBookAppUi - 
*				 responsible for handling user commands
*
*/


#ifndef C_DirectoryLocalizerExAPPUI_H
#define C_DirectoryLocalizerExAPPUI_H

#include <aknviewappui.h>
#include "directorylocalizerexcontainer.h"

class CDirectoryLocalizerExContainer;

class CDirectoryLocalizerExAppUi : public CAknAppUi
    {
    public: // Constructors and destructor

        /**
         * ctor
         */
        CDirectoryLocalizerExAppUi();

        /**
         * symbian 2nd ctor
         */
        void ConstructL();

        /**
         * dtor
         */
        virtual ~CDirectoryLocalizerExAppUi();

    private:

        /**
         * From CEikAppUi
         */
        void HandleCommandL( TInt aCommand );

    private: // data
        
        /**
         * pointor to the container.
         * own
         */
        CDirectoryLocalizerExContainer*  iAppContainer;

    };

#endif // C_DirectoryLocalizerExAPPUI_H

// End of File
