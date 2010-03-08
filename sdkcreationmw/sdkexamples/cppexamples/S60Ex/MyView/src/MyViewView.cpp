/*
* Copyright (c) 2002 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Defines first view, Local activation and message commands are 
*                given in HandleCommandL
*
*/


// INCLUDE FILES
#include  <aknviewappui.h>
#include  <avkon.hrh>
#include  <MyView.rsg>
#include  "MyViewView.h"
#include  "MyViewView2.h" //For view2 id
#include  "MyViewContainer.h" 
#include  "myview.hrh"


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMyViewView::ConstructL(const TRect& aRect)
// EPOC two-phased constructor
// ---------------------------------------------------------
//
void CMyViewView::ConstructL(CAknTabGroup* aTabGroup)
    {
    // construct R_MYVIEW_VIEW1 resources
    BaseConstructL( R_MYVIEW_VIEW1 );
    iTabGroup=aTabGroup;
    }

// ---------------------------------------------------------
// CMyViewView::~CMyViewView()
// Default destructor
// ---------------------------------------------------------
//
CMyViewView::~CMyViewView()
    {
    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        }

    delete iContainer;
    }

// ---------------------------------------------------------
// TUid CMyViewView::Id()
// This returns the view ID
// ---------------------------------------------------------
//
TUid CMyViewView::Id() const
    {
    return KViewId;
    }

// ---------------------------------------------------------
// CMyViewView::HandleCommandL(TInt aCommand)
// Here we handle commands for this view. 
// Each view has their own HandleCommandL()
// ---------------------------------------------------------
//
void CMyViewView::HandleCommandL(TInt aCommand)
    {   
    switch ( aCommand )
        {
        case EAknSoftkeyOk:
            {
            iEikonEnv->InfoMsg( _L("view1 ok") );
            break;
            }
        case EAknSoftkeyExit:
            {
            AppUi()->HandleCommandL(EEikCmdExit);
            break;
            }

        case EMyViewCmdView1Cmd1:
            {
            // Update the correct tab in the navipane.
            // View Management System does not adjust the navi pane tabs.
            iTabGroup->SetActiveTabByIndex(1);

            // Switch to view 2            
            AppUi()->ActivateLocalViewL(KView2Id);
            break;
            }
        case EMyViewCmdView1Cmd2:
            {
            // Update the correct tab in the navipane.
            // View Management System does not adjust the navi pane tabs.
            iTabGroup->SetActiveTabByIndex(1);

            // Switch to view 2 and send a message Uid. Message is set to null.
            const TUid KCustomMessageUid= { 2 };    // MessageUid to send
            AppUi()->ActivateLocalViewL(KView2Id, KCustomMessageUid, KNullDesC8);
            break;
            }
        case EMyViewCmdView1Cmd3:
            {
            // Update the correct tab in the navipane.
            // View Management System does not adjust the navi pane tabs.
            iTabGroup->SetActiveTabByIndex(1);

            // Switch to view 2 and send a message Uid and a message
            const TUid KCustomMessageUid= { 2 };    // MessageUid to send
            // Construct a message to send
            TBuf8<255> customMessage;
            customMessage.Copy(_L8("Some data here"));
            AppUi()->ActivateLocalViewL(KView2Id, KCustomMessageUid, customMessage);
            break;
            }

        default:
            {
            AppUi()->HandleCommandL( aCommand );
            break;
            }
        }
    }

// ---------------------------------------------------------
// CMyViewView::HandleClientRectChange()
// ---------------------------------------------------------
//
void CMyViewView::HandleClientRectChange()
    {
    if ( iContainer )
        {
        iContainer->SetRect( ClientRect() );
        }
    }

// ---------------------------------------------------------
// CMyViewView::DoActivateL(...)
// This is called when a view needs to be activated. 
// This creates container with its controls.
// It also receives messages sent to the view.
// aPrevViewId is passed s.t. we can return to the previous view.
// aCustomMessageId is an ID for the message.
// aCustomMessage is the actual message.
// ---------------------------------------------------------
//
void CMyViewView::DoActivateL(
   const TVwsViewId& /*aPrevViewId*/,TUid /*aCustomMessageId*/,
   const TDesC8& /*aCustomMessage*/)
    {
    if (!iContainer)
        {
        iContainer = new (ELeave) CMyViewContainer;
        iContainer->SetMopParent(this);
        iContainer->ConstructL( ClientRect() );
        AppUi()->AddToStackL( *this, iContainer );
        } 

    // Message handling would take place here.
   }

// ---------------------------------------------------------
// CMyViewView::HandleCommandL(TInt aCommand)
// This is called when a view needs to be deactivated. 
// This destroys container with its controls.
// ---------------------------------------------------------
//
void CMyViewView::DoDeactivate()
    {
    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        }
    
    delete iContainer;
    iContainer = NULL;
    }


// End of File

