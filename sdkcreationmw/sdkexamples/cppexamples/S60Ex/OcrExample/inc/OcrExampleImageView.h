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


#ifndef OCRIMAGEVIEW_H
#define OCRIMAGEVIEW_H

// INCLUDES
#include <aknview.h>
#include "OcrExampleImageContainer.h" // TScreenUpdateData
#include <f32file.h>
#include <ocrsrv.h>
#include <AknWaitNoteWrapper.h> // MAknBackgroundProcess
#include "ImageHandler.h"

/**
 * COcrExampleImageView implements the main view of the application
 * COcrExampleImageView is an observer for following objects:
 * - ImageHandler: to receive notifications of asynchronous image loading and
 *   scaling operations, and
 * - OCREngine: to receive notifications of asynchronous image analyze and
 *   recognize operations
 * - waitnote
 */
class COcrExampleImageView : public CAknView
                           , public MAknBackgroundProcess
                           , MOCREngineObserver
                           , NOCRUtils::MImageHandlerCallback
    {
    public: // Constructors and destructor

        /**
        * Symbian two-phased constructor.
        * @param aSettings the application level settings
		* @return A pointer to the created instance of COcrExampleImageView
        */
        static COcrExampleImageView* NewL(TOcrExampleSettingsData& aSettings);

        /** Destructor. */
        virtual ~COcrExampleImageView();

        /** Handle screen size change.*/
        void HandleSizeChange();

    private: // Functions from base classes

        /**
        * From CAknView returns Uid of View
        * @return TUid uid of the view
        */
        TUid Id() const;

        /**
        * From CAknView, handle commands
        * @param aCommand command to be handled
        */
        void HandleCommandL( TInt aCommand );

        /**
        * From CAknView Activate this view
        * @param aPrevViewId
        * @param aCustomMessageId
        * @param aCustomMessage
        */
        void DoActivateL( const TVwsViewId& aPrevViewId,
                          TUid aCustomMessageId,
                          const TDesC8& aCustomMessage );
        /**
        * From CAknView Deactivate this view
        */
        void DoDeactivate();

        /**
        * From MEikMenuObserver Dynamically customize menu items
        * @param aResourceId Menu pane resource ID
        * @param aMenuPane Menu pane pointer
        */
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

        /**
        * From MImageHandlerCallback.
        * Called when image loading/scaling has completed
        */
        void ImageOperationCompleteL(TInt aError);

        /**
        * From MOCREngineObserver.
        * Called when asynchronous recognize block operation completes.
        * Internally calls RecognizeDoneL.
        * @param aError  Error code to indicate the completion status
        * @param aBlocks  Pointer to the text information of the recognized
        *   image block. the memory ownership belongs to the OCR engine and
        *   will not be passed to your procedure after this calling
        * @param aBlockCount  Number of blocks identified
        */
        void RecognizeBlockComplete(TInt aError,
                                    const TOCRTextRgnInfo* aBlocks,
                                    const TInt aBlockCount);
        /**
        * From MOCREngineObserver.
        * Called when asynchronous recognize special region operation completes.
        * Internally calls RecognizeDoneL.
        * @param aError  Error code to indicate the completion status
        * @param aBlocks  Pointer to the text information of the recognized
        *   image block. the memory ownership belongs to the OCR engine and
        *   will not be passed to your procedure after this calling
        * @param aBlockCount  Number of blocks identified
        */
        void RecognizeSpecialRegionComplete(TInt aError,
                                            const TOCRTextRgnInfo* aBlocks,
                                            const TInt aBlockCount);
        /**
        * From MOCREngineObserver.
        * Called when asynchronous recognize operation (of layout analysed
        * regions) completes.
        * Internally calls RecognizeDoneL.
        * @param aError  Error code to indicate the completion status
        * @param aBlocks  Pointer to the text information of the recognized
        *   image block. the memory ownership belongs to the OCR engine and
        *   will not be passed to your procedure after this calling
        * @param aBlockCount  Number of blocks identified
        */
        void RecognizeComplete(TInt aError,
                               const TOCRTextRgnInfo* aBlocks,
                               const TInt aBlockCount);

        /** From MOCREngineObserver
        * Called when asynchronous Layout Analysis completes.
        * @param aError  Error code to indicate the completion status
        * @param aBlocks  Pointer to the text information of the recognized
        *   image block. the memory ownership belongs to the OCR engine and
        *   will not be passed to your procedure after this calling
        * @param aBlockCount  Number of blocks identified
        */
        void LayoutComplete(TInt aError,
                            const TOCRBlockInfo* aBlocks,
                            const TInt aBlockCount);
        /**
        * From MOCREngineObserver.
        * Progress reporting interface of OCR operations.
        */
        void RecognizeProcess(const TUint aPercent);

        /** from MAknBackgroundProcess (used by waitnote) */
    	void DialogDismissedL(TInt /*aButtonId*/);

        /** from MAknBackgroundProcess (used by waitnote) */
    	TBool IsProcessDone() const;

        /** from MAknBackgroundProcess (used by waitnote) */
    	void ProcessFinished();

        /** from MAknBackgroundProcess (used by waitnote) */
    	void StepL();

    private:
        /** C++ constructor. */
        COcrExampleImageView(TOcrExampleSettingsData& aSettings);

        /** Symbian 2nd phase constructor. */
        void ConstructL();

        /**
        * AnalyzeImageLayoutL triggers layout analysis operation.
        * OCR Engine is first initialized.
        * Operation settings are then inquiried from the user and the
        * asynchronous operation is started.
        */
        void AnalyzeImageLayoutL();

        /**
        * RecognizeImageL triggers recognize operation.
        * Recognition can be done after successful layout analysis. All blocks
        * are to be recognized.
        * asynchronous operation is started.
        */
        void RecognizeImageL();

        /**
        * RecognizeBlockL triggers recognize block -operation.
        * OCR Engine is first initialized.
        * Operation settings are then inquiried from the user for the block
        * marked by the crosshair. Asynchronous operation is started.
        */
        void RecognizeBlockL();

        /**
        * RecognizeBlockL triggers recognize special region -operation.
        * OCR Engine is first initialized.
        * Operation settings are then inquiried from the user for the block
        * marked by the crosshair. Asynchronous operation is started.
        */
        void RecognizeSpecialRegionL();

        /**
        * LayoutCompleteL writes layout analysis results to the datafile.
        * Then the text-view is activated to show the results.
        * @param aError  Error code to indicate the completion status
        * @param aBlocks  Pointer to the text information of the recognized
        *   image block. the memory ownership belongs to the OCR engine and
        *   will not be passed to your procedure after this calling
        * @param aBlockCount  Number of blocks identified
        */
        void LayoutCompleteL(TInt aError,
                            const TOCRBlockInfo* aBlocks,
                            const TInt aBlockCount);

        /**
        * RecognizeDone writes recognition results to the datafile.
        * Then the text-view is activated to show the results.
        * @param aError  Error code to indicate the completion status
        * @param aBlocks  Pointer to the text information of the recognized
        *   image block. the memory ownership belongs to the OCR engine and
        *   will not be passed to your procedure after this calling
        * @param aBlockCount  Number of blocks identified
        */
        void RecognizeDoneL(TInt aError,
                            const TOCRTextRgnInfo* aBlocks,
                            const TInt aBlockCount);

        /** initializes OCR Engine and sets languages according to settings */
        void InitializeOCREngineL( OCREngineFactory::TEngineType aEngineType );

        /** releases OCR Engine */
        void ReleaseOCREngine();

        /**
        * Loads selected image file and scales it to the screen.
        * File selection dialog is first executed and then user-selected image
        * is loaded and fitted to the screen.
        */
        void OpenImageL();

        /**
        * closes opened image.
        * As a side-effect OCR Engine is also released.
        */
        void CloseImage();

        /** help function to display warning note */
        void DisplayWarningL( const TDesC &aDescr, TInt aErr) const;

        /**
        * help function to execute waitnote
        * @return ETrue, if operation completed; EFalse, if cancelled.
        */
        TBool ExecuteWaitNoteL();

    private: // Data
        /** image container */
        COcrExampleImageContainer   *iContainer;

        /**
        * data needed by container to draw the screen. owned and controlled
        * by this class; container just refers to this
        */
        TScreenUpdateData           iData;

        /** file server session */
        RFs                         iFs;

        /** pointer to bitmap storing the original image */
        CFbsBitmap                  *iBitMap;

        /** pointer to utility object for loading and scaling the image */
        NOCRUtils::CImageHandler    *iHandler;

        /** pointer to OCR Engine */
        MOCREngineInterface         *iEngine;

        /** application level settings */
        TOcrExampleSettingsData     &iSettings;

       /**
        * enumeration values used for indicating in which state the application
        * currently is.
        */
        enum TState
            {
            EInit,              ///< appl started; image not loaded
            EImageOpened,       ///< image loaded
            EImageAnalyzed,     ///< layout analysis done
            EImageRecognized    ///< recognition done
            };

        /** state of the view */
        TState                      iState;

        /** indicates whether crosshair is active or not */
        TBool                       iCrosshairEnabled;

        /** flag indicating background process completion. used by waitnote. */
        TBool                       iBackgroundTaskDone;
    };

#endif //OCRIMAGEVIEW_H

// End of File
