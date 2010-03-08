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


#ifndef __LANDMARKS_CATEGORIES_ENGINE_H__
#define __LANDMARKS_CATEGORIES_ENGINE_H__

// INCLUDES
#include "LandmarksEngineBase.h"
#include "LandmarksOperationObserver.h"

// FORWARD DECLARATIONS
class CPosLmCategoryManager;
class CPosLandmarkCategory;
class CPosLandmarkDatabase;
class CPosLmItemIterator;

// CLASS DECLARATION

/**
*  Implements all functions of the engine related to categories. This engine 
*  servs the categories view.
*/
class CLandmarksCategoriesEngine : public CLandmarksEngineBase
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        *
        * @param aDb an instance of the default landmark database
        */
        static CLandmarksCategoriesEngine* NewL(CPosLandmarkDatabase& aDb);

        /**
        * Destructor.
        */
        ~CLandmarksCategoriesEngine();

    public: // New functions

        /**
        * AddCategoryL adds a category to the database.
        *
        * @param aCategory the landmark to add
        */
        void AddCategoryL(CPosLandmarkCategory& aCategory);

        /**
        * UpdateCategoryL updates a category in the database.
        *
        * @param aCategory the landmark to update
        */
        void UpdateCategoryL(const CPosLandmarkCategory& aCategory);

        /**
        * DeleteCategoryL deletes a category.
        *
        * @param aItemId the itemId identifying the category to be deleted
        */
        void DeleteCategoryL(TPosLmItemId aItemId);

        /**
        * CategoryLC reads a category in the default landmark database.
        * Ownership of the returned category is transferred to the caller. 
        * 
        * @param aItemId the ItemId identifying the category
        * @return a category
        */
        CPosLandmarkCategory* CategoryLC(TPosLmItemId aItemId);

        /**
        * StartSearchingCategoriesL starts an asynchronous search operation for
        * categories. When the search completes the supplied observer is 
        * notified and it is supposed to fetch the matches by calling 
        * @ref FetchCategorySearchResult. All categoiries in the database are 
        * returned.
        * 
        * @param aObserver the observer that is notified when the search 
        * operation completes
        */
        void StartSearchingCategoriesL(
            MLandmarksOperationObserver* aObserver);

        /**
        * StartSearchingCategoriesL starts an asynchronous search operation for
        * categories. The names of the categories are used as criterion for a
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
        void StartSearchingCategoriesL(
            const TDesC& aSearchPattern,
            TBool aSearchOnlyInPreviousMatches,
            MLandmarksOperationObserver* aObserver);

        /**
        * FetchSearchResultL should be called after a successful 
        * category search operation. It returns the matches from the 
        * previous category search operation. Ownership of the returned array 
        * is kept by this class.
        * 
        * @return an array containing matches from a previous categories search
        * operation
        */
        RArray<TPosLmItemId>* FetchSearchResult();

        /**
        * StartReadingCategoriesL should be called after a successful 
        * category search operation. It continously reads a number of 
        * categories until all landmarks found in a previous search operation 
        * are read. The method is asynchronous and every time a number of 
        * categories are read the supplied observer is notified. 
        * @ref FetchLandmarksLC should be called to fetch the read categories.
        *
        * @param aNrOfItemsToReadPerBundle the number of categories to read 
        * before notifying the supplied observer
        * @param aObserver the observer that is notified every time a bundle of 
        * categories are read
        */
        void StartReadingCategoriesL(
            TInt aNrOfItemsToReadPerBundle,
            MLandmarksOperationObserver* aObserver);

        /**
        * FetchCategoriesLC should be called after a successful category read
        * operation. It returns the categories that were read during a
        * preceding landmark read operation. Ownership of the returned array 
        * is transferred to the caller.
        *
        * @return an array containing partially read landmarks from a previous 
        * read operation
        */
        CArrayPtr<CPosLandmarkCategory>* FetchCategoriesLC();

        /**
        * CategoriesL reads all categories in the default database 
        * synchronously.Ownership of the returned array is transferred 
        * to the caller.
        *
        * @return an array containing all categories in the database.
        */
        CArrayPtr<CPosLandmarkCategory>* CategoriesL();

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
        CLandmarksCategoriesEngine(CPosLandmarkDatabase& aDb);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * ReadSomeLandmarksL reads a number of categories. The number of 
        * categories is specified when initiating an asynchronous categories 
        * read operation.
        */
        void ReadSomeCategoriesL();

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

        //! provides access to categories in the default database
        CPosLmCategoryManager* iCatManager;

        //! an array containing the item Ids of the last search
        RArray<TPosLmItemId> iItemIds;

        /** represents a subset of a search result. Used when 
            reading categories */
        RArray<TPosLmItemId> iSubSetOfIds;

        /** the observer to notify the progress of asynchronous 
            operations */
        MLandmarksOperationObserver* iObserver;

        //! keeps track of which type of operation is executed
        TOperation iActiveOperation;

        //! keeps track of which item to read
        TInt iCurrentItemId;

        //! keeps track of how many items to read per bundle
        TInt iNrOfItemsToRead;

        /** indicates if there is any previous result to 
            search */
        TBool iSearchResultExists;

        /** indicates if the current search operation has 
            carried out with or without search pattern */
        TBool iFilterSearch;

    };


#endif // __LANDMARKS_CATEGORIES_ENGINE_H__

// End of File

