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


#ifndef __SERVERDLL_H__
#define __SERVERDLL_H__

// INCLUDES
#include <w32adll.h>

// CLASS DECLARATION

/**
* CServerDll.
* An instance of CServerDll is a 'factory' object which creates a new
* instance of CAnim
*/
class CServerDll : public CAnimDll
    {
    public: // Constructor

        /**
        * CServerDll.
        * Construct an Animation Server dll
        */
        CServerDll();

        // Public destructor not created as this class does not own any data

    public: // From CAnimDll

        /**
        * CreateInstanceL.
        * Create an instance of the Animation Server image aType.
        * @param aType KAnimationSquare implies "construct a bouncing square"
        */
        IMPORT_C CAnim* CreateInstanceL( TInt aType );

    };

#endif // __SERVERDLL_H__

// End of File
