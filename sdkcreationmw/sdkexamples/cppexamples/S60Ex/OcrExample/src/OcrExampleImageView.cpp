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


// INCLUDE FILES
#include <aknutils.h>
#include <f32file.h>
#include <s32file.h>
#include <aknnotewrappers.h>
#include <akncommondialogs.h>

#include "OcrExampleAppUi.h"
#include <OcrExample.rsg>
#include "OcrExample.hrh"
#include "OcrExampleUids.h"
#include "OcrExampleImageContainer.h"
#include "OcrExampleSettingsData.h"
#include "OcrExampleImageView.h"
#include "LayoutForm.h"

// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// COcrExampleImageView::COcrExampleImageView
// C++ constructor
// ---------------------------------------------------------------------------
//
COcrExampleImageView::COcrExampleImageView(TOcrExampleSettingsData& aSettings)
    : iContainer(0)
    , iBitMap(0)
    , iHandler(0)
    , iEngine(0)
    , iSettings(aSettings)
    , iState(EInit)
    , iCrosshairEnabled(EFalse)
    , iBackgroundTaskDone(EFalse)
    {
    }


// ---------------------------------------------------------------------------
// COcrExampleImageView.::NewLC
// Symbian OS two-phased constructor.
// ---------------------------------------------------------------------------
//
COcrExampleImageView* COcrExampleImageView::NewL(TOcrExampleSettingsData& aSettings)
    {
    COcrExampleImageView* self = new (ELeave) COcrExampleImageView(aSettings);
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// COcrExampleImageView::ConstructL
// Symbian OS 2nd phase constructor
// ---------------------------------------------------------------------------
//
void COcrExampleImageView::ConstructL()
    {
    BaseConstructL(R_OCR_EXAMPLE_IMAGE_VIEW);
    iContainer = COcrExampleImageContainer::NewL(ClientRect(), iSettings, iData);
    iContainer->SetMopParent( this );

    AppUi()->AddToStackL( *this, iContainer );

    // create a new log-file
	User::LeaveIfError(iFs.Connect());
    RFile file;
	User::LeaveIfError(file.Replace(iFs, KLogFile, EFileWrite|EFileShareAny ));
	file.Close();

    // bitmap for the original image
    iBitMap = new (ELeave) CFbsBitmap;

    // bitmap for the image fitted to the screen
    iData.iScaledBitMap = new (ELeave) CFbsBitmap;

    // create handler to deal with asynchronous image loading and scaling
    iHandler = NOCRUtils::CImageHandler::NewL( iBitMap,
                                               iData.iScaledBitMap,
                                               iFs,
                                               *this );
    iContainer->ActivateL();
    }


// Destructor
COcrExampleImageView::~COcrExampleImageView()
    {
    if (iContainer)
        {
        AppUi()->RemoveFromStack(iContainer);
        delete iContainer;
        iContainer = NULL;
        }
    if (iEngine)
        {
        OCREngineFactory::ReleaseOCREngine(iEngine);
        }
    delete iHandler;
    delete iBitMap;
    delete iData.iScaledBitMap;
    delete iData.iCrosshair;
    iFs.Close();
    }


// ---------------------------------------------------------------------------
// COcrExampleImageView::Id
// From CAknView, returns Uid of View
// ---------------------------------------------------------------------------
//
TUid COcrExampleImageView::Id() const
    {
    return KUidImageView;
    }


// ---------------------------------------------------------------------------
// COcrExampleImageView::HandleCommandL
// From MEikMenuObserver delegate commands from the menu
// ---------------------------------------------------------------------------
//
void COcrExampleImageView::HandleCommandL(TInt aCommand)
    {
    switch (aCommand)
        {
        case EOcrExampleCmdOpenFile:
            OpenImageL();
        	break;

        case EOcrExampleCmdAnalyzeImage:
            AnalyzeImageLayoutL();
            break;

        case EOcrExampleCmdRecognizeImage:
            RecognizeImageL();
            break;

        case EOcrExampleCmdRecognizeBlock:
            RecognizeBlockL();
            break;

        case EOcrExampleCmdRecognizeRegion:
            RecognizeSpecialRegionL();
            break;

        case EOcrExampleCmdToggleCrosshair:
            if (iData.iCrosshair)
                {
                delete iData.iCrosshair;
                iData.iCrosshair = 0;
                }
            else
                {
                // note! not leaving new. if it fails, it fails.
                iData.iCrosshair = new TRect;
                }
            iCrosshairEnabled = (iData.iCrosshair != NULL);
            iContainer->DrawNow();
            break;

        case EOcrExampleCmdSettings:
            AppUi()->ActivateLocalViewL(KUidSettingsView);
            break;

        default:
            AppUi()->HandleCommandL( aCommand );
            break;
        }
    }


// ---------------------------------------------------------------------------
// COcrExampleImageView::DoActivateL
// Activate this view
// ---------------------------------------------------------------------------
//
void COcrExampleImageView::DoActivateL( const TVwsViewId& /* aPrevViewId */,
                                        TUid /*aCustomMessageId*/,
                                        const TDesC8& /*aCustomMessage*/ )
    {
    ASSERT(iContainer);
    iContainer->MakeVisible(ETrue);
    }


// ---------------------------------------------------------------------------
// COcrExampleImageView::DoDeactivate
// Deactivate this view
// Note! The container is not deleted, because we want to maintain existing
// view until re-activated.
// ---------------------------------------------------------------------------
//
void COcrExampleImageView::DoDeactivate()
    {
    if (iContainer)
        {
        iContainer->MakeVisible(EFalse);
        }
    }


// ---------------------------------------------------------------------------
// COcrExampleImageView::DynInitMenuPaneL
// Dynamically customize menu items
// ---------------------------------------------------------------------------
//
void COcrExampleImageView::DynInitMenuPaneL( TInt aResourceId,
                                             CEikMenuPane* aMenuPane )
    {
    if ( aResourceId == R_IMAGE_VIEW_MENU )
        {
        aMenuPane->SetItemDimmed( EOcrExampleCmdAnalyzeImage,
                                  ((iState == EInit) || iCrosshairEnabled));
        aMenuPane->SetItemDimmed( EOcrExampleCmdRecognizeImage,
                                  ((iState != EImageAnalyzed) || iCrosshairEnabled));
        aMenuPane->SetItemDimmed( EOcrExampleCmdToggleCrosshair,
                                  (iState == EInit));
        aMenuPane->SetItemDimmed( EOcrExampleCmdRecognizeBlockMenu,
                                  ((iState == EInit) || (!iCrosshairEnabled)));
        }
    }


// -----------------------------------------------------------------------------
// COcrExampleImageView::HandleSizeChange
// Handle screen size change.
// -----------------------------------------------------------------------------
//
void COcrExampleImageView::HandleSizeChange()
    {
    if (iContainer)
        {
        TRect mainPaneRect;
        AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, mainPaneRect);

        // if image is loaded, re-scale it
        if ((iHandler) && (iBitMap))
            {
            TRAP_IGNORE(iHandler->FitToScreenL( mainPaneRect.Size() ));
            }

        iContainer->SetRect(mainPaneRect);
        }
    }

// ----------------------------------------------------------------------------
// COcrExampleImageView::ImageOperationCompleteL()
// callback function called by ImageHandler, when an image has been loaded or
// scaled
// ----------------------------------------------------------------------------
//
void COcrExampleImageView::ImageOperationCompleteL(TInt aError)
    {
    iBackgroundTaskDone = ETrue; // to dismiss wait-note
    if (aError)
        {
        _LIT(KFailed,"Loading/scaling image failed");
        DisplayWarningL(KFailed, aError);
        }
    else
        {
        // we should now have the image and also another one (resized to fit
        // the screen). Calculate the factors (needed later, when adjusting
        // TRect-info analyzed from the original image)
        //
        iData.iRectScaler.CalculateFactor(iBitMap->SizeInPixels(),
                                          iData.iScaledBitMap->SizeInPixels());
        if (iState == EInit)
            {
            iState = EImageOpened;
            }
        iContainer->DrawDeferred();
        }
    }

void COcrExampleImageView::DialogDismissedL(TInt /*aButtonId*/)
    {
    // no implementation needed
    }

TBool COcrExampleImageView::IsProcessDone() const
    {
    return iBackgroundTaskDone;
    }

void COcrExampleImageView::ProcessFinished()
    {
    iBackgroundTaskDone = EFalse;
    }

void COcrExampleImageView::StepL()
    {
    // no implementation needed
    }

TBool COcrExampleImageView::ExecuteWaitNoteL()
    {
	CAknWaitNoteWrapper* waitNote = CAknWaitNoteWrapper::NewL();

	// Required reinterpret_cast as CAknWaitNoteWrapper inherits privately
	// from CActive
	CleanupStack::PushL(reinterpret_cast<CBase*>(waitNote));

	// this is a blocking call
	TBool completed = waitNote->ExecuteL(R_OCR_WAITNOTE, *this);

	CleanupStack::PopAndDestroy(waitNote);

	return completed;
    }

// -----------------------------------------------------------------------------
// COcrExampleImageView::DisplayWarningL()
// -----------------------------------------------------------------------------
//
void COcrExampleImageView::DisplayWarningL( const TDesC &aDescr, TInt aErr) const
    {
    ASSERT( aErr != KErrNone );

    _LIT(KFailed,"%S: %d");
    CAknWarningNote* note = new (ELeave) CAknWarningNote(ETrue);
    TBuf<128> text;
    text.Format(KFailed, &aDescr, aErr);
    note->ExecuteLD( text );
    }

// -----------------------------------------------------------------------------
// COcrExampleImageView::OpenImageL()
// -----------------------------------------------------------------------------
//
void COcrExampleImageView::OpenImageL()
    {
    // use dialog
    _LIT(KTitle, "Select image file");
    TFileName file(KNullDesC);
    if (AknCommonDialogs::RunSelectDlgLD(file, R_OCR_SELECT_DIALOG, KTitle))
        {
        // close possible previously loaded image
        CloseImage();

        TRect mainPaneRect;
        AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, mainPaneRect);
        TRAPD(err, iHandler->LoadFileAndScaleL( file, mainPaneRect.Size() ));
        if (err)
            {
            _LIT(KFileLoadFailed,"Opening image file failed");
            DisplayWarningL(KFileLoadFailed, err);
            }
        else
            {
            // this is a blocking call
            if (! ExecuteWaitNoteL())
                {
                // cancelled
                iHandler->Cancel();
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// COcrExampleImageView::CloseImage()
// -----------------------------------------------------------------------------
//
void COcrExampleImageView::CloseImage()
    {
    iBitMap->Reset();
    iData.iScaledBitMap->Reset();
    iState = EInit;
    ReleaseOCREngine();
    }

// -----------------------------------------------------------------------------
// COcrExampleImageView::ReleaseOCREngine()
// -----------------------------------------------------------------------------
//
void COcrExampleImageView::ReleaseOCREngine()
    {
    if (iEngine)
        {
        OCREngineFactory::ReleaseOCREngine(iEngine);
        iEngine = NULL;

        if (iData.iBlockInfo != NULL)
            {
            iData.iBlockInfo = NULL;
            iData.iBlockCount = 0;
            // content changed in UI
            iContainer->DrawDeferred();
            }
        }
    }

// -----------------------------------------------------------------------------
// COcrExampleImageView::InitializeOCREngineL()
// -----------------------------------------------------------------------------
//
void COcrExampleImageView::InitializeOCREngineL(
                            OCREngineFactory::TEngineType aEngineType )
    {
    TOcrEngineEnv env;
    env.iPriority = EPriorityLess;          // thread's priority
    env.iMaxHeapSize = 1200*KMinHeapGrowBy; // thread's heap maximum size

    // release create the OCR engine instance.
    ReleaseOCREngine();

    iEngine = OCREngineFactory::CreateOCREngineL(*this, env, aEngineType);

    // set active languages
    RArray<TLanguage> languages;
    CleanupClosePushL(languages);

    #ifdef _DEBUG
    _LIT(KInstalledLangs,"OCR: Installed languages:");
    _LIT(KLangId,"OCR: language id: %d");
    RDebug::Print(KInstalledLangs);
    iEngine->Base()->GetInstalledLanguage(languages);
    for (TInt i = 0; i < languages.Count(); ++i)
        {
        RDebug::Print(KLangId, languages[i]);
        }
    languages.Reset();
    #endif

    iSettings.GetLanguages(languages);

    #ifdef _DEBUG
    _LIT(KLangs,"OCR: languages to be set:");
    RDebug::Print(KLangs);
    for (TInt i = 0; i < languages.Count(); ++i)
        {
        RDebug::Print(KLangId, languages[i]);
        }
    #endif

    iEngine->Base()->SetActiveLanguageL(languages);

    CleanupStack::PopAndDestroy(&languages);
    }

// ----------------------------------------------------------------------------
// COcrExampleImageView::AnalyzeImageLayoutL()
//
// ----------------------------------------------------------------------------
//
void COcrExampleImageView::AnalyzeImageLayoutL()
    {
    ASSERT(iBitMap->Handle());

    TRAPD(err, InitializeOCREngineL( OCREngineFactory::EEngineLayoutRecognize ));
    if (err)
        {
        _LIT(KInitFailed,"Initializing OCR Engine failed");
        DisplayWarningL(KInitFailed, err);
        }
    else
        {
        // convert the instance from MOCREngineInterface point to
        // MOCREngineLayoutRecognize
        MOCREngineLayoutRecognize* engine =
            static_cast<MOCREngineLayoutRecognize*>(iEngine);

        // layout analysis
        TOCRLayoutSetting layoutSettings;
        layoutSettings.iBrightness = TOCRLayoutSetting::ENormal;
        layoutSettings.iSkew = ETrue;

        // create and execute form. Don't care about return value
        NOCRUtils::CLayoutForm::ExecuteFormLD( layoutSettings,
                                               R_ANALYZE_LAYOUT_FORM_DIALOG);

        #ifdef _DEBUG
        _LIT(KSettings,"OCR: settings skew: %d, brightness: %d");
        RDebug::Print(KSettings, layoutSettings.iSkew, layoutSettings.iBrightness);
        #endif

        // async operation. LayoutComplete() will be called, when ready
        TRAP(err, engine->LayoutAnalysisL(iBitMap->Handle(), layoutSettings));
        if (err)
            {
            _LIT(KLayoutFailed,"Starting analysis failed");
            DisplayWarningL(KLayoutFailed, err);
            }
        else
            {
             // this is a blocking call
            if (! ExecuteWaitNoteL())
                {
                // cancelled by user
                engine->Base()->CancelOperation();
                }
            }
        }
    }

// ----------------------------------------------------------------------------
// COcrExampleImageView::RecognizeImageL()
//
// ----------------------------------------------------------------------------
//
void COcrExampleImageView::RecognizeImageL()
    {
    ASSERT(iData.iBlockCount > 0);
    ASSERT(iData.iBlockInfo != NULL);
    ASSERT(iEngine != NULL);

    RArray<TInt> blockIndex;
    CleanupClosePushL(blockIndex);

    for (TInt i = 0; i < iData.iBlockCount; ++i)
        {
        blockIndex.Append(i);
        }

    TOCRRecognizeSetting dummy;
    dummy.iAny = NULL;

    // convert the instance from MOCREngineInterface point to
    // MOCREngineLayoutRecognize
    MOCREngineLayoutRecognize* engine = static_cast<MOCREngineLayoutRecognize*>(iEngine);

    // async operation. RecognizeComplete() will be called, when ready
    TRAPD(err, engine->RecognizeL(dummy, blockIndex));
    if (err)
        {
        _LIT(KFailed,"Starting recognition failed");
        DisplayWarningL(KFailed, err);
        }

    CleanupStack::PopAndDestroy(&blockIndex);
    }

// ----------------------------------------------------------------------------
// COcrExampleImageView::RecognizeBlockL()
//
// ----------------------------------------------------------------------------
//
void COcrExampleImageView::RecognizeBlockL()
    {
    ASSERT( iBitMap->Handle() );
    ASSERT( iData.iCrosshair != NULL );

    TRAPD(err, InitializeOCREngineL( OCREngineFactory::EEngineRecognizeBlock ));
    if (err)
        {
        _LIT(KInitFailed,"Initializing OCR Engine failed");
        DisplayWarningL(KInitFailed, err);
        }
    else
        {
        // convert the instance from MOCREngineInterface point to
        // MOCREngineLayoutRecognize
        MOCREngineRecognizeBlock* engine =
            static_cast<MOCREngineRecognizeBlock*>(iEngine);

        // fill struct with default values
        TOCRLayoutBlockInfo layoutInfo;
        layoutInfo.iLayout = EOcrLayoutTypeH;
        layoutInfo.iText = EOcrTextMultiLine;
        layoutInfo.iBackgroundColor = EOcrBackgroundLight;

        // calculate rect based on scaled crosshair
        layoutInfo.iRect = iData.iRectScaler.ScaleUp( *(iData.iCrosshair) );

        // create and execute form. Don't care about return value
        NOCRUtils::CLayoutForm::ExecuteFormLD( layoutInfo,
                                               R_RECOGNIZE_BLOCK_FORM_DIALOG);

        #ifdef _DEBUG
        _LIT(KSettings,"OCR: settings layout: %d, text: %d, brightness: %d");
        RDebug::Print(KSettings, layoutInfo.iLayout, layoutInfo.iText, layoutInfo.iBackgroundColor);
        #endif

        // async operation. LayoutComplete() will be called, when ready
        TRAP(err, engine->RecognizeBlockL(iBitMap->Handle(), layoutInfo));
        if (err)
            {
            _LIT(KFailed,"Starting recognize block failed");
            DisplayWarningL(KFailed, err);
            }
        else
            {
             // this is a blocking call
            if (! ExecuteWaitNoteL())
                {
                // cancelled by user
                engine->Base()->CancelOperation();
                }
            }
        }
    }

// ----------------------------------------------------------------------------
// COcrExampleImageView::RecognizeSpecialRegionL()
//
// ----------------------------------------------------------------------------
//
void COcrExampleImageView::RecognizeSpecialRegionL()
    {
    ASSERT( iBitMap->Handle() );
    ASSERT( iData.iCrosshair != NULL );

    TRAPD(err, InitializeOCREngineL( OCREngineFactory::EEngineRecognizeBlock ));
    if (err)
        {
        _LIT(KInitFailed,"Initializing OCR Engine failed");
        DisplayWarningL(KInitFailed, err);
        }
    else
        {
        // convert the instance from MOCREngineInterface point to
        // MOCREngineLayoutRecognize
        MOCREngineRecognizeBlock* engine =
            static_cast<MOCREngineRecognizeBlock*>(iEngine);

        // fill struct with default values
        TRegionInfo regionInfo;
        regionInfo.iBackgroundColor = EOcrBackgroundLight;
        regionInfo.iType = TRegionInfo::EEmailAddress;

        // calculate rect based on scaled crosshair
        regionInfo.iRect = iData.iRectScaler.ScaleUp( *(iData.iCrosshair) );

        // create and execute form. Don't care about return value
        NOCRUtils::CLayoutForm::ExecuteFormLD( regionInfo,
                                               R_RECOGNIZE_REGION_FORM_DIALOG);

        #ifdef _DEBUG
        _LIT(KSettings,"OCR: settings type: %d, brightness: %d");
        RDebug::Print(KSettings, regionInfo.iType, regionInfo.iBackgroundColor);
        #endif

        // async operation. LayoutComplete() will be called, when ready
        TRAP(err, engine->RecognizeSpecialRegionL(iBitMap->Handle(), regionInfo));
        if (err)
            {
            _LIT(KFailed,"Starting recognize special region failed");
            DisplayWarningL(KFailed, err);
            }
        else
            {
             // this is a blocking call
            if (! ExecuteWaitNoteL())
                {
                // cancelled by user
                engine->Base()->CancelOperation();
                }
            }
        }
    }

// ----------------------------------------------------------------------------
// COcrExampleImageView::LayoutComplete()
// callback function called by OCR Engine
// ----------------------------------------------------------------------------
//
void COcrExampleImageView::LayoutComplete( TInt aError,
                                           const TOCRBlockInfo* aBlocks,
                                           const TInt aBlockCount )
    {
    TRAP_IGNORE( LayoutCompleteL(aError, aBlocks, aBlockCount));
    }

// ----------------------------------------------------------------------------
// COcrExampleImageView::LayoutCompleteL()
//
// ----------------------------------------------------------------------------
//
void COcrExampleImageView::LayoutCompleteL( TInt aError,
                                            const TOCRBlockInfo* aBlocks,
                                            const TInt aBlockCount )
    {
    iBackgroundTaskDone = ETrue; // to dismiss wait-note

    if (aError)
        {
        _LIT(KFailed,"Layout analyzis of the image failed");
        DisplayWarningL(KFailed, aError);
        }
    else
        {
        iState = EImageAnalyzed;

        iData.iBlockInfo = aBlocks;
        iData.iBlockCount = aBlockCount;

        // content changed in UI
        iContainer->DrawDeferred();

        RFile       file;
        TFileText   fileText;

	    TInt err = file.Open(iFs, KLogFile, EFileWrite);
	    if (err == KErrNone)
	        {
    	    fileText.Set(file);

            // in a real life this should by dynamic
            TBuf<64> tmp;
            _LIT(KFormat1, "Layout analyzed, block count %d:");
            _LIT(KFormat2, " %d: layout %S");
            _LIT(KHor, "horizontal");
            _LIT(KVer, "vertical");
            tmp.Format(KFormat1, aBlockCount);

            fileText.Write(tmp);
            for (TInt i=0; i<aBlockCount; ++i)
                {
                tmp.Zero();
                tmp.Format(KFormat2, i+1,
                          ((aBlocks[i].iType == EOcrLayoutTypeH) ? &KHor : &KVer));
                fileText.Write(tmp);
                }
            file.Close();

            // switch to text-view to show the results
            AppUi()->ActivateLocalViewL(KUidTextView);
            }
        }
    }

// ----------------------------------------------------------------------------
// COcrExampleImageView::RecognizeDoneL()
// ----------------------------------------------------------------------------
//
void COcrExampleImageView::RecognizeDoneL( TInt aError,
                                           const TOCRTextRgnInfo* aBlocks,
                                           const TInt aBlockCount)
    {
    iBackgroundTaskDone = ETrue; // to dismiss wait-note

    if ((aError) && (aError != KErrCancel))
        {
        _LIT(KFailed,"Recognition of the image failed");
        DisplayWarningL(KFailed, aError);
        }
    else if (!aError)
        {
        iState = EImageRecognized;

        RFile       file;
        TFileText   fileText;

	    TInt err = file.Open(iFs, KLogFile, EFileWrite);
	    if (err == KErrNone)
	        {
    	    fileText.Set(file);

            // in a real life this should by dynamic
            TBuf<256> tmp;

            _LIT(KFormat2, "Block %d recognized, line count %d:");
            _LIT(KText, " %d: %S");
            for (TInt i = 0; i < aBlockCount; ++i)
                {
                tmp.Zero();
                TInt lines = aBlocks[i].iLineCount;
                tmp.Format(KFormat2, i+1, lines);
                fileText.Write(tmp);

                TOCRTextLineInfo *texts = aBlocks[i].iLines;
                for (TInt j = 0; j < lines; ++j)
                    {
                    tmp.Zero();
                    tmp.Format(KText, j+1, texts[j].iText);
                    fileText.Write(tmp);
                    }
                }
            file.Close();

            // switch to text-view to show the results
            AppUi()->ActivateLocalViewL(KUidTextView);
            }
        else
            {
            // file open failed, there's no switch to text-view.
            // we need to order draw...
            iContainer->DrawDeferred();
            }
        }
    }

// ----------------------------------------------------------------------------
// COcrExampleImageView::RecognizeProcess()
// callback function called by OCR Engine
// ----------------------------------------------------------------------------
//
void COcrExampleImageView::RecognizeProcess(const TUint aPercent)
    {
    _LIT(KFormat, "OCR: Progress: %d%%\n");
    RDebug::Print(KFormat, aPercent);
    }

// ----------------------------------------------------------------------------
// COcrExampleImageView::RecognizeComplete()
// callback function called by OCR Engine
// ----------------------------------------------------------------------------
//
void COcrExampleImageView::RecognizeComplete( TInt aError,
                                           const TOCRTextRgnInfo* aBlocks,
                                           const TInt aBlockCount)
    {
    TRAP_IGNORE( RecognizeDoneL( aError, aBlocks, aBlockCount ));
    }

// ----------------------------------------------------------------------------
// COcrExampleImageView::RecognizeBlockComplete()
// callback function called by OCR Engine
// ----------------------------------------------------------------------------
//
void COcrExampleImageView::RecognizeBlockComplete( TInt aError,
                                                const TOCRTextRgnInfo* aBlocks,
                                                const TInt aBlockCount)
    {
    TRAP_IGNORE( RecognizeDoneL( aError, aBlocks, aBlockCount ));
    }

// ----------------------------------------------------------------------------
// COcrExampleImageView::RecognizeSpecialRegionComplete()
// callback function called by OCR Engine
// ----------------------------------------------------------------------------
//
void COcrExampleImageView::RecognizeSpecialRegionComplete(TInt aError,
                                            const TOCRTextRgnInfo* aBlocks,
                                            const TInt aBlockCount)
    {
    TRAP_IGNORE( RecognizeDoneL( aError, aBlocks, aBlockCount ));
    }

// End of File
