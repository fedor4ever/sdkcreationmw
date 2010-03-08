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
#include "CCEWaitNote.h"

#include <CLFExample.rsg>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCEWaitNote::CCEWaitNote
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCEWaitNote::CCEWaitNote()
    {
    }

// -----------------------------------------------------------------------------
// CCEWaitNote::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCEWaitNote* CCEWaitNote::NewL()
    {
    CCEWaitNote* self = new (ELeave) CCEWaitNote;
    return self;
    }

// Destructor
CCEWaitNote::~CCEWaitNote()
    {
    if( iWaitDialog )
        {
        // Do not delete CAknWaitDialog object:
        // CAknWaitDialog::ProcessFinishedL should be used instead.
        TRAP_IGNORE( iWaitDialog->ProcessFinishedL() );
        }
    }

// -----------------------------------------------------------------------------
// CCEWaitNote::DialogDismissedL
// Gets called when the dialog is dismissed/closed.
// -----------------------------------------------------------------------------
//
void CCEWaitNote::DialogDismissedL( const TInt /*aButtonId*/ )
    {
    }

// -----------------------------------------------------------------------------
// CCEWaitNote::CloseWaitNoteL
// Close the wait note dialog.
// -----------------------------------------------------------------------------
//
void CCEWaitNote::CloseWaitNoteL()
    {
    // Close and delete the wait note dialog,
    // if it has not been dismissed yet
    if( iWaitDialog )
        {
        iWaitDialog->ProcessFinishedL();
        }
    }

// -----------------------------------------------------------------------------
// CCEWaitNote::OpenWaitNoteL
// Open the wait note dialog.
// -----------------------------------------------------------------------------
//
void CCEWaitNote::OpenWaitNoteL( const TInt aResourceId )
    {
    CloseWaitNoteL();

    // Create and view the wait note dialog
    iWaitDialog = new (ELeave) CAknWaitDialog(
        reinterpret_cast<CEikDialog**>( &iWaitDialog ) );
    iWaitDialog->SetCallback( this );
    iWaitDialog->ExecuteLD( aResourceId );
    }

//  End of File
