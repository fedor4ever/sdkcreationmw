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
* Description:  Implements the CLandmarksEngineBase class
*
*/


// INCLUDE FILES
#include <EPos_CPosLandmarkDatabase.h>
#include <EPos_CPosLandmarkSearch.h>

#include "LandmarksEngineBase.h"
#include "LandmarksLmOpWrapper.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLandmarksEngineBase::CLandmarksEngineBase
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLandmarksEngineBase::CLandmarksEngineBase(
    CPosLandmarkDatabase& aDb) 
:   CActive(CActive::EPriorityStandard), 
    iDb(aDb)
    {
    CActiveScheduler::Add(this);
    }

// -----------------------------------------------------------------------------
// CLandmarksEngineBase::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLandmarksEngineBase::ConstructL()
    {
    iSearcher = CPosLandmarkSearch::NewL(iDb);
    iLmOpWrapper = new (ELeave) CLandmarksLmOpWrapper();
    }

// Destructor
CLandmarksEngineBase::~CLandmarksEngineBase()
    {
    delete iLmOpWrapper;
    delete iSearcher;
    }

// -----------------------------------------------------------------------------
// CLandmarksEngineBase::SetPrio
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksEngineBase::SetPrio(CActive::TPriority aPriority)
    {
    iPriority = aPriority;
    }

// -----------------------------------------------------------------------------
// CLandmarksEngineBase::SetPriorityAndSetActive
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksEngineBase::SetPriorityAndSetActive()
    {
    if (iPriority != Priority())
        {
        CActive::SetPriority(iPriority);
        }
    SetActive();
    }

//  End of File  

