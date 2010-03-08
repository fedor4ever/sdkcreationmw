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
* Description:  Class representing the messages and message events of the system
*
*/


#ifndef ECMTMESSAGEEVENT_H
#define ECMTMESSAGEEVENT_H

//  INCLUDES
#include <e32base.h>
#include "EcmtMessagingIf.h"

/**
*  Class representing the messages and message events of the system. Plugins
*  must not use this class directly, but use instead the MEcmtMessaging interface
*  methods for creating and accessing message events.
*
*  @ingroup EcmtCore
*/

class CEcmtMessageEvent : public CBase
    {
    public:  // Constructors and destructor
    
	    /**
	    * Two-phased constructor.
	    */
	    static CEcmtMessageEvent* NewL( TUid aDest, const TDesC8& aData );
    
	    /**
	    * Destructor.
	    */
	    virtual ~CEcmtMessageEvent();
    
    public:     // New functions
        /**
        *
        */
        const TUid Uid() const;
        const TPtrC8 Message( ) const;
        TPtr8 Message( );

    
    private:

	    /**
	    * C++ default constructor.
	    */
	    CEcmtMessageEvent();
	    
	    /**
	    * By default Symbian 2nd phase constructor is private.
	    */
	    void ConstructL( TUid aDest, const TDesC8& aData );

	    /**
	    *
	    */        
	    void SetHeader( );
	        
	    /**
	    *
	    */        
	    static CEcmtMessageEvent* New( TUid aDest, TInt aDataLen );
	    
    private:    // Data
        TUint8*			iData;
        TPtr8			iPtr;
        TUid			iDest;
        TSglQueLink		iLink;
        TMessageChannel iChannelId;
        
        friend class CEcmtMessaging;
    };

#endif
        
// End of File
