/*
* Copyright (c) 2005 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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




#ifndef __LOCALIZATIONAPPLICATION_H__
#define __LOCALIZATIONAPPLICATION_H__

// INCLUDES
#include <aknapp.h>

// CLASS DECLARATION

/**
* CLocalizationApplication application class.
* Provides factory to create concrete document object.
* An instance of CLocalizationApplication is the application part of the
* AVKON application framework for the Localization example application.
*/
class CLocalizationApplication : public CAknApplication
    {
    public: // Functions from base classes

        /**
        * From CApaApplication, AppDllUid.
        * @return Application's UID (KUidLocalizationApp).
        */
        TUid AppDllUid() const;

    protected: // Functions from base classes

        /**
        * From CApaApplication, CreateDocumentL.
        * Creates CLocalizationDocument document object. The returned
        * pointer in not owned by the CLocalizationApplication object.
        * @return A pointer to the created document object.
        */
        CApaDocument* CreateDocumentL();
    };

#endif // __LOCALIZATIONAPPLICATION_H__

// End of File
