/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declares the document class for the Haptics Example
*
*/


#ifndef C_HAPTICSEXAMPLEDOCUMENT_H
#define C_HAPTICSEXAMPLEDOCUMENT_H

#include <akndoc.h>

class CEikApplication;

/**
* CHapticsExampleDocument application class.
* An instance of class CHapticsExampleDocument is the Document part of the
* AVKON application framework for the Haptics Example example application.
*/
class CHapticsExampleDocument : public CAknDocument
    {
public:

    /**
     * C++ default constructor.
     * @param aApp Application creating this document.
     */
    CHapticsExampleDocument( CEikApplication& aApp );

    /**
     * Virtual Destructor.
     */
    virtual ~CHapticsExampleDocument();


public:
    /**
     * From CEikDocument
     * Create a CHapticsExampleAppUi object and return a pointer to it.
     * The object returned is owned by the Uikon framework.
     * @return Pointer to created instance of AppUi.
     */
    CEikAppUi* CreateAppUiL();
    };

#endif // C_HAPTICSEXAMPLEDOCUMENT_H

// End of File

