/*
* Copyright (c) 2005 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
#include "DynamicSettingListAppUi.h"
#include "DynamicSettingListAppView.h"
#include "DynamicSettingList.hrh"

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------
// CDynamicSettingListAppUi::ConstructL()
// ConstructL is called by the application framework
// ----------------------------------------------------------
//
void CDynamicSettingListAppUi::ConstructL()
    {
    BaseConstructL(EAknEnableSkin);

    iAppView = new (ELeave) CDynamicSettingListAppView;
    iAppView->SetMopParent(this);
    iAppView->ConstructL(ClientRect());

    AddToStackL(iAppView);
    }

// Constructor
CDynamicSettingListAppUi::CDynamicSettingListAppUi()                              
    {
	// no implementation required
    }

// Destructor
CDynamicSettingListAppUi::~CDynamicSettingListAppUi()
    {
    if (iAppView)
        {
        RemoveFromStack(iAppView);
        delete iAppView;
        iAppView = NULL;
        }
    }

// ----------------------------------------------------
// CDynamicSettingListAppUi::HandleCommandL()
// takes care of command handling
// ----------------------------------------------------
//
void CDynamicSettingListAppUi::HandleCommandL(TInt aCommand)
    {
    switch(aCommand)
        {
        case EEikCmdExit:
        case EAknSoftkeyExit:
            Exit();
            break;
      default:
            break;
        }
    }

// End of file