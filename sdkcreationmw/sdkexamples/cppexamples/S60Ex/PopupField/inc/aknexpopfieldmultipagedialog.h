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


#ifndef __AKNEXPOPFIELDMULTIPAGEDIALOG_H__
#define __AKNEXPOPFIELDMULTIPAGEDIALOG_H__

// INCLUDES
#include "AknExPopFieldBaseDialog.h"
#include <AknQueryValueNumber.h>
#include <AknQueryValuePhone.h>
#include <AknQueryValueTime.h>
#include <AknQueryValueDate.h>
#include <AknQueryValueDuration.h>

// CONSTANTS
const TInt KAknExPopFieldMaxMultiPageValueField = 6;
const TInt KAknExPopFieldMultiPageIndexInitialize = 0;
const TInt KAknExPopFieldMultiPageCountInitialize = 0;
const TInt KAknExPopFieldMultiPageControlIdInitialize = 0;
const TInt KAknExPopFieldValueInitialize = 0;
const TInt KAknExPopFieldStepInitialize = 0;
const TInt KAknExPopFieldArrayTextBufLength = 256;
const TInt KAknExPopFieldMultiPageInitialArrayElementCount = 4;
const TInt KAknExPopFieldNumberArrayMinimumValue = 10;
const TInt KAknExPopFieldNumberArrayMaximumValue = 100;
const TInt KAknExPopFieldNumberArrayStepValue = 10;
const TInt KAknExPopFieldTimeArrayStepNumber = 24;
const TInt KAknExPopFieldDateArrayStepNumber = 31;
const TInt KAknExPopFieldDuraArrayMinimumValue = 600;
const TInt KAknExPopFieldDuraArrayMaximumValue = 6000;
const TInt KAknExPopFieldDuraArrayStepValue = 600;

#define KAknExPopFieldTimeArrayStepHour TTimeIntervalHours( 1 )
#define KAknExPopFieldDateArrayStepDay TTimeIntervalDays( 1 )


// FORWARD DECLARATIONS
class CAknQueryValueTextArray;
class CAknQueryValueNumberArray;
class CAknQueryValuePhoneArray;
class CAknQueryValueTimeArray;
class CAknQueryValueDateArray;
class CAknQueryValueDurationArray;
class CAknQueryValueText;
class CAknQueryValueNumber;
class CAknQueryValuePhone;
class CAknQueryValueTime;
class CAknQueryValueDate;
class CAknQueryValueDuration;



// CLASS DECLARATION

/**
* CAknExPopFieldMultiPageDialog
*/
class CAknExPopFieldMultiPageDialog : public CAknExPopFieldBaseDialog
    {
    public: // Constructors and destructor

        /**
        * NewLC
        * Two-phase constructor.
        */
        static CAknExPopFieldMultiPageDialog* NewLC();

        /**
        * CAknExPopFieldMultiPageDialog
        * Virtual Destructor.
        */
        virtual ~CAknExPopFieldMultiPageDialog();

    private: // Constructors.

        /**
        * CAknExPopFieldMultiPageDialog
        * C++ default constructor.
        */
        CAknExPopFieldMultiPageDialog();

    private: // New functions

        /**
        * DiscardArray
        * Discard array in popup field object.
        */
        void DiscardArray();

    private: // From CAknExPopFieldBaseDialog

        /**
        * From CAknExPopFieldBaseDialog, ConfigurePopfieldTextL
        * Configuration values into Popup Fields.
        */
        void ConfigurePopfieldValueL();


    private: // New functions
        /**
        * Configuration values (text) into Popup Fields.
        */
        void ConfigurePageTextL();

        /**
        * Configuration values (number) into Popup Fields.
        */
        void ConfigurePageNumberL();

        /**
        * Configuration values (phone) into Popup Fields.
        */
        void ConfigurePagePhoneL();

        /**
        * Configuration values (time) into Popup Fields.
        */
        void ConfigurePageTimeL();

        /**
        * Configuration values (date) into Popup Fields.
        */
        void ConfigurePageDateL();

        /**
        * Configuration values (duration) into Popup Fields.
        */
        void ConfigurePageDurationL();

        /**
        * From CAknForm 
        * Save the contents of the form.
        * @return TBool Save form data?
        */
        TBool SaveFormDataL(); 

        /**
        * From CAknForm, DoNotSaveFormDataL
        * Does not save the contents of the form.
        */
        void DoNotSaveFormDataL();


    private: // Data

        // For text array

        /**
        * iTextArray
        * Owned by CAknExPopFieldMultiPageDialog object.
        */
        CDesCArray*     iTextArray;
        
        /**
        * iValueTextArray
        * Owned by CAknExPopFieldMultiPageDialog object.
        */
        CAknQueryValueTextArray* iValueTextArray;
        
        /**
        * iTextValues
        * Owned by CAknExPopFieldMultiPageDialog object.
        */
        CAknQueryValueText* iTextValues[KAknExPopFieldMaxMultiPageValueField];


        // For number array
        
        /**
        * iNumberArray
        * Owned by CAknExPopFieldMultiPageDialog object.
        */
        CArrayFixFlat<TInt>* iNumberArray;
        /**
        * iValueNumberArray
        * Owned by CAknExPopFieldMultiPageDialog object.
        */
        CAknQueryValueNumberArray* iValueNumberArray;
        
        /**
        * iNumberValues
        * Owned by CAknExPopFieldMultiPageDialog object.
        */CAknQueryValueNumber* iNumberValues[
                                 KAknExPopFieldMaxMultiPageValueField];


        // For phone array

        /**
        * iPhoneArray
        * Owned by CAknExPopFieldMultiPageDialog object.
        */
        CDesCArray* iPhoneArray;
        
        /**
        * iValuePhoneArray
        * Owned by CAknExPopFieldMultiPageDialog object.
        */
        CAknQueryValuePhoneArray* iValuePhoneArray;
        
        /**
        * iTextArray
        * Owned by CAknExPopFieldMultiPageDialog object.
        */
        CAknQueryValuePhone* iPhoneValues[
                              KAknExPopFieldMaxMultiPageValueField];


        // For time array
        
        /**
        * iTimeArray
        * Owned by CAknExPopFieldMultiPageDialog object.
        */
        CArrayFixFlat<TTime>* iTimeArray;
        
        /**
        * iValueTimeArray
        * Owned by CAknExPopFieldMultiPageDialog object.
        */
        CAknQueryValueTimeArray* iValueTimeArray;
        
        /**
        * iTimeValues
        * Owned by CAknExPopFieldMultiPageDialog object.
        */
        CAknQueryValueTime* iTimeValues[
                             KAknExPopFieldMaxMultiPageValueField];


        // For date array
        
        /**
        * iDateArray
        * Owned by CAknExPopFieldMultiPageDialog object.
        */
        CArrayFixFlat<TTime>* iDateArray;
        
       /**
        * iValueDateArray
        * Owned by CAknExPopFieldMultiPageDialog object.
        */ 
        CAknQueryValueDateArray* iValueDateArray;
        
        /**
        * iDateValues
        * Owned by CAknExPopFieldMultiPageDialog object.
        */
        CAknQueryValueDate* iDateValues[
                             KAknExPopFieldMaxMultiPageValueField];

        // For duration array
        
        /**
        * iDurationArray
        * Owned by CAknExPopFieldMultiPageDialog object.
        */
        CArrayFixFlat<TTimeIntervalSeconds>* iDurationArray;
        
        /**
        * iValueDurationArray
        * Owned by CAknExPopFieldMultiPageDialog object.
        */
        CAknQueryValueDurationArray* iValueDurationArray;
        
        /**
        * iDurationValues
        * Owned by CAknExPopFieldMultiPageDialog object.
        */
        CAknQueryValueDuration* iDurationValues[
                                 KAknExPopFieldMaxMultiPageValueField];
        
        
        // Integers for field values.
        TInt iIndexValueText[KAknExPopFieldMaxMultiPageValueField];
        TInt iIndexValueNumber[KAknExPopFieldMaxMultiPageValueField];
        TInt iIndexValuePhone[KAknExPopFieldMaxMultiPageValueField];
        TInt iIndexValueTime[KAknExPopFieldMaxMultiPageValueField];
        TInt iIndexValueDate[KAknExPopFieldMaxMultiPageValueField];
        TInt iIndexValueDuration[KAknExPopFieldMaxMultiPageValueField];
    };

#endif  //__AKNEXPOPFIELDMULTIPAGEDIALOG_H__

// End of File
