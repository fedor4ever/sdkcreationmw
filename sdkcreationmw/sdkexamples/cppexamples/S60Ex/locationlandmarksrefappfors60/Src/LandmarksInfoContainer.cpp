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
* Description:  Implements the CLandmarksInfoContainer class
*
*/


// INCLUDE FILES
#include <aknsfld.h>
#include <aknnavi.h>
#include <aknlists.h> 
#include <eikclbd.h>
#include <akntitle.h>
#include <aknview.h>

#include <LmRefApp.rsg>
#include "LmRefApp.hrh"
#include "LandmarksCommonData.h"
#include "LandmarksInfoContainer.h"
#include "LandmarksInfoModel.h"
#include "LandmarksEditDialog.h"
#include "LandmarksApplicationEngine.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLandmarksInfoContainer::CLandmarksInfoContainer
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLandmarksInfoContainer::CLandmarksInfoContainer(
    CAknView& aView,
    CLandmarksApplicationEngine& aEngine,
    CAknNavigationDecorator* aNaviDecorator)
:   iView(aView),
    iEngine(aEngine), 
    iNaviDecorator(aNaviDecorator)
    {
    }

// -----------------------------------------------------------------------------
// CLandmarksInfoContainer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLandmarksInfoContainer::ConstructL(const TRect& aRect)
    {
    CreateWindowL();
    CreateListBoxL();

    // Create data model.
    iModel = new (ELeave) CLandmarksInfoModel(iEngine);

    SetRect(aRect);
    ActivateL();
    }

// Destructor
CLandmarksInfoContainer::~CLandmarksInfoContainer()
    {
    iFields.Close();
    delete iListBox;
	delete iLandmark;
    delete iModel;
    }

// -----------------------------------------------------------------------------
// CLandmarksCategoriesView::CountComponentControls
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLandmarksInfoContainer::CountComponentControls() const
    {
    TInt count = 0;
    if (iListBox)
        {
        count++;
        }

    return count; // Return the number of controls inside this container.
    }

// -----------------------------------------------------------------------------
// CLandmarksCategoriesView::ComponentControl
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCoeControl* CLandmarksInfoContainer::ComponentControl(TInt aIndex) const
    {
    switch (aIndex)
        {
        case 0:
            return iListBox; // Returns the pointer to listbox object.
        default:
            return NULL; // Exception : Returns NULL.
        }
    }

// -----------------------------------------------------------------------------
// CLandmarksInfoContainer::OfferKeyEventL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TKeyResponse CLandmarksInfoContainer::OfferKeyEventL(
    const TKeyEvent& aKeyEvent, TEventCode aType)
    {
	TKeyResponse response = EKeyWasNotConsumed;

    switch (aKeyEvent.iCode)
        {
        case EKeyUpArrow:
        case EKeyDownArrow:
            // Scrolling the listbox. Forward to iListBox.
            response = iListBox->OfferKeyEventL(aKeyEvent, aType);
            break;

        case EKeyLeftArrow:
        case EKeyRightArrow:
            // Eat these to disable switching views
            response = EKeyWasConsumed;
            break;

        case EKeyDevice3:
            EditLandmarkL();
            response = EKeyWasConsumed;
            break;
            
        default:
            // menu keys should be handled
            break;
        }
        
	return response;
    }
    
// -----------------------------------------------------------------------------
// CLandmarksInfoContainer::StoreTitlePaneL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksInfoContainer::StoreTitlePaneL()
    {
    CEikStatusPane* statusPane = iAvkonAppUi->StatusPane();
    iTitlePane = (CAknTitlePane*) statusPane->ControlL(
        TUid::Uid(EEikStatusPaneUidTitle));

    iOriginalTitle = iTitlePane->Text()->AllocL();
    }

// -----------------------------------------------------------------------------
// CLandmarksInfoContainer::RetoreTitlePane
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksInfoContainer::RestoreTitlePane()
    {
    if (iTitlePane && iOriginalTitle)
        {
        iTitlePane->SetText(iOriginalTitle); // ownership is transferred
        }
    else
        {
        delete iOriginalTitle;
        }
    iOriginalTitle = NULL;
    }

// -----------------------------------------------------------------------------
// CLandmarksInfoContainer::MakeNaviPaneVisibleL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksInfoContainer::MakeNaviPaneVisibleL(TBool aVisible)
    {
    CEikStatusPane* statusPane = iAvkonAppUi->StatusPane();
    CAknNavigationControlContainer* naviPane = 
        (CAknNavigationControlContainer*) statusPane->
            ControlL(TUid::Uid(EEikStatusPaneUidNavi));

    if (!aVisible)
        {
        // Remove tabs from this view's navi pane
        naviPane->Pop(iNaviDecorator);
        }
    else
        {
        // Recreate tabs to this view's navi pane
        naviPane->PushL(*iNaviDecorator);
        }
    }

// -----------------------------------------------------------------------------
// CLandmarksInfoContainer::CreateListBoxL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksInfoContainer::CreateListBoxL()
    {
    // Create a control to display a list of descriptors
    iListBox = new (ELeave) CAknSingleHeadingStyleListBox();
    iListBox->SetContainerWindowL(*this);
    iListBox->ConstructL(this, 0);

    // Create scrollbars
    CEikScrollBarFrame* scrollBar = iListBox->CreateScrollBarFrameL();
    scrollBar->SetScrollBarVisibilityL(
		CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);
    }

// -----------------------------------------------------------------------------
// CLandmarksInfoContainer::SizeChanged
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksInfoContainer::SizeChanged()
    {
	// Set list box size.
	iListBox->SetRect(Rect());
    }

// -----------------------------------------------------------------------------
// CLandmarksInfoContainer::HandleResourceChange
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksInfoContainer::HandleResourceChange(TInt aType)
    {
    CCoeControl::HandleResourceChange(aType);
    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        SetRect(iView.ClientRect());
        }
    }

// -----------------------------------------------------------------------------
// CLandmarksInfoContainer::LoadLandmarkL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksInfoContainer::LoadLandmarkL(TPosLmItemId aLandmarkId)
    {
	delete iLandmark;
	iLandmark = NULL;

    iLandmark = iEngine.LandmarkLC(aLandmarkId);
    CleanupStack::Pop(iLandmark);

    UpdateL();
    iListBox->View()->SetCurrentItemIndex(0);
    }
    
// -----------------------------------------------------------------------------
// CLandmarksInfoContainer::EditLandmarkL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksInfoContainer::EditLandmarkL()
    {
    // Create editor
    TBool isEdited = EFalse;

    TInt selectedLine = iListBox->View()->CurrentItemIndex();
    TInt activeField = 0;
    if (selectedLine >= 0 && selectedLine < iFields.Count())
        {
        activeField = iFields[selectedLine];
        }
    
    CLandmarksEditDialog* editDialog = CLandmarksEditDialog::NewLC(
        *iLandmark, EFalse, isEdited, iEngine, activeField);

    TInt err = editDialog->ExecuteLD(R_LMREFAPP_EDIT_DIALOG);
    CleanupStack::Pop(editDialog);
    User::LeaveIfError(err);

    // Commit if edited
    if (isEdited)
        {
        iEngine.CommitLandmarkL(*iLandmark);
        UpdateL();
    
        // move to first field
        iListBox->View()->SetCurrentItemIndex(0);
        }
    }

// -----------------------------------------------------------------------------
// CLandmarksInfoContainer::UpdateL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksInfoContainer::UpdateL()
    {
    CDesCArray* infoArray = iModel->GetLandmarkInfoL(*iLandmark, iFields);

    CTextListBoxModel* model = iListBox->Model();
    model->SetItemTextArray(infoArray);
    model->SetOwnershipType(ELbmOwnsItemArray);
    iListBox->HandleItemAdditionL();

    UpdateTitleBarL();
    }

// -----------------------------------------------------------------------------
// CLandmarksInfoContainer::UpdateTitleBarL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksInfoContainer::UpdateTitleBarL()
    {
    TPtrC ptr;
    if (iLandmark)
        {
        iLandmark->GetLandmarkName(ptr);
        iTitlePane->SetTextL(ptr);
        }
    else 
        {
        iTitlePane->SetTextL(*iOriginalTitle);
        }
    }

// End of File

