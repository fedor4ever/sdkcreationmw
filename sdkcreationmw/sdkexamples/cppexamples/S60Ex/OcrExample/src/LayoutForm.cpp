/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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


#include <AknPopupFieldText.h> // CAknPopupFieldText
#include <avkon.hrh> // Commands
#include <avkon.rsg> // R_AVKON_FORM_MENUPANE
#include <eikmenup.h> // CEikMenuPane
#include <ocrcommon.h>
#include "LayoutForm.h"
#include "OcrExample.hrh"

using namespace NOCRUtils;

// ----------------------------------------------------------------------------
// CLayoutForm-class member functions
// ----------------------------------------------------------------------------

TInt CLayoutForm::ExecuteFormLD( TOCRLayoutBlockInfo& aInfo, TInt aResourceId)
    {
    CLayoutForm* self = new (ELeave) CLayoutForm(&aInfo);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self->ExecuteLD(aResourceId);
    }

TInt CLayoutForm::ExecuteFormLD( TOCRLayoutSetting& aInfo, TInt aResourceId)
    {
    CLayoutForm* self = new (ELeave) CLayoutForm(NULL, &aInfo);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self->ExecuteLD(aResourceId);
    }

TInt CLayoutForm::ExecuteFormLD( TRegionInfo& aInfo, TInt aResourceId)
    {
    CLayoutForm* self = new (ELeave) CLayoutForm(NULL, NULL, &aInfo );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self->ExecuteLD(aResourceId);
    }

TBool CLayoutForm::SaveFormDataL()
    {
    CAknPopupFieldText* fieldText = 0;
    // check, which setting-struct was given in dialog
    if (iBlock)
        {
        fieldText = static_cast<CAknPopupFieldText*>(ControlOrNull(EOCRExampleFormLayout));
        if (fieldText)
            {
            switch (fieldText->CurrentValueIndex())
                {
                case 0: iBlock->iLayout = EOcrLayoutTypeH; break;
                case 1: // fall-through
                default: iBlock->iLayout = EOcrLayoutTypeV;
                }
            }

        fieldText = static_cast<CAknPopupFieldText*>(ControlOrNull(EOCRExampleFormBackground));
        if (fieldText)
            {
            switch (fieldText->CurrentValueIndex())
                {
                case 0: iBlock->iBackgroundColor = EOcrBackgroundLight; break;
                case 1: iBlock->iBackgroundColor = EOcrBackgroundDark; break;
                case 2: // fall-through
                default: iBlock->iBackgroundColor = EOcrBackgroundUnknown;
                }
            }

        fieldText = static_cast<CAknPopupFieldText*>(ControlOrNull(EOCRExampleFormTextType));
        if (fieldText)
            {
            switch (fieldText->CurrentValueIndex())
                {
                case 0: iBlock->iText = EOcrTextMultiLine; break;
                case 1: // fall-through
                default: iBlock->iText = EOcrTextSingleLine;
                }
            }
        }
    else if (iLayout)
        {
        fieldText = static_cast<CAknPopupFieldText*>(ControlOrNull(EOCRExampleFormSkew));
        if (fieldText)
            {
            // bool-conversion
            iLayout->iSkew = (! fieldText->CurrentValueIndex());
            }

        fieldText = static_cast<CAknPopupFieldText*>(ControlOrNull(EOCRExampleFormBrightness));
        if (fieldText)
            {
            switch (fieldText->CurrentValueIndex())
                {
                case 0: iLayout->iBrightness = TOCRLayoutSetting::ENormal; break;
                case 1: iLayout->iBrightness = TOCRLayoutSetting::ELight; break;
                case 2: // fall-through
                default: iLayout->iBrightness = TOCRLayoutSetting::EDark;
                }
            }
        }
    else if (iRegion)
        {
        fieldText = static_cast<CAknPopupFieldText*>(ControlOrNull(EOCRExampleFormBackground));
        if (fieldText)
            {
            switch (fieldText->CurrentValueIndex())
                {
                case 0: iRegion->iBackgroundColor = EOcrBackgroundLight; break;
                case 1: iRegion->iBackgroundColor = EOcrBackgroundDark; break;
                case 2: // fall-through
                default: iRegion->iBackgroundColor = EOcrBackgroundUnknown;
                }
            }

        fieldText = static_cast<CAknPopupFieldText*>(ControlOrNull(EOCRExampleFormContentType));
        if (fieldText)
            {
            switch (fieldText->CurrentValueIndex())
                {
                case 0: iRegion->iType = TRegionInfo::EEmailAddress; break;
                case 1: iRegion->iType = TRegionInfo::ETelephoneNumber; break;
                case 2: // fall-through
                default: iRegion->iType = TRegionInfo::EWWWAddress;
                }
            }
        }
    else // internal error
        {
        ASSERT(EFalse);
        }

    return ETrue;
    }

TBool CLayoutForm::QuerySaveChangesL()
    {
    return ETrue;
    }

TBool CLayoutForm::OkToExitL( TInt aButtonId )
    {
    // our form has only OK-button, but base-class apparently assumes that forms
    // has Options- and Back-buttons. hence, if ok given, we want to exit so
    // we pass Back to the base-class
    //
    if (aButtonId == EAknSoftkeyOk)
        {
        aButtonId = EAknSoftkeyBack;
        }
    return CAknForm::OkToExitL(aButtonId);
    }


