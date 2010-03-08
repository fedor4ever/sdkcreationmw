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


#ifndef HEADER_SHUTDOWN_OBJECT
#define HEADER_SHUTDOWN_OBJECT

#include <e32base.h>

class CShutdownObject: public CActive
	{
public:

	static CShutdownObject* NewL(void);

public:

	virtual ~CShutdownObject();

	virtual void DoCancel(void);
	virtual void RunL(void);

	void Shutdown(void);
	TInt Shutdown(TRequestStatus&);

private:

	CShutdownObject();

private:

	RThread			iHandle;
	TRequestStatus*	iCallerStatus;
	};

#endif /* !_MIDP_SHUTDOWN_OBJECT_H_ */
