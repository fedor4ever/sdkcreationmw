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



#ifndef __CHOICELISTEX_DOCUMENT_H__
#define __CHOICELISTEX_DOCUMENT_H__

// INCLUDES
#include <akndoc.h>

// FORWARD DECLARATIONS
class CChoiceListExAppUi;
class CEikApplication;


// CLASS DECLARATION

/**
* CChoiceListExDocument application class.
* An instance of class CChoiceListExDocument is the Document part of the
* AVKON application framework for the Choice list example application.
*/
class CChoiceListExDocument : public CAknDocument
    {
public:

    /**
    * NewL.
    * Two-phased constructor.
    * Construct a CChoiceListExDocument for the AVKON application aApp
    * using two phase construction, and return a pointer
    * to the created object.
    * @param aApp Application creating this document.
    * @return A pointer to the created instance of CChoiceListExDocument.
    */
    static CChoiceListExDocument* NewL( CEikApplication& aApp );

    /**
    * NewLC.
    * Two-phased constructor.
    * Construct a CChoiceListExDocument for the AVKON application aApp
    * using two phase construction, and return a pointer
    * to the created object.
    * @param aApp Application creating this document.
    * @return A pointer to the created instance of CChoiceListExDocument.
    */
    static CChoiceListExDocument* NewLC( CEikApplication& aApp );

    /**
    * ~CChoiceListExDocument
    * Virtual Destructor.
    */
    virtual ~CChoiceListExDocument();

public: // from CAknDocument

    /**
    * CreateAppUiL
    * From CEikDocument, CreateAppUiL.
    * Create a CChoiceListExAppUi object and return a pointer to it.
    * The object returned is owned by the Uikon framework.
    * @return Pointer to created instance of AppUi.
    */
    CEikAppUi* CreateAppUiL();

private:

    /**
    * ConstructL
    * 2nd phase constructor.
    */
    void ConstructL();

    /**
    * CChoiceListExDocument.
    * C++ default constructor.
    * @param aApp Application creating this document.
    */
    CChoiceListExDocument( CEikApplication& aApp );

    };


#endif // __CHOICELISTEX_DOCUMENT_H__
