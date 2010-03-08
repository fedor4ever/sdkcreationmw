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
* Description:  View class for LogContainer.
*
*/


#include <EcmtGui.rsg>

#include "EcmtGuiLogView.h"
#include "EcmtGuiLogContainer.h"
#include "EcmtGui.hrh"
#include "EcmtGuiAppUi.h"



// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CEcmtGuiLogView* CEcmtGuiLogView::NewL()
    {
    CEcmtGuiLogView* self = new (ELeave) CEcmtGuiLogView;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// C++ default constructor
// ---------------------------------------------------------------------------
//
CEcmtGuiLogView::CEcmtGuiLogView()
    {
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CEcmtGuiLogView::~CEcmtGuiLogView()
	{
	if ( iContainer )
      {
      AppUi()->RemoveFromStack( iContainer );
      delete iContainer; 
      }    
  if( iMessageList )
      {
      iMessageList->Reset();
      delete iMessageList;
      }
	}
// ---------------------------------------------------------------------------
// From class CAknView.
// From CAknView.
// ---------------------------------------------------------------------------
//
TUid CEcmtGuiLogView::Id() const 
    { 
    return TUid::Uid(EEcmtLogViewId); 
    }

// ---------------------------------------------------------------------------
// From class CAknView.
// Handles commands.
// ---------------------------------------------------------------------------
//
void CEcmtGuiLogView::HandleCommandL( TInt aCommand )
    {
    switch( aCommand )
        {
        case EEcmtGuiCmdSettings:
            AppUi()->ActivateLocalViewL( TUid::Uid( EEcmtSettingsViewId ) );
            break;
            
        default:
            AppUi()->HandleCommandL( aCommand ); 
            break;            
        }     
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CEcmtGuiLogView::ConstructL()
    {
    BaseConstructL(R_ECMTGUI_LOG_VIEW);
    iMessageList = new (ELeave) CDesCArrayFlat(10);
    }
    
// ---------------------------------------------------------------------------
// From class CAknView.
// Activates view.
// ---------------------------------------------------------------------------
//
void CEcmtGuiLogView::DoActivateL( const TVwsViewId& /*aPrevViewId*/,
                      TUid /*aCustomMessageId*/,
                      const TDesC8& /*aCustomMessage*/ )
    {    
    iContainer = new (ELeave) CEcmtGuiLogContainer( *iMessageList );
    iContainer->SetMopParent( this );
    
    iContainer->ConstructL(ClientRect());
        
    AppUi()->AddToStackL( *this, iContainer ); 
    iContainer->MakeVisible( ETrue );     
    //Start EcmtServer
    if( !iConnected )
        AppUi()->HandleCommandL( EEcmtGuiCmdStart );
    }

// ---------------------------------------------------------------------------
// From class CAknView.
// Deactivates view
// ---------------------------------------------------------------------------
//
void CEcmtGuiLogView::DoDeactivate()
    {
    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
        iContainer = NULL;
        }    
    }

// ---------------------------------------------------------------------------
// From class MEikAutoMenuObserver.
// Dynamically initialises a menu pane.
// ---------------------------------------------------------------------------
//
void CEcmtGuiLogView::DynInitMenuPaneL( TInt aResourceId, 
                                        CEikMenuPane *aMenuPane )
    {
    if ( aResourceId == R_ECMTGUI_LOG_MENU )
        {
        aMenuPane->SetItemDimmed( EEcmtGuiCmdStart, iConnected );
        aMenuPane->SetItemDimmed( EEcmtGuiCmdStop, !iConnected );
        aMenuPane->SetItemDimmed( EEcmtGuiCmdClearLog, 
                            iMessageList->Count() < 2 );
                     
#ifndef _ENABLE_SETINGS_ON_WINS_       
#ifdef __WINS__
        aMenuPane->SetItemDimmed( EEcmtGuiCmdSettings, ETrue ); 
#endif //__WINS__               
#else   //_ENABLE_SETINGS_ON_WINS_ is now defined
        aMenuPane->SetItemDimmed( EEcmtGuiCmdSettings, EFalse );      
#endif //_ENABLE_SETINGS_ON_WINS_

        }
    }
    
// ---------------------------------------------------------------------------
// Adds log message to the listbox.
// ---------------------------------------------------------------------------
//
void CEcmtGuiLogView::LogMessageL( TInt aMsgResId )
    {
    if( iContainer )
        iContainer->LogMessageL( aMsgResId );    
    }

// ---------------------------------------------------------------------------
// Adds log message to the listbox.
// ---------------------------------------------------------------------------
//
void CEcmtGuiLogView::LogMessageL( const TDesC& aMsg )
    {
    if( iContainer )
        iContainer->LogMessageL( aMsg );    
    }

// ---------------------------------------------------------------------------
// Clears the log.
// ---------------------------------------------------------------------------
//
void CEcmtGuiLogView::ClearLogL()
    {
    iMessageList->Reset();
    //Update Listbox
    if( iContainer )
        iContainer->ClearMessageListL();
    }

// ---------------------------------------------------------------------------
// Setter for iConnected.
// ---------------------------------------------------------------------------
//
void CEcmtGuiLogView::SetConnected( TBool aConnected )
    {
    iConnected = aConnected;
    }

// ---------------------------------------------------------------------------
// Getter for Ecmt Server connectivity state.
// ---------------------------------------------------------------------------
//
TBool CEcmtGuiLogView::IsConnected()
    {
    return iConnected;  
    }

// ---------------------------------------------------------------------------
// Called by HandleResourceChangeL() from CEcmtGuiAppUi when layout 
// is changed.
// ---------------------------------------------------------------------------
//
void CEcmtGuiLogView::HandleSizeChange( TInt aType )
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

