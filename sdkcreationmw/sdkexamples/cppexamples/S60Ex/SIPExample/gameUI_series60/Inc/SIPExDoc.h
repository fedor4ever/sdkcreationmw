/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __CSIPEXDOC_H__
#define __CSIPEXDOC_H__

// INCLUDES

#include <eikdoc.h>
#include <e32base.h>


// FORWARD DECLARATIONS
class CEikApplication;
class CSIPExEngine;
class MSIPExGameObserver;

// CLASS DECLARATION
/**
* The document class.
* Document part of the CKON application framework for the SIPLine.
* Creates appui.
*/
class CSIPExDoc 
    : public CEikDocument
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aApp reference to the application class instance
        */
        static CSIPExDoc* NewL( CEikApplication& aApp );
        
        /**
        * Destructor.
        */
        virtual ~CSIPExDoc();
    
    public: // New functions
    
        /**
        * Creates the application engine.
        * @param aObserver A reference to the game observer.
        * @return A pointer to engine. The client gets ownership of 
        *         the engine.
        */
        CSIPExEngine* CreateEngineL( MSIPExGameObserver& aObserver );        
 
    public: // Functions from base classes

        /**
        * From CEikDocument
        * Creates instance of appui class
        * @return Pointer to the appui class instance
        */
        CEikAppUi* CreateAppUiL();
        
    public:
        
        /**
        * Info text is always stored in document. Text will be appended
        * to existing info text unless max size is reached. In such situation
        * oldest text will be removed.
        * @param aText info text
        */
        void StoreInfoTextL( const TDesC& aText );
       
        /**
        * Returns reference to current info text
        */
        const TDesC& InfoText();
    
    private:

        /**
        * C++ default constructor.
        * @param aApp Reference to the application class
        */
        CSIPExDoc( CEikApplication& aApp );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();            


    private:    // Data
        // Owned: The application engine pointer
        CSIPExEngine*  iEngine;
        
        // Owned: Current info text
        HBufC* iInfoText;
    };

#endif      // __CSIPEXDOC_H__   
            
// End of File
