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
* Description:  Implements the CLandmarksLmOpWrapper class
*
*/


// INCLUDE FILES
#include <EPos_Landmarks.h>
#include <Epos_CPosLmOperation.h>

#include "LandmarksLmOpWrapper.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLandmarksLmOpWrapper::CLandmarksLmOpWrapper
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLandmarksLmOpWrapper::CLandmarksLmOpWrapper()
: CActive(CActive::EPriorityStandard)
    {
    CActiveScheduler::Add(this);
    }

// Destructor
CLandmarksLmOpWrapper::~CLandmarksLmOpWrapper()
    {
    Cancel();
    delete iLmOperation;
    iLmOperation = NULL;
    }

// -----------------------------------------------------------------------------
// CLandmarksLmOpWrapper::StartOperation
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksLmOpWrapper::StartOperation(
    CPosLmOperation* aLmOperation,
    TRequestStatus& aStatus,
    TBool aReportProgress)
    {
    delete iLmOperation;
    iLmOperation = aLmOperation;
    iCallerStatus = &aStatus;
    iReportProgress = aReportProgress;
    iProgress = 0;

    ExecuteNextStep();
    }

// -----------------------------------------------------------------------------
// CLandmarksLmOpWrapper::LmOperationPtr
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CPosLmOperation* CLandmarksLmOpWrapper::LmOperationPtr()
    {
    return iLmOperation;
    }

// -----------------------------------------------------------------------------
// CLandmarksLmOpWrapper::ExecuteNextStep
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksLmOpWrapper::ExecuteNextStep(TRequestStatus& aStatus)
    {
    iCallerStatus = &aStatus;
    ExecuteNextStep();
    }

// -----------------------------------------------------------------------------
// CLandmarksLmOpWrapper::Progress
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLandmarksLmOpWrapper::Progress()
    {
    return (TInt) (iProgress * 100);
    }

// -----------------------------------------------------------------------------
// CLandmarksLmOpWrapper::DoCancel
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksLmOpWrapper::DoCancel()
    {
    delete iLmOperation;
    iLmOperation = NULL;
    User::RequestComplete(iCallerStatus, KErrCancel);
    }

// -----------------------------------------------------------------------------
// CLandmarksLmOpWrapper::RunL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksLmOpWrapper::RunL()
    {
    if (iStatus == KErrNone)
        {
        // Operation finished.
        User::RequestComplete(iCallerStatus, KErrNone);
        }
    else if (iStatus == KPosLmOperationNotComplete)
        {
        if (iReportProgress)
            {
            User::RequestComplete(iCallerStatus, KPosLmOperationNotComplete);
            }
        else
            {
            ExecuteNextStep();
            }
        }
    }

// -----------------------------------------------------------------------------
// CLandmarksLmOpWrapper::ExecuteNextStep
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksLmOpWrapper::ExecuteNextStep()
    {
    iStatus = KRequestPending;
    iLmOperation->NextStep(iStatus, iProgress);
    SetActive();
    }

// End of File

