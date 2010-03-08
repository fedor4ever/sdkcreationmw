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
* Description:  Defines second view,Implentation also demonstrates external
*                application's view activation and message commands.
*                DoActivateL recieves Messages and MessageUids from first view.
*
*/



// INCLUDE FILES
#include  <viewcli.h>
#include  <aknviewappui.h>
#include  <avkon.hrh>
#include  <aknquerydialog.h>
#include  <MyView.rsg>
#include  "MyViewView2.h"
#include  "MyViewContainer2.h"
#include  "myview.hrh"
#include  <CPbkViewState.h> // For focused phonebook view.
                            // You need to have PbkView.lib in .mmp

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMyViewView2::ConstructL(const TRect& aRect)
// EPOC two-phased constructor
// ---------------------------------------------------------
//
void CMyViewView2::ConstructL()
    {
    BaseConstructL( R_MYVIEW_VIEW2 );
    }

// ---------------------------------------------------------
// CMyViewView2::~CMyViewView2()
// Default destructor
// ---------------------------------------------------------
//
CMyViewView2::~CMyViewView2()
    {
    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        }

    delete iContainer;
    }

// ---------------------------------------------------------
// TUid CMyViewView2::Id()
// This returns the view ID
// ---------------------------------------------------------
//
TUid CMyViewView2::Id() const
    {
    return KView2Id;
    }

// ---------------------------------------------------------
// CMyViewView2::HandleCommandL(TInt aCommand)
// Here we handle commands for this view.
// Each view has their own HandleCommandL()
// ---------------------------------------------------------
//
void CMyViewView2::HandleCommandL(TInt aCommand)
    {
    switch ( aCommand )
        {
        case EAknSoftkeyOk:
            {
            break;
            }
        case EAknSoftkeyExit:
            {
            AppUi()->HandleCommandL(EEikCmdExit);
            break;
            }
        case EMyViewCmdView2Cmd1:
            {
            // Open calendar application in month view.
            // If other than month view is defined as default view,
            // month view will open only if calendar application is running on background.
			// If calendar is not running, user defined default view will open instead.
			// This is done by framework, when calendar gets focus, the default view will open.
            const TUid KCalendarUid = { 0x10005901 };   // Application Uid
            const TUid KCalendarViewUid = { 1 };        // View Uid of the external application
            AppUi()->ActivateViewL(TVwsViewId(KCalendarUid, KCalendarViewUid));

            break;
            }
        case EMyViewCmdView2Cmd2:
            {

            //Open Messagin to main view
            const TUid KMessagingUid = { 0x100058C5 };   // Application Uid
            const TUid KInboxViewUid = { 1 };            // View Uid of the external application
            AppUi()->ActivateViewL(TVwsViewId(KMessagingUid, KInboxViewUid));

            break;
            }
        case EMyViewCmdView2Cmd3:
            {

            const TUid KRecorderUid = { 0x100058CA };   // Application Uid

	        // Start recorder in recorder view and have it start immeditately
	        const TUid KRecorderViewUid = { 1 };        // View Uid of the external application
	        const TUid KRecorderMessageUid = { 2 };     // MessageUid of the external application
	        AppUi()->ActivateViewL(TVwsViewId(KRecorderUid, KRecorderViewUid), KRecorderMessageUid,
	                KNullDesC8);

            break;
            }
        case EMyViewCmdView2Cmd4:
            {
            // Open phonebook application with focused view
            const TUid appUid = { 0x101f4cce }; // Application Uid
            const TUid KExtViewUid= { 4 };      // Contact Info View Uid of Phonebook Application

            TInt contact = 1;
            CAknNumberQueryDialog *dlg = CAknNumberQueryDialog::NewL(contact);
            if (dlg->ExecuteLD(R_CONTACT_NUMBER_QUERY))
                {
                // ok pressed

                // Costruct a message for Phonebook application
                CPbkViewState* pbkViewParam = CPbkViewState::NewLC();

                // Focus on contact with specified id-number. You have to have
                // the specified contact for this to work.
                pbkViewParam->SetFocusedContactId(contact);
                // Package and return object in a buffer.
                HBufC8* paramBuf = pbkViewParam->PackLC();

                // Activate the view
                AppUi()->ActivateViewL(TVwsViewId(appUid, KExtViewUid), CPbkViewState::Uid(),
                    *paramBuf);

                // Cleanup
                CleanupStack::PopAndDestroy(paramBuf);
                CleanupStack::PopAndDestroy(pbkViewParam);
                }
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
// CMyViewView2::HandleClientRectChange()
// ---------------------------------------------------------
//
void CMyViewView2::HandleClientRectChange()
    {
    if ( iContainer )
        {
        iContainer->SetRect( ClientRect() );
        }
    }

// ---------------------------------------------------------
// CMyViewView2::DoActivateL(...)
// This is called when a view needs to be activated.
// This creates container with its controls.
// ---------------------------------------------------------
//
void CMyViewView2::DoActivateL(
   const TVwsViewId& /*aPrevViewId*/,TUid aCustomMessageId,
   const TDesC8& aCustomMessage)
    {

    // Create Container
    if (!iContainer)
        {
        iContainer = new (ELeave) CMyViewContainer2;
        iContainer->SetMopParent(this);
        iContainer->ConstructL( ClientRect() );
        AppUi()->AddToStackL( *this, iContainer );
        }


    // Check aCustomMessageId to handle aCustomMessage
    // You could check aPrevViewId if you were curious to know
    // from where this view was called from.
    const TUid KLocalComparisionUid= {2};
    if (aCustomMessageId==KLocalComparisionUid)
        {
        TBuf8<255> customMessage;
        customMessage.Copy(aCustomMessage);

        // do something with aCustomMessage
        if (customMessage==KNullDesC8)
            iEikonEnv->InfoMsg(_L("MessageUid passed"));
        else
            iEikonEnv->InfoMsg(_L("Message and Uid passed"));

        }
   }

// ---------------------------------------------------------
// CMyViewView2::HandleCommandL(TInt aCommand)
// This is called when a view needs to be deactivated.
// This destroys container with its controls.
// ---------------------------------------------------------
//
void CMyViewView2::DoDeactivate()
    {
    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        }

    delete iContainer;
    iContainer = NULL;
    }


// End of File

