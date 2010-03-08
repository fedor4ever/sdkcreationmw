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
* Description:  View class for setting list
*
*/


#include <EcmtGui.rsg>

#include "EcmtGuiSettingsView.h"
#include "EcmtGuiSettingList.h"
#include "EcmtGui.hrh"
#include "EcmtGuiAppUi.h"
#include <aknnotewrappers.h>

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CEcmtGuiSettingsView* CEcmtGuiSettingsView::NewL()
    {
    CEcmtGuiSettingsView* self = new (ELeave) CEcmtGuiSettingsView;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// C++ default constructor
// ---------------------------------------------------------------------------
//
CEcmtGuiSettingsView::CEcmtGuiSettingsView()
    {
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CEcmtGuiSettingsView::~CEcmtGuiSettingsView()
	{
	if ( iContainer )
        {
        AppUi()->RemoveFromStack( iContainer );
        delete iContainer;
        }     
	}

// ---------------------------------------------------------------------------
// From class CAknView.
// Returns views id.
// ---------------------------------------------------------------------------
//
TUid CEcmtGuiSettingsView::Id() const 
    { 
    return TUid::Uid(EEcmtSettingsViewId); 
    }

// ---------------------------------------------------------------------------
// From class CAknView.
// Handles commands.
// ---------------------------------------------------------------------------
//
void CEcmtGuiSettingsView::HandleCommandL( TInt aCommand )
    {
    switch( aCommand )
        {
        case EEcmtGuiCmdChange:
            iContainer->EditItemL( 0, ETrue );
            break;
        case EAknSoftkeyBack:
		    		iContainer->StoreSelectionL();
            AppUi()->HandleCommandL( EEcmtGuiCmdStop );
            AppUi()->HandleCommandL( EEcmtGuiCmdClearLog );       
            AppUi()->ActivateLocalViewL( TUid::Uid( EEcmtLogViewId ) );               
            break;
        default:
            AppUi()->HandleCommandL(aCommand);  
            break;
        }
    }

// ---------------------------------------------------------------------------
// From class CAknView.
// Activates this view.
// ---------------------------------------------------------------------------
//
void CEcmtGuiSettingsView::DoActivateL( const TVwsViewId& /*aPrevViewId*/,
                      TUid /*aCustomMessageId*/,
                      const TDesC8& /*aCustomMessage*/ )
    {    
    iContainer = new (ELeave) CEcmtGuiSettingList();
    
    iContainer->SetMopParent( this );
    iContainer->ConstructFromResourceL( R_ECMTGUI_SETTING_ITEM_LIST );
    iContainer->SetDataL();
    AppUi()->AddToStackL( *this, iContainer );
    
    iContainer->MakeVisible( ETrue );
    iContainer->SetRect( ClientRect() );
    iContainer->ActivateL();
    iContainer->DrawNow();    
    }

// ---------------------------------------------------------------------------
// From class CAknView.
// Deactivates this view
// ---------------------------------------------------------------------------
//
void CEcmtGuiSettingsView::DoDeactivate()
    {
    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
        iContainer = NULL;
        }    
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CEcmtGuiSettingsView::ConstructL()
    {
    BaseConstructL(R_ECMTGUI_SETTINGS_VIEW);    
    }

// ---------------------------------------------------------------------------
// Called by HandleResourceChangeL() from CEcmtGuiAppUi when layout 
// is changed.
// ---------------------------------------------------------------------------
//
void CEcmtGuiSettingsView::HandleSizeChange( TInt aType )
    {
    if( iContainer )
        {
        iContainer->HandleResourceChange( aType );
        
        if ( aType==KEikDynamicLayoutVariantSwitch )
            {        
            TRect rect;
            AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, rect);
            iContainer->SetRect(rect);
            }
        }       
    }
    
