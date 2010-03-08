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
*     Declares main application class.
*
*/


#ifndef LMUIREFAPP_H
#define LMUIREFAPP_H 

// INCLUDES
#include <aknapp.h>

// CLASS DECLARATION

/**
* LmUiRefApp application class.
* Provides factory to create concrete document object.
* 
*/
class CLmUiRefApp : public CAknApplication
    {
    
    public: // Functions from base classes
    private:

        /**
        * From CApaApplication, creates CLmUiRefAppDocument document object.
        * @return A pointer to the created document object.
        */
        CApaDocument* CreateDocumentL();
        
        /**
        * From CApaApplication, returns application's UID (KUidLmUiRefApp).
        * @return The value of KUidLmUiRefApp.
        */
        TUid AppDllUid() const;
    };

#endif

// End of File

