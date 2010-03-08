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
*                and Mobility extension API
*
*/


#ifndef ALRAPPLICATIONUSINGNOPREF_H
#define ALRAPPLICATIONUSINGNOPREF_H

// INCLUDES
#include <e32base.h>
#include <in_sock.h>
#include <comms-infras/cs_mobility_apiext.h>

// FORWARD DECLARATIONS
class MDataMobilityPrinter;

// CLASS DECLARATION

/**
*  CALRApplicationUsingNoPref  example of class using connection
*                              and Mobility extension API.
*
*/
class CALRApplicationUsingNoPref : public CActive, public MMobilityProtocolResp
    {
    public:  // Constructors and destructor

        /**
        * NewL is first phase of two-phased constructor.
        */
        static CALRApplicationUsingNoPref* NewL( MDataMobilityPrinter& aObserver );

        /**
        * Destructor.
        */
        ~CALRApplicationUsingNoPref();

    public: // From CActive

        void RunL();

        void DoCancel();

        TInt RunError( TInt aError );

    public: // From MMobilityProtocolResp

        void PreferredCarrierAvailable( TAccessPointInfo aOldAPInfo,
                                        TAccessPointInfo aNewAPInfo,
                                        TBool aIsUpgrade,
                                        TBool aIsSeamless );

        void NewCarrierActive( TAccessPointInfo aNewAPInfo, TBool aIsSeamless );

        void Error( TInt aError );

    public: // New functions

        /**
        * Start the connection.
        */
        void StartL();

    private:

        /**
        * C++ default constructor.
        */
        CALRApplicationUsingNoPref( MDataMobilityPrinter& aObserver );

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

        /**
         * Helper method to send message to print to the observer.
         * @param aResourceId Resource identifier of the string to print.
         */
         void Print( TInt aResourceId );

         /**
         * Helper method to send message to print to the observer.
         * @param aResourceId Resource identifier of the string to print.
         * @param aInt Integer value replacing %N in the string.
         */
         void Print( TInt aResourceId, TInt aInt );

    private:    // Data

        MDataMobilityPrinter&         iObserver;       // Pointer to Observer
        RConnection                   iConnection;
        RSocketServ                   iSocketServer;
        CActiveCommsMobilityApiExt*   iMobility;
    };

#endif // ALRAPPLICATIONUSINGNOPREF_H

