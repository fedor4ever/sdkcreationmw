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

#include "noughtsandcrossescontainer.h"
#include "noughtsandcrossesdocument.h"
#include "noughtsandcrossesinformationandsettings.h"
#include "noughtsandcrossesview.h"

#include "nac.hrh"
#include <nac.rsg>

const TInt KBufSize = 20;
_LIT(KHumanWins, "H. Wins: %i");
_LIT(KHumanLosses, "H. Losses: %i");

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNoughtsAndCrossesView::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CNoughtsAndCrossesView* CNoughtsAndCrossesView::NewL()
    {
    CNoughtsAndCrossesView* self = new(ELeave) CNoughtsAndCrossesView;
    CleanupStack::PushL (self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CNoughtsAndCrossesView::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CNoughtsAndCrossesView::ConstructL()
    {
    BaseConstructL (R_NOUGHTSANDCROSSES_VIEW);
    }

// -----------------------------------------------------------------------------
// CNoughtsAndCrossesView::CNoughtsAndCrossesView()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CNoughtsAndCrossesView::CNoughtsAndCrossesView() : iContainer (NULL),
    iCursorRow (0), iCursorColumn (0)
    {
    ResetCoordinates();
    }

// -----------------------------------------------------------------------------
// CNoughtsAndCrossesView::~CNoughtsAndCrossesView()
// Destructor.
// -----------------------------------------------------------------------------
//
CNoughtsAndCrossesView::~CNoughtsAndCrossesView()
    {
    if (iContainer)
        {
        AppUi()->RemoveFromViewStack (*this, iContainer);
        }
	
    delete iContainer;
    }

// -----------------------------------------------------------------------------
// TUid CNoughtsAndCrossesView::Id()
// Returns the View Id.
// -----------------------------------------------------------------------------
//
TUid CNoughtsAndCrossesView::Id() const
    {
    return TUid::Uid (EMultiViewsPlayGameId);
    }

// -----------------------------------------------------------------------------
// CNoughtsAndCrossesView::HandleCommandL()
// Handles the commands.
// -----------------------------------------------------------------------------
//
void CNoughtsAndCrossesView::HandleCommandL (TInt aCommand)
    {
    switch (aCommand)
        {
        case ENoughtsAndCrossesCmdAppNewGame:
            {
            CNoughtsAndCrossesDocument* document = static_cast<
                CNoughtsAndCrossesDocument*> (AppUi()->Document());
            document->NewGame();			
            break;
            }

        case ENoughtsAndCrossesCmdAppSetting:
            {
            AppUi()->ActivateLocalViewL (TUid::Uid (EMultiViewsSettingId));
            break;
            }

        case ENoughtsAndCrossesCmdAppStatistics:
            {
            TBuf<KBufSize> wins;
            TBuf<KBufSize> losses;

            CNoughtsAndCrossesDocument* document = static_cast<
                CNoughtsAndCrossesDocument*> (AppUi()->Document());

            wins.Format ( KHumanWins, document->InformationAndSettings().HumanWins());
            losses.Format ( KHumanLosses, document->InformationAndSettings().HumanLosses());

            static_cast<CEikonEnv*> (iCoeEnv)->InfoWinL (wins, losses);
            break;
            }

        default:
            AppUi()->HandleCommandL (aCommand);	
        }	
    }

// -----------------------------------------------------------------------------
// CNoughtsAndCrossesView::DoActivateL()
// Creates the Container class object.
// -----------------------------------------------------------------------------
//
void CNoughtsAndCrossesView::DoActivateL(const TVwsViewId& /*aPrevViewId*/,
    TUid /*aCustomMessageId*/, const TDesC8& /*aCustomMessage*/)
    {
    if (!iContainer)
        {
        iContainer = CNoughtsAndCrossesContainer::NewL (ClientRect(),
            *this, iCursorRow, iCursorColumn);
		
        CNoughtsAndCrossesDocument* document = static_cast<
            CNoughtsAndCrossesDocument*> (AppUi()->Document());

        document->SetObserver (iContainer);
        AppUi()->AddToStackL (*this, iContainer);
        }

    iContainer->MakeVisible (ETrue);
    }

// -----------------------------------------------------------------------------
// CNoughtsAndCrossesView::DoDeactivate()
// Deletes the Container class object.
// -----------------------------------------------------------------------------
//
void CNoughtsAndCrossesView::DoDeactivate()
    {
    if (iContainer)
        {
        CNoughtsAndCrossesDocument* document = static_cast<
            CNoughtsAndCrossesDocument*> (AppUi()->Document());		
        document->SetObserver (NULL);
        AppUi()->RemoveFromStack (iContainer);
        delete iContainer;
        iContainer = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CNoughtsAndCrossesView::ResetCoordinates()
// 
// -----------------------------------------------------------------------------
//
void CNoughtsAndCrossesView::ResetCoordinates()
    {
    iWinningRowX    = -1;
    iWinningColumnX = -1;
    iWinningRowY    = -1;
    iWinningColumnY = -1;
    }

// End of File
