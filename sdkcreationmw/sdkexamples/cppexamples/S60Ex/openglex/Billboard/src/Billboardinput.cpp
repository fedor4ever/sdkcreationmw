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
* Description: 
*
*/


// INCLUDE FILES
#include <aknnotewrappers.h>
#include "billboardinput.h"

// MACROS
#define FRUSTUM_LEFT   -1.f     //left vertical clipping plane
#define FRUSTUM_RIGHT  +1.f     //right vertical clipping plane
#define FRUSTUM_BOTTOM -1.f     //bottom horizontal clipping plane
#define FRUSTUM_TOP    +1.f     //top horizontal clipping plane
#define FRUSTUM_NEAR   +3.f     //near depth clipping plane
#define FRUSTUM_FAR    +1000.f  //far depth clipping plane

// CONSTANTS

// ============================= MEMBER FUNCTIONS ==============================
// ============================= LOCAL FUNCTIONS ===============================
// ============================= MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CBillboardInput::NewL
// Symbian 2-phase constructor. Can leave.
// -----------------------------------------------------------------------------
//
CBillboardInput* CBillboardInput::NewL()
    {
    // Symbian 2-phase constructor. Calls both the default
    // C++ constructor and Symbian ConstructL methods
    CBillboardInput* self = new (ELeave) CBillboardInput();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// ----------------------------------------------------------------------------
// CBillboardInput::CBillboardInput
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
CBillboardInput::CBillboardInput()
    {
    }

// ----------------------------------------------------------------------------
// CBillboardInput::ConstructL
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CBillboardInput::ConstructL( void )
    {
    iInputPressed = new (ELeave) TBool[ MAX_NUM_INPUTS ];
    for( int i = 0; i < MAX_NUM_INPUTS; i++ )
        {
        iInputPressed[i] = EFalse;
        }
    }

// ----------------------------------------------------------------------------
// CBillboardInput::~CBillboardInput()
// Destructor.
// ----------------------------------------------------------------------------
//
CBillboardInput::~CBillboardInput()
    {
    delete iInputPressed;
    }

// ----------------------------------------------------------------------------
// CBillboardInput::KeyDown( const int aKeyCode )
// Sets the given keycode to pressed state.
// ----------------------------------------------------------------------------
//
TKeyResponse CBillboardInput::KeyDown( const int aKeyCode )
{
    TInt inputIdx = -1;
    for( int i = 0; i < KKeyCodeMapSize; i +=2 )
        {
        if( KKeyCodeMap[i] == aKeyCode )
            {
            inputIdx = KKeyCodeMap[i + 1];
            break;
            }
        }

    if( inputIdx >= 0 )
    {
        iInputPressed[ inputIdx ] = ETrue;
		return EKeyWasConsumed;
    }

    return EKeyWasNotConsumed;
}

// ----------------------------------------------------------------------------
// CBillboardInput::KeyUp( const int aKeyCode )
// Sets the given keycode to non-pressed state.
// ----------------------------------------------------------------------------
//
TKeyResponse CBillboardInput::KeyUp( const int aKeyCode )
{
    TInt inputIdx = -1;
    for( int i = 0; i < KKeyCodeMapSize; i +=2 )
        {
        if( KKeyCodeMap[i] == aKeyCode )
            {
            inputIdx = KKeyCodeMap[i + 1];
            break;
            }
        }

    if( inputIdx >= 0 )
    {
        iInputPressed[ inputIdx ] = EFalse;
		return EKeyWasConsumed;
    }

    return EKeyWasNotConsumed;
}

// ----------------------------------------------------------------------------
// CBillboardInput::IsInputPressed( Input aInput )
// Returns whether or not a given input is pressed at the moment or not.
// ----------------------------------------------------------------------------
//
TBool CBillboardInput::IsInputPressed( TUint aInput )
{
    return iInputPressed[ aInput ];
}


// End of File
