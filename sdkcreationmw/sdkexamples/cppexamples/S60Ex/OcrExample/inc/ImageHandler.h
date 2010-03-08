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


#ifndef OCR_IMAGEHANDLER_H
#define OCR_IMAGEHANDLER_H

#include <f32file.h>
#include <ImageConversion.h>
#include <BitmapTransforms.h>

/**
 * namespace contains various utility classes
 */
namespace NOCRUtils
{

/**
 * Listener interface that can be used to listen for image loading operation
 * completion events from CImageHandler.
 *
 * The class is intended to be implemented by a client class that observes the
 * loading operation of image handler. The methods in this class
 * are called by the image handler (class CImageHandler) when it loads
 * an image.
 *
 * Reference to implementations of this listener interface can be passed as
 * a parameter to the constructor of the image handler (class CImageHandler).
 */
class MImageHandlerCallback
    {
public:
   /**
    * Called by CImageHandler when an image has been loaded.
    * @param aError Error code given by the CImageHandler or 0 (zero) if the
    *   image was loaded successfully.
    */
    virtual void ImageOperationCompleteL(TInt aError) = 0;
    };

// ============================================================================

/**
*  CImageHandler
*  Image loader and scaler class.
*/
class CImageHandler : public CActive
    {
    public: // Constructors and destructor
       /**
        * Factory method that constructs a CImageHandler by using the NewLC method
        * and then cleans the cleanup stack.
        * @param aBitmap Bitmap where the image data is loaded to.
        * @param aScaledBitmap Bitmap where the scaled image data is loaded to.
        * @param aFs File server reference that is used to load the image data.
        * @param aCallback Listener interface implementation that is notified
        *  when an image has been loaded.
        * @return pointer to created CImageHandler-object
        */
        static CImageHandler* NewL(CFbsBitmap* aBitmap,
                                   CFbsBitmap* aScaledBitmap,
                                   RFs& aFs,
                                   MImageHandlerCallback& aCallback);

       /**
        * Factory method that constructs a CImageHandler and leaves it to the
        * cleanup stack.
        * @param aBitmap Bitmap where the image data is loaded to.
        * @param aScaledBitmap Bitmap where the scaled image data is loaded to.
        * @param aFs File server reference that is used to load the image data.
        * @param aCallback Listener interface implementation that is notified
        *  when an image has been loaded.
        * @return pointer to created CImageHandler-object
        */
        static CImageHandler* NewLC(CFbsBitmap* aBitmap,
                                    CFbsBitmap *aScaledBitmap,
                                    RFs& aFs,
                                    MImageHandlerCallback& aCallback);
       /**
        * Desctructor. Destroys the CImageDecoder used by the image handler.
        */
        virtual ~CImageHandler();

    public: // New functions
        /**
         * Sets the target bitmap where the bitmap data is loaded to.
         * @param aBitmap Bitmap where the image data is loaded to.
         * @return True if operation is successfull, false if not.
         */
        TBool SetBitmapL( CFbsBitmap *aBitmap);

        /**
         * Loads a the given frame from the given file and scale it to the
         * specified size
         * @param aFileName Filename wherefrom the bitmap data is loaded.
         * @param aSize the target size for the scaling
         * @param aSelectedFrame A single frame index in a multi-frame file.
         *  If not given the first frame is loaded.
         */
        void LoadFileAndScaleL(const TFileName& aFileName,
                               const TSize &aSize,
                               TInt aSelectedFrame = 0);

        /**
         * Returns the current frame information.
         * @return Current frame information.
         */
        const TFrameInfo& FrameInfo() const;

        /**
         * Re-scales the loaded image to the specified size
         * @param aSize the target size for the scaling
         */
        void FitToScreenL(const TSize &aSize);

    private:
        /**
         * Loads a the given frame from the given file.
         * @param aFileName Filename wherefrom the bitmap data is loaded.
         * @param aSelectedFrame A single frame index in a multi-frame file. If not given the first frame is loaded.
         */
        void LoadFileL(const TFileName& aFileName, TInt aSelectedFrame = 0);

       /**
        * Scales a loaded image
        * Scales a loaded image to the target size (given in LoadFileAndScaleL or
        * FitToScreenL). The scaled image is stored into iScaledBitmap.
        */
        void ScaleL();

    private: // Functions from base classes

        /**
         * CActive::RunL() implementation. Called on image load success/failure.
         */
        void RunL();
        /**
         * CActive::Cancel() implementation. Stops decoding.
         */
        void DoCancel();

    protected:
        /**
         * C++ default constructor. Just stores the given parameters to
         * corresponding attributes.
         * @param aBitmap Bitmap where the image data is loaded to.
         * @param aScaledBitmap Bitmap where the scaled image data is loaded to.
         * @param aFs File server reference that is used to load the image data.
         * @param aCallback Listener interface implementation that is notified
         *  when an image has been loaded.
         */
        CImageHandler(CFbsBitmap* aBitmap,
                      CFbsBitmap *aScaledBitmap,
                      RFs& aFs,
                      MImageHandlerCallback& aCallback);
        /**
         * 2nd phase constructor. Adds this object to the active scheduler.
         */
        void ConstructL();

    private: // Data
        /** Image decoder that is used to load the image data from file. */
        CImageDecoder           *iDecoder;

        /** Image scaler that is used to scale the image */
        CBitmapScaler           *iScaler;

        /** Listener that is notified when an image has been loaded. */
        MImageHandlerCallback   &iCallback;

        /** Bitmap (owned by the user of this class) where the loaded image is put */
        CFbsBitmap              *iBitmap;

        /** Bitmap (owned by the user of this class) where the SCALED image is put */
        CFbsBitmap              *iScaledBitmap;

        /** File server session (owned by the user of this class) */
        RFs                     &iFs;

        /** Current image frame information. */
        TFrameInfo              iFrameInfo;

        /** target size for scaled image */
        TSize                   iSize;
    };

} // namespace
#endif

