/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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

#include <avkon.hrh>
#include "HelloWorldBasicQueryDialog.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CHelloWorldQueryDialog::CHelloWorldQueryDialog( TDes& aBuf, HBufC *aDefInput )
    : CAknTextQueryDialog( aBuf )
    , iDefInput(*aDefInput)
    {
    }

// ---------------------------------------------------------
// CHelloWorldQueryDialog::PreLayoutDynInitL
// ---------------------------------------------------------
//
void  CHelloWorldQueryDialog::PreLayoutDynInitL()
    {
    // first we have to execute PreLayoutDynInitL() of the base-class
    CAknTextQueryDialog::PreLayoutDynInitL();

    // acquire pointer to editor-control and set the default input.
    CAknQueryControl* control = QueryControl();
    control->SetTextL(iDefInput);

    // enable OK-button, so that default text can be accepted as it is
    // without modifying the text
    MakeLeftSoftkeyVisible( ETrue );
    }

//  End of File

