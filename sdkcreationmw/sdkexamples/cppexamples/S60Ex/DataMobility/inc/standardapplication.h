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
* Description:  example of class declaration using connection
*
*/


#ifndef STANDARDAPPLICATION_H
#define STANDARDAPPLICATION_H

// INCLUDES
#include <e32base.h>
#include <in_sock.h>

// FORWARD DECLARATIONS
class MDataMobilityPrinter;

// CLASS DECLARATION

/**
*  CStandardApplication  example of class using connection.
*
*/
class CStandardApplication : public CActive
    {
    public:  // Constructors and destructor

        /**
        * NewL is first phase of two-phased constructor.
        */
        static CStandardApplication* NewL( MDataMobilityPrinter& aObserver );

        /**
        * Destructor of StandardApplication.
        */
        ~CStandardApplication();

    public: // From CActive

        void RunL();

        void DoCancel();

    public: // New functions

        /**
        * Start the connection.
        */
        void StartL();

    private:

        /**
        * C++ default constructor.
        */
        CStandardApplication( MDataMobilityPrinter& aObserver );

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
    };

#endif // STANDARDAPPLICATION_H

