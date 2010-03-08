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
#include <avkon.hrh>
#include <eikmenup.h>
#include <choicelistex.rsg>

#include "ChoiceListExAppUi.h"
#include "ChoiceListEx.hrh"
#include "ChoiceListExContainer.h"


// ============================ MEMBER FUNCTIONS ===============================
    
// -----------------------------------------------------------------------------
// CChoiceListExAppUi::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CChoiceListExAppUi::ConstructL()
    {
    BaseConstructL( EAknEnableSkin );
    
    iAppContainer = new ( ELeave ) CChoiceListExContainer();
    iAppContainer->SetMopParent( this );
    iAppContainer->ConstructL( ClientRect() );
    
    iMenuFlags.ClearAll();
    
    AddToStackL( iAppContainer );
    }

// -----------------------------------------------------------------------------
// CChoiceListExAppUi::CChoiceListExAppUi()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CChoiceListExAppUi::CChoiceListExAppUi()                              
    {
	// no implementation required
    }

// -----------------------------------------------------------------------------
// CChoiceListExAppUi::~CChoiceListExAppUi()
// Destructor.
// -----------------------------------------------------------------------------
//
CChoiceListExAppUi::~CChoiceListExAppUi()
    {
    if ( iAppContainer )
        {
        RemoveFromStack( iAppContainer );
        delete iAppContainer;
        }
    }

// -----------------------------------------------------------------------------
// CChoiceListExAppUi::HandleCommandL()
// Takes care of command handling.
// -----------------------------------------------------------------------------
//
void CChoiceListExAppUi::HandleCommandL( TInt aCommand )
    {
    switch( aCommand )
        {
        case EEikCmdExit:
        case EAknSoftkeyExit:
            Exit();
            break;
        
        case ECmdCreateDefault:
            iAppContainer->CreateDefaultChoiceListL();
            iMenuFlags.ClearAll();
            iMenuFlags.Set( ECmdCreateDefault );
            break;
        
        case ECmdCreateButton:
            iAppContainer->CreateButtonChoiceListL();
            iMenuFlags.ClearAll();
            iMenuFlags.Set( ECmdCreateButton );
            break;
            
        case ECmdCreateResource:
            iAppContainer->CreateResourceChoiceListL();
            iMenuFlags.ClearAll();
            iMenuFlags.Set( ECmdCreateResource );
            break;
            
        case ECmdShowList:
            iAppContainer->ShowListL();
            break;
                
        case ECmdAddItem:
            iAppContainer->AddItemL();
            break;
            
        case ECmdInsertItem:
            iAppContainer->InsertItemL();
            break;
            
        case ECmdRemoveItem:
            iAppContainer->RemoveItemL();
            break;
            
        case ECmdSelectItem:
            iAppContainer->SelectItemL();
            break;
        
        case ECmdListPosLeft:
            iAppContainer->ChangeListFlagsL( 
                CAknChoiceList::EAknChoiceListPositionLeft );
            break;
        case ECmdListPosRight:
            iAppContainer->ChangeListFlagsL(
                CAknChoiceList::EAknChoiceListPositionRight );
            break;
        
        case ECmdListPosBottom:
            iAppContainer->ChangeListFlagsL(
                CAknChoiceList::EAknChoiceListPositionBottom );
            break;
            
        default:
            return;
        }
    iAppContainer->DrawDeferred();        
    }

// -----------------------------------------------------------------------------
// CChoiceListExAppUi::DynInitMenuPaneL()
// Updates options menu
// -----------------------------------------------------------------------------
//
void CChoiceListExAppUi::DynInitMenuPaneL( TInt aMenuId, 
                                CEikMenuPane* aMenuPane )
    {
    // Options menu item dimming 
    if ( aMenuId == R_CHOICELISTEX_MENU )
        {
        TBool dim ( ETrue );
        if ( iMenuFlags.IsSet( ECmdCreateDefault ) || 
             iMenuFlags.IsSet( ECmdCreateButton) ||
             iMenuFlags.IsSet( ECmdCreateResource ))
            {
            dim = EFalse;
            }

        aMenuPane->SetItemDimmed( ECmdAddItem, dim );
        aMenuPane->SetItemDimmed( ECmdInsertItem, dim );
        aMenuPane->SetItemDimmed( ECmdRemoveItem, dim );
        aMenuPane->SetItemDimmed( ECmdSelectItem, dim );
        aMenuPane->SetItemDimmed( ECmdPositionMenu, dim );
        aMenuPane->SetItemDimmed( ECmdShowList, dim );
        aMenuPane->SetItemDimmed( ECmdShowList, dim );
        }
    }                                
                                

