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


#ifndef __GRAPHICSAPPUI_H__
#define __GRAPHICSAPPUI_H__

// INCLUDES
#include <aknappui.h>

// FORWARD DECLARATIONS
class CGraphicsAppView;

// CLASS DECLARATION

/**
* CGraphicsAppUi application UI class.
* An instance of class CGraphicsAppUi is the UserInterface part of the AVKON
* application framework for the Graphics example application
*/
class CGraphicsAppUi : public CAknAppUi
    {
    public: // Constructors and destructor

        /**
        * ConstructL.
        * 2nd phase constructor.
        */
        void ConstructL();

        /**
        * CGraphicsAppUi.
        * C++ default constructor. This needs to be public due to
        * the way the framework constructs the AppUi
        */
        CGraphicsAppUi();

        /**
        * ~CGraphicsAppUi.
        * Virtual Destructor.
        */
        virtual ~CGraphicsAppUi();


    public: // Functions from base classes

        /**
        * From CEikAppUi, HandleCommandL.
        * Takes care of command handling.
        * @param aCommand Command to be handled.
        */
        void HandleCommandL( TInt aCommand );

    private: // Data

        /**
        * iAppView
        * The application view
        * Owned by CGraphicsAppUi object.
        */
        CGraphicsAppView* iAppView;
    };

#endif // __GRAPHICSAPPUI_H__

// End of File
