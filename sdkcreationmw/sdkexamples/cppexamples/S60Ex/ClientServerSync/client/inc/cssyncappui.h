/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef __CSSYNCAPPUI_H__
#define __CSSYNCAPPUI_H__

// INCLUDE FILES
#include <aknappui.h>

// FORWARD DECLARATIONS
class CCSSyncAppView;
class CCSSyncDocument;

// CLASS DECLARATION
/**
* CCSSyncAppUi
*  An instance of class CCSSyncAppUi is the UserInterface part of the AVKON
*  application framework for the CSSync example application
*/
class CCSSyncAppUi : public CAknAppUi
    {
    public: // Constructors and destructors

        /**
        * CCSSyncAppUi.
        * C++ default constructor. This needs to be public due to
        * the way the framework constructs the AppUi.
        */
        CCSSyncAppUi();

        /**
        * ~CCSSyncAppUi.
        * Destructor.
        */
        virtual ~CCSSyncAppUi();

    public: // Functions from base classes

        /**
        * From CAknAppUi, HandleCommandL.
        * Handles user menu selections.
        * @param aCommand The enumerated code for the option selected.
        */
        void HandleCommandL( TInt aCommand );
        
        /**
        * HandleResourceChangeL()
        * Called by framework when layout is changed.
        * @param aType The type of resources that have changed.
        */
        virtual void HandleResourceChangeL( TInt aType );

    private: // Constructors and destructors

        /**
        * ConstructL.
        * 2nd phase constructor.
        */
        void ConstructL();

    private: // New functions

        /**
        * SyncDocument.
        * Casts iDocument member from CAknAppUi to CCSSyncDocument.
        */
        CCSSyncDocument* SyncDocument() const;

    private: // Data

        /**
        * iAppView, the application view.
        * Owned by CCSSyncAppUi object.
        */
        CCSSyncAppView* iAppView;
    };


#endif // __CSSYNCAPPUI_H__


// End of File
