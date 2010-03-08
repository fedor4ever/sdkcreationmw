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


#include "CommandLineArguments.h"

CCommandLineArguments* CCommandLineArguments::NewL(void)
	{
	CCommandLineArguments* obj = new (ELeave) CCommandLineArguments;
	CleanupStack::PushL(obj);
	obj->ConstructL();
	CleanupStack::Pop(obj);
	return obj;
	}

CCommandLineArguments::CCommandLineArguments(): 
	iLocalPort(KErrNotFound), iRemoteHost(NULL), iRemotePort(KErrNotFound)
	{
	}

void CCommandLineArguments::ConstructL()
	{
	HBufC*   rawArgs = HBufC::New(User::CommandLineLength());
	if (rawArgs == NULL)
	        {
        	User::Leave(KErrNoMemory);
	        }
	TPtr ptr = rawArgs->Des();
	User::CommandLine(ptr);
   	TLex lex(ptr);
	//local port
   	TInt err = lex.Val(iLocalPort);
   	if(err != KErrNone)
   		{
		return;
   		}

	//remote host, port
	TPtrC16 host = lex.NextToken();
	iRemoteHost = host.Alloc() ;
	if(iRemoteHost->Length() != 0)
		{
		lex.Inc();
		err = lex.Val(iRemotePort);
   		if(err != KErrNone)
	   		{
			return;
   			}
		}
	}

CCommandLineArguments::~CCommandLineArguments()
	{
	if(iRemoteHost != NULL)
		{
		delete iRemoteHost;
		}
	}

TInt CCommandLineArguments::GetLocalPort()
	{
	return iLocalPort;
	}

TInt CCommandLineArguments::GetRemotePort()
	{
	return iRemotePort;
	}

HBufC16* CCommandLineArguments::GetRemoteHost()
	{
	return iRemoteHost;
	}


