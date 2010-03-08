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



#ifndef __CCEAPPUI_H__
#define __CCEAPPUI_H__

// EXTERNAL INCLUDES
#include <aknviewappui.h>

// FORWARD DECLARATIONS
class MCLFContentListingEngine;
class MCLFItemListModel;

class CCEOperationObserver;

// CLASS DECLARATION

/**
*  Application UI for CLF Example Application.
*/
class CCEAppUi
    : public CAknViewAppUi
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        */
        CCEAppUi();

        /**
        * Destructor.
        */
        virtual ~CCEAppUi();

        /**
        * Symbian 2nd phase constructor.
        */
        void ConstructL();

    protected:  // Functions from base classes

        /**
        * From CEikAppUi. Handle user's commands.
        */
        void HandleCommandL( TInt aCommand );

    private:    // Data

    	// Application view (not owned)
        CAknView* iAppView;

        // Content listing engine (owned)
        MCLFContentListingEngine* iEngine;

        // Content listing model (owned)
        MCLFItemListModel* iModel;

        // Content listeng operation observer (owned)
        CCEOperationObserver* iObserver;
    };

#endif // __CCEAPPUI_H__

// End of File
