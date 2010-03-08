/*
* Copyright (c) 2005 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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




#ifndef __LOCALIZATIONAPPUI_H__
#define __LOCALIZATIONAPPUI_H__

// INCLUDES
#include <aknappui.h>


// FORWARD DECLARATIONS
class CLocalizationAppView;


// CLASS DECLARATION
/**
* CLocalizationAppUi application UI class.
* Interacts with the user through the UI and request message processing
* from the handler class
*/
class CLocalizationAppUi : public CAknAppUi
    {
    public: // Constructors and destructor

        /**
        * ConstructL.
        * 2nd phase constructor.
        */
        void ConstructL();

        /**
        * CLocalizationAppUi.
        * C++ default constructor. This needs to be public due to
        * the way the framework constructs the AppUi
        */
        CLocalizationAppUi();

        /**
        * ~CLocalizationAppUi.
        * Virtual Destructor.
        */
        virtual ~CLocalizationAppUi();

    private:  // Functions from base classes

        /**
        * From CEikAppUi, HandleCommandL.
        * Takes care of command handling.
        * @param aCommand Command to be handled.
        */
        void HandleCommandL( TInt aCommand );

        /**
        *  HandleStatusPaneSizeChange.
        *  Called by the framework when the application status pane
 		*  size is changed.
        */
		void HandleStatusPaneSizeChange();

    private: // Data

        /**
        * The application view
        * Owned by CLocalizationAppUi
        */
        CLocalizationAppView* iAppView;

    };

#endif // __LOCALIZATIONAPPUI_H__

// End of File
