/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
/** \file CommonFramework.h */

//
// Copyright (c) 2000 Symbian Ltd.  All rights reserved.
//

#ifndef __CommonFramework_H
#define __CommonFramework_H

#include <e32base.h>
#include <e32cons.h>

_LIT(KTxtEPOC32EX,"EXAMPLES");
_LIT(KTxtExampleCode,"Symbian OS Example Code");
_LIT(KFormatFailed,"failed: leave code=%d");
_LIT(KTxtOK,"ok");
_LIT(KTxtPressAnyKey,"\n[press any key]\n");

// public
LOCAL_D CConsoleBase* console; // write all your messages to this
LOCAL_C void doExampleL(); // code this function for the real example

// private
LOCAL_C void callExampleL(); // initialize with cleanup stack, then do example

/** \fn TInt E32Main()
 *
 * main function called by E32.
 * Creates clean-up stack and calls example initialization function
 */
GLDEF_C TInt E32Main()
{
    __UHEAP_MARK;
    CTrapCleanup* cleanup=CTrapCleanup::New(); // get clean-up stack
    TRAPD(error,callExampleL()); // more initialization, then do example
    __ASSERT_ALWAYS(!error,User::Panic(KTxtEPOC32EX,error));
    delete cleanup; // destroy clean-up stack
    __UHEAP_MARKEND;
    return 0; // and return
}

/** \fn LOCAL_C void callExampleL()
 *
 * creates console and calls example code under cleanup stack
 */
LOCAL_C void callExampleL()
{
    console=Console::NewL(KTxtExampleCode,TSize(KConsFullScreen,KConsFullScreen));
    CleanupStack::PushL(console);
    TRAPD(error,doExampleL()); // perform example function
    if (error)
        console->Printf(KFormatFailed, error);
    else
        console->Printf(KTxtOK);
    console->Printf(KTxtPressAnyKey);
    console->Getch(); // get and ignore character
    CleanupStack::PopAndDestroy(); // close console
}

#endif
