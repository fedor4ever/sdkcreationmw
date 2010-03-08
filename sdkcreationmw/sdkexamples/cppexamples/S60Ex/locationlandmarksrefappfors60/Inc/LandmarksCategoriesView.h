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


#ifndef __LANDMARKS_CATEGORIES_VIEW_H__
#define __LANDMARKS_CATEGORIES_VIEW_H__

// INCLUDES
#include <aknview.h>

// FORWARD DECLARATIONS
class CLandmarksCategoriesContainer;
class CLandmarksApplicationEngine;
class CEikMenuPane;

// CLASS DECLARATION

/**
*  CLandmarksCategoriesView is the view displaying categories in the Landmarks 
*  Reference application.
*/
class CLandmarksCategoriesView: public CAknView
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        *
        * @param aEngine the engine used for fetching category data. 
        * This object takes ownership of the engine.
        */
        static CLandmarksCategoriesView* NewLC(
            CLandmarksApplicationEngine* aEngine);

        /**
        * Destructor.
        */
        ~CLandmarksCategoriesView();

    public: // from CAknView

        /**
        * Id Identity of this view to the system
        *
        * @return the ID of this view
        */
        TUid Id() const;

        /** 
        * HandleCommandL handles user menu selections.
        *
        * @param aCommand the enumerated code for the option selected
        */
        void HandleCommandL(TInt aCommand);

        /**
        * DoActivateL Responsible for activating this view.
        *
        * @param aPrevViewId The id of the previous view
        * @param aCustomMessageId message identifier
        * @param aCustomMessage custom message provided when the view is changed
        */
        void DoActivateL(
            const TVwsViewId& aPrevViewId,
            TUid aCustomMessageId,
            const TDesC8& aCustomMessage);

        /**
        * DoDeactivate Responsible for deactivating this view.
        */
        void DoDeactivate();

        /** 
        * DynInitMenuPaneL initializes the menu pane before it is displayed.
        *
        * @param aResourceId a resource ID indetifying the menu bar containing 
        * the menu pane to be initialized.
        * @param aMenuPane the menu pane to initialize.
        */
        void DynInitMenuPaneL(
            TInt aResourceId,
            CEikMenuPane* aMenuPane);

    private:

        /**
        * C++ constructor.
        *        
        * @param aEngine the engine used for fetching categories data.
        */
        CLandmarksCategoriesView(
            CLandmarksApplicationEngine* aEngine);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Launches a text query dialog and adds a category to the database.
        */
        void AddCategoryL();

        /**
        * Launches a text query dialog and renames a category in the database.
        */
        void RenameCategoryL();

    private: // Data

        //! The container containing the graphical components of the view
        CLandmarksCategoriesContainer* iContainer;

        //! The application engine used for modifying categories
        CLandmarksApplicationEngine* iEngine;

    };

#endif // __LANDMARKS_CATEGORIES_VIEW_H__

// End of File

