/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __HELLOWORLDBASICAPPVIEW_H__
#define __HELLOWORLDBASICAPPVIEW_H__

// INCLUDES
#include <coecntrl.h>

// CLASS DECLARATION
class CHelloWorldBasicAppView : public CCoeControl
    {
    public: // New methods

        /**
        * NewL.
        * Two-phased constructor.
        * Create a CHelloWorldBasicAppView object, which will draw itself to aRect.
        * @param aRect The rectangle this view will be drawn to.
        * @return a pointer to the created instance of CHelloWorldBasicAppView.
        */
        static CHelloWorldBasicAppView* NewL( const TRect& aRect );

        /**
        * NewLC.
        * Two-phased constructor.
        * Create a CHelloWorldBasicAppView object, which will draw itself
        * to aRect.
        * @param aRect Rectangle this view will be drawn to.
        * @return A pointer to the created instance of CHelloWorldBasicAppView.
        */
        static CHelloWorldBasicAppView* NewLC( const TRect& aRect );

        /**
        * ~CHelloWorldBasicAppView
        * Virtual Destructor.
        */
        virtual ~CHelloWorldBasicAppView();

       /**
        * GetText
        * @return modifiable reference to the descriptor, which stores
        *   user-given text
        */
        TDes& GetText();

    public:  // Functions from base classes

        /**
        * From CCoeControl, Draw
        * Draw this CHelloWorldBasicAppView to the screen.
        * If the user has given a text, it is also printed to the center of
        * the screen.
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
        * CHelloWorldBasicAppView object.
        * @param aRect The rectangle this view will be drawn to.
        */
        void ConstructL(const TRect& aRect);

        /**
        * CHelloWorldBasicAppView.
        * C++ default constructor.
        */
        CHelloWorldBasicAppView();

    private:

       /**
        * font used, when printing user-given text to the screen
        */
        const CFont* iFont;

       /**
        * descriptor storing the user-given text
        */
        TBuf<24> iText;
    };

#endif // __HELLOWORLDBASICAPPVIEW_H__

// End of File

