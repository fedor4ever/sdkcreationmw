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
* Description: 
*
*/


#include <e32base.h>
#ifdef __WINS__
#include <estlib.h>
#include <estw32.h>
#endif

#include "CmdLineDriver.h"

IMPORT_C void RegisterWsExe(const TDesC &aName);

void DoL(void)
    {
	CCmdLineDriver* driver = CCmdLineDriver::NewL();
	//set agent port
    HBufC*   rawArgs = HBufC::New(User::CommandLineLength());

    if (rawArgs == NULL)
        {
        User::Leave(KErrNoMemory);
        }
    TPtr ptr = rawArgs->Des();

	User::CommandLine(ptr);
   	TLex lex(ptr);
   	TInt localPort;
   	TInt err = lex.Val(localPort);
   	if(err == KErrNone)
   		{
		driver->SetAgentPort(localPort);
   		}
   	else
	   	{
   		driver->SetAgentPort(KSeiSlavePort);	
   		}
	//ping if needed
	TPtrC16 host = lex.NextToken();
	if(host.Length() != 0)
		{
		TInt hostPort;
		lex.Inc();
		err = lex.Val(hostPort);
   		if(err == KErrNone)
   			{
   			driver->PingL(host, hostPort);
   			}
		}
	//run agent
	driver->InitL();
	delete driver;
    }

TInt E32Main(void)
    {
    CTrapCleanup* cleanupStack = CTrapCleanup::New();

    if (cleanupStack == NULL)
	    {
        return -1;
	    }


    TRAPD( error, DoL() );


    delete cleanupStack;
    return error;
    }
