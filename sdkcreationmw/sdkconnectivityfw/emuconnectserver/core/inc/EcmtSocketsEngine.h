/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Communication engine using sockets
*
*/


#ifndef CECMTSOCKETSENGINE_H
#define CECMTSOCKETSENGINE_H

#undef DEBUG_STREAM
#undef DEBUG_BOX

//  INCLUDES
#include "EcmtCommsEngineIf.h"
#include <e32base.h>

// FORWARD DECLARATIONS
class CEcmtSocketsReader;
class CEcmtSocketsWriter;
class CEcmtBiSocketFactory;
class CEcmtBiSocket;

// CLASS DECLARATION

/**
*  Class handling the socket communication controller functionality.
*  @ingroup EcmtCore
*/
class CEcmtSocketsEngine : public CActive
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CEcmtSocketsEngine* NewL( MEcmtCommsEngineObserver& aObserver,
                                         CEcmtBiSocketFactory& aFactory );

        /**
        * Destructor.
        */
        virtual ~CEcmtSocketsEngine();

    public: // New functions
        /**
        *
        */
        void ReportError( TInt aError );
    

    public: // Functions from base classes

        /**
        * From MEcmtCommsEngine
        */
        MEcmtCommsReader& Reader( ) const;

        /**
        * From MEcmtCommsEngine
        */
        MEcmtCommsWriter& Writer( ) const;

        /**
        * From MEcmtCommsEngine
        */
        void ResetConnection( );

        /**
        * From MEcmtCommsEngine
        */
        const TPtrC ParameterString( ) const;

        /**
        * From MEcmtCommsEngine
        */
        const TPtrC AddressFamily( ) const;

    protected: // Functions from base classes
        /**
        * From CActive
        */
        void DoCancel();

        /**
        * From CActive
        */
        void RunL();


    private:

        /**
        * C++ default constructor.
        */
        CEcmtSocketsEngine( MEcmtCommsEngineObserver& aObserver,
                            CEcmtBiSocketFactory& aFactory );
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
    private:    // Data

        enum TSocketsEngineState
            {
            EAccepting,
            EConnected,
            EClosing,
            EExiting
            };
    private:
        /**
        *
        */
        void ChangeState( TSocketsEngineState aNewState );
        
        /**
        *
        */
        void AcceptL();
        
    private:    // Data

        MEcmtCommsEngineObserver&	iObserver;
        CEcmtBiSocketFactory&		iSocketFactory;

        TSocketsEngineState		iEngineState;
        CEcmtBiSocket*			iSocket;
        CEcmtBiSocket*			iListeningSocket;
        CEcmtSocketsReader*		iSocketsReader;
        CEcmtSocketsWriter*		iSocketsWriter;

    };

#endif
        
// End of File
