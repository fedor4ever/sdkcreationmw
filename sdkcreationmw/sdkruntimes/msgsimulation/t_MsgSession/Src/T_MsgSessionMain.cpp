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


//  Include Files  

#include "T_MsgSessionApp.h"      // CT_MsgSessionApplication
#include "T_MsgSession.pan"       // TT_MsgSessionPanic

/****************************************************************************
*   E32Dll(TDllReason aReason)
*****************************************************************************
*  
*   Called when the DLL is loaded and unloaded. Note: have to define
*   epoccalldllentrypoints in MMP file to get this called in THUMB.
*
****************************************************************************/
GLDEF_C TInt E32Dll(TDllReason /*aReason*/)
    {
    return KErrNone;
    }
/****************************************************************************
*   Panic(TT_MsgSessionPanic aPanic)
*****************************************************************************
*
*   Panics the thread with given panic code
*
****************************************************************************/
GLDEF_C void Panic(TT_MsgSessionPanic aPanic)
    {
    User::Panic(_L("T_MSGSESSION"), aPanic);
    }
/****************************************************************************
*   NewApplication()
*****************************************************************************
*
*   Called by framework, entry point to new application
*
****************************************************************************/
EXPORT_C CApaApplication* NewApplication()
    {
    return new CT_MsgSessionApplication;
    }
// End of file
