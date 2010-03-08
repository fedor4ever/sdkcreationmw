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
* Description: 
*
*/



#ifndef __UEEX_APPLICATION_H__
#define __UEEX_APPLICATION_H__

// INCLUDES
#include <aknapp.h>


// CLASS DECLARATION

/**
* CUEExApplication application class.
* Provides factory to create concrete document object.
* An instance of CUEExApplication is the application part of the
* AVKON application framework for the UEEx example application.
*/
class CUEExApplication : public CAknApplication
    {
public:  // from CAknApplication

    /**
    * From CApaApplication, AppDllUid.
    * @return Application's UID (KUidUEExApp).
    */
    TUid AppDllUid() const;

protected: // from CAknApplication

    /**
    * From CApaApplication, CreateDocumentL.
    * Creates CUEExDocument document object. The returned
    * pointer in not owned by the CUEExApplication object.
    * @return A pointer to the created document object.
    */
    CApaDocument* CreateDocumentL();
    };

#endif // __UEEX_APPLICATION_H__
