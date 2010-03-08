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

#ifndef NCIMSGSVR_H
#define NCIMSGSVR_H

#include <e32base.h>
#include <e32cons.h>

LOCAL_D CConsoleBase* console;                  // messages to console

/**
* LOCAL_C void startNciMsgSrvL()  
* Instantiates CMsgSvr,
* Starts Activescheduler (loop) .
* Destroys CMsgSvr and CMsgScheduler after Active scheduler stops.
*/ 
LOCAL_C void startNciMsgSrvL();                

/** 
* Creates consol and calls startNciMsgSvrL.  
*/
LOCAL_C void callNciMsgSvrL();                  

// Literal Constants
_LIT(KTxtServerApplication, "NCI Message Server");
_LIT(KTxtServerStarting, "Starting server...\n");
_LIT(KTxtServerClosing, "...Closing server.\n");
_LIT(KFormatFailed,"failed: leave code=%d\n");
_LIT(KTxtPressAnyKey," [press any key]");

/** 
* Main function called by E32.
* Gets CleanupStack and calls callNciMsgSvrL under it
*/

GLDEF_C TInt E32Main()
    {
	__UHEAP_MARK;
	CTrapCleanup* cleanup=CTrapCleanup::New();  // get clean-up stack
	TRAPD( error,callNciMsgSvrL() );            // more initialization
	__ASSERT_ALWAYS( !error,User::Panic(KTxtServerApplication,error) );
	delete cleanup;                             // destroy clean-up stack
	__UHEAP_MARKEND;
	return 0;                                   // and return
    }

/**
* Initialize console call startNciMsgSvrL under cleanup stack
* Cleanup console after startNciMsgSvrL returns
* 
*/

LOCAL_C void callNciMsgSvrL() 
    {
	console=Console::NewL(
        KTxtServerApplication,
        TSize(KConsFullScreen,KConsFullScreen));
	CleanupStack::PushL(console);
	console->Printf(KTxtServerStarting);
	TRAPD(error,startNciMsgSrvL());             // start running msg server
	if (error)
		console->Printf(KFormatFailed, error);
	else
		console->Printf(KTxtServerClosing);
	console->Printf(KTxtPressAnyKey);
	console->Getch();                           // get and ignore character
	CleanupStack::PopAndDestroy();              // close console
    }

#endif