/*
* Copyright (c) 2004, 2006 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __BITMAPMETHODS__
#define __BITMAPMETHODS__

#define KColourDepth EColor4K

//Ensure these libraries are linked against:
//LIBRARY bitgdi.lib

// INCLUDES
#include <e32base.h>
#include <gdi.h>

// FORWARD DECLARATIONS
class CFbsBitGc;
class CFbsBitmapDevice;
class CWsBitmap;
class CFbsBitmap;

// CLASS DECLARATION
/**
* NBitmapMethods
* A set of functions to make bitmap manipulation easier
*/
namespace NBitmapMethods
    {
    /**
    * CreateGraphicsContextLC
    * Create a graphics context and leave it on the cleanup stack
    * @param aBitmapDevice a reference to a bitmap device
    */
    CFbsBitGc* CreateGraphicsContextLC( CFbsBitmapDevice& aBitmapDevice );

    /**
    * CreateGraphicsContextL
    * Create a graphics context
    * @param aBitmapDevice a reference to a bitmap device
    */
    CFbsBitGc* CreateGraphicsContextL( CFbsBitmapDevice& aBitmapDevice );

    /**
    * CreateBitmapDeviceLC
    * Create a bitmap device and leave it on the cleanup stack
    * @param aBitmap a reference to a bitmap
    */
    CFbsBitmapDevice* CreateBitmapDeviceLC( CFbsBitmap& aBitmap );

    /**
    * CreateBitmapDeviceL
    * Create a bitmap device
    * @param aBitmap a reference to a bitmap
    */
    CFbsBitmapDevice* CreateBitmapDeviceL( CFbsBitmap& aBitmap );

    /**
    * CreateBitmapLC
    * Create a bitmap and leave it on the cleanup stack
    * @param aSizeInPixels the size of the bitmap to be created
    * @param aDispMode the display mode of the bitmap
    */
    CFbsBitmap* CreateBitmapLC( TSize aSizeInPixels, TDisplayMode aDispMode );

    /**
    * CreateBitmapL
    * Create a bitmap
    * @param aSizeInPixels the size of the bitmap to be created
    * @param aDispMode the display mode of the bitmap
    */
    CFbsBitmap* CreateBitmapL( TSize aSizeInPixels, TDisplayMode aDispMode );

    /**
    * CreateBitmapLC
    * Create a bitmap from an mbm file bitmap and leave it on the cleanup stack
    * @param aFileName the name of an mbm file
    * @param aId the position of the bitmap in the mbm file
    */
    CFbsBitmap* CreateBitmapLC( const TDesC& aFileName, TInt aId );

    /**
    * CreateBitmapLC
    * Create a bitmap from an mbm file bitmap
    * @param aFileName the name of an mbm file
    * @param aId the position of the bitmap in the mbm file
    */
    CFbsBitmap* CreateBitmapL( const TDesC& aFileName, TInt aId );

    /**
    * BitBltMaskedEntireBitmap
    * Blit the entire of a bitmap with a mask onto a gc
    * @param aTargetGc gc to blit onto
    * @param aTopLeft the position at which the top left of the
    * bitmap will be placed on the gc
    * @param aBitmap the bitmap to blit
    * @param aBitMask the mask
    */
    void BitBltMaskedEntireBitmap( CFbsBitGc& aTargetGc,
                                   TPoint aTopLeft,
                                   const CFbsBitmap& aBitmap,
                                   const CFbsBitmap& aBitMask );
    /**
    * PartialReset
    * Reset a gc so that it is in a sensible state.
    * Not all settings are reset, just some of the more useful ones
    * @param aGc the gc to reset
    */
    void PartialReset( CFbsBitGc& aGc );
    }

#endif //__BITMAPMETHODS__

// End of File
