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
#ifndef __DEBUGLOGGING_H
#define __DEBUGLOGGING_H


#include "flogger.h"
#include "USBSocket.h"

#ifdef _ENABLE_USB_LOGGINGS_
#define __LOGWRITE(X)       RFileLogger::Write( _L("EcmtServer"), _L("EcmtServer.log"), EFileLoggingModeAppend, _L(X));
						    
#define __LOGWRITE1(X,A)    RFileLogger::WriteFormat(_L("EcmtServer"), _L("EcmtServer.log"), EFileLoggingModeAppend, _L(X), A);
#else
#define __LOGWRITE(X)       ((void)0)
#define __LOGWRITE1(X,A)    ((void)0)
#endif // _ENABLE_USB_LOGGINGS_


#endif //__DEBUGLOGGING_H