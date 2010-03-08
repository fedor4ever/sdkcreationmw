/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef DATAMOBILITYAPPLICATION_H
#define DATAMOBILITYAPPLICATION_H

// INCLUDES
#include <aknapp.h>

// CLASS DECLARATION

/**
* CDataMobilityApplication application class.
* Provides factory to create concrete document object.
* An instance of CDataMobilityApplication is the application part of the
* AVKON application framework for the datamobility example application.
*/
class CDataMobilityApplication : public CAknApplication
    {
    public: // Functions from base classes

        /**
        * From CApaApplication, AppDllUid.
        * @return Application's UID (KUidDataMobilityApp).
        */
        TUid AppDllUid() const;

    protected: // Functions from base classes

        /**
        * From CApaApplication, CreateDocumentL.
        * Creates CDataMobilityDocument document object. The returned
        * pointer in not owned by the CDataMobilityApplication object.
        * @return A pointer to the created document object.
        */
        CApaDocument* CreateDocumentL();
    };

#endif // DATAMOBILITYAPPLICATION_H

// End of File
