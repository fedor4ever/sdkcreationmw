/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef MSGCMDLST_H
#define MSGCMDLST_H

#include <e32base.h>
#include <e32std.h>
#include <e32cons.h>
#include <es_sock.h>

const TInt KSockBufferSize	= 1024;
const TInt KBufferSize	= 2048;
const TInt KTimeOutTime = 180000000; 


class CMessage;
class CMsgCommandConnection;
class CMsgSvrEng;
/**
* Abstract Base class of the Message Handler class that provides
* siplified interface to contorol Handler.
**/
class MMsgCommandObserver
    {
    private:
	   friend class CMsgCommandConnection;
       friend class CMsgSvrEng;
    private:
       /**
       * Pure virtual method to Process Send Command.
       * Derived class must implement this.
       * 
       * @param Reference to CMessage class containging data to be send.
       */
	   virtual void ProcessSendL(CMessage &aMsg) const =0;
	   
	   /**
       * Pure virtual method to Process receive Command.
       * Derived class must implement this.
       * 
       * @param Reference to CMessage class containging data to be received.
       */
	   virtual void ProcessReceiveL(CMessage &aMsg)=0;
	   
	   /**
       * Pure virtual method that cane be used to deliver a some data to be
       * output to UI.
       * Derived class must implement this.
       * 
       * @param Reference to TDesC descriptor containging data to be send.
       */
	   virtual void WriteToConsole(const TDesC &aMsg) = 0;
    };


/**
* Command timeout timer
**/
class CTimeOutTimer;


/**
* Connection class that takes care of socket connection. Class parses
* incomming commands and notifies MMsgCommandObserver class of them.
* Class provides also reply method for repling to client.
**/
class CMsgCommandConnection : public CActive
    {
    public:
       /**
       * Basic C++ Constructor
       */
       CMsgCommandConnection(); 
       
       /**
       * Basic C++ destructor
       */
	   virtual ~CMsgCommandConnection();

	   /**
       * EPOC default constructor.
       
       * @param aCmdLst_if pointer to MMsgCommandObserver class. 
       *        Method sets iNotify member variable.
       *
       * @param aEngine ponter to CMsgSvrEng class,
       *        Method sets iEngine member variable.
       */
	   void ConstructL(
	            MMsgCommandObserver* aCmdLst_if,
				CMsgSvrEng* aEngine);
	   
	   /**
       * Two phase constructor. Intializes instance of this class and returns
       * a pointer of itself.
       *
       * @return Returns a pointer of it self and sets itself to to top of
       *         the CleanupStack.
       *
       * @param aCmdLst_if pointer to MMsgCommandObserver class. 
       *        Method sets iNotify member variable.
       *
       * @param aEngine ponter to CMsgSvrEng class,
       *        Method sets iEngine member variable.
       */
	   static CMsgCommandConnection* NewLC(
                MMsgCommandObserver* aCmdLst_if,
                CMsgSvrEng* aEngine);
	   
	   /**
       * Two phase constructor. Intializes instance of this class and returns
       * a pointer of itself.
       *
       * @return Returns a pointer of it self.
       *
       * @param aCmdLst_if pointer to MMsgCommandObserver class. 
       *        Method sets iNotify member variable.
       *
       * @param aEngine ponter to CMsgSvrEng class,
       *        Method sets iEngine member variable.
       */
	   static CMsgCommandConnection* NewL(
		        MMsgCommandObserver* aCmdLst_if,
                CMsgSvrEng* aEngine);
       
       /**
       * Connection starts listening for incomming Commands.
       */         
	   void StartConnection();
      
       /**
       * Sends Message reply to the connected client.
       */
       void Reply(CMessage& aMsg);
       
       /**
       * Sends Descritor reply to the connected client.
       * Usually is is Status code
       */
       void Reply(const TDesC8& aValue);
       
    private:
       /**
       * Enumerated list of commands that are handled
       */ 
       enum TCommands{
	        ENoCommand = -1,
	        ESendSms,
	        EReceiveSms,
        	EStopServer = 1000};
       /**
       * Enumerated list of state machine states of the connection class.
       */
       enum TConState
	        {
		    EWaitingCommand,
            EReceivingCommand,
            EReceivedCommand,
            EClosingConnection,
            ESendReply,
            ESendError
	        };
    public:
       /**
       * Signals RunL with an error code.
       */
       void SignalRunL(TInt aError);
    private:
       /**
       * Implements CActive DoCancel pure virtual function.
       */
       void DoCancel();
       
       /**
       * Handles State machine  state after command, reply or error has been
       * occured.
       */
       void RunL();
       
       /**
       * Allows remote shutdown of the server.
       * Stop command handling is not yet implemented.
       */
	   void Stop();
	   
	   /**
       * Implements CActive RunError pure virtual function.
       */
       TInt RunError(TInt aError);
       
       /**
       * Sets connection to receive commands. Sets this object active.
       */
       void WaitForCommand();
       
       /**
       * Parses incomming command.
       */
       TInt ParseCommandL();
       
       /**
       * Process incomming command by activating Sending/Receiving.
       */
       TInt ProcessCommandL();
       
       /**
       * TimeOutTimer activates after timeout if timer is active.
       */
       void ConnectionTimeOutL();
    private:
       //Interface to server
	   MMsgCommandObserver* iNotify;
	   // pointer to the engine class
	   // this class is its friend
	   CMsgSvrEng* iEngine;
	   // pointer to iEngine->iCommandSocket
       RSocket* iSocket;
 	   // command timeout timer  
       CTimeOutTimer* iTimer;
    private:
       TCommands iCommand;
       TConState iConState;
    private:
       // timeout
       TTimeIntervalMicroSeconds32 TimeOutTime;
       // incomming messag buffer
       TBuf8<KSockBufferSize> iSockBuf;
       // command data descriptor
       TBuf8<KBufferSize> iCmdData;
       // reply data descriptor
	   TBuf8<KBufferSize> iReplyData;
	   // number of actual bits read
       TSockXfrLength iLen;
       // Message object
       CMessage *iMsg;
    private:
        friend class CTimeOutTimer;
        
    };
#endif //MSGCMDLST_H