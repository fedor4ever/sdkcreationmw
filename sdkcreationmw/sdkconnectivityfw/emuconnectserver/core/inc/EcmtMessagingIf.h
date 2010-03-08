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
* Description:  Interface class of messaging component
*
*/


#ifndef MECMTMESSAGINGIF_H
#define MECMTMESSAGINGIF_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CEcmtMessageEvent;

typedef TUint8 TMessageChannel;

/**
*  Interface of messaging observer for getting notifications of
*  received messages and other message events.
*
*  Plugins implement this method if they want to receive messages.
*
*  @ingroup EcmtCore
*/
class MEcmtMessageEventObserver
    {
    public:
    	/**
    	*  Called by the core when a message event is received. Currently
    	*  the only message events are arrivals of new messages.
    	*  @param aEvent Reference to the message event to be handled. Use
    	*  MEcmtMessaging::Message() method to retrieve the body of the message.
    	*/
        virtual void HandleNotifyL( const CEcmtMessageEvent& aEvent ) = 0;
    };

/**
*  Interface of message sender for sending messages.
*
*  Plugins obtain this interface by calling MEcmtMessaging::GetMessageSender()
*  if they want to send messages.
*
*  @ingroup EcmtCore
*/
class MEcmtMessageSender
    {
    public:
    	/**
    	*  Method for sending a previously created message. Message are queued in
    	*  the core and sent in FIFO order.
    	*  @param aMessage pointer to the message event to be sent. Ownership is
    	*  transferred to the core which deletes the message event after it has been
    	*  succesfully sent.
    	*/
        virtual void SendMessage( CEcmtMessageEvent* aMessage ) = 0;
        
        /**
    	*  Method for sending a previously created message with no queuing. The core does
    	*  not queue message with the same destination Uid and identical (non-zero) channel 
    	*  id. Instead, a newly sent message with non-zero channel id replaces any previous
    	*  message with same Uid and channel id. In this way, plugins that rapidly generate
    	*  a lot of data (such as the task manager) can refrain from consumin large 
    	*  amouts of buffer space by instructing the core to keep only the last message 
    	*  they have sent.
    	*  @param aMessage pointer to the message event to be sent. Ownership is
    	*  transferred to the core which deletes the message event after it has been
    	*  succesfully sent.
    	*  @param aChannelId Channel id for the message.
        */
        virtual void SendMessage( CEcmtMessageEvent* aMessage, TMessageChannel aChannelId ) = 0;
    };

/**
*  Interface of messaging component.
*
*  Plugins receive this interface in their BindMessaging() method and can then:
*  1) Register themselves as observers for incoming messages using SetMessageEventObserverL
*     if they want to receive messages.
*  2) Get the MEcmtMessageSender interface if they want to send messages.
*  3) Create a new message event using NewMessageEvent.
*  4) Get the message data from an incoming message event using Message()
*
*  @ingroup EcmtCore
*/
class MEcmtMessaging
    {
    public:
        /**
         *  Registers the caller as an observer of message events. The only message event type
         *  currently supported is the arrival of a new message.
         *  @param aObserver Reference to the MEcmtMessageEventObserver interface.
         *  @param aUid Uid of messages for which the message events are requested.
         */
        virtual void AddMessageEventObserverL( MEcmtMessageEventObserver& aObserver, TUid aUid ) = 0;

        /**
         *  Removes the caller from the message observer list for the specified UID.
         *  If none of the observer list members match both the observer and the UID, 
         *  nothing is done.
         *  @param aObserver Reference to the MEcmtMessageEventObserver interface.
         *  @param aUid Uid of messages for which the message events were requested.
         */
        virtual void RemoveMessageEventObserver( MEcmtMessageEventObserver& aObserver, TUid aUid ) = 0;
        
        /**
        *  Retrieves the MEcmtMessageSender interface that is used to send messages.
        *  @return Pointer to the MEcmtMessageSender interface that can be used to send messages.
        */
        virtual MEcmtMessageSender* GetMessageSender() = 0;
        
        /**
        *  Constructs a new message that is destined to the specified Uid and that contains
        *  the specified data in the body of the message.
        *  @param aDest Uid of the destination.
        *  @param aData Descriptor that contains the data for the body of the message.
        *  @return Pointer to the created message or 0 in out of memory condition.
        */
        virtual CEcmtMessageEvent* NewMessageEvent( TUid aDest, const TDesC8& aData ) = 0;

        /**
        *  Constructs a new message that is destined to the specified Uid and allocates
        *  the specified amount of memory for the body of the message. A descriptor
        *  through which the message body can be modified can be obtained with the
        *  Message() method.
        *  
        *  @param aDest Uid of the destination.
        *  @param aDataLen Length (in bytes) of the space to be reserved for the body
        *  of the message.
        *  @return Pointer to the created message or 0 in out of memory condition.
        */
        virtual CEcmtMessageEvent* NewMessageEvent( TUid aDest, TInt aDataLen ) = 0;
        
        /**
        *  Returns a non modifiable descriptor that represents the body of the message.
        *  @param aMessage Reference to a message event.
        *  @return Const descriptor that represents the body of the message.
        */
        virtual const TPtrC8 Message( const CEcmtMessageEvent& aMessage ) = 0;
        
        /**
        *  Returns a modifiable descriptor that represents the body of the message. This
        *  method can only be used on message events created by the plugin itself. It
        *  is typically used after having created a message event of a specified length
        *  with the NewMessageEvent( TUid, TInt) overload to enter the message data in 
        *  various steps. In this way there is no need for a buffer in the plugin's memory
        *  space since the memory reserved in the messaging system can be used directly.
        *  @param aMessage Reference to a modifiable message event.
        *  @return A modifiable descriptor that represents the body of the message. 
        */
        virtual TPtr8 Message( CEcmtMessageEvent& aMessage ) = 0;
    };

#endif
        
// End of File
