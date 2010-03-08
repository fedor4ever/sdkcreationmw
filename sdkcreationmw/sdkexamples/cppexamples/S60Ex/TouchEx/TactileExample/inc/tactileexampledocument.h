/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declares document class for application.
*
*/


#ifndef __TACTILEEXAMPLEDOCUMENT_H__
#define __TACTILEEXAMPLEDOCUMENT_H__

// INCLUDES
#include <akndoc.h>

// FORWARD DECLARATIONS
class CTactileExampleAppUi;
class CEikApplication;


// CLASS DECLARATION

/**
* CTactileExampleDocument application class.
* An instance of class CTactileExampleDocument is the Document part of the
* AVKON application framework for the TactileExample example application.
*/
class CTactileExampleDocument : public CAknDocument
    {
public:

    /**
     * CTactileExampleDocument.
     * C++ default constructor.
     * @param aApp Application creating this document.
     */
    CTactileExampleDocument( CEikApplication& aApp );

    /**
     * ~CTactileExampleDocument
     * Virtual Destructor.
     */
    virtual ~CTactileExampleDocument();


// from base class CEikDocument

    /**
     * From CEikDocument
     * Create a CTactileExampleAppUi object and return a pointer to it.
     * The object returned is owned by the Uikon framework.
     * @return Pointer to created instance of AppUi.
     */
    CEikAppUi* CreateAppUiL();
    };

#endif // __TACTILEEXAMPLEDOCUMENT_H__

// End of File

