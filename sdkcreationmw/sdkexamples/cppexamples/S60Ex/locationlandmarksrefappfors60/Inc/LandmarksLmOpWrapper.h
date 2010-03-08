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
* Description:  See class description below
*
*/


#ifndef __LANDMARKS_LM_OP_WRAPPER_H__
#define __LANDMARKS_LM_OP_WRAPPER_H__

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CLandmarksListContainer;
class CAknNavigationDecorator;
class CLandmarksEngine;
class CPosLmOperation;

// CLASS DECLARATION

/**
*  Active class that wraps an instance of CPosLmOperation.
*  The wrapped CPosLmOperation instance is executed incrementally. 
*  Execution progress can be reported.
*  This class is designed to be utilized by active objects.
*/
class CLandmarksLmOpWrapper: public CActive
    {
    public: // Constructors and destructor

        /**
        * C++ default constructor.
        * 
        */
        CLandmarksLmOpWrapper();

        /**
        * Destructor.
        */
        ~CLandmarksLmOpWrapper();

    public: // New Functions

        /**
        * Starts execution of the supplied operation.
        *
        * @param aLmOperation operation to execute. Ownership of the operation
        * is transferred to this instance.
        * @param aStatus the request status to complete when execution of the 
        * supplied LmOperation is finished.
        * @param aReportProgress ETrue if progress should be reported 
        * (incremental execution), EFalse otherwise.
        */
        void StartOperation(
            CPosLmOperation* aLmOperation,
            TRequestStatus& aStatus,
            TBool aReportProgress = EFalse);

        /**
        * Returns a pointer to the wrapped CPosLmOperation object. 
        * This object keeps ownership of the operation.
        *
        * @return a pointer to a CPosLmOperation instance
        */
        CPosLmOperation* LmOperationPtr();

        /**
        * Executes the next step of the incremental operation. Typically 
        * called when progress of the CPosLmOperaiotion is reported.
        *
        * @param aStatus the request status to complete when execution of the 
        * next step is finished.
        */
        void ExecuteNextStep(TRequestStatus& aStatus);

        /**
        * Return the current progress of the CPosLmnOperation.
        *
        * @return a number between 0-100. 100 means that the operation is 
        * completed
        */
        TInt Progress();

    protected: // from CActive

        /**
        * Implements cancellation of an outstanding request.
        */
        void DoCancel();

        /**
        * Handles an active object’s request completion event.
        */
        void RunL();

    private:

        /**
        * Executes the next step of the incremental operation.
        */
        void ExecuteNextStep();

    private: // Data

        //! the wrapped CPosLmOperation that is executed
        CPosLmOperation* iLmOperation;

        //! contains a value between 0-1 and indicates the progress
        TReal32 iProgress;

        //! the req. status of the caller executing the operation
        TRequestStatus* iCallerStatus;

        //! indicates if progress should be reported or not
        TBool iReportProgress;

    };


#endif // __LANDMARKS_LM_OP_WRAPPER_H__

// End of File

