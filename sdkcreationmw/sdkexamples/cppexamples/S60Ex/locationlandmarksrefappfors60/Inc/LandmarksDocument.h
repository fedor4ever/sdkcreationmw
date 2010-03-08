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
* Description:  See class description below
*
*/


#ifndef __LANDMARKS_DOCUMENT_H__
#define __LANDMARKS_DOCUMENT_H__

// INCLUDES
#include <akndoc.h>

// FORWARD DECLARATIONS
class CLandmarksAppUi;
class CEikApplication;

// CLASS DECLARATION

/**
*  An instance of CLandmarksDocument is the application part of the Avkon
*  application framework for the Landmarks reference application
*/
class CLandmarksDocument : public CAknDocument
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        *
        * @param aApp an eikon application instance
        */
        static CLandmarksDocument* NewL(CEikApplication& aApp);

        /**
        * Destructor.
        */
        ~CLandmarksDocument();

    public: // from CAknDocument

        /**
        * CreateAppUiL creates a CLandmarksAppUi object and return a pointer 
        * to it.
        *
        * @return a pointer to the created instance of the AppUi created
        */
        CEikAppUi* CreateAppUiL();

    private:

        /**
        * C++ constructor.
        */
        CLandmarksDocument(CEikApplication& aApp);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    };

#endif // __LANDMARKS_DOCUMENT_H__

// End of File

