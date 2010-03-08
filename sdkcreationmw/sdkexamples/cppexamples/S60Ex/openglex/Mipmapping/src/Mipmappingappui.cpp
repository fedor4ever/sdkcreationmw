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
#include "MipmappingAppUi.h"
#include "MipmappingContainer.h"
#include <Mipmapping.rsg>
#include "Mipmapping.hrh"

#include <avkon.hrh>

// ================= MEMBER FUNCTIONS =======================
//
// ----------------------------------------------------------
// CMipmappingAppUi::ConstructL()
// ----------------------------------------------------------
//
void CMipmappingAppUi::ConstructL()
    {
    BaseConstructL();
    iAppContainer = new (ELeave) CMipmappingContainer;
    iAppContainer->SetMopParent(this);
    iAppContainer->ConstructL( ClientRect(), this );
    AddToStackL( iAppContainer );
    }

// ----------------------------------------------------
// CMipmappingAppUi::~CMipmappingAppUi()
// Destructor. Frees any reserved resources.
// ----------------------------------------------------
//
CMipmappingAppUi::~CMipmappingAppUi()
	{
    if ( iAppContainer )
		{
        RemoveFromStack( iAppContainer );
        delete iAppContainer;
		}
	}

// ------------------------------------------------------------------------------
// CMipmappingAppUi::::DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane)
//  This function is called by the EIKON framework just before it displays
//  a menu pane. Its default implementation is empty, and by overriding it,
//  the application can set the state of menu items dynamically according
//  to the state of application data.
// ------------------------------------------------------------------------------
//
void CMipmappingAppUi::DynInitMenuPaneL(
    TInt /*aResourceId*/,CEikMenuPane* /*aMenuPane*/)
    {
    }

// ----------------------------------------------------
// CMipmappingAppUi::HandleKeyEventL(
//     const TKeyEvent& aKeyEvent,TEventCode /*aType*/)
// ----------------------------------------------------
//
TKeyResponse CMipmappingAppUi::HandleKeyEventL(
    const TKeyEvent& /*aKeyEvent*/,TEventCode /*aType*/)
    {
    return EKeyWasNotConsumed;
    }

// ----------------------------------------------------
// CMipmappingAppUi::HandleCommandL(TInt aCommand)
// ----------------------------------------------------
//
void CMipmappingAppUi::HandleCommandL(TInt aCommand)
    {
    switch ( aCommand )
        {
        case EAknSoftkeyBack:
        case EEikCmdExit:
            {
            Exit();
            break;
            }
        case EMipmappingMipmappedBilinearFiltering:
            iAppContainer->iMipmapping->SetFilteringMode( CMipmapping::EMipmappedBilinearFiltering );
            break;
        case EMipmappingMipmappedTrilinearFiltering:
            iAppContainer->iMipmapping->SetFilteringMode( CMipmapping::EMipmappedTrilinearFiltering );
            break;
        case EMipmappingMipmappedNearestFiltering:
            iAppContainer->iMipmapping->SetFilteringMode( CMipmapping::EMipmappedNearestNeighbourFiltering );
            break;
        case EMipmappingBilinearFiltering:
            iAppContainer->iMipmapping->SetFilteringMode( CMipmapping::EBilinearFiltering );
            break;
        case EMipmappingNearestFiltering:
            iAppContainer->iMipmapping->SetFilteringMode( CMipmapping::ENearestNeighbourFiltering );
            break;
        default:
            break;
        }
    }

// End of File
