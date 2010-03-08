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


#ifndef ISVTELCALLAPPDOCUMENT_H
#define ISVTELCALLAPPDOCUMENT_H

// INCLUDES
#include <akndoc.h>
#include <aknapp.h>

// FORWARD DECLARATIONS
class  CEikAppUi;

// CLASS DECLARATION
/**
* CIsvTelCallAppDocument application class.
*/
class CIsvTelCallAppDocument : public CAknDocument
    {
    public: // Constructors and destructor

        /**
        * EPOC two-phased constructor.
        * @return Pointer to created Document class object.
        * @param aApp Reference to Application class object.
        */
        static CIsvTelCallAppDocument* NewL( CAknApplication& aApp );

        /**
        * Destructor.
        */
        virtual ~CIsvTelCallAppDocument();

    private: 
    // Constructors

        /**
        * Overload constructor.
        * @param aApp Reference to Application class object.
        */
        CIsvTelCallAppDocument( CAknApplication& aApp );

    private: // From CAknDocument
        /**
        * From CEikDocument, CreateAppUiL.
        * Creates CIsvTelCallAppUi "App UI" object.
        * @return Pointer to created AppUi class object.
        */
        CEikAppUi* CreateAppUiL();

    private:
        CIsvTelCallAppUi* iIsvTelCallAppUiPtr;
    };

#endif  //ISVTELCALLAPPDOCUMENT_H

// End of File