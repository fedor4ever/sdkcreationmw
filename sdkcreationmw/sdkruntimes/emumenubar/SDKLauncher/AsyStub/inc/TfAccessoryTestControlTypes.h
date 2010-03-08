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

#ifndef __TFACCESSORYTESTCONTROLTYPES_H__
#define __TFACCESSORYTESTCONTROLTYPES_H__

#include <e32base.h>
#include <AccPolGenericid.h>


const TUid    KTFAccessoryTestProperty=     {0x10200c70};
const TUint32 KTFAccessoryMethod          =  0x00001003;

enum TTFASYReference
    {
    ETFAsyNone=0,
    ETFAsyConn,
    ETFAsyDisc    
    };

struct TTFAccessoryPublishAndSubscribe
    {
    TTFASYReference                iMethod;
    TInt64                         iParam1;
    TAccPolGenericID               iGenericID;
    };

#endif
