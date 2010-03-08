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
* Description:  Implements the CLandmarksContainerBase class
*
*/


// INCLUDE FILES
#include <aknsfld.h>
#include <aknview.h>
#include <eiklbx.h>

#include <EPos_Landmarks.h>

#include "LandmarksContainerBase.h"
#include "LandmarksListBox.h"
#include "LandmarksListBoxModel.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLandmarksCategoriesView::CLandmarksCategoriesView
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLandmarksContainerBase::CLandmarksContainerBase(
    CAknView& aView,
    CLandmarksApplicationEngine& aEngine)
:   iView(aView), iEngine(aEngine)
    {
    }

// -----------------------------------------------------------------------------
// CLandmarksCategoriesView::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLandmarksContainerBase::ConstructL(const TRect& aRect)
    {
    CreateWindowL();

    CreateListBoxL();
    CreateFindBoxL();

    SetRect(aRect);
    ActivateL();
    }

// Destructor
CLandmarksContainerBase::~CLandmarksContainerBase()
    {
    delete iListBox;
    delete iFindBox;
    delete iOldFilter;
    }

// -----------------------------------------------------------------------------
// CLandmarksContainerBase::IsItemSelected
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CLandmarksContainerBase::IsItemSelected()
    {
    return (iListBox->CurrentItemIndex() >= 0);
    }

// -----------------------------------------------------------------------------
// CLandmarksContainerBase::CountComponentControls
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLandmarksContainerBase::CountComponentControls() const
    {
    TInt count = 0;
    if (iListBox)
        {
        count++;
        }
    if (iFindBox)
        {
        count++;
        }

    return count; // Return the number of controls inside this container.
    }

// -----------------------------------------------------------------------------
// CLandmarksContainerBase::ComponentControl
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCoeControl* CLandmarksContainerBase::ComponentControl(TInt aIndex) const
    {
    switch (aIndex)
        {
        case 0:
            return iListBox; // Returns the pointer to listbox object.
        case 1:
            return iFindBox; // Returns the pointer to findbox object.
        default:
            return NULL; // Exception : Returns NULL.
        }
    }

// -----------------------------------------------------------------------------
// CLandmarksContainerBase::CreateFindBoxL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksContainerBase::CreateFindBoxL()
    {
    // Sets style of findbox.
    CAknSearchField::TSearchFieldStyle style(CAknSearchField::ESearch);

    // Creates FindBox.
    CGulIcon* defaultIcon = NULL;
    iFindBox = CAknSearchField::NewL(
        *this, style, defaultIcon, KPosLmMaxTextFieldLength);

    // Creates a filter (CAknListBoxFilterItems instance). This filter is not 
    // used in this app but the CAknSearchField class MUST have a filter 
    // anyway. The filter is bypassed by the CLandmarksListBoxModel class.
    iListBox->Model()->CreateFilterL(iListBox, iFindBox);

    // Initialize filter buffer
    iOldFilter = HBufC::NewL(0);
    }

// -----------------------------------------------------------------------------
// CLandmarksContainerBase::CreateListBoxL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksContainerBase::CreateListBoxL()
    {
    // Create a control to display a list of descriptors
    iListBox = new (ELeave) CLandmarksListBox();
    iListBox->SetContainerWindowL(*this);
    iListBox->ConstructL(this, CEikListBox::ELoopScrolling);

    // Create scrollbars
    CEikScrollBarFrame* scrollBar = iListBox->CreateScrollBarFrameL(ETrue);
    scrollBar->SetScrollBarVisibilityL(
		CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);
    }

// -----------------------------------------------------------------------------
// CLandmarksContainerBase::SizeChanged
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksContainerBase::SizeChanged()
    {
	// Set list box size.
	//iListBox->SetRect(Rect());
	iListBox->SizeChanged();

    // Set find box size.
    AknFind::HandleFixedFindSizeChanged(this, iListBox, iFindBox);
    }
    
// -----------------------------------------------------------------------------
// CLandmarksContainerBase::HandleResourceChange
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksContainerBase::HandleResourceChange(TInt aType)
    {
    CCoeControl::HandleResourceChange(aType);
    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        SetRect(iView.ClientRect());
        }
    }

// End of File

