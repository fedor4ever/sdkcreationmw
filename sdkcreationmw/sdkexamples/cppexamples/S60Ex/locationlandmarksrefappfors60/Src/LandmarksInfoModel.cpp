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
* Description:  Implements the CLandmarksInfoModel class
*
*/


// INCLUDE FILES
#include <eikenv.h>
#include <gulicon.h>
#include <AknsUtils.h>
#include <AknsItemID.h>

#include <LbsPosition.h>
#include <EPos_CPosLandmark.h>
#include <EPos_CPosLandmarkCategory.h>

#include <LmRefApp.rsg>
#include "LmRefApp.hrh"
#include "LandmarksInfoModel.h"
#include "LandmarksUtils.h"
#include "LandmarksCommonData.h"
#include "LandmarksApplicationEngine.h"

const TInt KFieldCaptions[] = 
    {
    0,                                  // this slot is not used
    R_LMREFAPP_LMNAME_LABEL_SHORT,      // ELandmarkNameField
    R_LMREFAPP_DESCRIPTION_LABEL_SHORT, // ELandmarkDescField
    R_LMREFAPP_CATEGORY_LABEL_SHORT,    // ELandmarkCategoryField    
    R_LMREFAPP_LAT_LABEL_SHORT,         // ELandmarkLatitudeField
    R_LMREFAPP_LON_LABEL_SHORT,         // ELandmarkLongitudeField
    R_LMREFAPP_ALT_LABEL_SHORT,         // ELandmarkAltitudeField
    R_LMREFAPP_HOR_ACC_LABEL_SHORT,     // ELandmarkHorAccField
    R_LMREFAPP_VER_ACC_LABEL_SHORT,     // ELandmarkVerAccField
    R_LMREFAPP_RADIUS_LABEL_SHORT,      // ELandmarkRadiusField
    R_LMREFAPP_STREET_LABEL_SHORT,      // ELandmarkStreetField
    R_LMREFAPP_POSTAL_LABEL_SHORT,      // ELandmarkPostalField
    R_LMREFAPP_CITY_LABEL_SHORT,        // ELandmarkCityField
    R_LMREFAPP_COUNTRY_LABEL_SHORT,     // ELandmarkCountryField
    };

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLandmarksInfoModel::CLandmarksInfoModel
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLandmarksInfoModel::CLandmarksInfoModel(
    CLandmarksApplicationEngine& aEngine)
    : iEngine(aEngine)
	{
    // Configure real format
    const TChar KDecimalChar = '.';
    iRealFormat.iPlaces = KNrOfDecimals;
    iRealFormat.iPoint = KDecimalChar;
    iRealFormat.iTriLen = 0;
    iRealFormat.iWidth = KPosLmMaxTextFieldLength;
    iRealFormat.iType = KRealFormatFixed;
	}

// Destructor
CLandmarksInfoModel::~CLandmarksInfoModel()
	{
	}

// -----------------------------------------------------------------------------
// CLandmarksInfoModel::GetLandmarkInfoL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CDesCArray* CLandmarksInfoModel::GetLandmarkInfoL(
    CPosLandmark& aLandmark,
    RArray<TInt>& aFieldIds)
    {
    TPtrC fieldValue;
    aFieldIds.Reset();

    const TInt KInfoArrayGranularity = 16;
    CDesCArray* infoArray = new (ELeave) CDesCArrayFlat(KInfoArrayGranularity);

    // Format landmark name
    fieldValue.Set(KNullDesC);
    aLandmark.GetLandmarkName(fieldValue);

    AddTextFieldL(ELandmarkNameField, fieldValue, *infoArray, aFieldIds);

    // Format description
    fieldValue.Set(KNullDesC);
    aLandmark.GetLandmarkDescription(fieldValue);

    AddTextFieldL(ELandmarkDescField, fieldValue, *infoArray, aFieldIds);

    // Format categories
    AddCategoryFieldsL(aLandmark, *infoArray, aFieldIds);

    // Format location fields
    TLocality location;
    aLandmark.GetPosition(location);
    AddLocationFieldsL(location, *infoArray, aFieldIds);

    // Format coverage Radius
    TReal32 coverageRadius;
    TBuf<KPosLmMaxTextFieldLength> radiusBuf;
    TInt res = aLandmark.GetCoverageRadius(coverageRadius);
    if (res != KErrNotFound)
        {
        radiusBuf.AppendNum(coverageRadius, iRealFormat);
        }

    AddTextFieldL(ELandmarkRadiusField, radiusBuf, *infoArray, aFieldIds);

    // Format street;
    fieldValue.Set(KNullDesC);
    aLandmark.GetPositionField(EPositionFieldStreet, fieldValue);

    AddTextFieldL(ELandmarkStreetField, fieldValue, *infoArray, aFieldIds);

    // Format postalCode;
    fieldValue.Set(KNullDesC);
    aLandmark.GetPositionField(EPositionFieldPostalCode, fieldValue);

    AddTextFieldL(ELandmarkPostalField, fieldValue, *infoArray, aFieldIds);

    // Format city;
    fieldValue.Set(KNullDesC);
    aLandmark.GetPositionField(EPositionFieldCity, fieldValue);

    AddTextFieldL(ELandmarkCityField, fieldValue, *infoArray, aFieldIds);

    // Format country
    fieldValue.Set(KNullDesC);
    aLandmark.GetPositionField(EPositionFieldCountry, fieldValue);

    AddTextFieldL(ELandmarkCountryField, fieldValue, *infoArray, aFieldIds);
    
    return infoArray;
    }

// -----------------------------------------------------------------------------
// CLandmarksInfoModel::AddTextFieldL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksInfoModel::AddTextFieldL(
    TInt aFieldId, 
    TDesC& aText, 
    CDesCArray& aArray,
    RArray<TInt>& aFieldIds)
    {
    if (aText.Length())
        {
        HBufC* caption = CCoeEnv::Static()->
            AllocReadResourceAsDes16LC(KFieldCaptions[aFieldId]);

        const TInt KNumTabsAndOther = 10;        
        HBufC* item = HBufC::NewLC(
            caption->Length() + aText.Length() + KNumTabsAndOther); 
            
        _LIT(KItemFormat, "%S\t%S\t\t");
        item->Des().Format(KItemFormat, caption, &aText); 
        
        // append new item to the list
        aFieldIds.AppendL(aFieldId);
        aArray.AppendL(*item); 
        CleanupStack::PopAndDestroy(2, caption);
        }
    }

// -----------------------------------------------------------------------------
// CLandmarksInfoModel::AddLocationFieldsL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksInfoModel::AddLocationFieldsL(
    TLocality& aLocation,
    CDesCArray& aArray,
    RArray<TInt>& aFieldIds)
    {
    TBuf<KPosLmMaxTextFieldLength> fieldValue;

    // Format latitude in deg min sec format
    LandmarksUtils::LatLongFloatToDegMinSecDesL(aLocation.Latitude(), fieldValue);
    AddTextFieldL(ELandmarkLatitudeField, fieldValue, aArray, aFieldIds);
    fieldValue.Zero();

    // Format longitude in deg min sec format
    LandmarksUtils::LatLongFloatToDegMinSecDesL(aLocation.Longitude(), fieldValue);
    AddTextFieldL(ELandmarkLongitudeField, fieldValue, aArray, aFieldIds);
    fieldValue.Zero();

    // Format altitude
    LandmarksUtils::FloatToDesL(aLocation.Altitude(), fieldValue, iRealFormat);
    AddTextFieldL(ELandmarkAltitudeField, fieldValue, aArray, aFieldIds);

    // Format horizontal accuracy
    LandmarksUtils::FloatToDesL(aLocation.HorizontalAccuracy(), fieldValue, iRealFormat);
    AddTextFieldL(ELandmarkHorAccField, fieldValue, aArray, aFieldIds);

    // Format vertical accuracy
    LandmarksUtils::FloatToDesL(aLocation.VerticalAccuracy(), fieldValue, iRealFormat);
    AddTextFieldL(ELandmarkVerAccField, fieldValue, aArray, aFieldIds);
    }

// -----------------------------------------------------------------------------
// CLandmarksInfoModel::AddCategoryFieldsL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksInfoModel::AddCategoryFieldsL(
    CPosLandmark& aLandmark,
    CDesCArray& aArray,
    RArray<TInt>& aFieldIds)
    {
    RArray< TPosLmItemId > categories;
    CleanupClosePushL(categories);
    aLandmark.GetCategoriesL(categories);
    TInt nrOfCategories = categories.Count();
    
    for (TInt i = 0; i < nrOfCategories; i++)
        {
        TPtrC categoryName;
        CPosLandmarkCategory* category = iEngine.CategoryLC(categories[i]);
        category->GetCategoryName(categoryName);

        AddTextFieldL(ELandmarkCategoryField, categoryName, aArray, aFieldIds);

        CleanupStack::PopAndDestroy(category);
        }

    CleanupStack::PopAndDestroy(&categories);
    }


// End of File

