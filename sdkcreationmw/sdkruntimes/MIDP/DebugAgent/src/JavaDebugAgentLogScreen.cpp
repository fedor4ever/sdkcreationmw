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


#include <eikon.hrh>
#include <coemain.h>
#include <eikenv.h>
#include <eikappui.h>
#include <eiksbfrm.h>
#include <aknsdrawutils.h>
#include <aknslayeredbackgroundcontrolcontext.h>
#include "JavaDebugAgent.hrh"
#include "JavaDebugAgentLogScreen.h"
#include "JavaDebugAgentBasicLogView.h"

const TInt KMaxLogLineLength = 512;
const TInt KMaxPrefixLength = 32;

const TInt KTextMarginTop = 3;
const TInt KTextMarginLeft = 3;
const TInt KTextMarginRight = 5;
const TInt KTextMarginBottom = 5;

#define SUPER CCoeControl
#define DEFAULT_FONT() CEikonEnv::Static()->DenseFont()

CJavaDebugAgentLogScreen* 
CJavaDebugAgentLogScreen::NewL(CJavaDebugAgentBasicLogView* aView,
                               const TRect& aRect)
{
    CJavaDebugAgentLogScreen* me = new(ELeave)CJavaDebugAgentLogScreen(aView);
    CleanupStack::PushL(me);
    me->ConstructL(aRect);
    CleanupStack::Pop(me);
    return me;
}

void CJavaDebugAgentLogScreen::ConstructL(const TRect& aRect)
{
    User::LeaveIfError(iFs.Connect());
    iFs.ShareProtected();

    CreateWindowL();
    SetRect(aRect);

    iSBFrame = new(ELeave)CEikScrollBarFrame(this, NULL, ETrue);
    iSBFrame->CreateDoubleSpanScrollBarsL(ETrue, EFalse, ETrue, EFalse);
    iSBFrame->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff,
        CEikScrollBarFrame::EOn);

    iViewRect = Rect();
    DoLayoutL();

    iCharFormatMask.SetAttrib(EAttColor);
    iCharFormatMask.SetAttrib(EAttFontHeight);
    const CFont* font = DEFAULT_FONT();
    if (font) {
        iCharFormatMask.SetAttrib(EAttFontTypeface);
        iCharFormat.iFontSpec = font->FontSpecInTwips();
    } else {
        iCharFormat.iFontSpec.iHeight = 90;
    }
    iDefaultFontHeightInTwips = iCharFormat.iFontSpec.iHeight;
    iCharFormat.iFontPresentation.iTextColor = TLogicalRgb(KRgbDarkBlue);
    iCharFormatLayer = CCharFormatLayer::NewL(iCharFormat, iCharFormatMask);

    iParaFormatMask.SetAttrib(EAttLineSpacing);
    iParaFormat.iLineSpacingInTwips = iCharFormat.iFontSpec.iHeight;
    iParaFormatLayer = CParaFormatLayer::NewL(&iParaFormat, iParaFormatMask);

    iGlobalText = CGlobalText::NewL(iParaFormatLayer, iCharFormatLayer);

    CBitmapDevice* bmp = CCoeEnv::Static()->ScreenDevice();
    iLayout = CTextLayout::NewL(iGlobalText, iTextRect.Width());
    iTextView = CTextView::NewL(iLayout, iTextRect, bmp, bmp, &Window(),0,
        &iCoeEnv->WsSession());

    ActivateL();
}

CJavaDebugAgentLogScreen::
CJavaDebugAgentLogScreen(CJavaDebugAgentBasicLogView* aView) :
iView(aView),
iFontSize(KDefaultDebugAgentFontSize)
{
}

CJavaDebugAgentLogScreen::~CJavaDebugAgentLogScreen()
{
    CloseLogFile();
    iFs.Close();
    iTextClipRegion.Close();
    delete iBgBitmap;
    delete iBgMask;
    delete iBgContext;
    delete iTextView;
    delete iLayout;
    delete iGlobalText;
    delete iCharFormatLayer;
    delete iParaFormatLayer;
    delete iSBFrame;
}

void CJavaDebugAgentLogScreen::CloseLogFile()
{
    if (iLogFileName)
    {
        delete iLogFileName;
        iLogFileName = NULL;
        iLogFile.Close();
    }
}

void CJavaDebugAgentLogScreen::SetFontSize(TInt aFontSize)
{
    TRAP_IGNORE(SetFontSizeL(aFontSize));
}

void CJavaDebugAgentLogScreen::SetFontSizeL(TInt aFontSize)
{
    if (UpdateFontSizeL(aFontSize))
    {
        iTextView->FormatTextL();
        UpdateScrollBars(ETrue);
        DrawNow(iTextRect);
    }
}

// Updates char and para format, returning True if it has actually changed
TBool CJavaDebugAgentLogScreen::UpdateFontSizeL(TInt aFontSize)
{
    iFontSize = aFontSize;
    TInt newHeight = (iDefaultFontHeightInTwips * iFontSize) / 100;
    if (iCharFormat.iFontSpec.iHeight != newHeight) {
        iCharFormat.iFontSpec.iHeight = newHeight;
        iCharFormatLayer->SetL(iCharFormat, iCharFormatMask);
        iParaFormat.iLineSpacingInTwips = iCharFormat.iFontSpec.iHeight;
        iParaFormatLayer->SetL(&iParaFormat, iParaFormatMask);
        return ETrue;
    }
    return EFalse;
}

void CJavaDebugAgentLogScreen::DoLayoutL()
{
    CEikAppUi* appui = iEikonEnv->EikAppUi();
    TRect appRect(appui->ApplicationRect());
    TRect appClientRect(appui->ClientRect());
    TRect clientRect(Rect());
    TRect screenRect(Position(),Size());

    TEikScrollBarModel m1;
    UpdateSBModel(&m1);
    TAknDoubleSpanScrollBarModel m2(m1);

    TRect inclusiveRect(Rect());
    iViewRect = inclusiveRect;
    TEikScrollBarFrameLayout layout;
    layout.iTilingMode = TEikScrollBarFrameLayout::EInclusiveRectConstant;
    layout.SetClientMargin(0);
    layout.SetInclusiveMargin(0);

    iSBFrame->TileL(NULL, &m2, iViewRect, inclusiveRect, layout);
    iTextRect = iViewRect;
    iTextRect.iTl.iX += KTextMarginTop; 
    iTextRect.iTl.iY += KTextMarginLeft;
    iTextRect.iBr.iX -= KTextMarginRight;
    iTextRect.iBr.iY -= KTextMarginBottom;

    iTextClipRegion.Clear();
    iTextClipRegion.AddRect(Rect());
    TRect subRect(iTextRect);
    subRect.iBr.iX++;
    subRect.iBr.iY++;
    iTextClipRegion.SubRect(subRect);

    // NOTE: gray-scale mask works as the alpha channel. We use that for
    // blending shadow with the background. Looks quite nice. The code is
    // written in such a way that even if alpha blending doesn't work, it
    // will still look OK.
    CFbsBitmap* m;
    CFbsBitmap* bg;
    TSize s(iViewRect.Size());

    m = CreateBackgroundBitmapLC(s,KRgbBlack,KRgbBlack,KRgbWhite,KRgbDarkGray);
    bg = CreateBackgroundBitmapLC(s,KRgbWhite,KRgbWhite,KRgbBlack,KRgbBlack);
    CleanupStack::Pop(bg);
    CleanupStack::Pop(m);

    // Replace the bitmaps
    delete iBgBitmap;
    delete iBgMask;
    iBgBitmap = bg;
    iBgMask = m;

    // Setup the background context
    if (iBgContext)
    {
        delete iBgContext;
        iBgContext = NULL;
    }

    if (screenRect == appClientRect)
    {
        iBgContext = CAknsBasicBackgroundControlContext::NewL(
            KAknsIIDQsnBgAreaMain, clientRect, ETrue);
    }
    else
    {
        TRect statusRect(appRect);
        TRect controlRect(appRect);
        statusRect.iBr.iY = appClientRect.iTl.iY;
        controlRect.iTl.iY = appClientRect.iBr.iY;
        CAknsLayeredBackgroundControlContext* layerContext = 
            CAknsLayeredBackgroundControlContext::NewL(
            KAknsIIDQsnBgAreaStatus, statusRect, ETrue, 3);
        layerContext->SetLayerImage(1,KAknsIIDQsnBgAreaMain);
        layerContext->SetLayerImage(2,KAknsIIDQsnBgAreaControl);
        layerContext->SetLayerRect(1,appClientRect);
        layerContext->SetLayerRect(2,controlRect);
        iBgContext = layerContext;
    }
}

void 
CJavaDebugAgentLogScreen::DrawBackground(
    CBitmapContext* aGc,
    const TRect& aRect,
    TRgb aBgColor,
    TRgb aFillColor,
    TRgb aBorderColor,
    TRgb aShadowColor)
{
    aGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
    aGc->SetBrushColor(aBgColor);
    aGc->Clear(aRect);

    TInt x1 = aRect.iTl.iX;
    TInt x2 = aRect.iBr.iX;
    TInt y1 = aRect.iTl.iY;
    TInt y2 = aRect.iBr.iY;

    TRect rect(x1+1,y1+1,x2-3,y2-3);
    aGc->SetPenColor(aBorderColor);
    aGc->SetPenStyle(CGraphicsContext::ESolidPen);
    aGc->SetBrushStyle(CGraphicsContext::ENullBrush);
    aGc->DrawRect(rect);

    rect.Shrink(1,1);
    aGc->SetPenColor(KRgbWhite); // White on both background and mask
    aGc->DrawRect(rect);

    aGc->SetPenColor(aShadowColor);
    aGc->DrawLine(TPoint(x1+3,y2-2),TPoint(x2-1,y2-2));
    aGc->DrawLine(TPoint(x1+2,y2-3),TPoint(x2-1,y2-3));
    aGc->DrawLine(TPoint(x2-3,y1+2),TPoint(x2-3,y2-3));
    aGc->DrawLine(TPoint(x2-2,y1+3),TPoint(x2-2,y2-3));

    rect.Shrink(1,1);
    aGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
    aGc->SetBrushColor(aFillColor);
    aGc->SetPenColor(aFillColor);
    aGc->DrawRect(rect);
}

CFbsBitmap* 
CJavaDebugAgentLogScreen::CreateBackgroundBitmapLC(
    const TSize& aSize,
    TRgb aBgColor,
    TRgb aFillColor,
    TRgb aBorderColor,
    TRgb aShadowColor)
{
    CFbsBitmap* bitmap = new(ELeave)CFbsBitmap;
    CleanupStack::PushL(bitmap);
    User::LeaveIfError(bitmap->Create(aSize, EGray256));
    CFbsBitmapDevice* bitmapDevice = CFbsBitmapDevice::NewL(bitmap);
    CleanupStack::PushL(bitmapDevice);
    CFbsBitGc* gc = NULL;
    User::LeaveIfError(bitmapDevice->CreateContext(gc));
    TRect rect(aSize);
    DrawBackground(gc,rect,aBgColor,aFillColor,aBorderColor,aShadowColor);
    delete gc;
    CleanupStack::PopAndDestroy(bitmapDevice);
    return bitmap;
}

void CJavaDebugAgentLogScreen::Draw(const TRect& /*aRect*/) const
{
    CWindowGc& gc = SystemGc();
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    if (skin && iBgContext)
    {
	    gc.SetClippingRegion(iTextClipRegion);
        AknsDrawUtils::Background(skin, iBgContext, gc, Rect());
        gc.CancelClippingRegion();
        TPoint p(0,0);
        TRect srcRect(iViewRect.Size());
        gc.BitBltMasked(TPoint(0,0),iBgBitmap, srcRect, iBgMask, EFalse);
    }
    else
    {
        gc.DrawBitmap(iViewRect.iTl, iBgBitmap);
    }

    TRAP_IGNORE(iTextView->DrawL(iTextRect, gc));
}

// Supplies background skin context for scroll bars.
TTypeUid::Ptr CJavaDebugAgentLogScreen::MopSupplyObject(TTypeUid aId)
{
    if (aId.iUid == MAknsControlContext::ETypeId && iBgContext)
    {
        return MAknsControlContext::SupplyMopObject(aId, iBgContext);
    }
    return SUPER::MopSupplyObject(aId);
}

TInt CJavaDebugAgentLogScreen::CountComponentControls() const
{
    return iSBFrame->CountComponentControls();
}

CCoeControl* CJavaDebugAgentLogScreen::ComponentControl(TInt aIndex) const
{
    return iSBFrame->ComponentControl(aIndex);
}

void CJavaDebugAgentLogScreen::ClearL()
{
    if (iLineCounter > 0) {
        iLineCounter = 0;
        iGlobalText->DeleteL(0,iGlobalText->DocumentLength());
        iTextView->FormatTextL();
        UpdateScrollBars(ETrue);
        DrawNow(iTextRect);
    }
}

void CJavaDebugAgentLogScreen::FormatPrefixL(TDes& aPrefix)
{
    _LIT(KDateString5,"%-B%:0%J%:1%T%:2%S%:3");
    _LIT(KDelim,"  ");
    TTime time;
    time.HomeTime();
    time.FormatL(aPrefix, KDateString5);
    aPrefix.Append(KDelim);
}

void CJavaDebugAgentLogScreen::LogL(const TDesC& aString)
{
    TBuf<KMaxPrefixLength> prefix;
    FormatPrefixL(prefix);
    LogToScreenL(prefix, aString);
    LogToFileL(prefix, aString);
}

void CJavaDebugAgentLogScreen::LogToFileL(const TDesC& aPrefix,
                                          const TDesC& aText)
{
    _LIT8(KCRLF8,"\r\n");
    if (iLogFileName)
    {
        TBuf8<KMaxLogLineLength> buf;
        buf.Copy(aPrefix);
        iLogFile.Write(buf);
        buf.Copy(aText);
        iLogFile.Write(buf);
        iLogFile.Write(KCRLF8);
    }
}

void CJavaDebugAgentLogScreen::LogToScreenL(const TDesC& aPrefix,
                                            const TDesC& aText)
{
    iLineCounter++;

    TInt pos = iGlobalText->LdDocumentLength();
    iGlobalText->InsertL(iGlobalText->LdDocumentLength(), aPrefix);
    iGlobalText->InsertL(iGlobalText->LdDocumentLength(), aText);
    iGlobalText->InsertL(iGlobalText->LdDocumentLength(), 
        CEditableText::EParagraphDelimiter);

    FormatAndScrollL();
    DrawNow(iTextRect);
}

void CJavaDebugAgentLogScreen::FormatAndScrollL()
{
    iTextView->FormatTextL();

    // Scroll the last line into the view
    TInt yPos = Rect().Height();
    iTextView->SetViewL(iGlobalText->LdDocumentLength(), yPos);
    UpdateScrollBars(ETrue);
}

void CJavaDebugAgentLogScreen::UpdateSBModel(TEikScrollBarModel* aModel) const
{
    if (iLayout)
    {
        TInt height = iViewRect.Height() - KTextMarginTop - KTextMarginBottom;
        aModel->iThumbPosition = iLayout->PixelsAboveBand();
        aModel->iScrollSpan =  iLayout->FormattedHeightInPixels();
        aModel->iThumbSpan = Min(aModel->iScrollSpan, height);
        if (aModel->iScrollSpan > aModel->iThumbSpan)
        {
            // We've got something non-trivial
            return;
        }
    }

    // This essentially disables the scrollbar
    aModel->iThumbPosition = 0;
    aModel->iScrollSpan = 0;
    aModel->iThumbSpan = 0;
}

void CJavaDebugAgentLogScreen::UpdateScrollBars(TBool aDrawNow)
{
    TEikScrollBarModel m1;
    UpdateSBModel(&m1);
    TAknDoubleSpanScrollBarModel m2(m1);
    iSBFrame->Tile(&m2);
    iSBFrame->MoveVertThumbTo(m1.iThumbPosition);
    if (aDrawNow) iSBFrame->DrawScrollBarsNow();
}

void CJavaDebugAgentLogScreen::SetLogFileL(const TDesC* aFileName)
{
    if ((!iLogFileName && aFileName) ||
        (iLogFileName && !aFileName) ||
        (iLogFileName && aFileName && iLogFileName->Compare(*aFileName)))
    {
        CloseLogFile();
        if (aFileName)
        {
            iLogFileName = aFileName->AllocL();
            const TDesC& fname = *iLogFileName;
            TInt err = iLogFile.Open(iFs,fname,EFileWrite|EFileShareAny);
            if (err != KErrNone)
            {
                iFs.MkDirAll(fname);
                err = iLogFile.Create(iFs,fname,EFileWrite|EFileShareAny);
            }

            if (err == KErrNone)
            {
                TInt sz;
                iLogFile.Size(sz);
                iLogFile.Seek(ESeekEnd,sz);
                LogFormatToScreen(_S("Log file %S"),&fname);
            }
            else
            {
                LogFormatToScreen(_S("Failed to open log file %S"),&fname);
                LogFormatToScreen(_S("Symbian error %d"),err);
                delete iLogFileName;
                iLogFileName = NULL;
            }
        }
    }
}

// Internal formatting function, only used by SetLogFileL
// Never logs to file, only to screen.
void CJavaDebugAgentLogScreen::LogFormatToScreen(const TText* aFormat, ...)
{
    VA_LIST va;
    VA_START(va, aFormat);
    TBuf<KMaxLogLineLength> buf;
    TPtrC format(aFormat);
    TBuf<KMaxPrefixLength> prefix;
    TRAPD(err, FormatPrefixL(prefix));
    buf.AppendFormatList(format, va, this);
    TRAP(err, LogToScreenL(prefix, buf));
}

void CJavaDebugAgentLogScreen::ClearLog()
{
    TRAP_IGNORE(ClearL());
}

void CJavaDebugAgentLogScreen::SetLogFile(const TDesC* aFileName)
{
    TRAP_IGNORE(SetLogFileL(aFileName));
}

void CJavaDebugAgentLogScreen::SizeChangedL()
{  
    const CFont* font = DEFAULT_FONT();
    if (font) {
        iDefaultFontHeightInTwips = font->FontSpecInTwips().iHeight;
        UpdateFontSizeL(iFontSize);
    }
    DoLayoutL();
    iLayout->SetWrapWidth(iTextRect.Width());
    iTextView->SetViewRect(iTextRect);
    FormatAndScrollL();
    DrawNow();
}

void CJavaDebugAgentLogScreen::SizeChanged()
{
    // This method is invoked during ConstructL when iTextView is still NULL
    if (iTextView) {
        TRAP_IGNORE(SizeChangedL());
    }
}

TKeyResponse CJavaDebugAgentLogScreen::OfferKeyEventL(
    const TKeyEvent& aKeyEvent, TEventCode aType)
{
    if (aType != EEventKey) return EKeyWasNotConsumed;
    TInt code=aKeyEvent.iCode;
    TCursorPosition::TMovementType movement;
    switch (code)
    {
    case EKeyOK:
    case EKeyEnter:
        iView->TryDisplayMenuBarL();
        return EKeyWasConsumed;

    // The rest has to do with scrolling
    case EKeyDownArrow:
        movement = TCursorPosition::EFLineDown;
        break;
    case EKeyUpArrow:
        movement = TCursorPosition::EFLineUp;
        break;
    case EKeyPageDown:
        movement = TCursorPosition::EFPageDown;
        break;
    case EKeyPageUp:
        movement = TCursorPosition::EFPageUp;
        break;
    default:
        return EKeyWasNotConsumed;
    }
    iTextView->ScrollDisplayL(movement);
    UpdateScrollBars(EFalse);
    return EKeyWasConsumed;
}

// MJavaDebugAgentLog
void CJavaDebugAgentLogScreen::Log(const TText* aString)
{
    TPtrC des(aString);
    TRAP_IGNORE(LogL(des));
}

void CJavaDebugAgentLogScreen::LogFormat(const TText* aFormat, ...)
{
    VA_LIST va;
    VA_START(va, aFormat);
    TBuf<KMaxLogLineLength> buf;
    TPtrC format(aFormat);
    buf.AppendFormatList(format, va, this);
    TRAP_IGNORE(LogL(buf));
}

// TDesOverflow
void CJavaDebugAgentLogScreen::Overflow(TDes& /*aDes*/)
{
}

/**
 * Local Variables:
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
