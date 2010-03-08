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



#ifndef __GUIENGINEDOCUMENT_H__
#define __GUIENGINEDOCUMENT_H__


// INCLUDES
#include <akndoc.h>
#include "ShapeListManager.h"


// FORWARD DECLARATIONS
class CGuiEngineAppUi;
class CEikApplication;


// CLASS DECLARATION

/**
* CGuiEngineDocument, An instance of class CGuiEngineDocument is the Document
* part of the CKON application framework for the GuiEngine example application
*/
class CGuiEngineDocument : public CEikDocument
    {

    public: // Constructors and destructor

        /**
        * NewL
        * Two-phased constructor.
        * Construct a CGuiEngineDocument for the AVKON application aApp.
        * Using two phase construction, and return a pointer to the
        * created object.
        * @param aApp Application creating this document.
        * @return Returns a pointer to the instance which is also on the
        * cleanup stack
        */
        static CGuiEngineDocument* NewL( CEikApplication& aApp );

        /**
        * NewLC
        * Two-phased constructor.
        * Construct a CGuiEngineDocument for the AVKON application aApp.
        * Using two phase construction, and return a pointer to the
        * created object.
        * @param aApp Application creating this document.
        * @return Returns a pointer to the instance which is also on the
        * cleanup stack
        */
        static CGuiEngineDocument* NewLC( CEikApplication& aApp) ;


        /**
        * ~CGuiEngineDocument
        * Virtual Destructor.
        */
        virtual ~CGuiEngineDocument();

    public: // New functions

        /**
        * Model
        * Get the model associated with the document
        * @return a pointer
        */
        CShapeListManager* Model();


    public: // from CEikDocument

        /**
        * StoreL
        * Store the document to the store
        * @param aStore the store to save the data to
        * @param aStreamDict the dictionary to hold the stream id of the data stream
        */
        void StoreL( CStreamStore& aStore,
                     CStreamDictionary& aStreamDict ) const;

        /**
        * RestoreL
        * Restore the document from the store
        * @param aStore the store to containing the data to
        * @param aStreamDict the dictionary that holds the stream id of the data stream
        */
        void RestoreL( const CStreamStore& aStore,
                       const CStreamDictionary& aStreamDict );

    private: // Constructors

        /**
        * CGuiEngineDocument.
        * C++ default constructor.
        * @param aApp Reference to Application class object
        */
        CGuiEngineDocument( CEikApplication& aApp );

        /**
        * ConstructL
        * 2nd phase constructor.
        */
        void ConstructL();

    private: // From CEikDocument

        /**
        * CreateAppUiL
        * Create a CGuiEngineAppUi object and return a pointer to it.
        * The object returned is owned by the Uikon framework.
        * @return Pointer to created instance of AppUi.
        */
        CEikAppUi* CreateAppUiL();

    private: // Data

        /**
        * iModel,  The model
        * Owned by CGuiEngineDocument object.
        */
        CShapeListManager*  iModel;

    };

#endif // __GUIENGINEDOCUMENT_H__


// End of File
