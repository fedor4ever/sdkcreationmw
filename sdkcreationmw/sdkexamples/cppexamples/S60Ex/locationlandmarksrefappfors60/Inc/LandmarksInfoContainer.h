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


#ifndef __LANDMARKS_INFOCONTAINER_H__
#define __LANDMARKS_INFOCONTAINER_H__

// INCLUDES
#include <coecntrl.h>
#include <EPos_Landmarks.h>
#include "LandmarksInfoContainer.h"

// FORWARD DECLARATIONS
class CEikTextListBox;
class CAknNavigationDecorator;
class CAknTitlePane;
class CAknView;
class CLandmarksApplicationEngine;
class CLandmarksInfoModel;
class CPosLandmark;

// CLASS DECLARATION

/**
*  This is the landmarks container that contains a listbox with landmark's
*  fields listed.
*/
class CLandmarksInfoContainer : public CCoeControl
    {
    public: // Constructors and destructor

        /**
        * C++ constructor.
        *
        * @param aView parent view
        * @param aEngine the landmark engine
        * @param aNaviDecorator the tab group to hide when editing landmarks
        */
        CLandmarksInfoContainer(
            CAknView& aView,
            CLandmarksApplicationEngine& aEngine,
            CAknNavigationDecorator* aNaviDecorator);

        /**
        * Symbian 2nd phase constructor. Should be called after constructor to
        * fully construct the object.
        *
        * @param aRect the rectangle this Container will be drawn to
        */
        void ConstructL(const TRect& aRect);

        /**
        * Destructor.
        */
        ~CLandmarksInfoContainer();

    public: // New functions

        /**
        * Enables/disables the navi pane.
        *
        * @param aVisible ETrue enables navipane, EFalse disables navipane
        */
        void MakeNaviPaneVisibleL(TBool aVisible);

        /** Stores title pane */
        void StoreTitlePaneL();

        /** Retores title pane */
        void RestoreTitlePane();

        /**
        * Loads specified landmark into the container
        * @param aLandmarkId The ID of the landmark to be viewed.
        */
        void LoadLandmarkL(TPosLmItemId aLandmarkId);

        /**
        * Launches a dialog for editing the current landmark.
        * Removes tabs on the navi pane before launching the dialog. Restores
        * the navi pane after the dialog is dismissed.
        */
        void EditLandmarkL();

    public: // from CoeControl

        /**
        * Gets the number of controls contained in a compound control
        *
        * @return Number of component controls
        */
        TInt CountComponentControls() const;

        /**
        * Gets the specified component
        *
        * @param aIndex specification for component pointer
        * @return Pointer to component control
        */
        CCoeControl* ComponentControl(TInt aIndex) const;

        /**
        * Called by framework when the view size is changed.
        */
        void SizeChanged();

        /**
        * Handles key events.
        *
        * @param aKeyEvent the key event
        * @param aType the type of key event
        * @return EKeyWasConsumed if keyevent was handled, EKeyWasNotConsumed
        * otherwise
        */
        TKeyResponse OfferKeyEventL(
            const TKeyEvent& aKeyEvent, TEventCode aType);

        /** Reacts to screen size change */
        void HandleResourceChange(TInt aType);

    private:

        /**
        * Creates the listbox.
        */
        void CreateListBoxL();

        /**
        * Updates the title bar from landmark name.
        */
        void UpdateTitleBarL();

        /**
        * Updates the view.
        */
        void UpdateL();

    private: // Data

        //! The control used to display landmark info.
        CEikTextListBox* iListBox;

        //! The parent view
        CAknView& iView;

        //! iEngine the application engine used for accessing landmarks
        CLandmarksApplicationEngine& iEngine;

        //! Decorates the navi pane (with navigation arrows etc)
        CAknNavigationDecorator* iNaviDecorator;
        
        //! The data model for the listbox
        CLandmarksInfoModel* iModel;

        //! The landmark being viewed
        CPosLandmark* iLandmark;

        //! The title pane that is dynamically updated
        CAknTitlePane* iTitlePane;

        //! The original title that the title pane displayed before modified
        HBufC* iOriginalTitle;

        //! Fields used in the list
        RArray<TInt> iFields;
    };

#endif // __LANDMARKS_INFOCONTAINER_H__

