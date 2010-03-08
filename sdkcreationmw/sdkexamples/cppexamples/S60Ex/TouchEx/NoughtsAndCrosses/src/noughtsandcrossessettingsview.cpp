/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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

#include "noughtsandcrossessettingslistbox.h"
#include "noughtsandcrossessettingsview.h"
#include "noughtsandcrossesdocument.h"

#include "nac.hrh"
#include <nac.rsg>


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNoughtsAndCrossesSettingsView::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CNoughtsAndCrossesSettingsView* CNoughtsAndCrossesSettingsView::NewL()
    {
    CNoughtsAndCrossesSettingsView* self = new (ELeave) CNoughtsAndCrossesSettingsView;
    CleanupStack::PushL (self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CNoughtsAndCrossesSettingsView::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CNoughtsAndCrossesSettingsView::ConstructL()
    {
    BaseConstructL (R_NOUGHTSANDCROSSES_SETTING_VIEW);
    }

// -----------------------------------------------------------------------------
// CNoughtsAndCrossesSettingsView::CNoughtsAndCrossesSettingsView()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CNoughtsAndCrossesSettingsView::CNoughtsAndCrossesSettingsView() : iListBox (NULL)
    {
    }

// -----------------------------------------------------------------------------
// CAknExGridContainerAsMenu::~CAknExGridView()
// Destructor.
// -----------------------------------------------------------------------------
//
CNoughtsAndCrossesSettingsView::~CNoughtsAndCrossesSettingsView()
    {
    if (iListBox)
        {
        AppUi()->RemoveFromStack (iListBox);
        }
    delete iListBox;
    }

// -----------------------------------------------------------------------------
// TUid CNoughtsAndCrossesSettingsView::Id()
// Returns the View Id.
// -----------------------------------------------------------------------------
//
TUid CNoughtsAndCrossesSettingsView::Id() const
    {
    return TUid::Uid (EMultiViewsSettingId);
    }

// -----------------------------------------------------------------------------
// CAknExGridView::HandleCommandL()
// Handles the commands.
// -----------------------------------------------------------------------------
//
void CNoughtsAndCrossesSettingsView::HandleCommandL (TInt aCommand)
    {
    switch (aCommand)
        {
        case EAknSoftkeyOk:
            {
            CNoughtsAndCrossesDocument* document = static_cast<
                CNoughtsAndCrossesDocument*> (AppUi()->Document());
            iListBox->StoreSettingsL();
            document->SaveSettingsAndInformationL();
            document->NewGame();
            AppUi()->ActivateLocalViewL (TUid::Uid (EMultiViewsPlayGameId));
            break;
            }

        case EAknSoftkeyCancel:
            {
            AppUi()->ActivateLocalViewL (TUid::Uid (EMultiViewsPlayGameId));
            break;
            }

        default:
            AppUi()->HandleCommandL (aCommand);	
        }
    }

// -----------------------------------------------------------------------------
// CNoughtsAndCrossesSettingsView::DoActivateL()
// Creates the Container class object.
// -----------------------------------------------------------------------------
//
void CNoughtsAndCrossesSettingsView::DoActivateL (const TVwsViewId& /*aPrevViewId*/,
    TUid /*aCustomMessageId*/, const TDesC8& /*aCustomMessage*/)
    {
    if (!iListBox)
        {
        CNoughtsAndCrossesDocument* document = static_cast<
            CNoughtsAndCrossesDocument*> (AppUi()->Document());

        iListBox = new (ELeave) CNoughtsAndCrossesSettingListbox 
        	(document->InformationAndSettings());

        iListBox->SetMopParent (this);
        iListBox->ConstructFromResourceL (R_NOUGHTSANDCROSSEST_SETTING_LIST_PLAYER_NAME_TEXT);
        AppUi()->AddToStackL (*this, iListBox);
        }

    iListBox->MakeVisible (ETrue);
    iListBox->SetRect(ClientRect());
    iListBox->ActivateL();
    iListBox->DrawNow();
    }

// -----------------------------------------------------------------------------
// CNoughtsAndCrossesSettingsView::DoDeactivate()
// Deletes the Container class object.
// -----------------------------------------------------------------------------
//
void CNoughtsAndCrossesSettingsView::DoDeactivate()
    {
    if (iListBox)
        {
        iListBox->MakeVisible (EFalse);
        }
    }

// End of File
