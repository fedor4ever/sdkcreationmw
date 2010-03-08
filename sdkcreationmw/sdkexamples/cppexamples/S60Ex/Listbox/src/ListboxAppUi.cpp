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


#include <avkon.hrh>
#include <aknnotewrappers.h>
#include <stringloader.h>
#include <Listbox.rsg>

#include "ListboxAppUi.h"
#include "ListboxNumberContainer.h"
#include "ListboxGraphicContainer.h"
#include "ListboxSettingView.h"
#include "Listbox.hrh"


// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CListboxAppUi::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CListboxAppUi::ConstructL()
    {
    // Enable skin and MSK.
    BaseConstructL(EAknEnableSkin | EAknEnableMSK );
    
    // Create view objects.
    iSingleNumberStyleView = CViewTemplate<CListboxNumberContainer>::NewL( 
        R_LISTBOX_NUMBER_VIEW, 
        0, 
        TUid::Uid( EListboxNumberView ) );    
    iSingleNumberStyleView->MenuItemToDimm( EListboxCommandToNumberStyle );
        
    iSingleGraphicStyleView = CViewTemplate<CListboxGraphicContainer>::NewL(
        R_LISTBOX_GRAPHIC_VIEW, 
        R_LISTBOX_GRAPHICVIEW_MSK_MENUBAR, 
        TUid::Uid( EListboxGraphicView ) );
    iSingleGraphicStyleView->MenuItemToDimm( EListboxCommandToGraphicStyle );
        
    iSettingView = CListboxSettingView::NewL();

    AddViewL( iSingleNumberStyleView );
    AddViewL( iSingleGraphicStyleView );
    AddViewL( iSettingView );
    
    SetDefaultViewL( *iSingleNumberStyleView );    
    }
    
// -----------------------------------------------------------------------------
// CListboxAppUi::CListboxAppUi()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CListboxAppUi::CListboxAppUi()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CListboxAppUi::~CListboxAppUi()
// Destructor.
// -----------------------------------------------------------------------------
//
CListboxAppUi::~CListboxAppUi()
    {
    }

// -----------------------------------------------------------------------------
// CListboxAppUi::HandleCommandL()
// Takes care of command handling.
// -----------------------------------------------------------------------------
//
void CListboxAppUi::HandleCommandL( TInt aCommand )
    {
    switch( aCommand )
        {
        case EListboxCommandToNumberStyle:
    	    {
    		ActivateLocalViewL( TUid::Uid( EListboxNumberView ) );  
    		}
    	    break;
        case EListboxCommandToGraphicStyle:
    		{	    		
    		ActivateLocalViewL( TUid::Uid( EListboxGraphicView ) );
    		}
    	    break;
        case EListboxCommandToSetting:
    		{
    		ActivateLocalViewL( TUid::Uid( EListboxSettingView ) );
    		}
    	    break;
        case EEikCmdExit:
        case EAknSoftkeyExit:
            Exit();
            break;
        default:
            break;
        }
    }

// -----------------------------------------------------------------------------
// CListboxAppUi::HandleResourceChangeL()
// Handles a change to the control's resources.
// -----------------------------------------------------------------------------
//
void CListboxAppUi::HandleResourceChangeL( TInt aType )
    {
    CAknAppUi::HandleResourceChangeL( aType );
       
    if ( aType==KEikDynamicLayoutVariantSwitch )
        {
        iSingleNumberStyleView->HandleSizeChange(aType);   
        iSingleGraphicStyleView->HandleSizeChange(aType); 
        iSettingView->HandleSizeChange(aType); 
        }           
    }

// End of File

