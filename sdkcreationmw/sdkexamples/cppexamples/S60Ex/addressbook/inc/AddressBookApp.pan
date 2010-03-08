/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ADDRESSBOOK application panic codes
*
*/


#ifndef ADDRESSBOOKAPP_PAN
#define ADDRESSBOOKAPP_PAN

// DATA TYPES
enum TAddressBookAppPanics 
    {
    EAddressBookAppUi = 1
    // add further panics here
    };


inline void Panic(TAddressBookAppPanics aReason)
    {
	_LIT(applicationName,"AddressBookApp");
    User::Panic(applicationName, aReason);
    }

#endif // ADDRESSBOOKAPP_PAN

// End of File
