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


#ifndef __AKNEXPOPFIELDTEXTVALUEDIALOG_H__
#define __AKNEXPOPFIELDTEXTVALUEDIALOG_H__

// INCLUDES
#include "AknExPopFieldBaseDialog.h"

// CONSTANTS
const TInt KAknExPopFieldMaxTextValueField = 12;
const TInt KAknExPopupFieldTextMaxValues = 2;
const TInt KAknExPopFieldIndexInitialize = 0;
const TInt KAknExPopFieldCountInitialize = 0;
const TInt KAknExPopFieldControlIdInitialize = 0;



// FORWARD DECLARATIONS
class CAknQueryValueTextArray;
class CAknQueryValueText;

// CLASS DECLARATION

/**
* CAknExPopFieldTextValueDialog
*/
class CAknExPopFieldTextValueDialog : public CAknExPopFieldBaseDialog
    {
    public: // Constructors and destructor

        /**
        * NewLC
        * Two-phase constructor.
        */
        static CAknExPopFieldTextValueDialog* NewLC();

        /**
        * CAknExPopFieldTextValueDialog
        * Virtual Destructor.
        */
        virtual ~CAknExPopFieldTextValueDialog();

    private: // Constructors.

        /**
        * CAknExPopFieldTextValueDialog
        * C++ default constructor.
        */
        CAknExPopFieldTextValueDialog();

    private: // New functions

        /**
        * DiscardTextArray
        * Discard text array in popup field object.
        */
        void DiscardTextArray();


    protected: // From CAknExPopFieldBaseDialog
        
        /**
        * From CAknExPopFieldBaseDialog, OkToExitL.
        * @param aButtonId 
        * @return TBool ok to exit?
        */
        TBool OkToExitL( TInt aButtonId );


    private: // From CAknExPopFieldBaseDialog

        /**
        * From CAknExPopFieldBaseDialog, ConfigurePopfieldTextL
        * Configuration values (text) into Popup Fields.
        */
        void ConfigurePopfieldValueL();

        /**
        * From CAknForm, SaveFormDataL
        * Save the contents of the form.
        * @return TBool save form data?
        */
        TBool SaveFormDataL(); 


        /**
        * From CAknForm. DoNotSaveFormDataL
        * Does not save the contents of the form.
        */
        void DoNotSaveFormDataL();


    private: // Data

        /**
        * iTextArray
        * Owned by CAknExPopFieldTextValueDialog object.
        */
        CDesCArray* iTextArray;

        /**
        * iValueTextArray
        * Owned by CAknExPopFieldTextValueDialog object.
        */
        CAknQueryValueTextArray* iValueTextArray;

        /**
        * iTextValues
        * Owned by CAknExPopFieldTextValueDialog object.
        */
        CAknQueryValueText* iTextValues[KAknExPopFieldMaxTextValueField];

        // Integers for field values.
        TInt iIndexValue[KAknExPopFieldMaxTextValueField];

    };

#endif  //__AKNEXPOPFIELDTEXTVALUEDIALOG_H__

// End of File
