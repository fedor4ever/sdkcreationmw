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


#ifndef OCR_EXAMPLE_SETTINGS_DATA_H
#define OCR_EXAMPLE_SETTINGS_DATA_H

// INCLUDES
#include <e32base.h>
#include <gdi.h>

class TOCRLayoutSetting;

/**
* TOcrExampleSettingsData contains the general settings for the application.
* This is a POD-type, but offers a constructor to initialize fields to default
* values and a couple 'added value'-functions.
*
* @note This class actually could be struct (instead of class with everything
*   public)... but for some reason winscw-compilation produces linking errors
*   in such case.
*/
class TOcrExampleSettingsData
    {
    public:
        /** ctor */
        TOcrExampleSettingsData();

        /**
        * fills array with the enabled languages according to the settings
        * @param aLanguages array of language identifications
        */
        void GetLanguages( RArray< TLanguage > &aLanguages ) const;

        /** @return returns color for the block rectangles */
        TRgb GetHighlightColor() const;

        /**
        * calculates the size for the crosshair based on settings and image size
        * @param aImageSize image size on the screen
        * @return crosshair size
        */
        TSize GetCrosshairSize( const TSize &aImageSize ) const;

        /** index of highlight color in the settings */
        TInt    iHighlightColor;

        /** flag indicating whether English is enabled in settings */
        TBool   iUseEnglish;

        /** flag indicating whether Chinese is enabled in settings */
        TBool   iUseChinese;

        /** flag indicating whether Japanese is enabled in settings */
        TBool   iUseJapanese;

        /** crosshair height as percentage compared to the actual image */
        TInt    iCrossHairHeight;

        /** crosshair width as percentage compared to the actual image */
        TInt    iCrossHairWidth;
    };

#endif //OCR_EXAMPLE_SETTINGS_DATA_H

// End of File
