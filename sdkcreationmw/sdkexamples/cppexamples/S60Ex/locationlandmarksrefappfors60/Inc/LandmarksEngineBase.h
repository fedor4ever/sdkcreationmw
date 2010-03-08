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
* Description:  The base class for engines
*
*/


#ifndef __LANDMARKS_ENGINE_BASE_H__
#define __LANDMARKS_ENGINE_BASE_H__

//  INCLUDES
#include <e32base.h>
#include <EPos_Landmarks.h>

// FORWARD DECLARATIONS
class CPosLandmarkDatabase;
class CPosLandmarkSearch;
class CLandmarksLmOpWrapper;

// CLASS DECLARATION

/**
*  Abstract base class for sub engines.
*
*/
class CLandmarksEngineBase : public CActive
    {
    public: // Constructors and destructor

        /**
        * Destructor.
        */
        ~CLandmarksEngineBase();

    public: // New functions

        /**
        * SetPrio updates the @ref iPriority data member.
        *
        * @param aPriority the priority to set
        */
        void SetPrio(CActive::TPriority aPriority);

    protected: // From CActive

        /**
        * RunL should be implemented by sub classes.
        */
        virtual void RunL() = 0;
        
        /**
        * DoCancel should be implemented by sub classes.
        */
        virtual void DoCancel() = 0;

        /**
        * RunError should be implemented by sub classes in case RunL might 
        * leave.
        */
        virtual TInt RunError(TInt aError) = 0;

    protected: // New functions

        /**
        * C++ constructor.
        * 
        * @param aDb an instance of the default landmark database
        */
        CLandmarksEngineBase(CPosLandmarkDatabase& aDb);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * SetPriorityAndSetActive updates the priority of this active object 
        * if necessary and sets it to active. This method should be used by sub 
        * classes rather than CActive::SetActive()
        */
        void SetPriorityAndSetActive();

    protected: // Data

        //! The default landmark database
        CPosLandmarkDatabase& iDb;

        //! Used for searching the database
        CPosLandmarkSearch* iSearcher;

        //! An active object wrapping a CPosLmOperation object
        CLandmarksLmOpWrapper* iLmOpWrapper;

    private: // Data

        //! Used when setting priority of this active object
        CActive::TPriority iPriority;
    };


#endif // __LANDMARKS_ENGINE_BASE_H__

// End of File

