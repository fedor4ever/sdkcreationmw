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
#include "clientimagecommander.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// RImageCommander::RImageCommander()
// Create a new animation object.
// -----------------------------------------------------------------------------
//
EXPORT_C RImageCommander::RImageCommander( RAnimDll &aAnimDll ) :
                                                              RAnim( aAnimDll )
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// RImageCommander::ImageConstruct()
// Construct a new animation object.
// -----------------------------------------------------------------------------
//
EXPORT_C void RImageCommander::ImageConstruct( const RWindowBase& aDevice,
                                             TInt aType, const TDesC8& aParams )
    {
    // Send the parameters to the animation server object construction
    RAnim::Construct( aDevice, aType, aParams );
    }

// -----------------------------------------------------------------------------
// RImageCommander::ImageCommand()
// Send a command (aCommand) to the animation server object.
// Because there is no way to return an error from the server side using this
// method, it is important that any server side code for these commands should
// not be able to fail or leave.  If the operation can fail, then the method
// TInt CommandReply(TInt aOpcode) should be used instead
// -----------------------------------------------------------------------------
//
EXPORT_C void RImageCommander::ImageCommand( TInt aCommand )
    {
    // Could optionally decode the commands here
    RAnim::Command( aCommand );
    }

// End of File
