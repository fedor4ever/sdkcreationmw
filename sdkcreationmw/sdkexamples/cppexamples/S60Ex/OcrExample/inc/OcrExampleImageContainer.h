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


#ifndef OCREXAMPLECONTAINER_H
#define OCREXAMPLECONTAINER_H

// INCLUDES
#include <coecntrl.h>
#include <ocrsrv.h>
#include "RectScaler.h"

// FORWARD DECLARATIONS
class TOcrExampleSettingsData;

/**
 * TScreenUpdateData collects various items the container needs to draw
 * to the screen. The container does not own any of the objects in this
 * class; it just refers to them.
 */
class TScreenUpdateData
    {
    public:
        /** pointer to bitmap storing the scaled image */
        CFbsBitmap              *iScaledBitMap;

        /** block info acquired from layout analyzis  */
        const TOCRBlockInfo     *iBlockInfo;

        /** count of blocks in iBlockInfo */
        TInt                    iBlockCount;

        /** utility to scale rectangles in iBlockInfo to match the scaled image */
        NOCRUtils::TRectScaler  iRectScaler;

        /** pointer to crosshair rectangle */
        TRect                   *iCrosshair;
    };

/**
 * COcrExampleImageContainer container control class.
 * This class does basically just contain intelligence, when it comes to
 * drawing things on screen. All the objects (images and rectangles) to be
 * drawn are controlled by the "owner" of the container. The container refers
 * to the drawable elements via shared TScreenUpdateData-object.
 */
class COcrExampleImageContainer : public CCoeControl
    {
    public:
		/**
		* NewL.
		* Two-phased constructor.
		* Construct a COcrExampleImageContainer using two phase construction,
		* and return a pointer to the created object
        * @param aRect the main area for the container to draw
        * @param aSettings the application level settings
        * @param aData collected data needed for drawing
		* @return A pointer to the created instance of COcrExampleImageContainer
		*/
        static COcrExampleImageContainer* NewL( const TRect& aRect,
                                                TOcrExampleSettingsData& aSettings,
                                                TScreenUpdateData &aData );
		/**
		* NewLC.
		* Two-phased constructor.
		* Construct a COcrExampleImageContainer using two phase construction,
		* and return a pointer to the created object
	    * @param aRect the main area for the container to draw
        * @param aSettings the application level settings
        * @param aData collected data needed for drawing
	    * @return A pointer to the created instance of COcrExampleImageContainer
		*/
		static COcrExampleImageContainer* NewLC( const TRect& aRect,
                                                TOcrExampleSettingsData& aSettings,
                                                TScreenUpdateData &aData );
        /** Destructor. */
        ~COcrExampleImageContainer();

	private:
        /** constructor */
	    COcrExampleImageContainer(TOcrExampleSettingsData& aSettingsData,
	                              TScreenUpdateData &aData );
		/**
        * Symbian default constructor.
        * @param aRect Frame rectangle for container.
        */
        void ConstructL( const TRect& aRect );

    public: // Functions from base classes

    private: // Functions from base classes

        /** From CCoeControl.
        * container uses key-events to move crosshair (if activated)
        */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );

        /** From CCoeControl */
        void Draw( const TRect& aRect ) const;

    private: //data

        /** settings */
        TOcrExampleSettingsData &iSettings;

        /** screen data */
        TScreenUpdateData       &iScrData;
    };

#endif

// End of File