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
* Description:  Class implementing messaging functionality
*
*/


#ifndef CECMTMESSAGING_H
#define CECMTMESSAGING_H

//  INCLUDES
#include "EcmtMessagingIf.h"
#include "EcmtMessageEvent.h" 
#include "EcmtCommsEngineIf.h"
#include "EcmtMessageFormat.h"

// CLASS DECLARATION

/**
 *  @ingroup EcmtCore
 */
class MEcmtMessagingObserver
{
public:
    virtual void HandleMessagingNotify( TInt aCode ) = 0;
};

/**
 *  Class implementing messaging functionality.
 *  @ingroup EcmtCore
 */
class CEcmtMessaging :	public CBase, 
                        public MEcmtMessaging,
                        public MEcmtMessageSender,
                        public MEcmtCommsReaderObserver,
                        public MEcmtCommsWriterObserver
{
public:  // Constructors and destructor
    
    /**
    * Two-phased constructor.
    */
    static CEcmtMessaging* NewL( MEcmtMessagingObserver& aObserver );
    
    /**
    * Destructor.
    */
    virtual ~CEcmtMessaging();

public: // Functions from base classes

    /**
     * From MEcmtMessaging
     */
    void AddMessageEventObserverL( MEcmtMessageEventObserver& aObserver, TUid aUid );
    void RemoveMessageEventObserver( MEcmtMessageEventObserver& aObserver, TUid aUid );

    MEcmtMessageSender* GetMessageSender();
    CEcmtMessageEvent* NewMessageEvent( TUid aDest, const TDesC8& aData );
    CEcmtMessageEvent* NewMessageEvent( TUid aDest, TInt aDataLen );        
    const TPtrC8 Message( const CEcmtMessageEvent& aMessage );        
    TPtr8 Message( CEcmtMessageEvent& aMessage );        
        
    /**
     * From MEcmtMessageSender
     */
    void SendMessage( CEcmtMessageEvent* aMessage );
    void SendMessage( CEcmtMessageEvent* aMessage, TMessageChannel aChannelId );
        
    /**
     * From MEcmtCommsReaderObserver
     */
    void HandleCommsReaderNotify( TEcmtCommsObjectStatus aStatus );

    /**
     * From MEcmtCommsWriteerObserver
     */
    void HandleCommsWriterNotify( TEcmtCommsObjectStatus aStatus );

    void SetReader( MEcmtCommsReader* aReader );
    void SetWriter( MEcmtCommsWriter* aWriter );
        
private:

    /**
     * C++ default constructor.
     */
    CEcmtMessaging( MEcmtMessagingObserver& aObserver );

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();
    
    TBool ParseHeader( TUint32& aUid, TUint32& aLen );
    TBool ParseInt( const TPtrC8 aStr, TUint32& aVal );
    void ResetReceiver();

private:    // Data

    MEcmtMessagingObserver&           iObserver;
    TSglQue<CEcmtMessageEvent>        iMessageQue;
    MEcmtCommsReader*                 iCommsReader;
    MEcmtCommsWriter*                 iCommsWriter;
    TBuf8<KEcmtMessageHeaderLength>   iHeader;
    TPtr8                             iPayload;
    CEcmtMessageEvent*                iIncomingMsg;
    TBool                             iMessageIsOutgoing;
    RArray<TUid>                      iObserverUidList;
    RPointerArray< MEcmtMessageEventObserver > iObserverList;

    enum TReceiverState
    {
        EIdle,
        EReceivingHeader,
        EReceivingBody
    };

    TReceiverState  iReceiverState;
};

#endif
        
// End of File
