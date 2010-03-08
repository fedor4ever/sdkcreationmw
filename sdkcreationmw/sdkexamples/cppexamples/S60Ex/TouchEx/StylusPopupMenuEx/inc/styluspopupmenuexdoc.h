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



#ifndef __STYLUSPOPUPMENUEX_DOCUMENT_H__
#define __STYLUSPOPUPMENUEX_DOCUMENT_H__

// INCLUDES
#include <akndoc.h>

// FORWARD DECLARATIONS
class CStylusPopupMenuExAppUi;
class CEikApplication;


// CLASS DECLARATION

/**
* CStylusPopupMenuExDoc application class.
* An instance of class CStylusPopupMenuExDoc is the Document part of the
* AVKON application framework for the StylusPopupMenu example application.
*/
class CStylusPopupMenuExDoc : public CAknDocument
    {
public:

    /**
    * NewL.
    * Two-phased constructor.
    * Construct a CStylusPopupMenuExDoc for the AVKON application aApp
    * using two phase construction, and return a pointer
    * to the created object.
    * @param aApp Application creating this document.
    * @return A pointer to the created instance of CStylusPopupMenuExDoc.
    */
    static CStylusPopupMenuExDoc* NewL( CEikApplication& aApp );

    /**
    * NewLC.
    * Two-phased constructor.
    * Construct a CStylusPopupMenuExDoc for the AVKON application aApp
    * using two phase construction, and return a pointer
    * to the created object.
    * @param aApp Application creating this document.
    * @return A pointer to the created instance of CStylusPopupMenuExDoc.
    */
    static CStylusPopupMenuExDoc* NewLC( CEikApplication& aApp );

    /**
    * ~CStylusPopupMenuExDoc
    * Virtual Destructor.
    */
    virtual ~CStylusPopupMenuExDoc();

public: // from CAknDocument

    /**
    * CreateAppUiL
    * From CEikDocument, CreateAppUiL.
    * Create a CStylusPopupMenuExAppUi object and return a pointer to it.
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
    * CStylusPopupMenuExDoc.
    * C++ default constructor.
    * @param aApp Application creating this document.
    */
    CStylusPopupMenuExDoc( CEikApplication& aApp );

    };


#endif // __STYLUSPOPUPMENUEX_DOCUMENT_H__
