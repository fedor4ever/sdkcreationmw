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
* Description:  Active object performing read operations on a socket
*
*/



#ifndef ECMTSOCKETSREADER_H
#define ECMTSOCKETSREADER_H

//  INCLUDES
#include "EcmtCommsEngineIf.h"

// FORWARD DECLARATIONS
class CEcmtBiSocket;
class CEcmtSocketsEngine;

// CLASS DECLARATION

/**
*  Active object for reading a socket.
*  @ingroup EcmtCore
*/
class CEcmtSocketsReader : public CActive, public MEcmtCommsReader 
    {
    public:  // Constructors and destructor
    
	    /**
	    * Two-phased constructor.
	    */
	    static CEcmtSocketsReader* NewL( CEcmtSocketsEngine& aEngine );
	    
	    /**
	    * Destructor.
	    */
	    virtual ~CEcmtSocketsReader();

    public: // New functions

        /**
        *
        */
        void SetSocket( CEcmtBiSocket* aSocket );
        

    public: // Functions from base classes

        /**
        * From MEcmtCommsReader
        */
        TBool ReaderReady( ) const;

        /**
        * From MEcmtCommsReader
        */
        void IssueRead( TDes8& aData );

        /**
        * From MEcmtCommsReader
        */
        void SetReaderObserver( MEcmtCommsReaderObserver& aObserver );
        
        
    protected:  // New functions
    
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
	    CEcmtSocketsReader( CEcmtSocketsEngine& aEngine );

	    /**
	    * By default Symbian 2nd phase constructor is private.
	    */
	    void ConstructL();

    private:    // Data
        CEcmtSocketsEngine&			iEngine;
        CEcmtBiSocket*				iSocket;
        MEcmtCommsReaderObserver*	iObserver;
        TDes8*						iClientBuffer;
        TPtr8						iTmpBuffer;
    };

#endif     
        
// End of File
