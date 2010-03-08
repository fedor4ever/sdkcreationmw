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


#include <listbox.rsg>
#include "ListboxSettingView.h"
#include "ListboxSettingList.h"
#include "Listbox.hrh"
#include "ListboxAppUi.h"

// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------------------------
// CListboxSettingView::NewL()
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CListboxSettingView* CListboxSettingView::NewL()
    {
    CListboxSettingView* self = new (ELeave) CListboxSettingView;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// CListboxSettingView::CListboxSettingView()
// C++ default constructor
// ---------------------------------------------------------------------------
//
CListboxSettingView::CListboxSettingView()
    {
    }

// ---------------------------------------------------------------------------
// CListboxSettingView::~CListboxSettingView()
// Destructor
// ---------------------------------------------------------------------------
//
CListboxSettingView::~CListboxSettingView()
	{
	if ( iContainer )
        {
        AppUi()->RemoveFromStack( iContainer );
        delete iContainer;
        }     
	}

// ---------------------------------------------------------------------------
// CListboxSettingView::Id()
// Returns views id.
// ---------------------------------------------------------------------------
//
TUid CListboxSettingView::Id() const 
    { 
    return TUid::Uid( EListboxSettingView ); 
    }

// ---------------------------------------------------------------------------
// CListboxSettingView::HandleCommandL()
// Handles commands.
// ---------------------------------------------------------------------------
//
void CListboxSettingView::HandleCommandL( TInt aCommand )
    {
    switch( aCommand )
        {
        case EListboxCmdChange:
            iContainer->EditItemL( 0, ETrue );
            break;
        default:
            AppUi()->HandleCommandL(aCommand);  
            break;
        }
    }

// ---------------------------------------------------------------------------
// CListboxSettingView::DoActivateL()
// Activates this view.
// ---------------------------------------------------------------------------
//
void CListboxSettingView::DoActivateL( const TVwsViewId& /*aPrevViewId*/,
                      TUid /*aCustomMessageId*/,
                      const TDesC8& /*aCustomMessage*/ )
    {    
    iContainer = CListboxSettingList::NewL();
    
    iContainer->SetMopParent( this );
    iContainer->ConstructFromResourceL( R_LISTBOX_SETTING_ITEM_LIST );
    AppUi()->AddToStackL( *this, iContainer );
    
    iContainer->MakeVisible( ETrue );
    iContainer->SetRect( ClientRect() );
    iContainer->ActivateL();
    }

// ---------------------------------------------------------------------------
// CListboxSettingView::DoDeactivate()
// Deactivates this view
// ---------------------------------------------------------------------------
//
void CListboxSettingView::DoDeactivate()
    {
    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
        iContainer = NULL;
        }    
    }

// ---------------------------------------------------------------------------
// CListboxSettingView::ConstructL()
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CListboxSettingView::ConstructL()
    {
    BaseConstructL(R_LISTBOX_SETTINGS_VIEW);    
    }

// ---------------------------------------------------------------------------
// CListboxSettingView::DynInitMenuPaneL()
// This function is called by the EIKON framework just before it displays a 
// menu pane.
// ---------------------------------------------------------------------------
//
void CListboxSettingView::DynInitMenuPaneL( TInt aResourceId, 
                                           CEikMenuPane* aMenuPane )
    {
    if( aResourceId == R_LISTBOX_CHANGE_SUBMENU )
        {   
        aMenuPane->SetItemDimmed( EListboxCommandToSetting, ETrue );  
        }
    }   

// -----------------------------------------------------------------------------
// CListboxSettingView::HandleSizeChange()
// Called by HandleResourceChangeL() from CListboxAppUi when layout is 
// changed.
// -----------------------------------------------------------------------------
//
void CListboxSettingView::HandleSizeChange( TInt aType )
    {
    if( iContainer )
        {
        iContainer->HandleResourceChange( aType );
        
        if( aType==KEikDynamicLayoutVariantSwitch )
            {        
            TRect rect;
            AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, rect);
            iContainer->SetRect(rect);
            }
        }         
    } 
