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
#include <avkon.hrh>
#include <aknnotewrappers.h>
#include <stringloader.h>
#include <Graphics.rsg>

#include "Graphics.pan"
#include "GraphicsAppUi.h"
#include "GraphicsAppView.h"
#include "Graphics.hrh"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CGraphicsAppUi::CGraphicsAppUi()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CGraphicsAppUi::CGraphicsAppUi()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CGraphicsAppUi::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CGraphicsAppUi::ConstructL()
    {
    BaseConstructL(EAknEnableSkin);

    iAppView = CGraphicsAppView::NewL( ClientRect() );
    AddToStackL( iAppView );
    }

// -----------------------------------------------------------------------------
// CGraphicsAppUi::~CGraphicsAppUi()
// Destructor.
// -----------------------------------------------------------------------------
//
CGraphicsAppUi::~CGraphicsAppUi()
    {
    if ( iAppView )
        {
        iEikonEnv->RemoveFromStack( iAppView );
        delete iAppView;
        iAppView = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CGraphicsAppUi::HandleCommandL()
// Takes care of key event handling.
// -----------------------------------------------------------------------------
//
void CGraphicsAppUi::HandleCommandL( TInt aCommand )
    {
    switch( aCommand )
        {
        case EEikCmdExit:
        case EAknSoftkeyExit:
            Exit();
            break;

        case EGaphicsNoOffScreenDemo:
            iAppView->StartNoOffScreenDemo();
            break;

        case EGaphicsOffScreenDemo:
            iAppView->StartOffScreenDemo();
            break;

        case EGaphicsStopDemo:
            iAppView->StopDemo();
            break;

        default:
            break;
        }
    }

// End of File
