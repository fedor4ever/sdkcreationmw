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


#ifndef __FOCUSEVENT_APPVIEW_H__
#define __FOCUSEVENT_APPVIEW_H__

// INCLUDES
#include <coecntrl.h>

// CONSTANTS
static const TInt KMaxTextLines = 7;
static const TInt KTextXMargin = 10;
static const TInt KTextMinY = 20;
static const TInt KTextDistanceBetweenLines = 20;
static const TInt KTestFrameworkMaxScreenMessageLength = 100;
typedef TBuf<KTestFrameworkMaxScreenMessageLength> TTextLine;


// CLASS DECLARATION
class CFocusEventAppView : public CCoeControl
    {
    public: // Constructors and destructors

        /**
        * NewL.
        * Two-phased constructor.
        * Create a CFocusEventAppView object, which will draw itself to aRect.
        * @param aRect The rectangle this view will be drawn to.
        * @return a pointer to the created instance of CFocusEventAppView.
        */
        static CFocusEventAppView* NewL( const TRect& aRect );

        /**
        * NewLC.
        * Two-phased constructor.
        * Create a CFocusEventAppView object, which will draw itself to aRect.
        * @param aRect Rectangle this view will be drawn to.
        * @return A pointer to the created instance of CFocusEventAppView.
        */
        static CFocusEventAppView* NewLC( const TRect& aRect );

        /**
        * ~CFocusEventAppView.
        * Virtual Destructor.
        */
        virtual ~CFocusEventAppView();

    public: // New functions

        /**
        * ShowTextOnScreen.
        * Display text on the screen.
        * @param aText the text to display
        */
        void ShowTextOnScreen( const TDesC& aText );

        /**
        * Print.
        * Display/Write text via the selected methods.
        * @param aText the text to display
        */
        void Print( const TDesC& aText );

        /**
        * PrintNewLineL.
        * Display/Write a new line via the selected methods.
        */
        void PrintNewLineL();

    private:  // from CCoeControl

        /**
        * Draw.
        * Draw this CFocusEventAppView to the screen.
        * @param aRect the rectangle of this view that needs updating
        */
        void Draw( const TRect& aRect ) const;
        
        /** From CoeControl, HandleResourceChange.
        * Called by framework when the view layout is changed.
        */
        virtual void HandleResourceChange(TInt aType);

    private: // Constructors

        /**
        * ConstructL.
        * 2nd phase constructor.
        * Perform the second phase construction of a CFocusEventAppView object.
        * @param aRect The rectangle this view will be drawn to.
        */
        void ConstructL( const TRect& aRect );

        /**
        * CFocusEventAppView.
        * C++ default constructor.
        */
        CFocusEventAppView();

    private: // Data

        /**
        * iTextLines
        * The lines of text being displayed on the screen
        */
        RArray <TTextLine> iTextLines;
        
    };

#endif // __FOCUSEVENT_APPVIEW_H__

// End of File
