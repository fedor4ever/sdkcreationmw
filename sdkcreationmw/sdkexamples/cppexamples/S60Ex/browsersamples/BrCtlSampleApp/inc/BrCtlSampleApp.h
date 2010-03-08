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
* Description:  Declares main application class.
*
*/


#ifndef BRCTLSAMPLEAPPAPP_H
#define BRCTLSAMPLEAPPAPP_H

// INCLUDES
#include <aknapp.h>

// CONSTANTS
// UID of the application
const TUid KUidBrCtlSampleApp = { 0xA0000181 };

// CLASS DECLARATION

/**
* CBrCtlSampleApp application class.
* Provides the factory to create a concrete document object.
* 
*/
class CBrCtlSampleApp : public CAknApplication
    {
    
    public: // Functions from base classes
        /**
        * From CApaApplication, overridden to enable INI file support.
        * @return CDictionaryStore*, A pointer to the dictionary store
        */
        CDictionaryStore* OpenIniFileLC(RFs& aFs) const;
    private:

        /**
        * From CApaApplication, creates CBrCtlSampleAppDocument document object.
        * @return CApaDocument*, A pointer to the created document object.
        */
        CApaDocument* CreateDocumentL();
        
        /**
        * From CApaApplication, returns application's UID (KUidBrCtlSampleApp).
        * @return TUid, The value of KUidBrCtlSampleApp.
        */
        TUid AppDllUid() const;
    };

#endif

// End of File

