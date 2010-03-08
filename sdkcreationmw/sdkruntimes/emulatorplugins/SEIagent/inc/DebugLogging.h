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

#define __LOGWRITE(X) RFileLogger::Write(_L("SEIAgent"), _L("SeiAgent.log"), EFileLoggingModeAppend, _L(X));\
						RDebug::Print( _L(X) );	
#define __LOGWRITE1(X,A) RFileLogger::WriteFormat(_L("SeiAgent"), _L("SeiAgent.log"), EFileLoggingModeAppend, _L(X), A);\
							RDebug::Print( _L(X) );


#endif //__DEBUGLOGGING_H