/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Application UI class, CDirectoryLocalizerExUi -
*                responsible for handling user commands
*
*/


#include <avkon.hrh>
#include <aknsutils.h>
#include <aknnotewrappers.h>
#include <CDirectoryLocalizer.h>

#include "directorylocalizerexappui.h"
#include "directorylocalizerex.hrh"
#include "directorylocalizerexcontainer.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ctro do nothing
// ---------------------------------------------------------------------------
//
CDirectoryLocalizerExAppUi::CDirectoryLocalizerExAppUi()
    {
    }

// ---------------------------------------------------------------------------
// symbian 2nd phase ctor
// ---------------------------------------------------------------------------
//
void CDirectoryLocalizerExAppUi::ConstructL()
    {
    BaseConstructL();

    iAppContainer = new (ELeave) CDirectoryLocalizerExContainer();
    iAppContainer->SetMopParent( this );
    iAppContainer->ConstructL( ClientRect());
    AddToStackL( iAppContainer );

    }

// ----------------------------------------------------------------------------
// CDirectoryLocalizerExAppUi::~CDirectoryLocalizerExAppUi()
// Destructor.
// ----------------------------------------------------------------------------
//
CDirectoryLocalizerExAppUi::~CDirectoryLocalizerExAppUi()
    {
    if (iAppContainer)
        {
        RemoveFromStack( iAppContainer );
        delete iAppContainer;
        iAppContainer = NULL;
        }
    }

// ----------------------------------------------------------------------------
// handle menu command events
// ----------------------------------------------------------------------------
//
void CDirectoryLocalizerExAppUi::HandleCommandL( TInt aCommand )
    {
    switch ( aCommand )
        {
        case EAknSoftkeyBack:

        case EDirLocalizerAddFromResId:
            {
            iAppContainer->LocalizedFromResIdL();
            break;
            }
        case EDirLocalizerSetupFromResRd:
            {
            iAppContainer->SetupFromResRdL();
            break;

            }
        case EDirLocalizerSetupFromResId:
            {
            iAppContainer->SetupFromResIdL();
            break;

            }
        case EDirLocalizerGetExtraData:
            {
            iAppContainer->GetExtraDataL();
            break;

            }

        case EDirLocalizerGetIconIndex:
            {
            iAppContainer->GetIconIndexL();
            break;

            }
        case EAknSoftkeyExit:
        case EEikCmdExit:
            {
            Exit();
            return;
            }
        default:
            break;
        }
    }

// End of File
