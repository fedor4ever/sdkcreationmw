/*
* Copyright (c) 2004 - 2006 Nokia Corporation and/or its subsidiary(-ies).
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


// INCLUDE FILES
#include "clientdll.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// RClientDll::RClientDll()
// Animation client dll construction code all encapsulated in original class.
// -----------------------------------------------------------------------------
//
EXPORT_C RClientDll::RClientDll( RWsSession& aSession )
    : RAnimDll( aSession )
    {
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// End of File
