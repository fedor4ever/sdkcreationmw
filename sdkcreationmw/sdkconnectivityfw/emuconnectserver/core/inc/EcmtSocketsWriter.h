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
* Description:  Active object performing write operations on a socket
*
*/



#ifndef ECMTSOCKETSWRITER_H
#define ECMTSOCKETSWRITER_H

//  INCLUDES
#include "EcmtCommsEngineIf.h"

// FORWARD DECLARATIONS
class CEcmtBiSocket;
class CEcmtSocketsEngine;

// CLASS DECLARATION

/**
*  Active object for writing to a socket.
*  @ingroup EcmtCore
*/
class CEcmtSocketsWriter : public CActive, public MEcmtCommsWriter 
    {
    public:  // Constructors and destructor
    
	    /**
	    * Two-phased constructor.
	    */
	    static CEcmtSocketsWriter* NewL( CEcmtSocketsEngine& aEngine );
	    
	    /**
	    * Destructor.
	    */
	    virtual ~CEcmtSocketsWriter();

	    public: // New functions

        /**
        *
        */
        void SetSocket( CEcmtBiSocket* aSocket );
    

    public: // Functions from base classes

        /**
        * From MEcmtCommsWriter
        */
        TBool WriterReady( ) const;

        /**
        * From MEcmtCommsWriter
        */
        void IssueWrite( const TDesC8& aData );

        /**
        * From MEcmtCommsWriter
        */
        void SetWriterObserver( MEcmtCommsWriterObserver& aObserver );
        
        
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
	    CEcmtSocketsWriter( CEcmtSocketsEngine& aEngine );

	    /**
	    * By default Symbian 2nd phase constructor is private.
	    */
	    void ConstructL();

    private:    // Data
        CEcmtSocketsEngine&			iEngine;
        CEcmtBiSocket*			iSocket;
        MEcmtCommsWriterObserver*	iObserver;
    };

#endif     
        
// End of File
