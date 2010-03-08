/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  SEI Forward Plugin
*
*/


#ifndef ECMTSOCKETLISTENER_H
#define ECMTSOCKETLISTENER_H

#include <e32base.h>
#include "EcmtMessagingIf.h"
#include "SymbianSocket.h"

/**
*  @ingroup EcmtPlugins
*/
class CSocketListener : public CActive, public MSocketContainer
	{
	public:
		static CSocketListener* NewL( TInt aChannel, RSocketServ* aSocketServ, CSeiForwardPlugin* aParent );
		~CSocketListener( );
		TInt GetListeningPort();

	public:
		void Remove( CSymbianSocket* aSocket );
		void ForwardL( CSymbianSocket* aSocket, TPtrC8 aMessage );
		void SendMessageL( TPtrC8 aMessage );

	protected:
		// from CActive
		void	RunL();
		TInt	RunError( TInt aError );
		void	DoCancel();
		
	private:
		CSocketListener( TInt aChannel, RSocketServ* aSocketServ, CSeiForwardPlugin* aParent );
		void ConstructL();
		void AcceptL();
		
	private:
    	enum TPluginState
    	{
    		EIdle,
			EAccepting
		};
	
		//Cannot be a reference since it is given only after construction
		CSeiForwardPlugin*				iParent;
		TPluginState					iState;
		RSocketServ*					iSocketServer;
		RSocket							iSocket;
		RSocket*						iBlankSocket;
        RPointerArray< CSymbianSocket> 	iSocketList;
        TInt							iListeningPort;
        TInt							iChannel;
	};
#endif

