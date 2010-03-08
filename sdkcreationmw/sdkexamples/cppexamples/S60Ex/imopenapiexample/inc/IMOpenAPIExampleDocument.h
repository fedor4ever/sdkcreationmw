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


#ifndef IMOPENAPIEXAMPLEDOCUMENT_H
#define IMOPENAPIEXAMPLEDOCUMENT_H

// INCLUDES
#include <akndoc.h>

   
// CONSTANTS

// FORWARD DECLARATIONS
class  CEikAppUi;
class  CIMOpenAPIExamplemodel;
class CIMOpenAPIExampleAppUi;

// CLASS DECLARATION

/**
*  CIMOpenAPIExampleDocument application class.
*/
class CIMOpenAPIExampleDocument : public CAknDocument
    {
    public: // Constructors and destructor
        /**
        * Two-phased constructor.
        */
        static CIMOpenAPIExampleDocument* NewL(CEikApplication& aApp);

        /**
        * Destructor.
        */
        virtual ~CIMOpenAPIExampleDocument();

    public: // Own functions
    
        /**
        * Document.
        * Returns a reference to itself
        * @return a reference to CIMOpenAPIExampleDocument
        */
    	CIMOpenAPIExampleDocument& Document();

    
        /**
        * Model.
        * Returns a reference to the model
        * @return a reference to CIMOpenAPIExamplemodel
        */
		CIMOpenAPIExamplemodel& Model();
		
        /**
        * UI.
        * Returns a pointer to application UI class
        * @return a reference to CIMOpenAPIExampleAppUi
        */
		CIMOpenAPIExampleAppUi* UI();
		
        /**
        * SetPointerToUI.
        * Sets the pointer to the application UI
        * @param a pointer to CIMOpenAPIExampleAppUi
        */
		void SetPointerToUI(CIMOpenAPIExampleAppUi* aUIPointer);

    private:

        /**
        * EPOC default constructor.
        */
        CIMOpenAPIExampleDocument(CEikApplication& aApp);
        
        /**
        * 2nd phase constructor.
        */
        void ConstructL();

        /**
        * From CEikDocument, create CIMOpenAPIExampleAppUi "App UI" object.
        */
        CEikAppUi* CreateAppUiL();
        
	private:	// data
        /**
        * iUI
        * Local poiner to application UI
        */
		CIMOpenAPIExampleAppUi* iUI;
		
        /**
        * iModel
        * Local poiner to application model
        */
        CIMOpenAPIExamplemodel* iModel;
    };

#endif //IMOPENAPIEXAMPLEDOCUMENT_H

// End of File

