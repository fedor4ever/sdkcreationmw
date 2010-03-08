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


#ifndef __LANDMARKS_CATEGORIES_DIALOG_H__
#define __LANDMARKS_CATEGORIES_DIALOG_H__

// INCLUDES
#include <e32base.h>
#include <aknselectionlist.h>
#include <EPos_Landmarks.h>

// FORWARD DECLARATIONS
class CLandmarksApplicationEngine;
class CLandmarksLmCategoriesModel;
class CPosLandmarkCategory;
class CAknTitlePane;
class CGulIcon;

// CLASS DECLARATION

/**
*  CLandmarksCategoriesDialog is a dilog for selecting which categories a 
*  landmark belongs to.
*/
class CLandmarksCategoriesDialog : public CAknMarkableListDialog
	{
    public: // Constructors and destructor

        /**
        * Creates, executes and destroys the dialog after dismissed.
        * @param aLmCategoriesModel landmark categories
        * @return EFalse if dialog was cancelled
        */
        static TInt RunDialogL(CLandmarksLmCategoriesModel& aLmCategoriesModel);

        /**
        * Destructor.
        */
	    ~CLandmarksCategoriesDialog();

    protected: // From CEikDialog

        /**
        * Called after the dialog is layed out.
        */
        void PostLayoutDynInitL();

        /**
        * OfferKeyEventL handles key events.
        *
        * @param aKeyEvent the key event
        * @param aType the type of key event
        * @return EKeyWasConsumed if keyevent was handled, EKeyWasNotConsumed 
        * otherwise 
        */
		TKeyResponse OfferKeyEventL(
            const TKeyEvent& aKeyEvent,
            TEventCode aType);

    private:

        /**
        * C++ constructor.
        *
        * @param aSelectedItem the selected item.
        * @param aMarkedItems an array of selected categories.
        * @param aListItems array of items to display.
        */
	    CLandmarksCategoriesDialog(
            TInt& aSelectedItem,
            CArrayFixFlat<TInt>* aMarkedItems,
            CDesCArray* aListItems);

    private: // Data

        //! The title pane that is dynamically updated
        CAknTitlePane* iTitlePane;

        //! The original title that the title pane displayed before modified
        HBufC* iOriginalTitle;
	};

#endif // __LANDMARKS_CATEGORIES_DIALOG_H__

// End of File

