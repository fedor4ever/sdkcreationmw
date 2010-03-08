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
* Description:  Implements the CLandmarksInfoView class
*
*/


// INCLUDE FILES
#include <eikmenup.h>
#include <aknviewappui.h>

#include <LbsPositionInfo.h>
#include <EPos_Landmarks.h>

#include <LmRefApp.rsg>
#include "LmRefApp.hrh"
#include "LandmarksUtils.h"
#include "LandmarksInfoView.h"
#include "LandmarksApplicationEngine.h"
#include "LandmarksInfoContainer.h"
#include "LandmarksEditDialog.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLandmarksInfoView::CLandmarksInfoView
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLandmarksInfoView::CLandmarksInfoView(
    CAknNavigationDecorator* aNaviDecorator,
    CLandmarksApplicationEngine& aEngine)
:   iEngine(aEngine), 
    iNaviDecorator(aNaviDecorator)
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CLandmarksInfoView::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLandmarksInfoView::ConstructL()
    {
    BaseConstructL(R_LMREFAPP_INFO_VIEW);
    iAppName = iCoeEnv->AllocReadResourceAsDes16L(R_LMREFAPP_APP_TITLE);
    }

// -----------------------------------------------------------------------------
// CLandmarksInfoView::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CLandmarksInfoView* CLandmarksInfoView::NewLC(
    CAknNavigationDecorator* aNaviDecorator,
    CLandmarksApplicationEngine& aEngine)
    {
    CLandmarksInfoView* self = new (ELeave) CLandmarksInfoView(
        aNaviDecorator, aEngine);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// Destructor
CLandmarksInfoView::~CLandmarksInfoView()
    {
    delete iContainer;
    delete iAppName;
    }

// -----------------------------------------------------------------------------
// CLandmarksInfoView::Id
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUid CLandmarksInfoView::Id() const
    {
    return TUid::Uid(ELandmarksInfoViewId);
    }

// -----------------------------------------------------------------------------
// CLandmarksInfoView::HandleCommandL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksInfoView::HandleCommandL(TInt aCommand)
    {
    switch (aCommand)
        {
        case EAknSoftkeyEdit:
            iContainer->EditLandmarkL();
            break;

        case EAknSoftkeyBack:
            AppUi()->ActivateLocalViewL(TUid::Uid(ELandmarksViewId));
            break;

        default:
            AppUi()->HandleCommandL(aCommand);
            break;
        }
    }

// -----------------------------------------------------------------------------
// CLandmarksInfoView::DoActivateL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksInfoView::DoActivateL(
    const TVwsViewId& /*aPrevViewId*/,
    TUid aCustomMessageId,
    const TDesC8& aCustomMessage)
    {
    __ASSERT_ALWAYS(aCustomMessageId.iUid == ELandmarksViewInfoById,
                    LandmarksUtils::Panic(KErrGeneral));
    
    if (!iContainer)
        {
        iContainer = new (ELeave) CLandmarksInfoContainer(
            *this, iEngine, iNaviDecorator);
        iContainer->SetMopParent(this);
	    iContainer->ConstructL(ClientRect());
        }

    iContainer->StoreTitlePaneL();

    // Enable receiving of keyboard events.
    AppUi()->AddToStackL(*this, iContainer);

    // custom message contains landmark id
    TPckgBuf<TPosLmItemId> lmid;
    lmid.Copy(aCustomMessage);
    iContainer->LoadLandmarkL(lmid());
    
    // Make view visible.
    iContainer->MakeNaviPaneVisibleL(EFalse);
    iContainer->MakeVisible(ETrue);

    // Notify that this view is active
    iEngine.NotifyViewActivated(Id(), ETrue);
    }

// -----------------------------------------------------------------------------
// CLandmarksInfoView::DoDeactivate
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksInfoView::DoDeactivate()
    {
    // Hide view.
    iContainer->MakeVisible(EFalse);
    TInt err = KErrNone;
    TRAP(err, iContainer->MakeNaviPaneVisibleL(ETrue));
    iContainer->RestoreTitlePane();

    // Notify that this view is inactive
    TBool isActive = EFalse;
    iEngine.NotifyViewActivated(Id(), isActive);

    // Disable receiving keyboard events.
    AppUi()->RemoveFromStack(iContainer);
    }

// End of File

