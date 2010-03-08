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


#ifndef __LANDMARKS_LM_CATEGORIES_MODEL_H__
#define __LANDMARKS_LM_CATEGORIES_MODEL_H__

// INCLUDES
#include <e32std.h>
#include <EPos_Landmarks.h>

// FORWARD DECLARATIONS
class CLandmarksApplicationEngine;
class CPosLandmarkCategory;
class CGulIcon;

// CLASS DECLARATION

/**
*  CLandmarksLmCategoriesModel holds the data displayed in the category 
*  listbox in the landmarks category edit dialog.
*/
class CLandmarksLmCategoriesModel : public CBase
	{
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        *
        * @param aEngine the landmarks application engine.
        * @param aMarkedCategories an array of selected landmarks.
        */
        static CLandmarksLmCategoriesModel* NewL(
            CLandmarksApplicationEngine& aEngine,
            RArray<TPosLmItemId>& aMarkedCategories);

        /**
        * Destructor.
        */
	    ~CLandmarksLmCategoriesModel();

    public: // New functions

        /**
        * Stores ids of selected categories.
        *
        * @param aSelectedIndexes array of indexes of selected catgeories
        */
	    void SelectCategoriesL(CArrayFixFlat<TInt>& aSelectedIndexes);

        /**
        * Returns a descriptor array of formatted list items.
        *
        * @return an descriptor array suitable for a two-column listbox
        */
        CDesCArray* FormattedCategoryNamesL();

        /**
        * Returns array of indices of selected categories.
        *
        * @return array of indexes of selected catgeories
        */
		CArrayFixFlat<TInt>* SelectedCategoriesL();

    private:

        /**
        * C++ constructor.
        *
        * @param aMarkedCategories an array of selected landmarks.
        */
	    CLandmarksLmCategoriesModel(
            RArray<TPosLmItemId>& aMarkedCategories);

        /**
        * By default Symbian 2nd phase constructor is private.
        *
        * @param aEngine the landmarks application engine.
        */
	    void ConstructL(CLandmarksApplicationEngine& aEngine);

    private: // Data

        //! contains all existing categories
        CArrayPtr<CPosLandmarkCategory>* iCategories;

        /** contains Ids for those categories that are selected for the 
            edited landmark. */
        RArray<TPosLmItemId>& iMarkedCategoryIds;

        /** An array of formatted category names suitable for a two-column 
            listbox. */
	    CDesCArrayFlat* iListItems;
	    
	};

#endif // __LANDMARKS_LM_CATEGORIES_MODEL_H__

// End of File

