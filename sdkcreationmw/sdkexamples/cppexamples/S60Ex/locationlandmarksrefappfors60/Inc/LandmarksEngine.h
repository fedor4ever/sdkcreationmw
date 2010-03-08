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


#ifndef __LANDMARKS_ENGINE_H__
#define __LANDMARKS_ENGINE_H__

// INCLUDES
#include "LandmarksEngineBase.h"
#include "LandmarksOperationObserver.h"
#include <EPos_TPosLmSortPref.h>

// FORWARD DECLARATIONS
class CPosLandmark;

// CLASS DECLARATION

/**
*  Implements all functions of the engine related to landmarks. This engine
*  servs the landmarks view.
*/
class CLandmarksEngine : public CLandmarksEngineBase
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        *
        * @param aDb an instance of the defaukt landmark database
        */
        static CLandmarksEngine* NewL(
            CPosLandmarkDatabase& aDb);

        /**
        * Destructor.
        */
        ~CLandmarksEngine();

    public: // New functions

        /**
        * StartInitializeDbIfNecessaryL indicates if the default database needs
        * to be initialised before it can be used. If it needs to be
        * initialized the initialization is started immediately and the
        * supplied observer is notified when initialization is ready.
        *
        * @param aObserver the observer to be notified when initialization is
        * ready
        * @return ETrue if initializing is necessary, EFalse otherwise
        */
        TBool StartInitializeDbIfNecessaryL(MLandmarksOperationObserver* aObserver);

        /**
        * AddLandmarkL adds a landmark to the database.
        *
        * @param aLandmark the landmark to add
        */
        void AddLandmarkL(CPosLandmark& aLandmark);

        /**
        * CommitLandmarkL commits a modified landmark.
        *
        * @param aLandmark the landmark to commit
        */
        void CommitLandmarkL(const CPosLandmark& aLandmark);

        /**
        * DeleteLandmarkL deletes a landmark.
        *
        * @param aItemId the itemId identifying the landmark to be deleted
        */
        void DeleteLandmarkL(TPosLmItemId aItemId);

        /**
        * LandmarkLC reads all fields of a landmark in the default
        * landmark database. Ownership of the returned landmark is transferred
        * to the caller.
        *
        * @param aItemId the ItemId identifying the landmark
        * @return a landmark
        */
        CPosLandmark* LandmarkLC(TPosLmItemId aItemId);

        /**
        * StartSearchingLandmarksL starts an asynchronous search operation for
        * landmarks. When the search completes the supplied observer is
        * notified and it is supposed to fetch the matches by calling
        * @ref FetchLandmarkSearchResult. All landmarks in the database are
        * returned.
        *
        * @param aObserver the observer that is notified when the search
        * operation completes
        */
        void StartSearchingLandmarksL(
            MLandmarksOperationObserver* aObserver);

        /**
        * StartSearchingLandmarksL starts an asynchronous search operation for
        * landmarks. The names of the landmarks are uses as criterion for a
        * match. When the search completes the supplied observer is notified
        * and it is supposed to fetch the matches by calling
        * @ref FetchLandmarkSearchResult.
        *
        * @param aSearchPattern search pattern to compare landmarks against
        * @param aSearchOnlyInPreviousMatches boolean indicating that only
        * previous matches should be searched
        * @param aObserver the observer that is notified when the search
        * operation completes
        */
        void StartSearchingLandmarksL(
            const TDesC& aSearchPattern,
            TBool aSearchOnlyInPreviousMatches,
            MLandmarksOperationObserver* aObserver);

        /**
        * FetchSearchResultL should be called after a successful
        * landmark search operation. It returns the matches from the
        * previous landmark search operation. Ownership of the returned array
        * is kept by this class.
        *
        * @return an array containing matches from a previous landmark search
        * operation
        */
        RArray<TPosLmItemId>* FetchSearchResult();

        /**
        * StartReadingLandmarksL should be called after a successful
        * landmark search operation. It continously reads a number of landmarks
        * partially, i.e. only the name and the icon of the landmarks are read,
        * until all landmarks found in a previous search operation are read.
        * The method is asynchronous and every time a number of landmarks are
        * read the supplied observer is notified, @ref FetchLandmarksLC should
        * be called to fetch the read landmarks.
        *
        * @param aNrOfItemsToReadPerBundle the number of items to read before
        * notifying the supplied observer
        * @param aObserver the observer that is notified every time a bundle of
        * landmarks are read
        */
        void StartReadingLandmarksL(
            TInt aNrOfItemsToReadPerBundle,
            MLandmarksOperationObserver* aObserver);

        /**
        * FetchLandmarksLC should be called after a successful landmark read
        * operation. It returns the landmarks that were partially read during a
        * preceding landmark read operation. Ownership of the returned array
        * is transferred to the caller.
        *
        * @return an array containing partially read landmarks from a previous
        * read operation
        */
        CArrayPtr<CPosLandmark>* FetchLandmarksLC();


    protected: // From CActive

        /**
        * Handles an active object’s request completion event.
        */
        void RunL();

        /**
        * Implements cancellation of an outstanding request.
        */
        void DoCancel();

        /**
        * Handles a leave occurring in the request completion event
        * handler RunL().
        *
        * @return KErrNone
        */
        TInt RunError(TInt aError);

    private: // New functions

        /**
        * C++ constructor.
        *
        * @param aDb an instance of the default landmark database
        */
        CLandmarksEngine(CPosLandmarkDatabase& aDb);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * ReadSomeLandmarksL reads a number of landmarks. The number of
        * landmarks is specified when initiating an asynchronous landmark read
        * operation.
        */
        void ReadSomeLandmarksL();

        /**
        * NotifyOperationReadyL notifies the observer of an asynchronous
        * operation that the operation has completed.
        *
        * @param aOperation the kind of operation that is finished
        * @param aErrorCode the complete code the operation returned
        */
        void NotifyOperationReadyL(TOperation aOperation, TInt aErrorCode);

        /**
        * Fetches the matches after a search operation and populates the data
        * member @ref iItemIds
        */
        void PopulateItemIdArrayL();

    private: // Data

        //! an array containing the item Ids of the last search
        RArray<TPosLmItemId> iItemIds;

        //! the observer to notify the progress asynchronous operations
        MLandmarksOperationObserver* iObserver;

        //! keeps track of which type of operation is executed
        TOperation iActiveOperation;

        //! keeps track of which item to read
        TInt iCurrentItemId;

        //! keeps track of how many items to read per bundle
        TInt iNrOfItemsToRead;

        //! indicates if there is any previous result to search
        TBool iSearchResultExists;

        /** indicates if the current search operation has been carried
        out with or without search pattern */
        TBool iFilterSearch;

        //! defines the sort preferences for this engine
        TPosLmSortPref iSortPref;

    };

#endif // __LANDMARKS_ENGINE_H__

// End of File

