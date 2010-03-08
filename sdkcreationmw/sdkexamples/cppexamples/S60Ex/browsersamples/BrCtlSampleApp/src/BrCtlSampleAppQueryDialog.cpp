/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Display a Query Dialog
*
*/


// INCLUDE FILES

#include <avkon.hrh>

#include "BrCtlSampleAppQueryDialog.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CBrCtlSampleAppQueryDialog::CBrCtlSampleAppQueryDialog( TDes& aDefInput, HBufC*& aRetBuf )
    : CAknTextQueryDialog( aDefInput )
    , iRetBuf( aRetBuf )
	{
	}

// ---------------------------------------------------------
// CBrCtlSampleAppQueryDialog::PreLayoutDynInitL
// ---------------------------------------------------------
//
void  CBrCtlSampleAppQueryDialog::PreLayoutDynInitL()
	{
    CAknTextQueryDialog::PreLayoutDynInitL();

    MakeLeftSoftkeyVisible( ETrue );
    } 

// ---------------------------------------------------------
// CBrCtlSampleAppQueryDialog::OkToExitL
// ---------------------------------------------------------
//
TBool CBrCtlSampleAppQueryDialog::OkToExitL( TInt aKeycode )
	{
	switch ( aKeycode )
		{
		case EAknSoftkeyOk:
			{
            CAknQueryControl* control = QueryControl();

            __ASSERT_DEBUG( control, User::Panic( KNullDesC, KErrGeneral ) );
            
            iRetBuf = HBufC::NewL( control->GetTextLength() + 1);
            TPtr temp( iRetBuf->Des() );
			control->GetText( temp );
            temp.ZeroTerminate();
			}
			// no break !!! same return value
		case EAknSoftkeyCancel:
			return ETrue;
		default:
			return EFalse;
		}
    }

// ---------------------------------------------------------
// CBrCtlSampleAppQueryDialog::HandleQueryEditorStateEventL
//
// This code is base on AknQueryDialog.cpp
// ---------------------------------------------------------
//
TBool CBrCtlSampleAppQueryDialog::HandleQueryEditorStateEventL(CAknQueryControl* /*aQueryControl*/, 
	TQueryControlEvent /*aEventType*/, TQueryValidationStatus /*aStatus*/)
    {
    return EFalse;
    }

//  End of File

