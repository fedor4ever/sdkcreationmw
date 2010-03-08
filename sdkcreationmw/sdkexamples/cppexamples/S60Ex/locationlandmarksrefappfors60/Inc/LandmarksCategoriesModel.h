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


#ifndef __LANDMARKS_CATEGORIES_MODEL_H__
#define __LANDMARKS_CATEGORIES_MODEL_H__

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CPosLandmarkCategory;

// CLASS DECLARATION

/**
*  CLandmarksCategoriesModel holds the data displayed in the category listbox.
*/
class CLandmarksCategoriesModel : public CBase
	{
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CLandmarksCategoriesModel* NewL();

        /**
        * Destructor.
        */
	    ~CLandmarksCategoriesModel();

    public: // New functions

        /**
        * Sets the data member iItemIds
        * 
        * @param aItemIds a pointer to an array of item ids
        */
        void SetItemIds(RArray<TPosLmItemId>* aItemIds);

        /**
        * ItemList returns a pointer to an array to be used for the listbox
        *
        * @return a pointer to an array of descriptors to be displayed in the 
        * listbox
        */
	    CDesCArraySeg* ItemList();

        /**
        * SetCurrentItem sets the item in the model that is selected in the 
        * listbox.
        *
        * @param aIndex the index in the list box that is selected
        */
	    void SetCurrentItem(TInt aIndex);
	    
        /**
        * CurrentItemId returns the id of the current item from the model.
        *
        * @return the id of the current item from the model, KPosLmNullItemId 
        * if there is no current item.
        */
	    TPosLmItemId CurrentItemId();
	    
        /**
        * CreateIconListL creates and returns an array of icons that is 
        * displayed in the listbox. Caller takes ownership of the returned 
        * array.
        *
        * @return a pointer to an array to be displayed in the listbox
        */
        CIconList* CreateIconListL();

        /**
        * Repopulates a part of the model.
        *
        * @param aCategories the categories to insert to the model
        */
        void RepopulateModelL(
            CArrayPtr<CPosLandmarkCategory>* aCategories);

    private:

        /**
        * C++ constructor.
        */
	    CLandmarksCategoriesModel();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
	    void ConstructL();

        /**
        * Formats a list item to fit a CEikColumnistBox with one graphic 
        * column and one descriptor column.
        *
        * @param aCategory the category this list item will represent
        * @return a formatted descriptor buffer
        */
        HBufC* FormatListItemLC(CPosLandmarkCategory& aCategory);

        /**
        * Appends an icen to the icon array.
        *
        * @param aMbmFile the multi bitmap file containing the icon to add
        * @param aIconOffset the offset in the mbm-file where the icon is 
        * located
        * @param aMaskOffset the offset in the mbm-file where the icon mask is
        * located
        */
        void AppendIconL(
            const TDesC& aMbmFile, 
            TInt aIconOffset, 
            TInt aMaskOffset);

    private: // Data

        //! contains the ids idenetifying the categories in the DB
	    RArray<TPosLmItemId>* iItemIds;

        //! indicates which item in the model is selected
	    TInt iCurrentItem;

        /** Array of formated descriptors suitable to display in a column 
            listbox */
	    CDesCArraySeg* iListItems;

        //! an array of category icons
        CIconList* iIconList;

	};

#endif // __LANDMARKS_CATEGORIES_MODEL_H__

// End of File

