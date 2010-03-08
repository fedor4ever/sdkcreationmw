/*
* Copyright (c) 2005 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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




#ifndef __LOCALIZATIONAPPVIEW_H__
#define __LOCALIZATIONAPPVIEW_H__

// INCLUDES
#include <coecntrl.h>

// FORWARD DECLARATIONS
class CFbsBitmap;

// CLASS DECLARATION
class CLocalizationAppView : public CCoeControl
    {
    public: // New methods

        /**
        * NewL.
        * Two-phased constructor.
        * Create a CLocalizationAppView object, which will draw itself to aRect.
        * @param aRect The rectangle this view will be drawn to.
        * @return a pointer to the created instance of CLocalizationAppView.
        */
        static CLocalizationAppView* NewL( const TRect& aRect );

        /**
        * NewLC.
        * Two-phased constructor.
        * Create a CLocalizationAppView object, which will draw itself
        * to aRect.
        * @param aRect Rectangle this view will be drawn to.
        * @return A pointer to the created instance of CLocalizationAppView.
        */
        static CLocalizationAppView* NewLC( const TRect& aRect );

        /**
        * ~CLocalizationAppView
        * Virtual Destructor.
        */
        virtual ~CLocalizationAppView();

    public:  // Functions from base classes

        /**
        * DrawImage
        * Draw image to screen
        * @param aImage the image to be drawn
        */
		void DrawImage(CFbsBitmap* aImage);

        /**
        * From CCoeControl, Draw
        * Draw this CLocalizationAppView to the screen.
        * @param aRect the rectangle of this view that needs updating
        */
        void Draw( const TRect& aRect ) const;

        /**
        * From CoeControl, SizeChanged.
        * Called by framework when the view size is changed.
        */
        virtual void SizeChanged();

    private: // Constructors

        /**
        * ConstructL
        * 2nd phase constructor.
        * Perform the second phase construction of a
        * CLocalizationAppView object.
        * @param aRect The rectangle this view will be drawn to.
        */
        void ConstructL(const TRect& aRect);

        /**
        * CLocalizationAppView.
        * C++ default constructor.
        */
        CLocalizationAppView();

    private: // data

        CFbsBitmap* iImage; // (owned)

    };

#endif // __LOCALIZATIONAPPVIEW_H__

// End of File
