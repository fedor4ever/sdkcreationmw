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


#ifndef __LANDMARKS_OPERATION_OBSERVER_H__
#define __LANDMARKS_OPERATION_OBSERVER_H__

// INCLUDES
#include <e32base.h>

// CONSTANTS
const TInt KOperationReady = 100;
const TInt KOperationNotReady = 0;

// DATA TYPES
/**
* TOperation indicates which operation type has been processed
*/
enum TOperation
    {
    ENoOperation,
    EInitializeDb,
    ELandmarkSearch,
    ECategorySearch,
    ELandmarkRead,
    ECategoryRead,
    EAcquiringLocation
    };

// CLASS DECLARATION

/**
*  An observer interface used for notifying the execution progress of 
*  asynchronous opearations. 
*  Typically used by objects deriving from other classes than CActive.
*/
class MLandmarksOperationObserver
    {
    public: // New functions

        /**
        * NotifyOperationProgressL notifies the progress and status of an
        * asynchronous operation.
        *  
        * @param aOperation the type of operation that is reported.
        * @param aProgress the progress of the operation.
        * @param aErrorCode the status of the operation.
        */
        virtual void NotifyOperationProgressL(
            TOperation aOperation, 
            TInt aProgress, 
            TInt aErrorCode) = 0;

    };

#endif // __LANDMARKS_OPERATION_OBSERVER_H__

// End of File

