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
* Description: 
*
*/



#ifndef __CSASYNCAPPVIEW_H__
#define __CSASYNCAPPVIEW_H__

// INCLUDE FILES
#include <coecntrl.h>

// FORWARD DECLARATIONS
class CCSAsyncDocument;

// CLASS DECLARATION
/**
* CCSAsyncAppView
*  An instance of the Application View object for the CSAsync
*  example application.
*/
class CCSAsyncAppView : public CCoeControl
    {
    public: // Constructors and destructors

        /**
        * NewL.
        * Two-phased constructor.
        * Creates a CCSAsyncAppView object, which will draw itself to aRect.
        * @param aRect The rectangle this view will be drawn to.
        * @param aDocument The document to read time data from.
        * @return A pointer to the created instance of CCSAsyncAppView.
        */
        static CCSAsyncAppView* NewL( const TRect& aRect,
                                      CCSAsyncDocument& aDocument );

        /**
        * NewLC.
        * Two-phased constructor.
        * Creates a CCSAsyncAppView object, which will draw itself to aRect.
        * @param aRect The rectangle this view will be drawn to.
        * @param aDocument The document to read time data from.
        * @return A pointer to the created instance of CCSAsyncAppView.
        */
        static CCSAsyncAppView* NewLC( const TRect& aRect,
                                       CCSAsyncDocument& aDocument );

        /**
        * ~CCSAsyncAppView.
        * Destructor.
        * Destroys the object and release all memory objects.
        */
        virtual ~CCSAsyncAppView();

    public:  // Functions from base classes

        /**
        * From CCoeControl, Draw.
        * Draws this CCSAsyncAppView to the screen.
        * @param aRect The rectangle of this view that needs updating.
        */
        void Draw( const TRect& aRect ) const;

    private: // Constructors and destructors

        /**
        * CCSAsyncAppView.
        * C++ default constructor.
        * Performs the first phase of two phase construction.
        * @param aDocument The document to read time data from.
        */
        CCSAsyncAppView( CCSAsyncDocument& aDocument );

        /**
        * ConstructL.
        * Performs the second phase construction of a CCSAsyncAppView object.
        * @param aRect The rectangle this view will be drawn to.
        */
        void ConstructL( const TRect& aRect );

    private: // Data

        /**
        * iDocument, the application document.
        */
        CCSAsyncDocument& iDocument;
    };


#endif // __CSASYNCAPPVIEW_H__


// End of File
