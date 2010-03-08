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


#ifndef __LANDMARKS_VIEW_H__
#define __LANDMARKS_VIEW_H__

// INCLUDES
#include <aknview.h>

// FORWARD DECLARATIONS
class CLandmarksApplicationEngine;
class CAknNavigationDecorator;
class CLandmarksContainer;
class CLandmarksEngine;

// CLASS DECLARATION

/**
*  CLandmarksView is the view displaying list of landmarks
*/
class CLandmarksView: public CAknView
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        *
        * @param aNaviDecorator the navigation decorator displaying the tabs 
        * @param aEngine the engine used for fetching category data.
        */
        static CLandmarksView* NewLC(
            CAknNavigationDecorator* aNaviDecorator,
            CLandmarksApplicationEngine& aEngine);

        /**
        * Destructor.
        */
        ~CLandmarksView();

    public: // from CAknView

        /**
        * Id Identity of this view to the system.
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
        * DoActivateL is responsible for activating this view.
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
        * DoDeactivate is responsible for deactivating this view.
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
        * @param aNaviDecorator the navigation decorator displaying the tabs 
        * @param aEngine the engine used for fetching categories data.
        */
        CLandmarksView(
            CAknNavigationDecorator* aNaviDecorator, 
            CLandmarksApplicationEngine& aEngine);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Launches a landmark edit dialog and adds a landmark to the database.
        *
        * @param aUseCurrentLocation ETrue if the landmark will be based on 
        * current location. EFalse if no location should be predefined.
        */
        void AddLandmarkL(TBool aUseCurrentLocation);

    private: // Data

        //! The container containing the graphical components of the view
        CLandmarksContainer* iContainer;

        //! The application engine used for modifying categories
        CLandmarksApplicationEngine& iEngine;

        //! Decorates the navi pane (with navigation arrows etc)
        CAknNavigationDecorator* iNaviDecorator;
    };


#endif // __LANDMARKS_VIEW_H__

// End of File

