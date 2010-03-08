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
#include "EcmtMessagingIf.h"

// CONSTANTS

// CLASS DECLARATION

class CSocketForwardPlugin;

/**
* CSymbianSocket class.
* 
*/
class CSymbianSocket : public CActive
    {
    public:
    	static CSymbianSocket* NewL( CSocketForwardPlugin* aParent, RSocket* aSocket, MEcmtMessaging* aMessaging );
    	void ConnectL();
    	~CSymbianSocket();

	protected:
		void RunL();
		void DoCancel();

    private:
    	CSymbianSocket( CSocketForwardPlugin* aParent, RSocket* aSocket, MEcmtMessaging* aMessaging );
    	void ConstructL();
    	void Read();
    	void ForwardL();
    	
	// data

	private:
    	enum TPluginState
    	{
    		EIdle,
			EReading,
			EDone
		};

		CSocketForwardPlugin*	iParent;
		RSocket*				iSocket;
		MEcmtMessaging*			iMessaging;
		TUint8*					iData;
		TPtr8					iReadBuffer;
		TSockXfrLength			iReadLen;
		TPluginState			iState;
    };

#endif

// End of File

