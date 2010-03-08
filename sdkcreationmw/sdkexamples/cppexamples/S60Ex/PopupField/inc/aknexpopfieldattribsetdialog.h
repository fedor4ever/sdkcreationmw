/*
* Copyright (c) 2004 - 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/


#ifndef __AKNEXPOPFIELDATTRIBSETDIALOG_H__
#define __AKNEXPOPFIELDATTRIBSETDIALOG_H__

// INCLUDES
#include <eikmenub.h>
#include "AknExPopFieldBaseDialog.h"


// CONSTANTS
const TInt KAknExPopFieldAttribSetControlCount = 2;
const TInt KAknExPopFieldTextMaxValues = 2;
const TInt KAknExPopFieldAttribSetIndexInitialize = 0;
const TInt KAknExPopFieldMaxNumberLines  = 7;
const TInt KAknExPopFieldReplacementTextBufLength = 256;
const TInt KAknExPopFieldGranularityForEmptyArray = 16;


// FORWARD DECLARATIONS
class CAknQueryValueTextArray;
class CAknQueryValueText;

// CLASS DECLARATION

/**
* CAknExPopFieldAttribSetDialog
*/
class CAknExPopFieldAttribSetDialog : public CAknExPopFieldBaseDialog
    {
    public: // Constructors and destructor

        /**
        * ConstructL.
        * 2nd phase constructor.
        */
        static CAknExPopFieldAttribSetDialog* NewLC();

        /**
        * CAknExPopFieldAttribSetDialog
        * Virtual Destructor.
        */
        virtual ~CAknExPopFieldAttribSetDialog();

    private: // Constructor.

        /**
        * CAknExPopFieldAttribSetDialog
        * C++ default constructor.
        */
        CAknExPopFieldAttribSetDialog();

    private: // New functions

        /**
        * DiscardTextArray
        * Discard text array in popup field object.
        */
        void DiscardTextArray();

        /**
        * SetIsSaveMenu
        * Set flag to indicate if we are in the save menu.
        * @param aVal
        */
        void SetIsSaveMenu( TBool aVal );

        /**
        * IsSaveMenu
        * Check if we are in the save menu.
        * @return TBool is menu saved or not?
        */
        TBool IsSaveMenu();

    public: // From CAknDialog

        /**
        * From CAknExPopfieldBaseDialog, DynInitMenuPaneL.
        * Dims "Label", "Add" and "Delete" menu items.
        * @param aResourceId 
        * @param aMenuPane Pointer to menu-pane object.
        */
        void DynInitMenuPaneL( TInt aResourceId, 
                               CEikMenuPane* aMenuPane );

        /**
        * From CAknDialog, ProcessCommandL.
        * @param aCommandId
        */
        void ProcessCommandL( TInt aCommandId );

    protected: // From CAknDialog
    
        /**
        * From CAknDialog, OkToExitL.
        * @param aButtonId 
        * @return TBool is it ok to exit?
        */
        TBool OkToExitL( TInt aButtonId );


    private: // From CAknExPopFieldBaseDialog

        /**
        * From CAknExPopFieldBaseDialog, ConfigurePopfieldTextL
        * Configuration values (text) into Popup Fields.
        */
        void ConfigurePopfieldValueL();

        /**
        * From CAknForm 
        * Save the contents of the form.
        * @return TBool Save Form Data?
        */
        TBool SaveFormDataL();

        /**
        * From CAknForm DoNotSaveFormDataL
        * Does not save the contents of the form.
        */
        void DoNotSaveFormDataL();


    private: // Data


         /**
        * iTextEmptyArray
        * Owned by CAknExPopFieldAttribSetDialog object.
        */
        CDesCArray* iTextEmptyArray[KAknExPopFieldAttribSetControlCount]; 

        /**
        * iTextEmptyArray
        * Owned by CAknExPopFieldAttribSetDialog object.
        */
        CAknQueryValueTextArray* iValueTextArrayWithNoElements
                         [KAknExPopFieldAttribSetControlCount];

        /**
        * iTextValues
        * Owned by CAknExPopFieldAttribSetDialog object.
        */
        CAknQueryValueText* iTextValues[KAknExPopFieldAttribSetControlCount];


        /** Flag for SaveMenu? **/
        TBool iIsSaveMenuFlag;

        /** Counter for indexvalue **/
        TInt iIndexValue[KAknExPopFieldAttribSetControlCount];

    };

#endif  //__AKNEXPOPFIELDATTRIBSETDIALOG_H__

// End of File
