/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
#include <longtapdetectorexample.rsg>

#include "longtapdetectorexappui.h"
#include "longtapdetectorex.hrh"
#include "longtapdetectorexcontainer.h"


// ============================ MEMBER FUNCTIONS ===============================
    
// -----------------------------------------------------------------------------
// CLongTapDetectorExAppUi::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLongTapDetectorExAppUi::ConstructL()
    {
    BaseConstructL( EAknEnableSkin );
    
    iAppContainer = new ( ELeave ) CLongTapDetectorExContainer();
    iAppContainer->SetMopParent( this );
    iAppContainer->ConstructL( ClientRect() );
    
    AddToStackL( iAppContainer );
    }

// -----------------------------------------------------------------------------
// CLongTapDetectorExAppUi::CLongTapDetectorExAppUi()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CLongTapDetectorExAppUi::CLongTapDetectorExAppUi()                              
    {
	// no implementation required
    }

// -----------------------------------------------------------------------------
// CLongTapDetectorExAppUi::~CLongTapDetectorExAppUi()
// Destructor.
// -----------------------------------------------------------------------------
//
CLongTapDetectorExAppUi::~CLongTapDetectorExAppUi()
    {
    if ( iAppContainer )
        {
        RemoveFromStack( iAppContainer );
        delete iAppContainer;
        }
    }

// -----------------------------------------------------------------------------
// CLongTapDetectorExAppUi::HandleCommandL()
// Takes care of command handling.
// -----------------------------------------------------------------------------
//
void CLongTapDetectorExAppUi::HandleCommandL( TInt aCommand )
    {
    switch( aCommand )
        {
        case EEikCmdExit:
        case EAknSoftkeyExit:
            Exit();
            break;
        default:
            return;
        }
    iAppContainer->DrawDeferred();        
        }
                                    
// ----------------------------------------------------------------------------
// void CLongTapDetectorExAppUi::HandleResourceChangeL( TInt aType )
// Handles resource changes.
// ----------------------------------------------------------------------------
//
/*void CLongTapDetectorExAppUi::HandleResourceChangeL( TInt aType )
    {
    CAknAppUi::HandleResourceChangeL( aType );
    if  ( aType == KEikDynamicLayoutVariantSwitch )
    	{
        iAppContainer->SizeChanged();
        }
    }*/
