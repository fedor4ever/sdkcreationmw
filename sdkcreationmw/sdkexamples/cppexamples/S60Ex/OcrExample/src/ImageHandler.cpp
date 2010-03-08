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


#include "ImageHandler.h"

using namespace NOCRUtils;

// =============================================================================
// CImageHandler, a utility class for loading images.
// =============================================================================

// ============================= MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CImageHandler::CImageHandler
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CImageHandler::CImageHandler( CFbsBitmap* aBitmap,
                              CFbsBitmap* aScaledBitmap,
                              RFs& aFs,
                              MImageHandlerCallback& aCallback )
    : CActive(CActive::EPriorityStandard)
    , iDecoder(NULL)
    , iScaler(NULL)
    , iCallback(aCallback)
    , iBitmap(aBitmap)
    , iScaledBitmap(aScaledBitmap)
    , iFs(aFs)
    , iSize(0,0)
    {
    }

// -----------------------------------------------------------------------------
// CImageHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CImageHandler::ConstructL()
    {
    CActiveScheduler::Add(this);
    }

// -----------------------------------------------------------------------------
// CImageHandler::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CImageHandler* CImageHandler::NewL(CFbsBitmap* aBitmap,
                                   CFbsBitmap* aScaledBitmap,
                                   RFs& aFs,
                                   MImageHandlerCallback& aCallback)
    {
    CImageHandler* self = NewLC(aBitmap, aScaledBitmap, aFs, aCallback);
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CImageHandler::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CImageHandler* CImageHandler::NewLC(CFbsBitmap* aBitmap,
                                    CFbsBitmap *aScaledBitmap,
                                    RFs& aFs,
                                    MImageHandlerCallback& aCallback)
    {
    CImageHandler* self = new (ELeave) CImageHandler(aBitmap,
                                                     aScaledBitmap,
                                                     aFs,
                                                     aCallback);
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

// Destructor
CImageHandler::~CImageHandler()
    {
    delete iDecoder;
    delete iScaler;
    }

// -----------------------------------------------------------------------------
// CImageHandler::SetBitmapL
// Set the destination bitmap.
// -----------------------------------------------------------------------------
//
TBool CImageHandler::SetBitmapL( CFbsBitmap *aBitmap )
    {
    if ( IsActive() )
        {
        return EFalse; // If active, fail.
        }
    // else
    iBitmap = aBitmap;
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CImageHandler::LoadFileL
// Loads a selected frame from a named file
// -----------------------------------------------------------------------------
//
void CImageHandler::LoadFileL(const TFileName& aFileName, TInt aSelectedFrame)
    {
    __ASSERT_ALWAYS(!IsActive(),User::Invariant());
    if ( iDecoder )
        {
        delete iDecoder;
        }
    iDecoder = NULL;
    iDecoder = CImageDecoder::FileNewL(iFs, aFileName);
    // Get image information
    iFrameInfo = iDecoder->FrameInfo(aSelectedFrame);
    // Resize to fit.
    TRect bitmap = iFrameInfo.iFrameCoordsInPixels;

    // release possible previous image
    iBitmap->Reset();
    iBitmap->Create(bitmap.Size(), EColor16M);

    // Decode as bitmap.
    iDecoder->Convert(&iStatus, *iBitmap, aSelectedFrame);
    SetActive();
    }

void CImageHandler::LoadFileAndScaleL(const TFileName& aFileName,
                                      const TSize &aSize,
                                      TInt aSelectedFrame)
    {
    __ASSERT_ALWAYS(!IsActive(),User::Invariant());
    iSize = aSize;
    iScaledBitmap->Reset();
    iScaledBitmap->Create(aSize, EColor16M);
    LoadFileL(aFileName, aSelectedFrame);
    }

void CImageHandler::ScaleL()
    {
    __ASSERT_ALWAYS(!IsActive(),User::Invariant());
    if ( iScaler )
        {
        delete iScaler;
        }
    iScaler = NULL;
    iScaler = CBitmapScaler::NewL();
    iScaler->Scale(&iStatus, *iBitmap, *iScaledBitmap, ETrue);
    SetActive();
    }

void CImageHandler::FitToScreenL(const TSize &aSize)
    {
    if (! iBitmap->Handle())
        {
        return;
        }

    iScaledBitmap->Reset();
    iScaledBitmap->Create(aSize, EColor16M);
    ScaleL();
    }

// -----------------------------------------------------------------------------
// CImageHandler::FrameInfo
// Get the current frame information.
// -----------------------------------------------------------------------------
//
const TFrameInfo& CImageHandler::FrameInfo() const
    {
    return iFrameInfo;
    }

// -----------------------------------------------------------------------------
// CImageHandler::RunL
// CActive::RunL() implementation. Called on image load success/failure.
// -----------------------------------------------------------------------------
//
void CImageHandler::RunL()
    {
    if ((! iStatus.Int()) && (iSize.iWidth != 0) && (iSize.iHeight != 0))
        {
        ScaleL();
        }
    else
        {
        // Invoke callback.
        iCallback.ImageOperationCompleteL(iStatus.Int());
        }
    iSize.SetSize(0,0);
    }

// -----------------------------------------------------------------------------
// CImageHandler::DoCancel
// CActive::Cancel() implementation. Stops decoding.
// -----------------------------------------------------------------------------
//
void CImageHandler::DoCancel()
    {
    if ( iDecoder )
        {
        iDecoder->Cancel();
        }
    if ( iScaler )
        {
        iDecoder->Cancel();
        }
    }



