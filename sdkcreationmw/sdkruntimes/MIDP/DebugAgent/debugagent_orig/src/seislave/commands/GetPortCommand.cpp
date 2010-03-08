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


#include "SeiCommands.h"

#include <es_sock.h>
#include <in_sock.h>



CGetPortCommand* CGetPortCommand::NewL(
    CDriver* aDriver, 
    CSocketReader* aReader,
    CSocketWriter* aWriter)
    {
    CGetPortCommand* object = new (ELeave) CGetPortCommand(aDriver, aReader, aWriter);
    CleanupStack::PushL(object);
    object->ConstructL();
    CleanupStack::Pop(object);
    return object;
    }

CGetPortCommand::CGetPortCommand(
    CDriver* aDriver, 
    CSocketReader* aReader,
    CSocketWriter* aWriter)
    : CSeiCommand(aDriver, aReader, aWriter)
    {
    }

CGetPortCommand::~CGetPortCommand()
    {
	iFreePorts.Close();
    }

void CGetPortCommand::ConstructL()
    {
    }

void CGetPortCommand::DoReadL()
    {
	iFreePortsNum = iReader->ReadIntL();
    }

void CGetPortCommand::DoExecuteL()
    {
	TInt errorCode = KErrNone;
	RSocketServ	socketServer;
	errorCode = socketServer.Connect();
	if(errorCode  == KErrNone)
		{
	    RSocket listeningSocket;
		TSockAddr localAddress;
		//TODO: add retrying to find ports
		for(int i = 0; errorCode == KErrNone && i < iFreePortsNum; i++)
			{
			TInt port = iDriver->NextRandomPort();
			listeningSocket.Open(socketServer, KAfInet, KSockStream, KProtocolInetTcp);
			localAddress.SetPort(port); 
			errorCode = listeningSocket.Bind(localAddress);
			if(errorCode == KErrNone)
				{
				iFreePorts.Append(port);
				}
			listeningSocket.Close();
			}
		}
	socketServer.Close();
	User::LeaveIfError(errorCode);
    }

void CGetPortCommand::DoWriteL()
    {
		for(int i = 0; i < iFreePortsNum; i++)
		{
			iWriter->WriteIntL(iFreePorts[i]);
		}
    }



