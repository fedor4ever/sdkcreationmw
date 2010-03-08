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


#ifndef __LANDMARKS_INFOVIEW_H__
#define __LANDMARKS_INFOVIEW_H__

// INCLUDES
#include <aknview.h>
#include "LandmarksOperationObserver.h"

// FORWARD DECLARATIONS
class CLandmarksApplicationEngine;
class CAknNavigationDecorator;
class CLandmarksInfoContainer;
class CLandmarksEngine;

// CLASS DECLARATION

/**
*  CLandmarksInfoView is the view displaying landmark's details
*/
class CLandmarksInfoView: public CAknView
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        *
        * @param aNaviDecorator the navigation decorator displaying the tabs 
        * @param aEngine the engine used for fetching category data.
        */
        static CLandmarksInfoView* NewLC(
            CAknNavigationDecorator* aNaviDecorator,
            CLandmarksApplicationEngine& aEngine);

        /**
        * Destructor.
        */
        ~CLandmarksInfoView();

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

    private:

        /**
        * C++ constructor.
        *        
        * @param aNaviDecorator the navigation decorator displaying the tabs 
        * @param aEngine the engine used for fetching categories data.
        */
        CLandmarksInfoView(
            CAknNavigationDecorator* aNaviDecorator, 
            CLandmarksApplicationEngine& aEngine);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Launches a landmark edit dialog
        */
        void EditLandmarkL();

    private: // Data

        //! The container containing the graphical components of the view
        CLandmarksInfoContainer* iContainer;

        //! The application engine used for accessing landmark data
        CLandmarksApplicationEngine& iEngine;

        //! Ddecorates the navi pane (with navigation arrows etc)
        CAknNavigationDecorator* iNaviDecorator;
        
        //! Application name
        HBufC* iAppName;
    };


#endif // __LANDMARKS_INFOVIEW_H__

// End of File

