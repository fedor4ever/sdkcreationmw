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


#ifndef __HELPEXAMPLE_APPVIEW_H__
#define __HELPEXAMPLE_APPVIEW_H__

// INCLUDES
#include <coecntrl.h>

/**
*  CHelpExampleAppView
*  An instance of the Application View object for the HelpExample 
*  example application
*/
class CHelpExampleAppView : public CCoeControl
    {
        public: // Constructors and Destructor.

            /**
            * NewL()
            * Create a CHelpExampleAppView object, which will draw 
            * itself to aRect
            * @param aRect The rectangle this view will be drawn to
            * @return A pointer to the created instance of CHelpExampleAppView
            */
            static CHelpExampleAppView* NewL( const TRect& aRect );

            /**
            * NewLC()
            * Create a CHelpExampleAppView object, which will draw 
            * itself to aRect
            * @param aRect The rectangle this view will be drawn to
            * @return A pointer to the created instance of CHelpExampleAppView
            */
            static CHelpExampleAppView* NewLC( const TRect& aRect );


            /**
            * ~CHelpExampleAppView()
            * Destroy the object
            */
            virtual ~CHelpExampleAppView();


        public:  // from CCoeControl

            /**
            * Draw()
            * Draw this CHelpExampleAppView to the screen
            * @param aRect The rectangle of this view that needs updating
            */
            void Draw( const TRect& aRect ) const;
              

        private:    // Constructors.

            /**
            * ConstructL()
            * Perform the second phase construction of a CHelpExampleAppView object
            * @param aRect The rectangle this view will be drawn to
            */
            void ConstructL( const TRect& aRect );

            /**
            * CHelpExampleAppView()
            * Perform the first phase of two phase construction 
            */
            CHelpExampleAppView();
    };


#endif // __HELPEXAMPLE_APPVIEW_H__

// End of File
