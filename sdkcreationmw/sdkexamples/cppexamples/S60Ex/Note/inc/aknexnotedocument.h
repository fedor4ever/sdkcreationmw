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


#ifndef __AKNEXNOTEDOCUMENT_H__
#define __AKNEXNOTEDOCUMENT_H__


// INCLUDES
#include <eikdoc.h>



// FORWARD DECLARATIONS
class  CEikAppUi;

// CLASS DECLARATION

/**
*  CAknExNoteDocument application class.
*/
class CAknExNoteDocument : public CEikDocument
    {
    public: // Constructors and destructor

        /**
        * NewL.
        * Two-phased constructor.
        * Construct a CAknExNoteDocument for the AVKON application aApp.
        * Using two phase construction,and return a pointer 
        * to the created object.
        * @param aApp Reference to Application class object.
        * @return Pointer to created Document class object.
        */
        static CAknExNoteDocument* NewL( CEikApplication& aApp );

        /**
        * ~CAknExNoteDocument.
        * Virtual Destructor.
        */
        virtual ~CAknExNoteDocument();

    private: // Constructors

        /**
        * CAknExNoteDocument.
        * C++ default constructor.
        * @param aApp Reference to Application class object.
        */
        CAknExNoteDocument( CEikApplication& aApp );

    private: // From CEikDocument

        /**
        * CreateAppUiL
        * Create CAknExNoteAppUi "App UI" object.
        * @return Pointer to created AppUi class object.
        */
        CEikAppUi* CreateAppUiL();

    };

#endif  //__AKNEXNOTEDOCUMENT_H__

// End of File
