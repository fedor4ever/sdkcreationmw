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
* Description:  Implementation for HierarchicalColumnListAppUi class.
*
*/


#include <avkon.hrh>
#include <aknnotewrappers.h> 

#include "HierarchicalColumnList.pan"
#include "HierarchicalColumnListAppUi.h"
#include "HierarchicalColumnListAppView.h"
#include "HierarchicalColumnList.hrh"
#include <HierarchicalColumnList.rsg>

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ConstructL is called by the application framework.
// ---------------------------------------------------------------------------
//
void CHierarchicalColumnListAppUi::ConstructL()
    {
    BaseConstructL( EAknEnableSkin | EAknEnableMSK );

    iAppView = CHierarchicalColumnListAppView::NewL( ClientRect() ); 
    iAppView->SetMopParent( this );

    AddToStackL( iAppView );
    }


CHierarchicalColumnListAppUi::CHierarchicalColumnListAppUi()                              
    {
	// no implementation required
    }


CHierarchicalColumnListAppUi::~CHierarchicalColumnListAppUi()
    {
    if ( iAppView )
        {
        RemoveFromStack( iAppView );
        delete iAppView;
        iAppView = NULL;
        }
    }


// ---------------------------------------------------------------------------
// From class CAknAppUi.
// Dynamically initialises a menu pane.
// ---------------------------------------------------------------------------
//
void CHierarchicalColumnListAppUi::DynInitMenuPaneL( TInt aResourceId,
    CEikMenuPane* aMenuPane )
    {
    if ( aResourceId == R_HIERARCHICALCOLUMNLIST_MENU )
        {        
        iAppView->DynInitOptionsMenuPaneL( aMenuPane );
        }
    else if ( aResourceId == R_HIERARCHICALCOLUMNLIST_SETTINGS_MENU )
        {
        iAppView->DynInitSettingsMenuPaneL( aMenuPane );
        }
    else if ( aResourceId == R_AVKON_MENUPANE_MARKABLE_LIST_IMPLEMENTATION )
        {
        iAppView->DynInitMarkingMenuPaneL( aMenuPane );
        }
    }


// ---------------------------------------------------------------------------
// From class CAknAppUi.
// Handle any menu commands
// ---------------------------------------------------------------------------
//
void CHierarchicalColumnListAppUi::HandleCommandL( TInt aCommand )
    {
    switch( aCommand )
        {
        case EEikCmdExit:
        case EAknSoftkeyExit:
            Exit();
            break;

        case EHclExampleCmdExpand:
            iAppView->ExpandFocusedNode();
            break;

        case EHclExampleCmdCollapse:
            iAppView->CollapseFocusedNode();
            break;

        case EHclExampleCmdOpen:
            iAppView->OpenFocusedItem();
            break;

        case EHclExampleCmdChangeLooping:
            iAppView->ChangeLooping();
            break;

        case EHclExampleCmdChangeMarquee:
            iAppView->ChangeMarquee();
            break;

        case EHclExampleCmdChangeMarkable:
            iAppView->ChangeMarkable();
            break;

        case EHclExampleCmdChangeIndicators:
            iAppView->ChangeFunctionIndicatorsL();
            break;

        case EAknCmdMark:
            iAppView->Mark();
            break;

        case EAknCmdUnmark:
            iAppView->Unmark();
            break;

        case EAknMarkAll:
            iAppView->MarkAll();
            break;
        
        case EAknUnmarkAll:
            iAppView->UnmarkAll();
            break;

        default:
            break;
        }
    }


// ---------------------------------------------------------------------------
// From class CAknAppUi.
// Handles foreground event.
// ---------------------------------------------------------------------------
//
void CHierarchicalColumnListAppUi::HandleForegroundEventL( TBool aForeground )
    {
    if ( aForeground )
        {
        AddToStackL( iAppView );
        }
    else
        {
        RemoveFromStack( iAppView );
        }
    }


// ---------------------------------------------------------------------------
// From class CAknAppUi.
// Handles resource changes.
// ---------------------------------------------------------------------------
//
void CHierarchicalColumnListAppUi::HandleResourceChangeL( TInt aType )
    {
    CAknAppUi::HandleResourceChangeL( aType );
    if ( iAppView  )
        {
        if ( aType == KEikDynamicLayoutVariantSwitch )
            {
            iAppView->SetRect( ClientRect() );
            }
        iAppView->HandleResourceChange( aType );
        }
    }



