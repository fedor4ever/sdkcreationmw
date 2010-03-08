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
* Description:  Container class for log view.
*
*/


#include <EcmtGui.rsg>
#include <stringloader.h>

#include "EcmtGuiLogContainer.h"
#include "EcmtGuiAppUi.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// C++ default constructor
// ---------------------------------------------------------------------------
//
CEcmtGuiLogContainer::CEcmtGuiLogContainer( CDesCArrayFlat& aMessageList ) 
  : iMessageList( aMessageList )
    {
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CEcmtGuiLogContainer::~CEcmtGuiLogContainer()
    {
    if( iListBox )
      delete iListBox;
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CEcmtGuiLogContainer::ConstructL(const TRect& aRect)
    {
    iAutoScroll = ETrue;
    
    CreateWindowL();
       
    SetRect( aRect );
    
    iListBox = new (ELeave) CAknDoubleStyleListBox(); 
    iListBox->SetContainerWindowL( *this ); 
    iListBox->ConstructL( NULL, EAknListBoxViewerFlags );
    
    // Screate scroll bars
    iListBox->CreateScrollBarFrameL( ETrue );
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOn, CEikScrollBarFrame::EAuto );
	  
    // Give it to the control
    iListBox->Model()->SetItemTextArray( &iMessageList );
    iListBox->Model()->SetOwnershipType( ELbmDoesNotOwnItemArray );

    iListBox->SetRect( aRect );
	
    // Activate the window, which makes it ready to be drawn
    iListBox->ActivateL();
    ActivateL();
    }

// ---------------------------------------------------------------------------
// Loads message from resource and calls AddMessageL.
// ---------------------------------------------------------------------------
//
void CEcmtGuiLogContainer::LogMessageL( TInt aMsgResId )
    {
    TBuf<KMaxLogMsgLen> msg;
    StringLoader::Load( msg, aMsgResId );
    LogMessageL( msg );
    }

// ---------------------------------------------------------------------------
// Calls AddMessageL
// ---------------------------------------------------------------------------
//
void CEcmtGuiLogContainer::LogMessageL( const TDesC& aMsg )
    {
    iLastMsg.SetLength(0);
    _LIT( KTimeFormat, "\t%:0%J%:1%T%:2%S% " );

    TTime curTime;
    curTime.HomeTime();
    curTime.FormatL( iLastMsg, KTimeFormat );
    iLastMsg.Append( aMsg );

    AddMessageL(iLastMsg, iAutoScroll);
    }

// ---------------------------------------------------------------------------
// Adds message to the listbox and to the message list iMessageList.
// ---------------------------------------------------------------------------
//
void CEcmtGuiLogContainer::AddMessageL( const TDesC& aText, TBool aAutoScroll )
    {
    iMessageList.AppendL( aText );
    iListBox->HandleItemAdditionL();
    if ( aAutoScroll )
        {
    	iListBox->SetCurrentItemIndexAndDraw( iMessageList.MdcaCount() - 1 );
        }
    }

// ---------------------------------------------------------------------------
// Clears logs from the listbox.
// ---------------------------------------------------------------------------
//
void CEcmtGuiLogContainer::ClearMessageListL()
    {
    iListBox->HandleItemRemovalL();
    iListBox->Reset();
    }

// ---------------------------------------------------------------------------
// From class CCoeControl.
// Draws the control.
// ---------------------------------------------------------------------------
//
void CEcmtGuiLogContainer::Draw(const TRect& /*aRect*/) const
    {
      
    }

// ---------------------------------------------------------------------------
// From class CCoeControl.
// Responds to changes to the size and position of the contents of this 
// control.
// ---------------------------------------------------------------------------
//
void CEcmtGuiLogContainer::SizeChanged()
    {
    
    }

// ---------------------------------------------------------------------------
// From class CCoeControl.
// Responds to changes to the size and position of the contents of this 
// control.
// ---------------------------------------------------------------------------
//
TInt CEcmtGuiLogContainer::CountComponentControls() const
    {
   	return 1;
    }

// ---------------------------------------------------------------------------
// From class CCoeControl.
// Gets an indexed component of a compound control.
// ---------------------------------------------------------------------------
//
CCoeControl* CEcmtGuiLogContainer::ComponentControl(TInt aIndex) const
    {
    switch( aIndex )
        {                
        case 0:
            return iListBox; 
        default:
            return NULL;
        }
    }
    
