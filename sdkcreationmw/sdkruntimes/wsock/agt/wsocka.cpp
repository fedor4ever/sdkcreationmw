/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* which accompanies this distribution, and is available
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
*
*/


#define TRACE_PREFIX "WSOCKA: "
#include "wsock.h"

#ifndef EKA2
GLDEF_C TInt E32Dll(TDllReason /*aReason*/)
{
    return(KErrNone);
}
#endif // !EKA2

// Entry point #1
extern "C" EXPORT_C CNifAgentFactory * Install()
{
    return WsockCreateAgentFactory();
}
