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


#ifndef __LANDMARKS_CONTAINER_BASE_H__
#define __LANDMARKS_CONTAINER_BASE_H__

// INCLUDES
#include "LandmarksOperationObserver.h"
#include "LandmarksDbObserver.h"
#include <coecntrl.h>

// FORWARD DECLARATIONS
class CAknView;
class CLandmarksApplicationEngine;
class CLandmarksListBox;
class CAknSearchField;

// CLASS DECLARATION

/**
*  CLandmarksContainerBase is an abstrct class that contains functions and data
*  common for all view containers in this application.
*/
class CLandmarksContainerBase :
    public CCoeControl,
    public MLandmarksDbObserver,
    public MLandmarksOperationObserver
    {
    public: // Constructors and destructor

        /**
        *  Destructor
        */
        ~CLandmarksContainerBase();

    public: // New functions

        /**
        * Indicates whether a listbox item is selected or not.
        */
        TBool IsItemSelected();

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
        
        /** Reacts to screen size change */
        void HandleResourceChange(TInt aType);

    public: // From MLandmarksDbObserver

        /**
        * Notifies that an event has occurred in the default
        * landmark database.
        *
        * @param aEvent contains info about the event.
        * @param aErrorCode possible error codes if reporting the event was
        * not successful.
        */
        void NotifyDbEventL(TPosLmEvent& aEvent, TInt aErrorCode) = 0;

    public: // From MLandmarksOperationObserver

        /**
        * Notifies the progress and status of an
        * asynchronous operation.
        *
        * @param aOperation the type of operation that is reported.
        * @param aProgress the progress of the operation.
        * @param aErrorCode the status of the operation.
        */
        void NotifyOperationProgressL(
            TOperation aOperation,
            TInt aProgress,
            TInt aErrorCode) = 0;

    protected: // Constructors

        /**
        * C++ default constructor.
        *
        * @param aView The parent view.
        * @param aEngine The application engine.
        */
        CLandmarksContainerBase(
            CAknView& aView,
            CLandmarksApplicationEngine& aEngine);

        /**
        * Performs the second phase construction of a
        * CLandmarksContainerBase object. Should be called by subclasses.
        *
        * @param aRect Frame rectangle for container.
        */
        void ConstructL(const TRect& aRect);

    private:

        /**
        * Creates the find box.
        */
        void CreateFindBoxL();

        /**
        * Creates the listbox.
        */
        void CreateListBoxL();

    protected: // Data

        //! the control used to display icons and descriptors.
        CLandmarksListBox* iListBox;

        //! the control used to display a search field.
        CAknSearchField* iFindBox;

        //! parent view
        CAknView& iView;

        //! the application engine used for accessing landmarks
        CLandmarksApplicationEngine& iEngine;

        //! a buffer containing the last descriptor in the find box.
        HBufC* iOldFilter;

    };

#endif // __LANDMARKS_CONTAINER_BASE_H__

// End of File

