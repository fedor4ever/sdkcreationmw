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


#ifndef __LANDMARKS_INFOMODEL_H__
#define __LANDMARKS_INFOMODEL_H__

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class TLocality;
class CPosLandmark;
class CLandmarksApplicationEngine;

// CLASS DECLARATION

/**
*  Contains the data model of the landmarks view.
*  
*/
class CLandmarksInfoModel : public CBase
	{
    public: // Constructors and destructor

        /**
        * C++ constructor.
        */
	    CLandmarksInfoModel(CLandmarksApplicationEngine& aEngine);

        /**
        * Destructor.
        */
	    ~CLandmarksInfoModel();

    public: // New functions

        /**
        * Generates descriptor array from landmark's info.
        * Caller takes ownership of returned object
        *
        * @param aLandmark The landmark to get info of.
        * @param aFieldIds On return, landmark info field ids.
        * @return The landmark's info as descriptor array
        */
        CDesCArray* GetLandmarkInfoL(
            CPosLandmark& aLandmark,
            RArray<TInt>& aFieldIds);

    private:

        /**
        * Adds new string describing landmark field to the info array.
        * Adds also field's ID to array of field IDs.
        * @param aCaption The caption of the field
        * @param aText The value of the field
        * @param aArray Target array
        * @param aFieldIds Target field IDs array
        */
        void AddInfoFieldL(
            const TDesC& aCaption, 
            const TDesC& aText,
            CDesCArray& aArray,
            RArray<TInt>& aFieldIds);

        /**
        * Adds location fields of the landmark to arrays.
        *
        * @param aLocation contains the location data to update location fields
        * with.
        * @param aArray Target array
        * @param aFieldIds Target field IDs array
        */
        void AddLocationFieldsL(
            TLocality& aLocation,
            CDesCArray& aArray,
            RArray<TInt>& aFieldIds);

        /**
        * Adds the category fields to arrays.
        * @param aLandmark used to retrieve category information
        * @param aArray Target array
        * @param aFieldIds Target field IDs array
        */
        void AddCategoryFieldsL(
            CPosLandmark& aLandmark, 
            CDesCArray& aArray,
            RArray<TInt>& aFieldIds);

        /**
        * Adds other text field and to arrays.
        * @param aCaptionId Caption string resource ID
        * @param aText The value of the field
        * @param aArray Target array
        * @param aFieldIds Target field IDs array
        */
        void AddTextFieldL(
            TInt aCaptionId, 
            TDesC& aText,
            CDesCArray& aArray,
            RArray<TInt>& aFieldIds);

    private: // Data

        //! The application engine used for accessing landmarks
        CLandmarksApplicationEngine& iEngine;

        //! Defines how real numbers are displayed
        TRealFormat iRealFormat;
	};

#endif // __LANDMARKS_INFOMODEL_H__

// End of File

