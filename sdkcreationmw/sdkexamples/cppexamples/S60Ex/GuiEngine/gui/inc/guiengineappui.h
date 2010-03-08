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



#ifndef __GUIENGINEAPPUI_H__
#define __GUIENGINEAPPUI_H__


// INCLUDES
#include <aknappui.h>
#include <eikmenup.h>
#include "ShapeListManager.h"


// FORWARD DECLARATIONS
class CGuiEngineAppView;
class CGuiEngineDocument;


// CLASS DECLARATION

/**
* CGuiEngineAppUi
* The Application User Interface.
*/
class CGuiEngineAppUi : public CAknAppUi
{
    public: // Constructors and destructor

        /**
        * ConstructL.
        * 2nd phase constructor.
        */
        void ConstructL();

        /**
        * CGuiEngineAppUi.
        * C++ default constructor.
        * @param aDocument the model
        */
        CGuiEngineAppUi( CGuiEngineDocument* aDocument );

        /**
        * ~CGuiEngineAppUi.
        * Virtual Destructor.
        */
        virtual ~CGuiEngineAppUi();


    public:  // New Functions

        /**
        * StoreL
        * Store the user interface settings to a store
        * @param aStore the store to save the data to
        * @return Returns the id of the stream containing the data
        */
        TStreamId StoreL( CStreamStore& aStore ) const;

        /**
        * RestoreL
        * Restore the user interface settings from a store
        * @param aStore the store to read the data from
        * @param aStreamId the id of the stream containing the data
        */
        void RestoreL( const CStreamStore& aStore,
                       TStreamId aStreamId);


    public: //  from CAknAppUi

        /**
        * HandleModelChangeL
        * Update the system to reflect changes in the model
        */
        void HandleModelChangeL();

        /**
        * HandleCommandL
        * From CAknView, takes care of command handling.
        * @param aCommand Command to be handled
        */
        void HandleCommandL( TInt aCommand );

    private:    // Data

        /**
        * iAppView The application view
        * Owned by CGuiEngineAppUi object.
        */
        CGuiEngineAppView* iAppView;

        /**
        * iDocument,  The current document
        * Not owned by CGuiEngineAppUi object.
        */
        CGuiEngineDocument* iDocument;
    };


#endif // __GUIENGINEAPPUI_H__

// End of File
