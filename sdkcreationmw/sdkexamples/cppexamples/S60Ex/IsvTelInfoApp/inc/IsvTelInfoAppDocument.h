/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef ISVTELINFOAPPDOCUMENT_H
#define ISVTELINFOAPPDOCUMENT_H

// INCLUDES
#include <akndoc.h>
#include <aknapp.h>

// FORWARD DECLARATIONS
class  CEikAppUi;

// CLASS DEFINITION
/**
* CIsvTelInfoAppDocument application class.
*/
class CIsvTelInfoAppDocument : public CAknDocument
    {
    public: // Constructors and destructor

        /**
        * EPOC two-phased constructor.
        * @return Pointer to created Document class object.
        * @param aApp Reference to Application class object.
        */
        static CIsvTelInfoAppDocument* NewL( CAknApplication& aApp );

        /**
        * Destructor.
        */
        virtual ~CIsvTelInfoAppDocument();

    private: // Constructors

        /**
        * Overload constructor.
        * @param aApp Reference to Application class object.
        */
        CIsvTelInfoAppDocument( CAknApplication& aApp );

    private: // From CAknDocument

        /**
        * From CEikDocument, CreateAppUiL.
        * Creates CIsvTelInfoAppUi "App UI" object.
        * @return Pointer to created AppUi class object.
        */
        CEikAppUi* CreateAppUiL();

    private:
        CIsvTelInfoAppUi* iIsvTelInfoAppUiPtr;

    };

#endif //ISVTELINFOAPPDOCUMENT_H

// End of File
