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


#ifndef __JAVA_DEBUG_AGENT_LOG_SCREEN_H_
#define __JAVA_DEBUG_AGENT_LOG_SCREEN_H_

#include <coecntrl.h>
#include <txtglobl.h>
#include <frmtview.h>
#include <txtfmlyr.h>
#include <txtrich.h>
#include "JavaDebugAgentLog.h"

class CAknsBasicBackgroundControlContext;
class CJavaDebugAgentBasicLogView;
class CJavaDebugAgentLogScreen :
    public CCoeControl, 
    public MJavaDebugAgentLog,
    TDesOverflow
{
private:
    RFs iFs;
    RFile iLogFile;
    HBufC* iLogFileName;
    CEikScrollBarFrame* iSBFrame;
    TRect iViewRect;
    TRect iTextRect;
    RRegion iTextClipRegion;
    CGlobalText* iGlobalText;
    CJavaDebugAgentBasicLogView* iView;
    TCharFormat iCharFormat;
    TCharFormatMask iCharFormatMask;
    CCharFormatLayer* iCharFormatLayer;
    CParaFormat iParaFormat;
    TParaFormatMask iParaFormatMask;
    CParaFormatLayer* iParaFormatLayer;
    TInt iDefaultFontHeightInTwips;
    TInt iFontSize;
    CTextLayout* iLayout;
    CTextView* iTextView;
    TInt iLineCounter;
    CAknsBasicBackgroundControlContext* iBgContext;
    CFbsBitmap* iBgBitmap;
    CFbsBitmap* iBgMask;

public:
    static CJavaDebugAgentLogScreen* NewL(CJavaDebugAgentBasicLogView* aView,
        const TRect& aRect);
    virtual ~CJavaDebugAgentLogScreen();

public:
    void ClearLog();
    void SetLogFile(const TDesC* aFileName);
    void SetFontSize(TInt aFontSize);

    // CCoeControl
    virtual void Draw(const TRect& aRect) const;
    virtual void SizeChanged();
    virtual TInt CountComponentControls() const;
    virtual CCoeControl* ComponentControl(TInt aIndex) const;
    virtual TTypeUid::Ptr MopSupplyObject(TTypeUid aId);
    virtual TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, 
        TEventCode aType);

    // MJavaDebugAgentLog
    virtual void Log(const TText* aString);
    virtual void LogFormat(const TText* aFormat, ...);

private:

    // TDesOverflow
    virtual void Overflow(TDes& aDes);

private:
    CJavaDebugAgentLogScreen(CJavaDebugAgentBasicLogView* aView);
    void ConstructL(const TRect& aRect);
    void LogToScreenL(const TDesC& aPrefix, const TDesC& aText);
    void LogToFileL(const TDesC& aPrefix, const TDesC& aText);
    void LogL(const TDesC& aString);
    void LogFormatToScreen(const TText* aFormat, ...);
    void FormatPrefixL(TDes& aPrefix);
    void FormatAndScrollL();
    void SetLogFileL(const TDesC* aFileName);
    void SetFontSizeL(TInt aFontSize);

    static CFbsBitmap* CreateBackgroundBitmapLC(const TSize& aSize,
        TRgb aBgColor, TRgb aFillColor, TRgb aBorderColor, TRgb aShadowColor);
    static void DrawBackground(CBitmapContext* aGc, const TRect& aRect,
        TRgb aBgColor, TRgb aFillColor, TRgb aBorderColor, TRgb aShadowColor);

    TBool UpdateFontSizeL(TInt aFontSize);
    void UpdateSBModel(TEikScrollBarModel* aModel) const;
    void UpdateScrollBars(TBool aDrawNow);
    void SizeChangedL();
    void DoLayoutL();
    void CloseLogFile();
    void ClearL();
};

#endif // __JAVA_DEBUG_AGENT_LOG_SCREEN_H_

/**
 * Local Variables:
 * mode: c++
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
