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


// INCLUDE FILES
#include <ocrcommon.h>
#include "OcrExampleSettingsData.h"

// ================= MEMBER FUNCTIONS =========================================

// Constructor
//
TOcrExampleSettingsData::TOcrExampleSettingsData()
    : iHighlightColor(0)
    , iUseEnglish(ETrue)
    , iUseChinese(EFalse)
    , iUseJapanese(EFalse)
    , iCrossHairHeight(30)
    , iCrossHairWidth(30)
    {
    }

void TOcrExampleSettingsData::GetLanguages( RArray< TLanguage > &aLangs ) const
    {
    aLangs.Reset();
    if (iUseEnglish)  aLangs.Append(ELangEnglish);
    if (iUseChinese)  aLangs.Append(ELangPrcChinese);
    if (iUseJapanese) aLangs.Append(ELangJapanese);
    }

TRgb TOcrExampleSettingsData::GetHighlightColor() const
    {
    switch (iHighlightColor)
        {
        case 1:  return KRgbYellow;
        case 2:  return KRgbBlack;
        default: return KRgbRed;
        }
    }

TSize TOcrExampleSettingsData::GetCrosshairSize(const TSize &aImageSize) const
    {
    return TSize((iCrossHairWidth * aImageSize.iWidth)/100,
                (iCrossHairHeight * aImageSize.iHeight)/100);
    }
// End of File

