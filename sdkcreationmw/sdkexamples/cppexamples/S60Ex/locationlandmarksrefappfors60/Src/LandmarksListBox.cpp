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
#include "LandmarksListBox.h"
#include "LandmarksListBoxModel.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLandmarksListBox::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLandmarksListBox::ConstructL(
    const CCoeControl* aParent,
    TInt aFlags)
    {
    // Create a specialized model, forwarding the filtering algorithm to the 
    // landmark API.
    iModel = new (ELeave) CLandmarksListBoxModel;

    // Continue construction of the object.
    CEikColumnListBox::ConstructL(aParent, aFlags);
    }

// -----------------------------------------------------------------------------
// CLandmarksListBox::CreateModelL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksListBox::CreateModelL()
    {
    // Do nothing. Model is already constructed in constructor.
    // This method must be overridden to prevent base class to recreate a new, 
    // for our purpuses, non-suitable model.
    }

// -----------------------------------------------------------------------------
// CLandmarksListBox::Model
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CLandmarksListBoxModel* CLandmarksListBox::Model() const
    {
    return static_cast <CLandmarksListBoxModel*> (iModel);
    }

// End of File

