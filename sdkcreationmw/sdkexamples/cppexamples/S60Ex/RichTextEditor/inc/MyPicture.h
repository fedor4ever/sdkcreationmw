/*
* Copyright (c) 2006 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef CMYPICTURE_H
#define CMYPICTURE_H

// INCLUDES
#include <gdi.h>

// FORWARD DECLARATIONS
class TSize;
class CFbsBitmap;

// CLASS DECLARATION
/**
*  CMyPicture the class which draw the image.
*/
class CMyPicture :public CPicture
    {
    public:  // Constructors and NO destructor (bitmap not owned)
        
        /**
        * C++ default constructor.
		* @param aSize Size of the picture in twips.
		* @param aBitmap Bitmap
        */
        CMyPicture( TSize aSize, CFbsBitmap& aBitmap );

	public: // From CPicture

		/**
		* Prohibit linebreaks.
		*/
		TBool LineBreakPossible( TUint aClass,
								 TBool aBeforePicture,
								 TBool aHaveSpaces ) const;
		/**
        * Draw the picture
        */
		 void Draw( CGraphicsContext& aGc,
						   const TPoint& aTopLeft,
						   const TRect& aClipRect,
						   MGraphicsDeviceMap* aMap ) const;
		/**
        * There's no need for it in this , but must be implemented.
        */
		 void ExternalizeL( RWriteStream& aStream ) const;
		
        /**
        * Sets the picture's size in twips.
		* @param aSize Size.
        */
		 void SetOriginalSizeInTwips( TSize aSize );

        /**
        * Returns the picture's size in twips.
		* @param aSize Size.
        */
		 void GetOriginalSizeInTwips( TSize& aSize ) const;

	protected:	// Data

		TSize iSizeInTwips; // Size of the bitmap data
		CFbsBitmap* iBitmap; // reference to the Bitmap data
    };

#endif
            
