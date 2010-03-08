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


// INCLUDE FILES
#include "MyPicture.h"
#include <fbs.h>

// ---------------------------------------------------------
// Constructor
// ---------------------------------------------------------
//
CMyPicture::CMyPicture( TSize aSize, CFbsBitmap& aBitmap )
	: iSizeInTwips(aSize), iBitmap(&aBitmap)
    {
    }

// ---------------------------------------------------------
// LineBreakPossible()
// ---------------------------------------------------------
//
TBool CMyPicture::LineBreakPossible( TUint /*aClass*/,
										   TBool /*aBeforePicture*/,
										   TBool /*aHaveSpaces*/ ) const
	{
	return EFalse;
	}

// ---------------------------------------------------------
// Draw()
// ---------------------------------------------------------
//
void CMyPicture::Draw( CGraphicsContext& aGc,
						     const TPoint& aTopLeft,
						     const TRect& aClipRect,
						     MGraphicsDeviceMap* aMap ) const
	{
	TRect bitmapRect=aMap->TwipsToPixels(TRect(TPoint(),iSizeInTwips));
	bitmapRect.Move(aTopLeft);
	aGc.Reset();
	aGc.SetClippingRect(aClipRect);
	aGc.DrawBitmap(bitmapRect, iBitmap);
	}

// ---------------------------------------------------------
// ExternalizeL()
// ---------------------------------------------------------
//
void CMyPicture::ExternalizeL( RWriteStream& /*aStream*/ ) const
	{
    // No implementation required
	}
		
// ---------------------------------------------------------
// SetOriginalSizeInTwips()
// ---------------------------------------------------------
//
void CMyPicture::SetOriginalSizeInTwips( TSize aSize )
	{
	iSizeInTwips = aSize;
	}

// ---------------------------------------------------------
// GetOriginalSizeInTwips()
// ---------------------------------------------------------
//
void CMyPicture::GetOriginalSizeInTwips( TSize& aSize ) const
	{
	aSize = iSizeInTwips;
	}

//EOF
