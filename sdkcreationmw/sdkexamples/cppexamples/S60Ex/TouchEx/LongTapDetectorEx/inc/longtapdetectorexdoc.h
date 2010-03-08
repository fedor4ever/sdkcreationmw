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



#ifndef __BUTTONEX_DOCUMENT_H__
#define __BUTTONEX_DOCUMENT_H__

// INCLUDES
#include <akndoc.h>

// FORWARD DECLARATIONS
class CLongTapDetectorExAppUi;
class CEikApplication;


// CLASS DECLARATION

/**
* CLongTapDetectorExDoc application class.
* An instance of class CLongTapDetectorExDoc is the Document part of the
* AVKON application framework for the LongTapDetector example application.
*/
class CLongTapDetectorExDoc : public CAknDocument
    {
public:

    /**
    * NewL.
    * Two-phased constructor.
    * Construct a CLongTapDetectorExDoc for the AVKON application aApp
    * using two phase construction, and return a pointer
    * to the created object.
    * @param aApp Application creating this document.
    * @return A pointer to the created instance of CLongTapDetectorExDoc.
    */
    static CLongTapDetectorExDoc* NewL( CEikApplication& aApp );

    /**
    * NewLC.
    * Two-phased constructor.
    * Construct a CLongTapDetectorExDoc for the AVKON application aApp
    * using two phase construction, and return a pointer
    * to the created object.
    * @param aApp Application creating this document.
    * @return A pointer to the created instance of CLongTapDetectorExDoc.
    */
    static CLongTapDetectorExDoc* NewLC( CEikApplication& aApp );

    /**
    * ~CLongTapDetectorExDoc
    * Virtual Destructor.
    */
    virtual ~CLongTapDetectorExDoc();

public: // from CAknDocument

    /**
    * CreateAppUiL
    * From CEikDocument, CreateAppUiL.
    * Create a CLongTapDetectorExAppUi object and return a pointer to it.
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
    * CLongTapDetectorExDoc.
    * C++ default constructor.
    * @param aApp Application creating this document.
    */
    CLongTapDetectorExDoc( CEikApplication& aApp );

    };


#endif // __BUTTONEX_DOCUMENT_H__
