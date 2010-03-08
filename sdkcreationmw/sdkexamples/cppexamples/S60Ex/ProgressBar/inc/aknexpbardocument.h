/*
* Copyright (c) 2004 - 2006 Nokia Corporation and/or its subsidiary(-ies).
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




#ifndef __AKNEXPBARDOCUMENT_H__
#define __AKNEXPBARDOCUMENT_H__


// INCLUDES
#include <eikdoc.h>


// FORWARD DECLARATIONS
class  CEikAppUi;
class  CAknExPbarAppUi;


// CLASS DECLARATION
/**
*  CAknExPbarDocument document class.
*/
class CAknExPbarDocument : public CEikDocument
    {
    public: // Constructors and destructor
       
        /**
        * NewL
        * Two-phased constructor.
        * Construct a CAknExPbarDocument for the AVKON application aApp.
        * Using two phase construction, and return a pointer 
        * to the created object.
        * @param aApp Application creating this document.
        * @return A pointer to the created instance of CAknExPbarDocument.
        */
        static CAknExPbarDocument* NewL( CEikApplication& aApp );

        /**
        * NewLC
        * Two-phased constructor.
        * Construct a CAknExPbarDocument for the AVKON application aApp.
        * Using two phase construction, and return a pointer 
        * to the created object.
        * @param aApp Application creating this document.
        * @return A pointer to the created instance of CAknExPbarDocument.
        */
        static CAknExPbarDocument* NewLC( CEikApplication& aApp );

        /**
        * ~CAknExPbarDocument
        * Virtual Destructor.
        */
        virtual ~CAknExPbarDocument();


    private: // Constructors
       
        /**
        * CAknExPbarDocument.
        * C++ default constructor.
        * @param aApp Reference to Application class object
        */
        CAknExPbarDocument( CEikApplication& aApp );

        /**
        * ConstructL
        * 2nd phase constructor.
        */
        void ConstructL();


    private:  // From CEikDocument
        
        /**
        * CreateAppUiL
        * From CEikDocument, CreateAppUiL.
        * Create a CAknExPbarAppUi object and return a pointer to it.
        * The object returned is owned by the Uikon framework.
        * @return Pointer to created instance of AppUi.
        */
        CEikAppUi* CreateAppUiL();

    };

#endif // __AKNEXPBARDOCUMENT_H__

// End of File

