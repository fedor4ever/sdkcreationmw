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
* Description:  Implements the CLandmarksEditDialog class
*
*/


// INCLUDE FILES
#include <eikmenub.h>
#include <akntitle.h>
#include <aknnotewrappers.h>
#include <aknappui.h>
#include <e32math.h>
#include <eikcapc.h>
#include <aknselectionlist.h>

#include <LbsPosition.h>
#include <EPos_CPosLandmark.h>
#include <EPos_CPosLandmarkCategory.h>

#include "LmRefApp.hrh"
#include <LmRefApp.rsg>

#include "LandmarksUtils.h"
#include "LandmarksCommonData.h"
#include "LandmarksEditDialog.h"
#include "LandmarksCategoriesDialog.h"
#include "LandmarksPositionRequest.h"
#include "LandmarksApplicationEngine.h"
#include "LandmarksLmCategoriesModel.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLandmarksEditDialog::CLandmarksEditDialog
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLandmarksEditDialog::CLandmarksEditDialog(
    CPosLandmark& aLandmark, 
    TBool aUseCurrentLocation,
    TBool& aEdited, 
    CLandmarksApplicationEngine& aEngine,
    TInt aInitialFocusFieldId)
:   iLandmark(&aLandmark), 
    iUseCurrentLocation(aUseCurrentLocation),
    iEdited(aEdited),
    iEngine(aEngine),
    iInitialFocusFieldId(aInitialFocusFieldId)
    {
    iEdited = EFalse;

    // Configure real format
    const TChar KDecimalChar = '.';
    iRealFormat.iPlaces = KNrOfDecimals;
    iRealFormat.iPoint = KDecimalChar;
    iRealFormat.iTriLen = 0;
    iRealFormat.iWidth = KPosLmMaxTextFieldLength;
    iRealFormat.iType = KRealFormatFixed;
    }

// -----------------------------------------------------------------------------
// CLandmarksEditDialog::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLandmarksEditDialog::ConstructL()
    {
    CAknForm::ConstructL(R_LMREFAPP_EDIT_MENU_BAR);

    // Save original title
    CEikStatusPane* statusPane = iAvkonAppUi->StatusPane();
    iTitlePane = (CAknTitlePane*) statusPane->ControlL(
        TUid::Uid(EEikStatusPaneUidTitle));
    iOriginalTitle = iTitlePane->Text()->AllocL();

    // Create iPositionRequest
    HBufC* appName = iCoeEnv->AllocReadResourceAsDes16LC(R_LMREFAPP_APP_TITLE);
    iPositionRequest = CLandmarksPositionRequest::NewL(*appName, this);
    CleanupStack::PopAndDestroy(appName);

    // Initialize iCategoryIds
    iLandmark->GetCategoriesL(iCategoryIds);
    
    iLmCategoriesModel = 
            CLandmarksLmCategoriesModel::NewL(iEngine, iCategoryIds);

    // Initialise states of real value editors
    for (TInt i = ELatitudeEditor; i <= ERadiusEditor; i++)
        {
        User::LeaveIfError(iIsRealValueEdited.Append(EFalse));
        }
    }

// -----------------------------------------------------------------------------
// CLandmarksEditDialog::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CLandmarksEditDialog* CLandmarksEditDialog::NewLC(
    CPosLandmark& aLandmark,
    TBool aUseCurrentLocation,
    TBool& aEdited,
    CLandmarksApplicationEngine& aEngine,
    TInt aInitialFocusFieldId)
    {
    CLandmarksEditDialog* self = new (ELeave) CLandmarksEditDialog(
        aLandmark, aUseCurrentLocation, aEdited, aEngine, aInitialFocusFieldId);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// Destructor
CLandmarksEditDialog::~CLandmarksEditDialog()
    {
    delete iAppName;
    delete iPositionRequest;
    iCategoryIds.Close();
    iIsRealValueEdited.Close();
    delete iLmCategoriesModel;

    // Restore title pane. Ownership of iTitlePane transferred.
    if (iTitlePane && iOriginalTitle)
        {
        iTitlePane->SetText(iOriginalTitle);
        }
    }

// -----------------------------------------------------------------------------
// CLandmarksEditDialog::DynInitMenuPaneL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksEditDialog::DynInitMenuPaneL(
    TInt aResourceId,
    CEikMenuPane* aMenuPane)
    {
    CAknForm::DynInitMenuPaneL(aResourceId, aMenuPane);
    if (aResourceId == R_AVKON_FORM_MENUPANE)
        {
        // We do not want to allow the user to modify our Form therefore 
        // Disable the Label, Add, Delete, and Save Field 
        aMenuPane->SetItemDimmed(EAknFormCmdLabel, ETrue); 
        aMenuPane->SetItemDimmed(EAknFormCmdAdd, ETrue); 
        aMenuPane->SetItemDimmed(EAknFormCmdDelete, ETrue); 
        aMenuPane->SetItemDimmed(EAknFormCmdSave, ETrue);

        aMenuPane->SetItemDimmed(ELandmarksUpdateToCurrentLocation, 
            Control(ELandmarkCategoryField)->IsFocused());
        aMenuPane->SetItemDimmed(ELandmarksEditCategories, 
            !Control(ELandmarkCategoryField)->IsFocused());
        }
    }

// -----------------------------------------------------------------------------
// CLandmarksEditDialog::ProcessCommandL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksEditDialog::ProcessCommandL(TInt aCommandId)
	{
    switch (aCommandId)
        {
        case ELandmarksUpdateToCurrentLocation:
            UpdateToCurrentLocationL();
            break;

        case ELandmarksEditCategories:
            EditCategoriesL();
            break;

        default:
            break;
        }

	CAknForm::ProcessCommandL(aCommandId);
	}

// -----------------------------------------------------------------------------
// CLandmarksEditDialog::NotifyOperationProgressL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksEditDialog::NotifyOperationProgressL(
    TOperation aOperation, 
    TInt /*aProgress*/, 
    TInt aErrorCode)
    {
    if (aErrorCode == KErrNone && 
        aOperation == EAcquiringLocation)
        {
        // Update location fields with new location
        TPosition position;
        iPositionInfo.GetPosition(position);
        ConfigureLocationFieldsL(position);
        SetChangesPending(ETrue);

        // Indicate that editors have been updated
        for (TInt i = ELatitudeEditor; i <= EVerticalAccEditor; i++)
            {
            iIsRealValueEdited[i] = ETrue;
            }
        }
    }

// -----------------------------------------------------------------------------
// CLandmarksEditDialog::OkToExitL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CLandmarksEditDialog::OkToExitL(TInt aButtonId)
	{
    if (aButtonId == EAknSoftkeyDone)
        {
        // exit from CAknForm's "edit" mode
        return CAknForm::OkToExitL(EAknSoftkeyBack);
        }
    else
        {
    	return CAknForm::OkToExitL(aButtonId);
        }
	}

// -----------------------------------------------------------------------------
// CLandmarksEditDialog::PreLayoutDynInitL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksEditDialog::PreLayoutDynInitL()
    {
    SetEditableL(ETrue);
    ConfigureFieldsL();
    ConfigureCategoryFieldL();
    }

// -----------------------------------------------------------------------------
// CLandmarksEditDialog::PostLayoutDynInitL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksEditDialog::PostLayoutDynInitL()
    {
    SetChangesPending(ETrue);

    UpdateTitleBarL();
    CAknForm::PostLayoutDynInitL();
    
    if (iUseCurrentLocation)
        {
        UpdateToCurrentLocationL();
        }
    }

// -----------------------------------------------------------------------------
// CLandmarksEditDialog::SetInitialCurrentLine
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksEditDialog::SetInitialCurrentLine()
    {
    CAknForm::SetInitialCurrentLine();
    
    if (iInitialFocusFieldId >= ELandmarkNameField &&
        iInitialFocusFieldId <= ELandmarkCountryField)
        {
        TInt err = KErrNone;
        TRAP(err, TryChangeFocusToL(iInitialFocusFieldId));
        }
    }

// -----------------------------------------------------------------------------
// CLandmarksEditDialog::HandleControlEventL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksEditDialog::HandleControlEventL(
    CCoeControl* aControl, 
    TCoeEvent aEventType)
	{
    CAknForm::HandleControlEventL(aControl, aEventType);

    if (aEventType == EEventStateChanged)
        {
        if (aControl == ControlOrNull(ELandmarkLatitudeField))
            {
            iIsRealValueEdited[ELatitudeEditor] = ETrue;
            }
        else if (aControl == ControlOrNull(ELandmarkLongitudeField))
            {
            iIsRealValueEdited[ELongitudeEditor] = ETrue;
            }
        else if (aControl == ControlOrNull(ELandmarkAltitudeField))
            {
            iIsRealValueEdited[EAltitudeEditor] = ETrue;
            }
        else if (aControl == ControlOrNull(ELandmarkHorAccField))
            {
            iIsRealValueEdited[EHorizontalAccEditor] = ETrue;
            }
        else if (aControl == ControlOrNull(ELandmarkVerAccField))
            {
            iIsRealValueEdited[EVerticalAccEditor] = ETrue;
            }
        else if (aControl == ControlOrNull(ELandmarkRadiusField))
            {
            iIsRealValueEdited[ERadiusEditor] = ETrue;
            }
        else if (aControl == ControlOrNull(ELandmarkNameField))
            {
            UpdateTitleBarL(ETrue); // get it from name field
            }
        }
	}

// -----------------------------------------------------------------------------
// CLandmarksEditDialog::SaveFormDataL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CLandmarksEditDialog::SaveFormDataL()
    {
    CEikEdwin* editor = NULL;

    // Fetch name
    TBuf<KPosLmMaxTextFieldLength> name;
    editor = static_cast <CEikEdwin*> (Control(ELandmarkNameField));
    editor->GetText(name);

    // Fetch description
    HBufC* descBuf = HBufC::NewLC(KPosLmMaxDescriptionLength);
    TPtr desc = descBuf->Des();
    editor = static_cast <CEikEdwin*> (Control(ELandmarkDescField));
    editor->GetText(desc);

    // Fetch existing location
    TLocality existingLocation;
    TBool locationExists = 
        (iLandmark->GetPosition(existingLocation) == KErrNone);
    TRealX floatFieldValue;

    // Fetch Latitude
    TReal latitude = existingLocation.Latitude();
    if (iIsRealValueEdited[ELatitudeEditor] || !locationExists)
        {
        FetchFloatL(floatFieldValue, ELandmarkLatitudeField, 
            R_LMREFAPP_LATITUDE_ERROR);
        floatFieldValue.GetTReal(latitude);
        }

    // Fetch Longitude
    TReal longitude = existingLocation.Longitude();
    if (iIsRealValueEdited[ELongitudeEditor] || !locationExists)
        {
        FetchFloatL(floatFieldValue, ELandmarkLongitudeField, 
            R_LMREFAPP_LONGITUDE_ERROR);
        floatFieldValue.GetTReal(longitude);
        }

    // Fetch Altitude
    TReal32 altitude = existingLocation.Altitude();
    if (iIsRealValueEdited[EAltitudeEditor] || !locationExists)
        {
        FetchFloatL(floatFieldValue, ELandmarkAltitudeField, 
            R_LMREFAPP_ALTITUDE_ERROR);
        floatFieldValue.GetTReal(altitude);
        }

    // Fetch horizontal accuracy
    TReal32 horAcc = existingLocation.HorizontalAccuracy();
    if (iIsRealValueEdited[EHorizontalAccEditor] || !locationExists)
        {
        FetchFloatL(floatFieldValue, ELandmarkHorAccField, 
            R_LMREFAPP_HOR_ACC_ERROR);
        floatFieldValue.GetTReal(horAcc);
        }

    // Fetch vertical accuracy
    TReal32 verAcc = existingLocation.VerticalAccuracy();
    if (iIsRealValueEdited[EVerticalAccEditor] || !locationExists)
        {
        FetchFloatL(floatFieldValue, ELandmarkVerAccField, 
            R_LMREFAPP_VER_ACC_ERROR);
        floatFieldValue.GetTReal(verAcc);
        }

    // Fetch Coverage Radius
    TReal32 radius;
    TInt radiusExists = (iLandmark->GetCoverageRadius(radius) == KErrNone);
    if (iIsRealValueEdited[ERadiusEditor] || !radiusExists)
        {
        FetchFloatL(floatFieldValue, ELandmarkRadiusField, 
            R_LMREFAPP_RADIUS_ERROR);
        floatFieldValue.GetTReal(radius);
        }

    // Fetch street
    TBuf<KPosLmMaxTextFieldLength> street;
    editor = 
        static_cast <CEikEdwin*> (Control(ELandmarkStreetField));
    editor->GetText(street);

    // Fetch postal code
    TBuf<KPosLmMaxTextFieldLength> postalCode;
    editor = 
        static_cast <CEikEdwin*> (Control(ELandmarkPostalField));
    editor->GetText(postalCode);

    // Fetch city
    TBuf<KPosLmMaxTextFieldLength> city;
    editor = static_cast <CEikEdwin*> (Control(ELandmarkCityField));
    editor->GetText(city);

    // Fetch country
    TBuf<KPosLmMaxTextFieldLength> country;
    editor = 
        static_cast <CEikEdwin*> (Control(ELandmarkCountryField));
    editor->GetText(country);

    // Update category
    iLandmark->RemoveLandmarkAttributes(CPosLandmark::ECategoryInfo);
    for (TInt i = 0; i < iCategoryIds.Count(); i++)
        {
        iLandmark->AddCategoryL(iCategoryIds[i]);
        }
    // Check that at least some data is entered
    if (!name.Length() && !desc.Length() && 
        !street.Length() && !city.Length() && 
        !country.Length() && !postalCode.Length() &&
        !iCategoryIds.Count() &&
        Math::IsNaN(latitude) && Math::IsNaN(longitude) &&
        Math::IsNaN(altitude) && Math::IsNaN(radius) &&
        Math::IsNaN(horAcc) && Math::IsNaN(verAcc))
        {
        NotifyErrorToUserL(R_LMREFAPP_EMPTY_LANDMARK);
        }

    // Update name
    iLandmark->SetLandmarkNameL(name);

    // Update description
    iLandmark->SetLandmarkDescriptionL(desc);
    CleanupStack::PopAndDestroy(descBuf);

    // Update coverage radius
    if (!Math::IsNaN(radius) && radius < 0)
        {
        NotifyErrorToUserL(R_LMREFAPP_RADIUS_ERROR);
        }
    iLandmark->SetCoverageRadius(radius);

    // Update street
    iLandmark->SetPositionFieldL(EPositionFieldStreet, street);

    // Update postal code
    iLandmark->SetPositionFieldL(EPositionFieldPostalCode, postalCode);

    // Update city
    iLandmark->SetPositionFieldL(EPositionFieldCity, city);

    // Update country
    iLandmark->SetPositionFieldL(EPositionFieldCountry, country);

    // Update location. There are some rules specified by TLocality and 
    // TCoordinate how to update a location. These rules are taken into 
    // account below when validating the location data.
    if (Math::IsNaN(latitude) && Math::IsNaN(longitude))
        {
        // If lat long are undefined, then alt and hAcc
        // must be undefined
        if (!Math::IsNaN(horAcc))
            {
            NotifyErrorToUserL(R_LMREFAPP_HACC_LAT_LONG_ERROR);
            }
        else if (!Math::IsNaN(altitude))
            {
            NotifyErrorToUserL(R_LMREFAPP_ALT_LAT_LONG_ERROR);
            }
        else if (!Math::IsNaN(verAcc))
            {
            NotifyErrorToUserL(R_LMREFAPP_VACC_ALT_ERROR);
            }
        else 
            {
            // None position fields are set, Remove position info.
            iLandmark->RemoveLandmarkAttributes(CPosLandmark::EPosition);
            }
        }
    else if (!Math::IsNaN(latitude) && !Math::IsNaN(longitude))
        {
        // If lat long are defined, hAcc must be positive if defined
        if (!Math::IsNaN(horAcc) && horAcc < 0)
            {
            NotifyErrorToUserL(R_LMREFAPP_HOR_ACC_ERROR);
            }

        // if altitude is undefined, then vertical accuracy 
        // must be undefined
        if (Math::IsNaN(altitude) && !Math::IsNaN(verAcc))
            {
            NotifyErrorToUserL(R_LMREFAPP_VACC_ALT_ERROR);
            }
            
        // vertical accuracy must be positive if defined
        if (!Math::IsNaN(verAcc) && verAcc < 0)
            {
            NotifyErrorToUserL(R_LMREFAPP_VER_ACC_ERROR);
            }
            
        // We are allowed to set the new locality
        TCoordinate coordinate(latitude, longitude, altitude);
        TLocality newLocation(coordinate, horAcc, verAcc);
        iLandmark->SetPositionL(newLocation);
        }
    else 
        {
        // Inform user that no valid lat/long has been entered
        NotifyErrorToUserL(R_LMREFAPP_LAT_LONG_ERROR);
        }

    iEdited = ETrue;
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CLandmarksEditDialog::ConfigureFieldsL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksEditDialog::ConfigureFieldsL()
    {
    TPtrC textFieldValue;

    // Configure landmark name
    textFieldValue.Set(KNullDesC);
    iLandmark->GetLandmarkName(textFieldValue);

    ConfigureTextFieldL(ELandmarkNameField, textFieldValue);

    // Configure description
    textFieldValue.Set(KNullDesC);
    iLandmark->GetLandmarkDescription(textFieldValue);

    ConfigureTextFieldL(ELandmarkDescField, textFieldValue);

    // Configure street;
    textFieldValue.Set(KNullDesC);
    iLandmark->GetPositionField(EPositionFieldStreet, textFieldValue);

    ConfigureTextFieldL(ELandmarkStreetField, textFieldValue);

    // Configure postalCode;
    textFieldValue.Set(KNullDesC);
    iLandmark->GetPositionField(EPositionFieldPostalCode, textFieldValue);

    ConfigureTextFieldL(ELandmarkPostalField, textFieldValue);

    // Configure city;
    textFieldValue.Set(KNullDesC);
    iLandmark->GetPositionField(EPositionFieldCity, textFieldValue);

    ConfigureTextFieldL(ELandmarkCityField, textFieldValue);

    // Configure country
    textFieldValue.Set(KNullDesC);
    iLandmark->GetPositionField(EPositionFieldCountry, textFieldValue);

    ConfigureTextFieldL(ELandmarkCountryField, textFieldValue);

    // Configure location fields
    TLocality location;
    iLandmark->GetPosition(location);
    ConfigureLocationFieldsL(location);

    // Configure coverage Radius
    TReal32 coverageRadius;
    TBuf<KPosLmMaxTextFieldLength> radiusBuf;
    TInt res = iLandmark->GetCoverageRadius(coverageRadius);
    if (res != KErrNotFound)
        {
        radiusBuf.AppendNum(coverageRadius, iRealFormat);
        }

    ConfigureTextFieldL(ELandmarkRadiusField, radiusBuf);
    }

// -----------------------------------------------------------------------------
// CLandmarksEditDialog::ConfigureLocationFieldsL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksEditDialog::ConfigureLocationFieldsL(TLocality& aLocation)
    {
    TBuf<KPosLmMaxTextFieldLength> textFieldValue;

    // Configure latitude in float format
    LandmarksUtils::LatLongToDes(aLocation.Latitude(), textFieldValue);
    ConfigureTextFieldL(ELandmarkLatitudeField, textFieldValue);

    // Configure longitude in float format
    LandmarksUtils::LatLongToDes(aLocation.Longitude(), textFieldValue);
    ConfigureTextFieldL(ELandmarkLongitudeField, textFieldValue);

    // Configure altitude
    LandmarksUtils::FloatToDesL(aLocation.Altitude(), textFieldValue, iRealFormat);
    ConfigureTextFieldL(ELandmarkAltitudeField, textFieldValue);

    // Configure horizontal accuracy
    LandmarksUtils::FloatToDesL(aLocation.HorizontalAccuracy(), textFieldValue, iRealFormat);
    ConfigureTextFieldL(ELandmarkHorAccField, textFieldValue);

    // Configure vertical accuracy
    LandmarksUtils::FloatToDesL(aLocation.VerticalAccuracy(), textFieldValue, iRealFormat);
    ConfigureTextFieldL(ELandmarkVerAccField, textFieldValue);
    }

// -----------------------------------------------------------------------------
// CLandmarksEditDialog::ConfigureCategoryFieldL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksEditDialog::ConfigureCategoryFieldL()
    {
    TInt nrOfCategories = iCategoryIds.Count();
    const TInt KExtraChars = 2; // CRLF
    
    HBufC* lmCategoriesBuf = HBufC::NewLC(
        (KPosLmMaxTextFieldLength + KExtraChars) * nrOfCategories);
    
    TPtr lmCategories = lmCategoriesBuf->Des();
    for (TInt i = 0; i < nrOfCategories; i++)
        {
        if (i > 0)
            {
            lmCategories.Append(CEditableText::ELineBreak);
            }

        CPosLandmarkCategory* category = iEngine.CategoryLC(iCategoryIds[i]);
        TPtrC categoryName;
        category->GetCategoryName(categoryName);
        lmCategories.Append(categoryName);
        CleanupStack::PopAndDestroy(category);
        }

    ConfigureTextFieldL(ELandmarkCategoryField, *lmCategoriesBuf);

    CleanupStack::PopAndDestroy(lmCategoriesBuf);
    }

void CLandmarksEditDialog::ConfigureTextFieldL(TInt aId, const TDesC& aText)
    {
    CEikEdwin* editor = static_cast<CEikEdwin*>(Control(aId));
	editor->SetTextL(&aText);
    }

// -----------------------------------------------------------------------------
// CLandmarksEditDialog::UpdateTitleBarL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksEditDialog::UpdateTitleBarL(TBool aFromField)
    {
    if (aFromField)
        {
        TBuf<KPosLmMaxTextFieldLength> name;

        CEikEdwin* nameEditor = 
            static_cast<CEikEdwin*>(Control(ELandmarkNameField));
        nameEditor->GetText(name);
        iTitlePane->SetTextL(name);
        }
    else
        {
        TPtrC ptr;
        iLandmark->GetLandmarkName(ptr);
        iTitlePane->SetTextL(ptr);
        }
    }

// -----------------------------------------------------------------------------
// CLandmarksEditDialog::FetchFloatL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksEditDialog::FetchFloatL(
    TRealX& aFloat, 
    TInt aEditorResourceId, 
    TInt aErrorResourceId)
    {
    const TChar KDecimalPoint = '.';
    TBuf<KPosLmMaxTextFieldLength> textFieldValue;
    TLex fieldLex;
    TInt result(KErrNone);
    CEikEdwin* editor = static_cast <CEikEdwin*> (Control(aEditorResourceId));

    editor->GetText(textFieldValue);
    if (textFieldValue == KNullDesC)
        {
        // User has indicated that he/she wants this value to be undefined.
        aFloat.SetNaN();
        }
    else 
        {
        // Try to extract value
        fieldLex = textFieldValue;
        TReal floatValue(0);
        result = fieldLex.Val(floatValue, KDecimalPoint);
        if (result != KErrNone)
            {
            NotifyErrorToUserL(aErrorResourceId);
            }

        // Update output parameter
        User::LeaveIfError(aFloat.Set(floatValue));
        }
    }

// -----------------------------------------------------------------------------
// CLandmarksEditDialog::NotifyErrorToUserL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksEditDialog::NotifyErrorToUserL(TInt aResourceId)
    {
    HBufC* msg = iCoeEnv->AllocReadResourceLC(aResourceId);
    CAknErrorNote* errorNote = new (ELeave) CAknErrorNote(ETrue);
    errorNote->ExecuteLD(*msg);
    CleanupStack::PopAndDestroy(msg);

    // Leave to interrupt saving of data. Other error codes than KErrNone 
    // results in an additional error dialog.
    User::Leave(KErrNone);
    }

// -----------------------------------------------------------------------------
// CLandmarksEditDialog::EditCategoriesL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksEditDialog::EditCategoriesL()
    {
    TInt res = CLandmarksCategoriesDialog::RunDialogL(*iLmCategoriesModel); 
    if (res)
        {
        ConfigureCategoryFieldL();
        DrawDeferred();
        SetChangesPending(ETrue);
        }
    }
	
// -----------------------------------------------------------------------------
// CLandmarksEditDialog::UpdateToCurrentLocationL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksEditDialog::UpdateToCurrentLocationL()
    {
    // Update location fields to current location
    HBufC* title = 
        iCoeEnv->AllocReadResourceAsDes16LC(R_LMREFAPP_ACQUIRING_LOC);
    iPositionRequest->FetchPositionInfoL(*title, iPositionInfo);
    CleanupStack::PopAndDestroy(title);
    }
    
// End of File

