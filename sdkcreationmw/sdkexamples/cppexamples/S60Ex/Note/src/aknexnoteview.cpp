/*
* Copyright (c) 2004 - 2006 Nokia Corporation and/or its subsidiary(-ies).
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
#include <aknviewappui.h>
#include <akntitle.h>
#include <avkon.hrh>
#include <AknExNote.rsg>
#include <AVKON.RSG>

#include "AknExNoteView.h"
#include "AknExNoteContainer.h"
#include "aknexnote.hrh"


// CONSTANTS
#include "Aknexnoteconstants.h"


// ========================= MEMBER FUNCTIONS ===================================

// ------------------------------------------------------------------------------
// CAknExNoteView::CAknExNoteView()
// C++ default constructor can NOT contain any code, that might leave.
// ------------------------------------------------------------------------------
//
CAknExNoteView::CAknExNoteView()
: iContainer( NULL ),iCurrentOutline( EAknExNoteCmdEmptyOutline )
    {
    // No implementation required
    }

// ------------------------------------------------------------------------------
// CAknExNoteView::~CAknExNoteView()
// Destructor.
// ------------------------------------------------------------------------------
//
CAknExNoteView::~CAknExNoteView()
    {
    delete iContainer;
    }

// ------------------------------------------------------------------------------
// CAknExNoteView::ConstructL()
// Symbian 2nd phase constructor can leave.
// ------------------------------------------------------------------------------
//
void CAknExNoteView::ConstructL()
    {
    BaseConstructL( R_AKNEXNOTE_VIEW );
    }

// ------------------------------------------------------------------------------
// CAknExNoteView::DisplayNextOutLineL()
// Displays Next Outline
// ------------------------------------------------------------------------------
//
void CAknExNoteView::DisplayNextOutlineL()
    {
    switch ( iCurrentOutline )
        {
        case EAknExNoteCmdEmptyOutline:
            HandleCommandL( EAknExNoteCmdOutline01 );
            break;

        case EAknExNoteCmdOutline01: 
            HandleCommandL( EAknExNoteCmdOutline05 );
            break;

        case EAknExNoteCmdOutline05:
            HandleCommandL( EAknExNoteCmdOutline08 );
            break;

        case EAknExNoteCmdOutline08:
            HandleCommandL( EAknExNoteCmdOutline35 );
            break;

        case EAknExNoteCmdOutline35:
            HandleCommandL( EAknExNoteCmdOutline53 );
            break;

        case EAknExNoteCmdOutline53:
        default:
            HandleCommandL( EAknExNoteCmdEmptyOutline );
            break;
        }
    }

// -----------------------------------------------------------------------------
// CAknExNoteView::Id()
// Returns ID of view.
// -----------------------------------------------------------------------------
//
TUid CAknExNoteView::Id() const
    {
    return KViewId;
    }

// -----------------------------------------------------------------------------
// CAknExNoteView::HandleCommandL()
// Handles the commands.
// -----------------------------------------------------------------------------
//
void CAknExNoteView::HandleCommandL( TInt aCommand )
    {
    switch ( aCommand )
        {
        // Soft Back
        case EAknSoftkeyBack:
            AppUi()->ProcessCommandL( EAknCmdExit );
            break;

        // Exit
        case EAknCmdExit:
            AppUi()->HandleCommandL( EAknCmdExit );
            break;

        case EAknExNoteCmdEmptyOutline:
            iCurrentOutline = aCommand;
            iContainer->SetTextToLabelL( R_AKNEXNOTE_TEXT_LABEL_TITLE );
            IndicateTitlePaneTextL( aCommand );
            break;
        
        // Error Note
        case EAknExNoteCmdOutline01: 
            iCurrentOutline = aCommand;
            iContainer->SetTextToLabelL( R_AKNEXNOTE_TEXT_LABEL_OPERATION );
            IndicateTitlePaneTextL( aCommand );
            iContainer->ShowGeneralNoteL( R_AKNEXNOTE_ERROR_NOTE,
                                          CAknNoteDialog::ELongTimeout,
                                          CAknNoteDialog::EErrorTone );
            break;

        // Wait Note
        case EAknExNoteCmdOutline05:
            iCurrentOutline = aCommand;
            iContainer->SetTextToLabelL( R_AKNEXNOTE_TEXT_LABEL_OPERATION );
            IndicateTitlePaneTextL( aCommand );
            iContainer->ShowWaitNoteL(R_AKNEXNOTE_WAIT_NOTE_SOFTKEY_CANCEL,
                                      EAknExNoteCtrlIdWaitNote);
            break;
         
        // Dynamic text
        case EAknExNoteCmdOutline08:
            iCurrentOutline = aCommand;
            iContainer->SetTextToLabelL( R_AKNEXNOTE_TEXT_LABEL_OPERATION );
            IndicateTitlePaneTextL( aCommand );
            iContainer->ShowDynamicNumberNoteL();
            break;
        
        // Progress note 
        case EAknExNoteCmdOutline35: 
            iCurrentOutline = aCommand;
            iContainer->SetTextToLabelL( R_AKNEXNOTE_TEXT_LABEL_OPERATION );
            IndicateTitlePaneTextL( aCommand );
            iContainer->ShowProgressNoteUnderSingleProcessL( 
                                            R_AKNEXNOTE_PROGRESS_NOTE,
                                            EAknExNoteCtrlIdProgressNote );
            break;
        
        // EAknGlobalConfirmationNote
        case EAknExNoteCmdOutline53:
            iCurrentOutline = aCommand;
            iContainer->SetTextToLabelL( R_AKNEXNOTE_TEXT_LABEL_OPERATION );
            IndicateTitlePaneTextL( aCommand );
            iContainer->ShowShowNoteL(
                        EAknGlobalConfirmationNote,
                        R_AKNEXNOTE_SHOWNOTEL_GLOBALCONFIRMATIONNOTE_TEXT );
            break;
        
        // Next one
        case EAknExNoteCbaCmdNext:
            DisplayNextOutlineL();
            break;
        default:
            break;
        }
    }

// -----------------------------------------------------------------------------
// CAknExNoteView::IndicateTitlePaneTextL()
// Set title text to status-pane
// -----------------------------------------------------------------------------
//
void CAknExNoteView::IndicateTitlePaneTextL( TInt aCommand )
    {

    TBool useDefaultText( EFalse );
    TInt resourceId( KAknExNoteInitializeResourceId );

    switch ( aCommand )
        {
        case EAknExNoteCmdEmptyOutline:
            useDefaultText = ETrue;
            break;

        case EAknExNoteCmdOutline01:
            resourceId = R_AKNEXNOTE_TEXT_STATUS_PANE_TITLE01;
            break;

        case EAknExNoteCmdOutline05:
            resourceId = R_AKNEXNOTE_TEXT_STATUS_PANE_TITLE05;
            break;

        case EAknExNoteCmdOutline08:
            resourceId = R_AKNEXNOTE_TEXT_STATUS_PANE_TITLE08;
            break;

        case EAknExNoteCmdOutline35:
            resourceId = R_AKNEXNOTE_TEXT_STATUS_PANE_TITLE35;
            break;

        case EAknExNoteCmdOutline53:
            resourceId = R_AKNEXNOTE_TEXT_STATUS_PANE_TITLE53;
            break;

        default:
            break;
        }

    CAknTitlePane* titlePane = static_cast<CAknTitlePane*> 
                               ( StatusPane()->ControlL( TUid::Uid
                               ( EEikStatusPaneUidTitle ) ) );
    
    if ( useDefaultText )
        {
        titlePane->SetTextToDefaultL();
        }
    else
        {
        
        //Allocate TBuf with constant length.
        TBuf<KAknExNoteTitleBufLength> titleText( NULL );
        
         // Reads a resource into a descriptor.
        iEikonEnv->ReadResource( titleText, resourceId );
        
        // Set the text for TitlePane
        titlePane->SetTextL( titleText );
        }
    }

// -----------------------------------------------------------------------------
// CAknExNoteView::HandleViewRectChange()
// Handles the size of the clientrect.
// -----------------------------------------------------------------------------
//
void CAknExNoteView::HandleViewRectChange()
    {
    if ( iContainer )
        {
        iContainer->SetRect( ClientRect() );
        }
    }

// -----------------------------------------------------------------------------
// CAknExNoteView::DoActivateL()
// Creates container class object.
// -----------------------------------------------------------------------------
//
void CAknExNoteView::DoActivateL( const TVwsViewId& /*aPrevViewId*/,
                                  TUid /*aCustomMessageId*/,
                                  const TDesC8& /*aCustomMessage*/ )
    {

    // Creates container object
    if ( !iContainer )
        {
        iContainer = new ( ELeave ) CAknExNoteContainer( this );
        iContainer->ConstructL( ClientRect() );
        iContainer->SetMopParent( this );
        }

    // Adds container to view control stack.
    AppUi()->AddToStackL( *this, iContainer );
    iContainer->MakeVisible( ETrue );
    }

// -----------------------------------------------------------------------------
// CAknExNoteView::DoDeactivate()
// Deletes container class object.
// -----------------------------------------------------------------------------
//
void CAknExNoteView::DoDeactivate()
    {
    // Deletes the container class object.
    if ( iContainer )
        {
        iContainer->MakeVisible( EFalse );
        // Removes container from view control stack.
        AppUi()->RemoveFromStack( iContainer );
        }
    }

// End of File
