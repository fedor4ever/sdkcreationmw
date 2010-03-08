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


#ifndef __LANDMARKS_MODEL_H__
#define __LANDMARKS_MODEL_H__

// INCLUDES
#include <e32base.h>
#include <badesca.h>

#include <EPos_Landmarks.h>

// FORWARD DECLARATIONS
class CPosLandmark;

// CLASS DECLARATION

/**
*  Contains the data model of the landmarks view.
*  
*/
class CLandmarksModel : public CBase
	{
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CLandmarksModel* NewL();

        /**
        * Destructor.
        */
	    ~CLandmarksModel();

    public: // New functions

        /**
        * Sets the data member iItemIds
        * 
        * @param aItemIds a pointer to an array of item ids
        */
        void SetItemIds(RArray<TPosLmItemId>* aItemIds);

	    /**
        * Returns a pointer to an array to be used for a list box.
        *
        * @return an array of descriptors formatted to fit a CEikColumnListBox 
        * with one icon column to the left and one descriptor column to the 
        * right.
        */
	    CDesCArraySeg* ItemList();
		
	    /**
        * Sets current item in the model. The current item should match the  
        * current selected one in the listbox displaying this model.
        *
        * @param aIndex the index of the current item in the list.
        */
	    void SetCurrentItem(TInt aIndex);
	
	    /**
        * Returns the item id of the current item in the model.
        *
        * @return the item id of the current item in the model. 
        * KPosLmNullItemId if no item is set as current
        */
	    TPosLmItemId CurrentItemId();

        /**
        * Creates an array of icons that should be displayed be the listbox
        * displaying the data of this model.
        *
        * @return an array of icons. Ownership of CIconList* is tranferred to 
        * caller
        */
        CIconList* CreateIconListL();
	
        /**
        * Repopulates a part of the model.
        *
        * @param aLandmarks the landmarks to insert to the model
        */
        void RepopulateModelL(
            CArrayPtr<CPosLandmark>* aLandmarks);

    private:

        /**
        * Formats a list item to fit a CEikColumnistBox with one graphic 
        * column and one descriptor column.
        *
        * @param aLandmark the landmarks this list item will represent
        * @return a formatted descriptor buffer
        */
        HBufC* FormatListItemLC(CPosLandmark& aLandmark);

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

    private:

        /**
        * C++ constructor.
        */
	    CLandmarksModel();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
	    void ConstructL();

    private:

        //! contains the current item in the model
        TInt iCurrentItem;

        //! an array of formatted descriptors
        CDesCArraySeg* iListItems;

        //! an array of item ids matching the landmarks represented by iListItems
        RArray<TPosLmItemId>* iItemIds;

        //! an array of landmark icons
        CIconList* iIconList;

        //! the next index to repopulate
        TInt iIndexToPopulate;

	};

#endif // __LANDMARKS_MODEL_H__

// End of File

