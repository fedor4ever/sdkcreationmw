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



#ifndef __CSSYNCDOCUMENT_H__
#define __CSSYNCDOCUMENT_H__

// INCLUDE FILES
#include <akndoc.h>
#include "TimeServerSession.h"

// FORWARD DECLARATIONS
class CCSSyncAppUi;
class CEikApplication;

// CLASS DECLARATION
/**
* CCSSyncDocument
*  An instance of class CCSSyncDocument is the Document part of the AVKON
*  application framework for the CSSync example application
*/
class CCSSyncDocument : public CAknDocument
    {
    public: // Constructors and destructors

        /**
        * NewL.
        * Two-phased constructor.
        * Constructs a CCSSyncDocument for the AVKON application aApp 
        * using two phase construction, and returns a pointer to the 
        * created object.
        * @param aApp Application creating this document.
        * @return A pointer to the created instance of CCSSyncDocument.
        */
        static CCSSyncDocument* NewL( CEikApplication& aApp );

        /**
        * NewLC.
        * Two-phased constructor.
        * Constructs a CCSSyncDocument for the AVKON application aApp 
        * using two phase construction, and returns a pointer to the 
        * created object.
        * @param aApp Application creating this document.
        * @return A pointer to the created instance of CCSSyncDocument.
        */
        static CCSSyncDocument* NewLC( CEikApplication& aApp );

        /**
        * ~CCSSyncDocument.
        * Destructor.
        * Destroys the object and release all memory objects.
        */
        virtual ~CCSSyncDocument();

    public: // New functions

        /**
        * UpdateTime.
        * Asks the time server to update this documents locally stored time.
        */
        void UpdateTime();

        /**
        * Time.
        * Returns the currently stored time.
        * @return The time.
        */
        const TTime& Time() const;

    public: // Functions from base classes

        /**
        * From CAknDocument, CreateAppUiL.
        * Creates a CCSSyncAppUi object and returns a pointer to it.
        * @return A pointer to the created instance of the AppUi created.
        */
        CEikAppUi* CreateAppUiL();

    private: // Constructors and destructors

        /**
        * CCSSyncDocument.
        * C++ default constructor.
        * Performs the first phase of two phase construction.
        * @param aApp Application creating this document.
        */
        CCSSyncDocument( CEikApplication& aApp );

        /**
        * ConstructL.
        * Performs the second phase construction of a CCSSyncDocument object.
        */
        void ConstructL();

private: // Data

        /**
        * iServer, interface to the Time Server.
        */
        RTimeServerSession iServer;

        /**
        * iTime, the currently displayed time.
        */
        TTime iTime;
};


#endif // __CSSYNCDOCUMENT_H__


// End of File
