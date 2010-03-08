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


#ifndef __ANIMATIONAPPVIEW_H__
#define __ANIMATIONAPPVIEW_H__

// INCLUDES
#include <coecntrl.h>

/**
* CAnimationAppView.
* An instance of the Application View object for the Animation.
* example application.
*/
class CAnimationAppView : public CCoeControl
    {
    public:  // Constructors and destructor

        /**
        * NewL.
        * Two-phased constructor.
        * Create a CAnimationAppView object, which will draw itself to aRect.
        * @param aRect The rectangle this view will be drawn to.
        * @return A pointer to the created instance of CAnimationAppView.
        */
        static CAnimationAppView* NewL( const TRect& aRect );

        /**
        * NewLC.
        * Two-phased constructor.
        * Create a CAnimationAppView object, which will draw itself to aRect.
        * @param aRect Rectangle this view will be drawn to.
        * @return A pointer to the created instance of CAnimationAppView.
        */
        static CAnimationAppView* NewLC( const TRect& aRect );

        /**
        * ~CAnimationAppView
        * Virtual Destructor.
        */
        virtual ~CAnimationAppView();

    public:  // from CCoeControl

        /**
        * Draw
        * Draw this CAnimationAppView to the screen.
        */
        void Draw( const TRect& /*aRect*/ ) const;
        
        /**
        * HandleResolutionChange.
        * Handles a resolution change
        * @param aRect the new rectangle of this view.
        */
        void HandleResolutionChange(TRect aRect);

    private: // Constructors

        /**
        * CAnimationAppView.
        * C++ default constructor.
        */
        CAnimationAppView();

        /**
        * ConstructL
        * 2nd phase constructor.
        * Perform the second phase construction of a CAnimationAppView object.
        * @param aRect The rectangle this view will be drawn to.
        */
        void ConstructL( const TRect& aRect );
    };

#endif //__ANIMATIONAPPVIEW_H__

// End of File
