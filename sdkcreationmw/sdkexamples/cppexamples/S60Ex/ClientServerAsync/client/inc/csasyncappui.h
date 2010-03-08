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



#ifndef __CSASYNCAPPUI_H__
#define __CSASYNCAPPUI_H__

// INCLUDE FILES
#include <aknappui.h>
#include "AsyncTimeObserver.h"

// FORWARD DECLARATIONS
class CCSAsyncAppView;
class CCSAsyncDocument;

// CLASS DECLARATION
/**
* CCSAsyncAppUi
*  An instance of class CCSAsyncAppUi is the UserInterface part of the AVKON
*  application framework for the CSAsync example application
*/
class CCSAsyncAppUi : public CAknAppUi, public MAsyncTimeObserver
    {
    public: // Constructors and destructors

        /**
        * CCSAsyncAppUi.
        * C++ default constructor. This needs to be public due to
        * the way the framework constructs the AppUi.
        */
        CCSAsyncAppUi();

        /**
        * ~CCSAsyncAppUi.
        * Destructor.
        * Destroys the object and release all memory objects.
        */
        virtual ~CCSAsyncAppUi();

    public: // Functions from base classes

        /**
        * From CAknAppUi, HandleCommandL.
        * Handles user menu selections.
        * @param aCommand The enumerated code for the option selected.
        */
        void HandleCommandL( TInt aCommand );

        /**
        * From CAknAppUi, DynInitMenuPaneL.
        * Prepares the menu prior to display.
        * @param aResourceId The resource for the menu to be displayed.
        * @param aMenuPane The handle to the menu.
        */
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

        /**
        * From MAsyncTimeObserver, HandleTimeUpdate.
        * Handles updates to the time.
        */
        void HandleTimeUpdate();
        
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
        * AsyncDocument.
        * Casts iDocument member from CAknAppUi to CCSAsyncDocument.
        */
        CCSAsyncDocument* AsyncDocument() const;

    private: // Data

        /**
        * iAppView, the application view.
        * Owned by CCSAsyncAppUi object.
        */
        CCSAsyncAppView* iAppView;
    };

#endif // __CSASYNCAPPUI_H__

// End of File
