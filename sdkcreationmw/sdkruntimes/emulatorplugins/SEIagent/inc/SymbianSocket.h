/*
* ============================================================================
*  Name     : CSymbianSocket from SymbianSocket.h
*  Part of  : HelloEcmt
*  Created  : 28.4.2005
*  Description:
*     Declares main application class.
*  Version  :
*  Copyright: 
* ============================================================================
*/

#ifndef SYMBIANSOCKET_H
#define SYMBIANSOCKET_H

// INCLUDES
#include <e32base.h>
#include <es_sock.h>
#include <in_sock.h>
#include "SocketContainerIf.h"

// CONSTANTS

// CLASS DECLARATION

class CSeiForwardPlugin;

/**
* CSymbianSocket class.
* 
*/
class CSymbianSocket : public CActive
    {
    public:
    	static CSymbianSocket* NewL( MSocketContainer* aParent, RSocketServ* aSocketServ, RSocket* aSocket );
    	static CSymbianSocket* NewL( MSocketContainer* aParent, RSocketServ* aSocketServ, TInt aPort );
    	void ConnectL();
    	void SendL( TPtrC8 aMessage );
    	~CSymbianSocket();

	protected:
		void RunL();
		void DoCancel();

    private:
    	CSymbianSocket( MSocketContainer* aParent, RSocketServ* aSocketServ, RSocket* aSocket );
    	void ConstructForReadL();
    	void ConstructForWriteL( TInt aPort );
    	void Read();
    	void ConnectL( TInt aPort );
    	void ForwardL();
    	
	// data

	private:
    	enum TPluginState
    	{
    		EIdle,
    		EConnecting,
			EReading,
			EWriting,
			EDone
		};

		MSocketContainer*		iParent;
		RSocketServ*			iSocketServer;
		RSocket*				iSocket;
		TUint8*					iData;
		TPtr8					iReadBuffer;
		TSockXfrLength			iReadLen;
		TPluginState			iState;
    };

#endif

// End of File

