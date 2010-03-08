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


#ifndef _WSOCK_INTERFACE_FACTORY_H_
#define _WSOCK_INTERFACE_FACTORY_H_

#include <nifif.h>

class CWinsockInterfaceFactory : public CNifIfFactory
{
public:
    CWinsockInterfaceFactory();

protected:
    virtual ~CWinsockInterfaceFactory();

    // CNifIfFactory
    virtual CNifIfBase *NewInterfaceL(const TDesC& aName);
    virtual CNifIfBase *NewInterfaceL(const TDesC& aName, MNifIfNotify* aNotify);
    virtual TInt Info(TNifIfInfo& aInfo, TInt aIndex) const;

    // CNifFactory
    virtual void InstallL();
};

#endif // _WSOCK_INTERFACE_FACTORY_H_
