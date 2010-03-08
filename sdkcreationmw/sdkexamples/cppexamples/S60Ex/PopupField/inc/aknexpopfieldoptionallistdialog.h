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


#ifndef __AKNEXPOPFIELDOPTIONALLISTDIALOG_H__
#define __AKNEXPOPFIELDOPTIONALLISTDIALOG_H__


// INCLUDES
#include "AknExPopFieldBaseDialog.h"



// CONSTANTS
const TInt KAknExPopFieldMaxOLTextValueField = 13;
const TInt KAknExPopFieldOLIndexInitialize = 0;
const TInt KAknExPopFieldOLCountInitialize = 0;
const TInt KAknExPopFieldOLControlIdInitialize = 0;
const TInt KAknExPopFieldEmptyFieldIndex = 0;
const TInt KAknExPopFieldInitialArrayElementCount = 4;



// FORWARD DECLARATIONS
class CAknQueryValueTextArray;
class CAknQueryValueText;



// CLASS DECLARATION

/**
* CAknExPopFieldOptionalListDialog
*/
class CAknExPopFieldOptionalListDialog : public CAknExPopFieldBaseDialog
    {
   
    public: // Constructors and destructor

        /**
        * NewLC
        * Two-phase constructor.
        */
        static CAknExPopFieldOptionalListDialog* NewLC();

        /**
        * CAknExPopFieldOptionalListDialog
        * Virtual Destructor.
        */
        virtual ~CAknExPopFieldOptionalListDialog();

    private: // Constructors.

        /**
        * CAknExPopFieldOptionalListDialog
        * C++ default constructor.
        */
        CAknExPopFieldOptionalListDialog();


    private: // New functions

        /**
        * DiscardTextArray
        * Discard text array in popup field object.
        */
        void DiscardTextArray();

    
    private: // From CAknExPopFieldBaseDialog

        /**
        * From CAknExPopFieldBaseDialog, ConfigurePopfieldTextL
        * Configuration values (text) into Popup Fields.
        */
        void ConfigurePopfieldValueL();

        /**
        * From CAknForm, SaveFormDataL
        * Save the contents of the form.
        * @return TBool Save form data?
        */
        TBool SaveFormDataL(); 

        /**
        * From CAknForm, DoNotSaveFormDataL
        * Does not save the contents of the form.
        */
        void DoNotSaveFormDataL();


     private: // Enumerations for Text Array

        enum TAknExPopFieldTextArrayIndex
            {
            EAknExPopFieldTextArrayIndex0,
            EAknExPopFieldTextArrayIndex1,
            EAknExPopFieldTextArrayIndex2,
            EAknExPopFieldTextArrayIndex3,
            EAknExPopFieldTextArrayIndex4,
            EAknExPopFieldTextArrayIndex5,
            EAknExPopFieldTextArrayIndex6,
            EAknExPopFieldTextArrayIndex7,
            EAknExPopFieldTextArrayIndex8,
            EAknExPopFieldTextArrayIndex9,
            EAknExPopFieldTextArrayIndex10,
            EAknExPopFieldTextArrayIndex11,
            EAknExPopFieldTextArrayIndex12
            };


    private: // Data


        /**
        * iTextArray
        * Owned by CAknExPopFieldOptionalListDialog object.
        */
        CDesCArray* iTextArray[KAknExPopFieldMaxOLTextValueField];

        /**
        * iValueTextArray
        * Owned by CAknExPopFieldOptionalListDialog object.
        */
        CAknQueryValueTextArray* iValueTextArray[KAknExPopFieldMaxOLTextValueField];

        /**
        * iTextValues
        * Owned by CAknExPopFieldOptionalListDialog object.
        */
        CAknQueryValueText*     iTextValues[KAknExPopFieldMaxOLTextValueField];

        // Integers for field values.
        TInt iIndexValue[KAknExPopFieldMaxOLTextValueField];

    };

#endif  //__AKNEXPOPFIELDOPTIONALLISTDIALOG_H__

// End of File
