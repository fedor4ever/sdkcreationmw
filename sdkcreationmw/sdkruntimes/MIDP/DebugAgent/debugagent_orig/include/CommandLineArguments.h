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


#ifndef COMMAND_LINE_ARGUMENT_HEADER
#define COMMAND_LINE_ARGUMENT_HEADER

	
#include <e32base.h>

class CCommandLineArguments
	{
public:
	~CCommandLineArguments();
	static CCommandLineArguments* NewL();
	TInt GetLocalPort();
	TInt GetRemotePort();
	HBufC16* GetRemoteHost();

private:
	CCommandLineArguments();
	void ConstructL();
	TInt iLocalPort;
	HBufC16* iRemoteHost;
	TInt iRemotePort;
	};

#endif