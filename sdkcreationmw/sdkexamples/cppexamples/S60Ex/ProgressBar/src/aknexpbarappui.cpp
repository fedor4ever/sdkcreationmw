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
#include <AknExPbar.rsg>
#include <avkon.hrh>
#include <aknutils.h> 

#include "AknExPbarAppUi.h"
#include "AknExPbar.hrh"
#include "AknExPbarContainer.h"


// ========================= MEMBER FUNCTIONS ==================================

// -----------------------------------------------------------------------------
// CAknExPbarAppUi::CAknExPbarAppUi()
// C++ default constructor can NOT contain any code, that might leave. 
// -----------------------------------------------------------------------------
//
CAknExPbarAppUi::CAknExPbarAppUi() 
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CAknExPbarAppUi::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAknExPbarAppUi::ConstructL()
    {
    // Initialise app UI with standard value.
    BaseConstructL( EAknEnableSkin );
    
    iContainer = CAknExPbarContainer::NewL( ClientRect() );
    AddToStackL( iContainer );
    }

// -----------------------------------------------------------------------------
// CAknExPbarAppUi::~CAknExPbarAppUi()
// Destructor.
// -----------------------------------------------------------------------------
//
CAknExPbarAppUi::~CAknExPbarAppUi()
    {
    if ( iContainer )
        {
        RemoveFromStack( iContainer );
        delete iContainer;
        iContainer = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CAknExPbarAppUi::HandleKeyEventL()
// Takes care of key event handling.
// -----------------------------------------------------------------------------
//
void CAknExPbarAppUi::HandleCommandL( TInt aCommand )
    {
    TInt increment = KInitIncrement;
    
    switch ( aCommand )
        {
        case EAknCmdExit:
        case EAknSoftkeyExit:
        case EEikCmdExit:
            Exit();
            return; // returns if exit

        case EProgCmdChangeSetFinalValue1:
            iContainer->SetFinalValue();
            break;

        case EProgCmdChangeShowInfo0:
            iContainer->ShowInfo( EAknExpBarBar0 );
            break;

        case EProgCmdChangeShowInfo1:
            iContainer->ShowInfo( EAknExpBarBar1 );
            break;

        default:
            break;
        }
        
    if ( increment )
        {
        iContainer->IncrementBarsAndDraw( increment );
        }
   
    }

// -----------------------------------------------------------------------------
// CAknExPbarAppUi::HandleResourceChangeL()
// Called by framework when layout is changed.
// -----------------------------------------------------------------------------
//
void CAknExPbarAppUi::HandleResourceChangeL( TInt aType )
    {
    CAknAppUi::HandleResourceChangeL( aType );
       
    if ( aType==KEikDynamicLayoutVariantSwitch )
        {
        TRect rect;
        AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, rect);
        iContainer->SetRect( rect );
        iContainer->ReConstructProgressBarsL();
        }   
    //Controls derived from CCoeControl, handled in container class
    iContainer->HandleResourceChange( aType );                   
    }

// End of File  
