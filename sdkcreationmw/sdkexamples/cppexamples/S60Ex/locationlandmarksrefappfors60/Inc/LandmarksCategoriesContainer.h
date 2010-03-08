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


#ifndef __LANDMARKS_CATEGORIES_CONTAINER_H__
#define __LANDMARKS_CATEGORIES_CONTAINER_H__

// INCLUDES
#include "LandmarksContainerBase.h"
#include <coecobs.h>

// FORWARD DECLARATIONS
class CLandmarksCategoriesModel;
class CLandmarksDbEventHandler;
class CLandmarksApplicationEngine;
class CPosLandmarkCategory;
class CEikMenuBar;

// CLASS DECLARATION

/**
*  This is the categories container that contains a listbox and a search field
*  for listing categories.
*/
class CLandmarksCategoriesContainer : public CLandmarksContainerBase, 
public MCoeControlObserver
    {
    public: // Constructors and destructor

        /**
        * C++ constructor.
        *
        * @param aView the parent view
        * @param aEngine the landmark engine
        * @param aMenuBar the categories menubar
        */
        CLandmarksCategoriesContainer(
            CAknView& aView,
            CLandmarksApplicationEngine& aEngine, 
            CEikMenuBar& aMenuBar);

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
        ~CLandmarksCategoriesContainer();

    public: // New functions

        /**
        * Displays a onfirmation dialog and deletes the current category if 
        * positive response.
        */ 
        void DeleteCategoryL();

        /**
        * Returns the current category id.
        *
        * @return the current category id
        */
        TPosLmItemId CurrentCategoryId();

        /**
        * Indicates whether a category is global or not.
        *
        * @return ETrue if global, EFalse otherwise.
        */
        TBool IsGlobalL();

    public: // from CoeControl

        /**
        * OfferKeyEventL handles key events.
        *
        * @param aKeyEvent the key event
        * @param aType the type of key event
        * @return EKeyWasConsumed if keyevent was handled, EKeyWasNotConsumed 
        * otherwise 
        */
        TKeyResponse OfferKeyEventL(
            const TKeyEvent& aKeyEvent, TEventCode aType);

    public: // From MLandmarksOperationObserver

        /**
        * NotifyOperationProgressL notifies the progress and status of an
        * asynchronous operation.
        *  
        * @param aOperation the type of operation that is reported.
        * @param aProgress the progress of the operation.
        * @param aErrorCode the status of the operation.
        */
        void NotifyOperationProgressL(
            TOperation aOperation, 
            TInt aProgress, 
            TInt aErrorCode);

    public: // From MLandmarksDbObserver

        /**
        * NotifyDbEventL notifies that an event has occurred in the default 
        * landmark database.
        *  
        * @param aEvent contains info about the event.
        * @param aErrorCode possible error codes if reporting the event was 
        * not successful.
        */
        void NotifyDbEventL(TPosLmEvent& aEvent, TInt aErrorCode);

    public: // From MCoeControlObserver

        /**
        * HandleControlEventL handles an event from an observed control.
        *
        * @param aControl the control that reported the event
        * @param aEventType contains info about the event
        */
        void HandleControlEventL(CCoeControl* aControl, TCoeEvent aEventType);

    private:

        /**
        * StartUpdateListBoxL initiates an asynchronous refresh of the listbox.
        */
        void StartUpdateListBoxL();

        /**
        * HandleSearchReadyL updates the state of this class when a search 
        * operation has completed.
        *
        * @param aErrorCode the error code of the search operation
        */
        void HandleSearchReadyL(TInt aErrorCode);

        /**
        * HandleReadReadyL updates the state of this class when a read
        * operation has completed.
        *
        * @param aErrorCode the error code of the search operation
        */
        void HandleReadReadyL(TInt aErrorCode);

    private: // Data

        //! The menubar displayed for this vew
        CEikMenuBar& iMenuBar;

        //! Contains the data model for the listbox
        CLandmarksCategoriesModel* iModel;

        //! Indicates if the first page of landmarks has been read.
        TBool iReadingFirstPage;
    };


#endif // __LANDMARKS_CATEGORIES_CONTAINER_H__

// End of File

