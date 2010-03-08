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


#ifndef __LANDMARKS_APPLICATION_ENGINE_H__
#define __LANDMARKS_APPLICATION_ENGINE_H__

// INCLUDES
#include <e32base.h>
#include <EPos_TPosLmSortPref.h>

// FORWARD DECLARATIONS
class CPosLandmark;
class CPosLmOperation;
class CPosLandmarkDatabase;
class CPosLandmarkSearch;
class CLandmarksEngine;
class CLandmarksCategoriesEngine;
class CLandmarksDbEventHandler;
class MLandmarksDbObserver;
class MLandmarksOperationObserver;
class CPosLandmarkCategory;

// CLASS DECLARATION

/**
*  CLandmarksApplicationEngine provides the main interface to the application 
*  engine component.
*  
*  CLandmarksApplicationEngine is the only engine class UI-components need to 
*  interact with. It uses the default landmark database for reading/writing
*  landmarks and categories.
*
*/
class CLandmarksApplicationEngine : public CBase
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        *
        * @returns A new instance of this class.
        */
        static CLandmarksApplicationEngine* NewL();

        /**
        * Destructor.
        */
        ~CLandmarksApplicationEngine();

    public: // New functions

        /**
        * NotifyViewActivated notifies the engine which view is in focus. Should
        * be called by views when they are activated/deactivated. This makes it
        * possible for the engine to prioritize its asynchronous operations.
        *
        * @param aViewId the TUid identifying the view
        * @param aIsActive ETrue if view is active, EFalse otherwise
        */
        void NotifyViewActivated(TUid aViewId, TBool aIsActive);

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
        TBool StartInitializeDbIfNecessaryL(
            MLandmarksOperationObserver* aObserver);

        /**
        * AddDbObserverL registers the supplied observer as observer of 
        * database events.
        *
        * @param aObserver the observer to be notified when a database event 
        * occurs
        */
        void AddDbObserverL(MLandmarksDbObserver* aObserver);

        // Landmark related functions

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
        * AddLandmarkL adds a landmark to the database.
        *
        * @param aLandmark the landmark to add
        */
        void AddLandmarkL(CPosLandmark& aLandmark);

        /**
        * StartSearchingLandmarksL starts an asynchronous search operation for
        * landmarks. When the search completes the supplied observer is 
        * notified and it is supposed to fetch the matches by calling 
        * @ref FetchLandmarkSearchResult(). All landmarks in the database are 
        * returned.
        * 
        * @param aObserver the observer that is notified when the search 
        * operation completes
        */
        void StartSearchingLandmarksL(
            MLandmarksOperationObserver* aObserver);

        /**
        * StartSearchingLandmarksL starts an asynchronous search operation for
        * landmarks. The names of the landmarks are used as criterion for a 
        * match. When the search completes the supplied observer is notified 
        * and it is supposed to fetch the matches by calling 
        * @ref FetchLandmarkSearchResult().
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
        * FetchLandmarkSearchResult should be called after a successful 
        * landmark search operation. It returns the matches from the 
        * previous landmark search operation. Ownership of the returened array
        * is kept by this class.
        * 
        * @return an array containing matches from a previous landmark search
        * operation
        */
        RArray<TPosLmItemId>* FetchLandmarkSearchResult();

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
        * preceding landmark read operation. Ownership of the returned array is
        * transferred to the caller.
        *
        * @return an array containing partially read landmarks from a previous 
        * read operation
        */
        CArrayPtr<CPosLandmark>* FetchLandmarksLC();

        // Category related functions

        /**
        * CategoryLC reads all fields of a category in the default 
        * landmark database. Ownership of the returned category is transferred
        * to the caller.
        * 
        * @param aItemId the ItemId identifying the category
        * @return a category
        */
        CPosLandmarkCategory* CategoryLC(TPosLmItemId aItemId);

        /**
        * DeleteCategoryL deletes a category.
        *
        * @param aItemId the itemId identifying the category to be deleted
        */
        void DeleteCategoryL(TPosLmItemId aItemId);

        /**
        * UpdateCategoryL updates a modified category.
        *
        * @param aCategory the category to update
        */
        void UpdateCategoryL(const CPosLandmarkCategory& aCategory);

        /**
        * AddCategoryL adds a category to the default database.
        *
        * @param aCategory the category to add
        */
        void AddCategoryL(CPosLandmarkCategory& aCategory);

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
        * @ref FetchLandmarkSearchResult().
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
        * FetchCategorySearchResult should be called after a successful 
        * category search operation. It returns the matches from the 
        * previous category search operation. Ownership of the returned array
        * is kept by this class.
        * 
        * @return an array containing matches from a previous categories search
        * operation
        */
        RArray<TPosLmItemId>* FetchCategorySearchResult();

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
        * preceding landmark read operation. Ownership of teh returned array is
        * transferred to the caller.
        *
        * @return an array containing partially read landmarks from a previous 
        * read operation
        */
        CArrayPtr<CPosLandmarkCategory>* FetchCategoriesLC();

        /**
        * CategoriesL reads all categories in the default database 
        * synchronously. Ownership of the returned array is transferred to the
        * caller.
        *
        * @return an array containing all categories in the database.
        */
        CArrayPtr<CPosLandmarkCategory>* CategoriesL();

    private:

        /**
        * C++ default constructor.
        */
        CLandmarksApplicationEngine();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * CompactIfNeededL compacts the database synchronously if necessary.
        */
        void CompactIfNeededL();

    private: // Data

        //! Default landmark database
        CPosLandmarkDatabase* iDb;

        //! Landmarks engine part
        CLandmarksEngine* iLandmarksEngine;

        //! Categories engine part
        CLandmarksCategoriesEngine* iCategoriesEngine;

        //! Database event handler
        CLandmarksDbEventHandler* iDbEventHandler;

    };


#endif // __LANDMARKS_APPLICATION_ENGINE_H__

// End of File

