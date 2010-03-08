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


#ifndef __AKNEXPOPFIELDMANYLINEDIALOG_H__
#define __AKNEXPOPFIELDMANYLINEDIALOG_H__

// INCLUDES
#include "AknExPopFieldBaseDialog.h"

// CONSTANTS
const TInt KAknExPopFieldMaxMLTextValueField = 4;
const TInt KAknExPopFieldMLIndexInitialize = 0;
const TInt KAknExPopFieldMLCountInitialize = 0;
const TInt KAknExPopFieldMLControlIdInitialize = 0;


// FORWARD DECLARATIONS
class CAknQueryValueTextArray;
class CAknQueryValueText;

// CLASS DECLARATION

/**
* CAknExPopFieldManyLineDialog
*/
class CAknExPopFieldManyLineDialog : public CAknExPopFieldBaseDialog
    {
    public: // Constructors and destructor

        /**
        * NewLC
        * Two-phase constructor.
        */
        static CAknExPopFieldManyLineDialog* NewLC();

        /**
        * CAknExPopFieldManyLineDialog
        * Virtual Destructor.
        */
        virtual ~CAknExPopFieldManyLineDialog();

    private: // Constructors.

        /**
        * CAknExPopFieldManyLineDialog
        * C++ default constructor.
        */
        CAknExPopFieldManyLineDialog();

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
        * @return TBool ave form data?
        */
        TBool SaveFormDataL(); 

        /**
        * From CAknForm, DoNotSaveFormDataL
        * Does not save the contents of the form.
        */
        void DoNotSaveFormDataL();

    private: // Data


        /**
        * iTextArray
        * Owned by CAknExPopFieldManyLineDialog object.
        */
        CDesCArray*                 iTextArray;
        /**
        * iValueTextArray
        * Owned by CAknExPopFieldManyLineDialog object.
        */
        CAknQueryValueTextArray*    iValueTextArray;

        /**
        * iTextValues[]
        * Owned by CAknExPopFieldManyLineDialog object.
        */
        CAknQueryValueText*         iTextValues[KAknExPopFieldMaxMLTextValueField];


        /** Counter for indexvalues **/
        TInt    iIndexValue[KAknExPopFieldMaxMLTextValueField];

    };

#endif  //__AKNEXPOPFIELDMANYLINEDIALOG_H__

// End of File
