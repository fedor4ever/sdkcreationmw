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


#include "RectScaler.h"

using namespace NOCRUtils;

// ----------------------------------------------------------------------------
// TRectScaler-class member functions
// ----------------------------------------------------------------------------

TRectScaler::TRectScaler() : iFactor(0)
    {
    }

void TRectScaler::CalculateFactor( TSize aOriginal, TSize aScaled )
    {
    // calculate relative factor from width
    iFactor = TReal(aOriginal.iWidth) / TReal(aScaled.iWidth);
    }

TRect TRectScaler::ScaleUp( const TRect& aScaledRect ) const
    {
    ASSERT(iFactor > 0);

    TPoint Tl = aScaledRect.iTl;
    Tl.iX *= iFactor;
    Tl.iY *= iFactor;

    TPoint Br = aScaledRect.iBr;
    Br.iX *= iFactor;
    Br.iY *= iFactor;

    return TRect( Tl, Br);
    }

TRect TRectScaler::ScaleDown( const TRect& aOriginalRect ) const
    {
    ASSERT(iFactor > 0);

    TPoint Tl = aOriginalRect.iTl;
    Tl.iX /= iFactor;
    Tl.iY /= iFactor;

    TPoint Br = aOriginalRect.iBr;
    Br.iX /= iFactor;
    Br.iY /= iFactor;

    return TRect( Tl, Br);
    }

