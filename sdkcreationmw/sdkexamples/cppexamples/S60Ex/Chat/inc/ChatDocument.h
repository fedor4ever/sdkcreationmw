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
* Description:  
*
*/


#ifndef CHATDOCUMENT_H
#define CHATDOCUMENT_H

// INCLUDES
#include <akndoc.h>
   
// CONSTANTS

// FORWARD DECLARATIONS
class  CEikAppUi;

// CLASS DECLARATION

/**
*  CChatDocument application class.
*/
class CChatDocument : public CAknDocument
    {
    public: // Constructors and destructor
        /**
        * Two-phased constructor.
        */
        static CChatDocument* NewL(CEikApplication& aApp);

        /**
        * Destructor.
        */
        virtual ~CChatDocument();

    public: // New functions

    public: // Functions from base classes
    
    protected:  // New functions

    protected:  // Functions from base classes

    private:

        /**
        * default constructor.
        */
        CChatDocument(CEikApplication& aApp);
        
        void ConstructL();

    private:

        /**
        * From CEikDocument, create CChatAppUi "App UI" object.
        */
        CEikAppUi* CreateAppUiL();
    };

#endif // CHATDOCUMENT_H

// End of File

