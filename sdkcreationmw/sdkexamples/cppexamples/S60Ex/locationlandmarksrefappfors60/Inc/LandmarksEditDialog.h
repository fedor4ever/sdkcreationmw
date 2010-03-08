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


#ifndef __LANDMARKS_EDIT_DIALOG_H__
#define __LANDMARKS_EDIT_DIALOG_H__

// INCLUDES
#include <aknform.h>
#include <lbspositioninfo.h>
#include <EPos_Landmarks.h>
#include "LandmarksOperationObserver.h"

// DATA TYPES

// FORWARD DECLARATIONS
class CLandmarksApplicationEngine;
class CLandmarksLmCategoriesModel;
class CLandmarksPositionRequest;
class CPosLandmarkCategory;
class CAknTitlePane;
class CPosLandmark;
class TLocality;

// CLASS DECLARATION

/**
*  A dialog that makes it possible to edit certain fields of a landmark.
*/
class CLandmarksEditDialog : public CAknForm, public MLandmarksOperationObserver
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        *
        * @param aLandmark the landmark to edit
        * @param aUseCurrentLocation whether to initiate landmark with
        * current location
        * @param aEdited ETrue if the landmark has been modified,
        * EFalse otherwise
        * @param aEngine the application engine used for accessing categories
        * @param aInitialFocusFieldId Item to select on startup
        */
        static CLandmarksEditDialog* NewLC(
            CPosLandmark& aLandmark,
            TBool aUseCurrentLocation,
            TBool& aEdited,
            CLandmarksApplicationEngine& aEngine,
            TInt aInitialFocusFieldId = 0);

        /**
        * Destructor.
        */
        ~CLandmarksEditDialog();

    public: // From CAknDialog

        /**
        * DynInitMenuPaneL initializes the menu pane before it is displayed.
        *
        * @param aResourceId a resource ID indetifying the menu bar containing
        * the menu pane to be initialized.
        * @param aMenuPane the menu pane to initialize.
        */
        void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);

        /**
        * Processes user commands.
        *
        * @param aCommandId id of the command to respond to
        */
        void ProcessCommandL(TInt aCommandId);

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

    protected: // From CAknForm

        /**
        * Handles a dialog button press for the specified dialog button.
        *
        * @param aButtonId
        * @return ETrue if OK to exit, EFalse to keep the dialog active
        */
        TBool OkToExitL(TInt aButtonId);


        /**
        * SaveFormDataL deals with the occasion of a change from
        * edit->view mode and the user wishes to save the changes.
        *
        * @return ETrue if the editable state can be left, EFalse otherwise
        */
        TBool SaveFormDataL();

    private: // From CEikDialog

        /**
        * PreLayoutDynInitL initialises the dialog's controls before the dialog
        * is sized and layed out.
        */
        void PreLayoutDynInitL();

        /**
        * PostLayoutDynInitL initialises the dialog's controls after the dialog
        * has been sized but before it has been activated.
        */
        void PostLayoutDynInitL();

        /**
        * Selects desired line on startup
        */
        void SetInitialCurrentLine();

        /**
        * Handles control events.
        *
        * @param aControl The control reporting the event
        * @param aEventType The event type
        */
        void HandleControlEventL(
            CCoeControl* aControl,
            TCoeEvent aEventType);

    private:

        /**
        * Constructor.
        *
        * @param aLandmark the landmark to edit
        * @param aUseCurrentLocation whether to initiate landmark with
        * current location
        * @param aEdited ETrue if the landmark has been modified, EFalse otherwise
        * @param aEngine the application engine used for accessing categories
        * @param aInitialFocusFieldId Item to select on startup
        */
        CLandmarksEditDialog(
            CPosLandmark& aLandmark,
            TBool aUseCurrentLocation,
            TBool& aEdited,
            CLandmarksApplicationEngine& aEngine,
            TInt aInitialFocusFieldId);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Configures the fields of the landmark to view/edit.
        */
        void ConfigureFieldsL();

        /**
        * Configures the location fields of the landmark to view/edit.
        *
        * @param aLocation contains the location data to update location fields
        * with.
        */
        void ConfigureLocationFieldsL(TLocality& aLocation);

        /**
        * Configures the category field.
        */
        void ConfigureCategoryFieldL();

        /**
        * Sets text to the specified field
        *
        * @param aControlId ID of control in the dialog
        * @param aText Text to set to the control
        */
        void ConfigureTextFieldL(TInt aControlId, const TDesC& aText);

        /**
        * Updates the title bar based on the contents in the name field.
        * @param aFromField Whether to get text from name field or from
        * landmark.
        */
        void UpdateTitleBarL(TBool aFromField = EFalse);

        /**
        * Retrieves a float from am EDWIN editor.
        *
        * @param aFloat float that is retrieved from editor.
        * @param aEditorResourceId the resource id of the editor to read
        * @param aErrorResourceId a resource id for a descriptor containing an
        * error message that is displayed in case fetching failed
        */
        void FetchFloatL(
            TRealX& aFloat,
            TInt aEditorResourceId,
            TInt aErrorResourceId);

        /**
        * Launches an error dialog displaying an error message to the user.
        *
        * @param aResourceId a resource id for a descriptor containing an
        * error message
        */
        void NotifyErrorToUserL(TInt aResourceId);

        /**
        * Launches a markable list dialog for editing categories for this
        * landmark.
        */
        void EditCategoriesL();
        
        /**
        * Start location request to update landmark
        * with current location
        */
        void UpdateToCurrentLocationL();

    private: // Data

        //! The landmark to be viewed/edited
        CPosLandmark* iLandmark;
        
        //! Whether to get location on startup
        TBool iUseCurrentLocation;

        //! Indicates if the landmark has been edited or not
        TBool& iEdited;

        //! Engine is used for reading categories
        CLandmarksApplicationEngine& iEngine;

        //! Item selected on startup
        TInt iInitialFocusFieldId;

        //! The title pane that is dynamically updated
        CAknTitlePane* iTitlePane;

        //! The original title that the title pane displayed before modified
        HBufC* iOriginalTitle;

        //! Contains a location if the user wants to update to current location
        TPositionInfo iPositionInfo;

        //! The name of the application. Used when fetching locations
        HBufC* iAppName;

        //! A position request used for retriveing locations
        CLandmarksPositionRequest* iPositionRequest;

        //! The listbox model for categories editor
        CLandmarksLmCategoriesModel* iLmCategoriesModel;

        //! The category ids that are set for this landmark
        RArray<TPosLmItemId> iCategoryIds;

        //! Defines how real numbers are formatted
        TRealFormat iRealFormat;

        //! Defines which real values that have been edited
        RArray<TBool> iIsRealValueEdited;
    };

#endif // __LANDMARKS_EDIT_DIALOG__

// End of File

