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
