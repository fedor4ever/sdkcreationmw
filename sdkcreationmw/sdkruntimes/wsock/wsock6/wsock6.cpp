/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies).
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


#include "wsock.h"

#ifndef EKA2
GLDEF_C TInt E32Dll(TDllReason /*aReason*/)
{
    return(KErrNone);
}
#endif // !EKA2

// Entry point #1
extern "C"EXPORT_C CProtocolFamilyBase * Install()
{
    return WsockCreateProtocolFamily6();
}
