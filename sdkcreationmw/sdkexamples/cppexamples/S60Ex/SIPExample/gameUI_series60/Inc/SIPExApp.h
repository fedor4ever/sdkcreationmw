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


#ifndef __CSIPEXAPP_H__
#define __CSIPEXAPP_H__

//  INCLUDES

#include <eikapp.h>

// CONSTANTS
const TUid KUidSIPExApp = { 0xA00001EB };

// CLASS DECLARATION

/**
* The application class.
* This class is the application part of the CKON application framework 
* for the SIPEx game Application. Creates document class.
*/
class CSIPExApp : public CEikApplication
    {
        
    public: // Functions from base classes

        /**
        * From CApaApplication
        * Gets the application specific UID	 	 
        * @return The application specific UID
        */        
        TUid AppDllUid() const;
                            
    protected:  // Functions from base classes
        
        /**
        * From CApaApplication
        * Creates an instance of the document class
        * @return A pointer to the created document instance
        */        
        CApaDocument* CreateDocumentL();
    };

#endif      // __CSIPEXAPP_H__  
            
// End of File
