/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implements the CLandmarksView class
*
*/


// INCLUDE FILES
#include <eikmenup.h>
#include <aknviewappui.h>

#include <LmRefApp.rsg>
#include "LmRefApp.hrh"
#include "LandmarksView.h"
#include "LandmarksApplicationEngine.h"
#include "LandmarksContainer.h"
#include "LandmarksEditDialog.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLandmarksView::CLandmarksView
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLandmarksView::CLandmarksView(
    CAknNavigationDecorator* aNaviDecorator,
    CLandmarksApplicationEngine& aEngine)
:   iEngine(aEngine), 
    iNaviDecorator(aNaviDecorator)
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CLandmarksView::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLandmarksView::ConstructL()
    {
    BaseConstructL(R_LMREFAPP_VIEW);
    }

// -----------------------------------------------------------------------------
// CLandmarksView::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CLandmarksView* CLandmarksView::NewLC(
    CAknNavigationDecorator* aNaviDecorator,
    CLandmarksApplicationEngine& aEngine)
    {
    CLandmarksView* self = new (ELeave) CLandmarksView(
        aNaviDecorator, aEngine);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// Destructor
CLandmarksView::~CLandmarksView()
    {
    delete iContainer;
    }

// -----------------------------------------------------------------------------
// CLandmarksView::Id
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUid CLandmarksView::Id() const
    {
    return TUid::Uid(ELandmarksViewId);
    }

// -----------------------------------------------------------------------------
// CLandmarksView::HandleCommandL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksView::HandleCommandL(TInt aCommand)
    {
    switch (aCommand)
        {
        case ELandmarksAddCurrent:
            AddLandmarkL(ETrue); // use current location
            break;

        case ELandmarksAddBlank:
            AddLandmarkL(EFalse); // create blank
            break;

        case ELandmarksDelete:
            iContainer->DeleteLandmarkL();
            break;

        case ELandmarksOpen:
            {
            TPckgC<TPosLmItemId> lmId(iContainer->CurrentLandmark());
            
            AppUi()->ActivateLocalViewL(
                TUid::Uid(ELandmarksInfoViewId),
                TUid::Uid(ELandmarksViewInfoById),
                lmId);
            break;
            }

        default:
            AppUi()->HandleCommandL(aCommand);
            break;
        }
    }

// -----------------------------------------------------------------------------
// CLandmarksView::DoActivateL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksView::DoActivateL(
    const TVwsViewId& /*aPrevViewId*/,
    TUid /*aCustomMessageId*/,
    const TDesC8& /*aCustomMessage*/)
    {
    if (!iContainer)
        {
        iContainer = new (ELeave) CLandmarksContainer(
            *this, iEngine, iNaviDecorator);
        iContainer->SetMopParent(this);
	    iContainer->ConstructL(ClientRect());
        }

    // Enable receiving of keyboard events.
    AppUi()->AddToStackL(*this, iContainer);

    // Make view visible.
    iContainer->MakeVisible(ETrue);

    // Notify that this view is active
    iEngine.NotifyViewActivated(Id(), ETrue);
    }

// -----------------------------------------------------------------------------
// CLandmarksView::DoDeactivate
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksView::DoDeactivate()
    {
    // Hide view.
    iContainer->MakeVisible(EFalse);

    // Notify that this view is inactive
    iEngine.NotifyViewActivated(Id(), EFalse);

    // Disable receiving keyboard events.
    AppUi()->RemoveFromStack(iContainer);
    }

// -----------------------------------------------------------------------------
// CLandmarksView::DynInitMenuPaneL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksView::DynInitMenuPaneL(
    TInt aResourceId,
    CEikMenuPane* aMenuPane)
    {
    CAknView::DynInitMenuPaneL(aResourceId, aMenuPane);
    if (aResourceId == R_LMREFAPP_MENU)
        {
        // If no landmark is selected we need to hide some command options.
        TBool isListEmpty  = !iContainer->IsItemSelected();
        aMenuPane->SetItemDimmed(ELandmarksDelete, isListEmpty);
        aMenuPane->SetItemDimmed(ELandmarksOpen, isListEmpty);
        }
    }

// -----------------------------------------------------------------------------
// CLandmarksView::AddLandmarkL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksView::AddLandmarkL(TBool aUseCurrentLocation)
    {
    // Create new landmark
    CPosLandmark* newLandmark = CPosLandmark::NewLC();

    // Create edit dialog
    TBool isEdited(EFalse);
    CLandmarksEditDialog* editDialog = CLandmarksEditDialog::NewLC(
        *newLandmark, aUseCurrentLocation, isEdited, iEngine);

    // Remove tabs from this view
    iContainer->MakeNaviPaneVisibleL(EFalse);

    // Dialog must not leave without restoring tab group. Must be trapped.
    TRAPD(err, editDialog->ExecuteLD(R_LMREFAPP_EDIT_DIALOG));

    // Restore tabs to this view
    iContainer->MakeNaviPaneVisibleL(ETrue);

    User::LeaveIfError(err);
    CleanupStack::Pop(editDialog);

    if (isEdited)
        {
        iEngine.AddLandmarkL(*newLandmark);
        }

    CleanupStack::PopAndDestroy(newLandmark);
    }

// End of File

