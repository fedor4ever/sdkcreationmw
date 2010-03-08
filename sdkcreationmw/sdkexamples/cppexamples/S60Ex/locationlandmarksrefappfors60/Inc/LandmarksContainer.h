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


#ifndef __LANDMARKS_CONTAINER_H__
#define __LANDMARKS_CONTAINER_H__

// INCLUDES
#include "LandmarksContainerBase.h"
#include <coecobs.h>

// FORWARD DECLARATIONS
class CLandmarksApplicationEngine;
class CAknNavigationDecorator;
class CLandmarksModel;

// CLASS DECLARATION

/**
*  This is the landmarks container that contains a listbox and a search field
*  for listing landmarks.
*/
class CLandmarksContainer : 
    public CLandmarksContainerBase, 
    public MCoeControlObserver
    {
    public: // Constructors and destructor

        /**
        * C++ constructor.
        *
        * @param aView parent view
        * @param aEngine the landmark engine
        * @param aNaviDecorator the tab group to hide when editing landmarks
        */
        CLandmarksContainer(
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
        ~CLandmarksContainer();

    public: // New functions

        /**
        * Enables/disables the navi pane.
        *
        * @param aVisible ETrue enables navipane, EFalse disables navipane
        */
        void MakeNaviPaneVisibleL(TBool aVisible);

        /**
        * Gets currently highlighted landmarks
        * @return The landmark item id.
        */
        TPosLmItemId CurrentLandmark();

        /**
        * Displays a confirmation dialog and deletes the current landmark if 
        * positive response.
        */ 
        void DeleteLandmarkL();

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
            const TKeyEvent& aKeyEvent, 
            TEventCode aType);

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
        * @param aFinished ETrue if there are no more landmarks to read, 
        * EFalse otherwise
        */
        void HandleReadReadyL(TInt aErrorCode, TBool aFinished);

        /**
        * Indicates if it is necessary to refresh the listbox 
        * when a landmark has been updated.
        *
        * @param aModifiedLandmark the id of the landmark that has been updated
        * @return ETrue if refresh is necessary, EFalse otherwise
        */
        TBool IsRefreshNeededL(TPosLmItemId& aModifiedLandmark);

        /**
        * Stores the name of the current landmark.
        */
        void StoreCurrentLandmarkNameL();

    private: // Data

        //! Decorates the navi pane (with navigation arrows etc)
        CAknNavigationDecorator* iNaviDecorator;

        //! The data model for the listbox
        CLandmarksModel* iModel;

        //! Indicates if the first page of landmarks has been read.
        TBool iReadingFirstPage;

        //! Keeps the name of the current item
        HBufC* iCurrentName;

        //! Indicates if the landmark list is being refreshed
        TBool iIsActive;
    };

#endif // __LANDMARKS_CONTAINER_H__

