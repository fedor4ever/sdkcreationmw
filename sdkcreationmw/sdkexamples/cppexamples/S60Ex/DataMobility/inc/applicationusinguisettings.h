/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Example of class declaration using connection
*                and connection UI settings.
*
*/


#ifndef APPLICATIONUSINGUISETTINGS_H
#define APPLICATIONUSINGUISETTINGS_H

// INCLUDES
#include <e32base.h>
#include <in_sock.h>
#include <cmapplicationsettingsui.h>

// FORWARD DECLARATIONS
class MDataMobilityPrinter;

// CLASS DECLARATION

/**
*  CApplicationUsingUiSettings  example of class using connection
*                               and connection UI settings.
*
*/
class CApplicationUsingUiSettings : public CActive
    {
    public:  // Constructors and destructor

        /**
        * NewL is first phase of two-phased constructor.
        */
        static CApplicationUsingUiSettings* NewL( MDataMobilityPrinter& aObserver );

        /**
        * Destructor of ApplicationUsingUiSettings.
        */
        ~CApplicationUsingUiSettings();

    public: // From CActive

        void RunL();

        void DoCancel();

    public: // New functions

        /**
        * Start the connection.
        */
        void StartL();

        TBool ConnectionSettingL();

    private:

        /**
        * C++ default constructor.
        */
        CApplicationUsingUiSettings( MDataMobilityPrinter& aObserver );

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

        /**
         * Helper method to send message to print to the observer.
         * @param aResourceId Resource identifier of the string to print.
         */
         void PrintL( TInt aResourceId );

         /**
         * Helper method to send message to print to the observer.
         * @param aResourceId Resource identifier of the string to print.
         * @param aInt Integer value replacing %N in the string.
         */
         void PrintL( TInt aResourceId, TInt aInt );

    private:    // Data

        MDataMobilityPrinter& iObserver;       // Pointer to Observer
        RConnection           iConnection;
        RSocketServ           iSocketServer;
        TCmSettingSelection   iUserSelection;
    };

#endif // APPLICATIONUSINGUISETTINGS_H

