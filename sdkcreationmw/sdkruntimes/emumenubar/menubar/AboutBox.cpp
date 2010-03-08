/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  About box for the Series 60 emulator
*
*/


static char rcsid[] = "$Id: AboutBox.cpp,v 1.12 2006/02/23 01:56:35 slava Exp $";

// INCLUDE FILES

#include "WinMenubar.h"
#include "EcmtResource.h"
#include "SdkProductInfo.h"
#include "ImageDataObject.h"

// SETTEXTEX and related defines are missing from CodeWarrior 3.1 headers
// This struct is passed in the wparam of EM_SETTEXTEX  message
#ifndef ST_DEFAULT
#  define ST_DEFAULT    0
#  define ST_KEEPUNDO   1
#  define ST_SELECTION  2
#  define ST_NEWCHARS   4
typedef struct _settextex {
    DWORD flags;
    UINT  codepage;
} SETTEXTEX;
#endif // ST_DEFAULT

// ---------------------------------------------------------------------------
// Support for scrolling text below the product name. Only needed if
// product registration is enabled
// ---------------------------------------------------------------------------

#ifdef SDK_REGISTRATION

#define SCROLL_LINE_SPACE     4 // spacing between the lines
#define SCROLL_INIT_OFFSET    4 // initial offset
#define SCROLL_TIMER        123 // scroll timer id
#define SCROLL_DELAY_TIMER  321 // initial delay timer
#define SCROLL_TIMER_PERIOD 250 // scroll speed (milliseconds per pixel)
#define SCROLL_TIMER_DELAY 5000 // initial delay

typedef struct _ScrollTextData  // control context
{
    BOOL bScrolling;            // TRUE if scrolling is active
    BOOL bFirstLoop;            // TRUE if scrolling for the first time
    BOOL bOneTime;              // TRUE to stop scrolling after one loop
    int offset;                 // current offset 
    int lineCount;              // number of lines
    LPSTR * lines;              // text to display
    HFONT hFont;                // font handle
    LOGFONT font;               // cached font info
}
ScrollTextData;

// had some problems linking strdup...
static char* sdup(const char* s)
{
    if (s)
    {
        int len = strlen(s);
        char* s2 = (char*)malloc(sizeof(*s)*(len+1));
        if (s2)
        {
            strcpy(s2, s);
            return s2;
        }
    }
    return NULL;
}

// Creates ScrollTextData reflecting the current registration state
static 
ScrollTextData* 
TitleCreateScrollData(
    HWND hWnd, 
    RegProduct * product)
{
    ScrollTextData* data = (ScrollTextData*)malloc(sizeof(ScrollTextData));
    if (data)
    {
        BOOL registered = (REG_ProductStatus(product) == RegRegistered);
        ZeroMemory(data, sizeof(*data));
        data->hFont = GetWindowFont(hWnd);
        data->bOneTime = registered;
        if (GetObject(data->hFont,sizeof(data->font),&data->font))
        {
            if (data->font.lfHeight == 0)
            {
                data->font.lfHeight = 11;
            } 
            else if (data->font.lfHeight < 0) 
            {
                data->font.lfHeight *= -1;
            }
            data->lineCount = 3;
            data->lines = (LPSTR*)malloc(sizeof(LPSTR)*data->lineCount);
            if (data->lines)
            {
                int i = 0;
                data->lines[i] = sdup("Product key:");
                if (data->lines[i])
                {
                    data->lines[++i] = sdup(REG_ProductKey(product));
                    if (data->lines[i]) {
                        if (registered)
                        {
                            data->lines[++i] = sdup("Registered");
                        } 
                        else
                        {
                            int n1 = REG_DaysSinceFirstRun(product);
                            int n2 = REG_FreeTrialPeriod(product);
                            if (n2 > n1) {
                                int n = n2-n1;
                                if (n2 == n1)
                                {
                                    data->lines[++i] = sdup("Expires today");
                                }
                                else
                                {
                                    // This code won't survive localization.
                                    // It's very specific to the English
                                    // grammar.
                                    char buf[128];
                                    if (n != 11 && (n%10) == 1)
                                    {
                                        sprintf(buf, "Expires in %d day",n);
                                    }
                                    else
                                    {
                                        sprintf(buf, "Expires in %d days",n);
                                    }
                                    data->lines[++i] = sdup(buf);
                                }
                            }
                            else
                            {
                                data->lines[++i] = sdup("Trial period has expired");
                            }
                        }
                        if (data->lines[i])
                        {
                            return data;
                        }
                        free(data->lines[--i]);
                    }
                    free(data->lines[--i]);
                }
                free(data->lines);
            }
        }
        free(data);
    }
    return NULL;
}

// Deletes ScrollTextData
static 
void 
TitleDeleteScrollData(
    ScrollTextData* data)
{
    if (data)
    {
        int i;
        for (i=0; i<data->lineCount; i++) free(data->lines[i]);
        free(data->lines);
        free(data);
    }
}

#endif // SDK_REGISTRATION

// ---------------------------------------------------------------------------
// TitleWndProc()
// Window proc for the product name in the About dialog
// ---------------------------------------------------------------------------

typedef struct _TitleData
{
    BOOL bRepaint;
    WNDPROC superProc;
    HFONT hFont;
    HBITMAP hBitmap;
    HBITMAP hOffScreenBg;
    HBITMAP hOffScreen;
    HDC hBitmapDC;
    HDC hOffScreenDC;
    char* productName;
    const char* productId;
#ifdef SDK_REGISTRATION
    HBITMAP hGradientBitmap;
    ScrollTextData* scrollText;
#endif // SDK_REGISTRATION
}
TitleData;

static void TitlePaint(TitleData* data, HWND hWnd, HDC dc)
{
    BITMAP bm;
    ZeroMemory(&bm, sizeof(bm));
    GetObject(data->hBitmap, sizeof(bm), &bm);

    // If nothing changed since last paint, use the cached bitmap
    RECT clientRect;
    GetClientRect(hWnd, &clientRect);
    int x0 = (RECT_Width(&clientRect) - bm.bmWidth)/2;
    int y0 = (RECT_Height(&clientRect) - bm.bmHeight)/2;
    if (!data->bRepaint)
    {
        BitBlt(dc,x0,y0,bm.bmWidth,bm.bmHeight,data->hOffScreenDC,0,0,SRCCOPY);
        return;
    }

    // Copy the current image and fill the interior with black
    // What we essentially want is dest = (screen AND (NOT shape))
    // There is no ((NOT src) AND dest) raster operation in Windows,
    // so what we do is a combination of NOTSRCCOPY (dest = (NOT source))
    // and NOTSRCERASE (dest = (NOT src) AND (NOT dest)). NOT is applied
    // twice to the data we have copied from the screen device context,
    // restoring the original color.
    SelectBitmap(data->hOffScreenDC, data->hOffScreenBg);
    SelectBitmap(data->hBitmapDC, data->hBitmap);
    BitBlt(data->hOffScreenDC,0,0,bm.bmWidth,bm.bmHeight,dc,x0,y0,NOTSRCCOPY);
    BitBlt(data->hOffScreenDC,0,0,bm.bmWidth,bm.bmHeight,data->hBitmapDC,
        0,0,NOTSRCERASE);

    // Wipe out the offscreen bitmap with white
    RECT bitmapRect = {0, 0, bm.bmWidth, bm.bmHeight};
    HBRUSH hWhiteBrush = GetStockBrush(WHITE_BRUSH);
    SelectBitmap(data->hOffScreenDC, data->hOffScreen);
    FillRect(data->hOffScreenDC, &bitmapRect, hWhiteBrush);
    
    // Calculate the size of the product name string
    const char * prodName = data->productName;
    if (!prodName) prodName = data->productId;
    int prodLen = strlen(prodName);
    SIZE prodNameSize = {0,0};
    if (data->hFont) SelectFont(data->hOffScreenDC, data->hFont);
    GetTextExtentPoint32A(data->hOffScreenDC,prodName,prodLen,&prodNameSize);

    // Adjust product name rectangle because the cloud is not quite symmetric
#ifdef SDK_REGISTRATION
    int y = 12;
    int height = (bm.bmHeight - y)/2;
#else  // !SDK_REGISTRATION
    int y = 0;
    int height = bm.bmHeight - 10;
#endif // !SDK_REGISTRATION

    RECT prodNameRect;
    prodNameRect.left = 0;
    prodNameRect.right = bm.bmWidth;
    prodNameRect.top = y + (height - prodNameSize.cy)/2;
    prodNameRect.bottom = prodNameRect.top + prodNameSize.cy;

#ifdef SDK_REGISTRATION
    
    // Add more space around the product name
    InflateRect(&prodNameRect, 0, SCROLL_LINE_SPACE);
    ScrollTextData* scroll = data->scrollText;
    if (scroll && scroll->bScrolling) {
        RECT rc;
        rc.left = prodNameRect.left;
        rc.right = prodNameRect.right;
        BOOL rollover = FALSE;
        int y1 = prodNameRect.bottom - scroll->offset;
        if (scroll->hFont) SelectFont(data->hOffScreenDC, scroll->hFont);
        for (int i=0; y1 < bm.bmHeight; i++)
        {
            if (i == scroll->lineCount) 
            {
                i = 0;
                y1 += SCROLL_LINE_SPACE;
                rollover = TRUE;
            }

            // Skipping the first line when scrolling for the first time
            if (!scroll->bFirstLoop || rollover || i > 0)
            {
                LPCSTR line = scroll->lines[i];
                if (!line) line = " ";
                rc.top = y1;
                rc.bottom = y1 + scroll->font.lfHeight + SCROLL_LINE_SPACE;
                if (rc.bottom >= prodNameRect.bottom) {
                    if (rc.top < bm.bmHeight) {
                        DrawTextA(data->hOffScreenDC, line, strlen(line), &rc,
                            DT_NOCLIP | DT_CENTER | DT_SINGLELINE);
                    }
                }
            }
            y1 += scroll->font.lfHeight + SCROLL_LINE_SPACE;
        }

        // Erase the portion of the bitmap that overlaps with the product name
        int bottom = prodNameRect.bottom + SCROLL_LINE_SPACE;
        SelectBitmap(data->hBitmapDC, data->hGradientBitmap);
        BitBlt(data->hOffScreenDC, 0, 0, bm.bmWidth, bottom, 
            data->hBitmapDC, 0, bm.bmHeight - bottom, SRCPAINT);

        // Restore the product name font
        if (data->hFont) SelectFont(data->hOffScreenDC, data->hFont);
    }

#endif // SDK_REGISTRATION

    // Draw the product name
    DrawTextA(data->hOffScreenDC, prodName, prodLen, &prodNameRect,
        DT_NOCLIP | DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    // Fill the exterior with black
    SelectBitmap(data->hBitmapDC, data->hBitmap);
    BitBlt(data->hOffScreenDC,0,0,bm.bmWidth,bm.bmHeight,
           data->hBitmapDC,0,0,SRCAND);

    // Overlap the two images
    SelectBitmap(data->hBitmapDC, data->hOffScreenBg);
    BitBlt(data->hOffScreenDC,0,0,bm.bmWidth,bm.bmHeight,
           data->hBitmapDC,0,0,SRCPAINT);

    // Copy the offscreen bitmap to the screen
    BitBlt(dc,x0,y0,bm.bmWidth,bm.bmHeight,data->hOffScreenDC,0,0,SRCCOPY);
    data->bRepaint = FALSE;
}

#ifdef SDK_REGISTRATION

// Stops scroller, shows the edit control with the product key
static void TitleStopScroller(HWND hDlg)
{
    HWND hTitle = GetDlgItem(hDlg, IDC_PRODUCT_NAME);
    TitleData* data = (TitleData*)GetWindowLong(hTitle,GWL_USERDATA);
    if (data)
    {
        ScrollTextData* scroll = data->scrollText;
        if (scroll->bScrolling)
        {
            // Stop scrolling
            scroll->bScrolling = FALSE;
            data->bRepaint = TRUE;
            InvalidateRect(hTitle, NULL, FALSE);

            // Kill scroll timer
            KillTimer(hTitle, SCROLL_TIMER);

            // Show the edit control
            HWND hEdit = GetDlgItem(hDlg, IDC_PRODUCT_KEY);
            ShowWindow(hEdit, SW_SHOWNORMAL);
            InvalidateRect(hEdit, NULL, FALSE);
        }

        // Reset the initial delay
        KillTimer(hTitle,SCROLL_DELAY_TIMER);
        SetTimer(hTitle,SCROLL_DELAY_TIMER,SCROLL_TIMER_DELAY,NULL);
    }
}

#endif // SDK_REGISTRATION

static LRESULT CALLBACK TitleWndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
    TitleData* data = (TitleData*)GetWindowLong(hWnd, GWL_USERDATA);
    switch (msg)
    {

#ifdef SDK_REGISTRATION

    case WM_TIMER:
        {
            ScrollTextData* scroll = data->scrollText;
            switch (wp) 
            {
            case SCROLL_TIMER:
                data->bRepaint = TRUE;
                scroll->offset++;
                if (scroll->offset >= ((scroll->font.lfHeight * 
                    scroll->lineCount) + SCROLL_LINE_SPACE * (
                    scroll->lineCount + 1)))
                {
                    scroll->offset = 0;
                    scroll->bFirstLoop = FALSE;
                }
                if (scroll->offset == SCROLL_INIT_OFFSET && scroll->bOneTime)
                {
                    TitleStopScroller(GetParent(hWnd));
                }
                else
                {
                    InvalidateRect(hWnd, NULL, FALSE);
                }
                break;

            case SCROLL_DELAY_TIMER:
                KillTimer(hWnd, SCROLL_DELAY_TIMER);
                SetTimer(hWnd, SCROLL_TIMER, SCROLL_TIMER_PERIOD, NULL);
                ShowWindow(GetDlgItem(GetParent(hWnd),IDC_PRODUCT_KEY),SW_HIDE);
                scroll->bScrolling = TRUE;
                scroll->bFirstLoop = TRUE;
                scroll->offset = SCROLL_INIT_OFFSET;
                data->bRepaint = TRUE;
                InvalidateRect(hWnd, NULL, FALSE);
                break;

            default:
                // Forward the message to the default window proc
                return CallWindowProc(data->superProc, hWnd, msg, wp, lp);
            }
        }
        return 0;

#endif // SDK_REGISTRATION

    case WM_PAINT:
        if (data->hBitmap)
        {
            RECT rc;
            if (GetUpdateRect(hWnd, &rc, FALSE)) 
            {
                PAINTSTRUCT paint;
                HDC dc = BeginPaint(hWnd, &paint);
                if (dc) 
                {
                    int w = RECT_Width(&rc);
                    int h = RECT_Height(&rc);
                    if (!data->hOffScreen)
                    {
                        data->hOffScreen = CreateCompatibleBitmap(dc,w,h);
                    }
                    if (!data->hOffScreenBg)
                    {
                        data->hOffScreenBg = CreateCompatibleBitmap(dc,w,h);
                    }
                    if (!data->hOffScreenDC && data->hOffScreen)
                    {
                        data->hOffScreenDC = CreateCompatibleDC(dc);
                        if (data->hOffScreenDC)
                        {
                            SetTextColor(data->hOffScreenDC, RGB(0,0,0));
                            SetBkColor(data->hOffScreenDC, RGB(255,255,255));
                            SetBkMode(data->hOffScreenDC, OPAQUE);
                        }
                    }
                    if (!data->hBitmapDC)
                    {
                        data->hBitmapDC = CreateCompatibleDC(dc);
                    }

                    if (data->hBitmapDC &&
                        data->hOffScreenDC && 
                        data->hOffScreen &&
                        data->hOffScreenBg)
                    {
                        TitlePaint(data, hWnd, dc);
                    }
                    
                    EndPaint(hWnd, &paint);
                    return 1;
                }
            }
        }
        return 0;
        
    case WM_DESTROY:
        {
            // Cleanup
            LRESULT result = CallWindowProc(data->superProc, hWnd, msg, wp, lp);
            SetWindowLong(hWnd, GWL_WNDPROC, (LONG)data->superProc);
            SetWindowLong(hWnd, GWL_USERDATA, 0);

#ifdef SDK_REGISTRATION
            if (data->hGradientBitmap) DeleteBitmap(data->hGradientBitmap);
            TitleDeleteScrollData(data->scrollText);
            data->scrollText = NULL;
#endif // SDK_REGISTRATION

            if (data->hOffScreenDC) DeleteDC(data->hOffScreenDC);
            if (data->hOffScreen) DeleteBitmap(data->hOffScreen);
            if (data->hBitmapDC) DeleteDC(data->hBitmapDC);
            if (data->hBitmap) DeleteBitmap(data->hBitmap);
            if (data->hFont) DeleteFont(data->hFont);
            free(data->productName);

            free(data);
            return result;
        }
    
    default:
        // Forward the message to the default window proc
        return CallWindowProc(data->superProc, hWnd, msg, wp, lp);
    }
}

// ---------------------------------------------------------------------------
// CWinMenubar::AboutProc()
// Dialog Window proc for About dialog
// ---------------------------------------------------------------------------
BOOL CALLBACK CWinMenubar::AboutProc(HWND hDlg, UINT message, 
                                     WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam); // unless SDK_REGISTRATION is defined
    switch (message) 
    {
    case WM_INITDIALOG:
        {
        TCHAR text[128];
        CWinMenubar* menubar = (CWinMenubar*)lParam;
        HINSTANCE hMod = menubar->ModuleHandle();
        GetWindowText(GetParent(hDlg), text, sizeof(text)/sizeof(text[0]));
        SetWindowText(hDlg, text);

        HBITMAP hBitmap = NULL;
        UINT textScrollID = IDC_ABOUT_TEXT_SCROLL;
        UINT textNoScrollID = IDC_ABOUT_TEXT_NO_SCROLL;
        UINT setTextMsg = WM_SETTEXT;
        WPARAM setTextParam = 0;
        SETTEXTEX setText;
        setText.flags = ST_SELECTION;
        setText.codepage = CP_ACP;

        char* aboutText = NULL;
        if (menubar->iRichEdit)
        {
            aboutText = ReadTextFile(ABOUTBOX_RTF_FILE);
        }
        if (aboutText)
        {
            setTextMsg = EM_SETTEXTEX;
            textScrollID = textNoScrollID = IDC_ABOUT_RICHTEXT;
            setTextParam = (WPARAM)&setText;
            hBitmap = LoadBitmap(hMod,MAKEINTRESOURCE(IDB_JAVA_POWERED));

#ifdef HAVE_WLIB

            HWND hText = GetDlgItem(hDlg, IDC_ABOUT_RICHTEXT);
            SendMessage(hText, EM_SETEVENTMASK, 0, 
            SendMessage(hText, EM_GETEVENTMASK, 0,0) | ENM_LINK);
            SendMessage(hText, EM_AUTOURLDETECT, TRUE, 0);

#endif // HAVE_WLIB

        }

        if (!aboutText && menubar->iRichEdit)
        {
            // Load default about text from the resource
            void* ptr = NULL;
            UINT size = 0;
            HGLOBAL hg = 0;
            HRSRC hr = FindResourceA(hMod,MAKEINTRESOURCEA(IDR_ABOUT_RTF),"RTF");
            if (hr)
            {
                size = SizeofResource(ModuleHandle(), hr);
                hg = LoadResource(ModuleHandle(), hr);
                ptr = LockResource(hg);
            }
            if (ptr)
            {
                aboutText = (char*)Alloc(size+1);
                if (aboutText)
                {
                    memcpy(aboutText, ptr, size);
                    aboutText[size] = 0;
                    setTextMsg = EM_SETTEXTEX;
                    textScrollID = IDC_ABOUT_RICHTEXT;
                    textNoScrollID = IDC_ABOUT_RICHTEXT;
                    hBitmap = LoadBitmap(hMod,MAKEINTRESOURCE(IDB_JAVA_POWERED));
                    setTextParam = (WPARAM)&setText;
                }
                UnlockResource(hg);
            }
        }
        if (aboutText)
        {   
            // Check if we need the scrollbar. 12 lines of text
            // will fit into the edit control.
            HWND hText = GetDlgItem(hDlg, textNoScrollID);
            SendMessageA(hText,setTextMsg,setTextParam,(LPARAM)aboutText);
            if (SendMessageA(hText, EM_GETLINECOUNT,0,0) <= 12)
            {
                // No need for the scrollbar, show the edit
                // control without the WS_VSCROLL style
                ShowWindow(hText, SW_SHOWNORMAL);
            }
            else 
            {
                // We need the vertical scrollbar, show the edit control
                // with the WS_VSCROLL style. Note that  textScrollID and
                // textNoScrollID may point to the same window, so reset
                // the text in the control (bug #DPEV-6JUJSQ)
                hText = GetDlgItem(hDlg, textScrollID);
                SendMessageA(hText,WM_SETTEXT,0,(LPARAM)"");
                SendMessageA(hText,setTextMsg,setTextParam,(LPARAM)aboutText);
                ShowWindow(hText, SW_SHOWNORMAL);
            }            
            
            // Add full SDK name to the RichEdit control by replacing <sdk_name> tag with the 
            // name defined in SdkProductInfo.h. Note that decision between MIDP and C++ SDK 
            // product names is made using CWinMenubar iMidpSdk member.
            static const TCHAR keywordHeading[] = TEXT("<sdk_name>");
            static const UINT keywordHeadingLen = COUNT(keywordHeading)-1;
            
            FINDTEXT findHeading;
            ZeroMemory(&findHeading, sizeof(findHeading));
            findHeading.chrg.cpMax = -1;
            findHeading.lpstrText = (LPTSTR)keywordHeading;
            // Select whole content
            SendMessage(hText, EM_SETSEL, 0, 0);
            // Find from selection
            int pos = SendMessage(hText,EM_FINDTEXT, FR_MATCHCASE|FR_DOWN,
                (LPARAM)&findHeading);
            if (pos >= 0) {       
                // Sdk name tag found         
                IRichEditOle* re = NULL;
                SendMessage(hText, EM_GETOLEINTERFACE, 0, (LPARAM)&re);
                if (re) {
                    // Select SDK name tag and replace it with the full SDK name
                    SendMessage(hText, EM_SETSEL, pos, pos+keywordHeadingLen);
                    if(menubar->iMidpSDK)
                        SendMessage(hText, EM_REPLACESEL, true, (LPARAM)TEXT(SDK_FULL_PRODUCT_NAME_MIDP));
                    else
                        SendMessage(hText, EM_REPLACESEL, true, (LPARAM)TEXT(SDK_FULL_PRODUCT_NAME_CPP));
                    re->Release();
                }
            }
            
            if (hBitmap)
            {
                static const TCHAR keyword[] = TEXT("<java logo>");
                static const UINT keywordLen = COUNT(keyword)-1;

                FINDTEXT find;
                ZeroMemory(&find, sizeof(find));
                find.chrg.cpMax = -1;
                find.lpstrText = (LPTSTR)keyword;
                SendMessage(hText, EM_SETSEL, 0, 0);
                int pos = SendMessage(hText,EM_FINDTEXT, FR_MATCHCASE|FR_DOWN,
                    (LPARAM)&find);
                if (pos >= 0) {
                    IRichEditOle* re = NULL;
                    SendMessage(hText, EM_GETOLEINTERFACE, 0, (LPARAM)&re);
                    if (re) {
                        CImageDataObject* bmp = new CImageDataObject(hBitmap);
                        if (bmp) {
                            SendMessage(hText,EM_SETSEL,pos,pos+keywordLen);
                            bmp->Insert(re);
                            bmp->Release();
                        }
                        re->Release();
                    }
                }
            }
            Free(aboutText);
        }

#ifdef SDK_REGISTRATION

        // Show product key
        if (gRegProductContext)
        {
            HWND hProductKey = GetDlgItem(hDlg, IDC_PRODUCT_KEY);
            const char* szKey = REG_ProductKey(gRegProductContext);
            SetWindowTextA(hProductKey, szKey);
            // The control is created invisible, show it
            ShowWindow(hProductKey, SW_SHOWNORMAL);
            if (REG_ProductStatus(gRegProductContext) != RegRegistered)
            {
                // Show the Register button
                ShowWindow(GetDlgItem(hDlg, IDC_REGISTER), SW_SHOWNORMAL);
            }
        }
                    
#endif // SDK_REGISTRATION

        // Subclass the product name control
        HWND hName = GetDlgItem(hDlg, IDC_PRODUCT_NAME);
        HINSTANCE hInst = (HINSTANCE)GetWindowLong(hDlg,GWL_HINSTANCE);
        if (hName)
        {
            // Allocate TitleData structure. It will be deallocated
            // by TitleWndProc when it receives WM_DESTROY message
            TitleData* data = (TitleData*)malloc(sizeof(TitleData));
            if (data)
            {
                LOGFONTA logFont;
                ZeroMemory(data, sizeof(*data));
                ZeroMemory(&logFont, sizeof(logFont));
                logFont.lfHeight = 24;
                logFont.lfWeight = FW_BOLD;
                logFont.lfCharSet = ANSI_CHARSET;
                logFont.lfOutPrecision = OUT_DEFAULT_PRECIS;
                logFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
                logFont.lfQuality = ANTIALIASED_QUALITY;
                logFont.lfPitchAndFamily = VARIABLE_PITCH;

                // Try Nokia font first
                strcpy(logFont.lfFaceName, "Nokia Sans");
                data->hFont = CreateFontIndirectA(&logFont);
                if (!data->hFont)
                {
                    // No Nokia font, try Arial then
                    strcpy(logFont.lfFaceName, "Arial");
                    data->hFont = CreateFontIndirectA(&logFont);
                }
                
                // Load bitmaps
                data->hBitmap = LoadBitmapA(hInst, 
                    MAKEINTRESOURCEA(IDB_ABOUT_TOP));

                // load product name from the file
                data->productName = ReadTextFile(PRODUCTNAME_FILE);
                data->productId = menubar->GetProductId();

                // Request initial bitmap
                data->bRepaint = TRUE;

#ifdef SDK_REGISTRATION

                data->hGradientBitmap = LoadBitmapA(hInst, 
                    MAKEINTRESOURCEA(IDB_ABOUT_GRADIENT));

                if (gRegProductContext)
                {
                    data->scrollText = TitleCreateScrollData(hName,
                        gRegProductContext);
                    if (data->scrollText)
                    {
                        // Initial delay
                        SetTimer(hName, SCROLL_DELAY_TIMER, 
                            SCROLL_TIMER_DELAY, NULL);
                    }
                }
#endif // SDK_REGISTRATION
        
                // Subclass the window
                data->superProc = (WNDPROC)GetWindowLong(hName,GWL_WNDPROC);
                SetWindowLong(hName, GWL_USERDATA, (LONG)data);
                SetWindowLong(hName, GWL_WNDPROC, (LONG)TitleWndProc);
            }
        }

#ifdef HAVE_WLIB
        WIN32_CenterWindow(hDlg, NULL);
#endif // HAVE_WLIB
        return TRUE;
        }
        
    case WM_COMMAND:
        switch (wParam)
        {

#ifdef SDK_REGISTRATION

        case IDC_REGISTER:
            if (REG_RegisterNow(gRegProductContext, hDlg))
            {
                TitleStopScroller(hDlg);

                // Update the scroller
                HWND hTitle = GetDlgItem(hDlg, IDC_PRODUCT_NAME);
                ScrollTextData* scroll = TitleCreateScrollData(
                    hTitle, gRegProductContext);

                if (scroll)
                {
                    TitleData* data;
                    data = (TitleData*)GetWindowLong(hTitle,GWL_USERDATA);
                    TitleDeleteScrollData(data->scrollText);
                    data->scrollText = scroll;
                }
            }
            if (REG_ProductStatus(gRegProductContext) == RegRegistered)
            {
                // The button has served its purpose. Hide it
                ShowWindow(GetDlgItem(hDlg, IDC_REGISTER), SW_HIDE);
            }
            return TRUE;

#endif // SDK_REGISTRATION

        case IDOK:
        case IDCANCEL:
            EndDialog(hDlg, wParam);
            return TRUE;
        default:
            return FALSE;
        }

#ifdef HAVE_WLIB // Need wlib to use WIN32_BrowseURL

    case WM_NOTIFY:
        if (((LPNMHDR)lParam)->idFrom == IDC_ABOUT_RICHTEXT)
        {
            char url[256];
            ENLINK* link = (ENLINK*)lParam;
            if (link->msg == WM_LBUTTONDOWN &&
                (link->chrg.cpMax - link->chrg.cpMin) <
                (COUNT(url)/2-1)) // EM_GETTEXTRANGE may think it's in WCHARs
            {
                // Visual effect
                HWND hText = link->nmhdr.hwndFrom;
                SendMessage(hText,EM_SETSEL,link->chrg.cpMin,link->chrg.cpMax);

                // It's not very clear what EM_GETTEXTRANGE message writes
                // into the buffer. Since we use SendMessageA (ANSI API),
                // I would expect that it returns ANSI string as most other
                // text-related messages do. Yet, at least some versions of
                // RichEdit control seem to return UCS-2 characters. Internet
                // search produces conflicting results. Let's be prepared for
                // anything. After all, there are only two possiblities. It's
                // either UCS-2 or ASCII.
                TEXTRANGEA range;
                range.chrg = link->chrg;
                range.lpstrText = url;
                url[0] = url[1] = 0;
                SendMessageA(hText, EM_GETTEXTRANGE, 0, (LPARAM)&range);
                url[COUNT(url)-1] = url[COUNT(url)-2] = 0;
                if (!_wcsnicmp((WCHAR*)url,L"http://",7))
                {
                    // Looks like UCS-2 string to me. Convert it to ASCII
                    LPWSTR w = (WCHAR*)url;
                    LPSTR p = url;
                    do { *p++ =  (char)*w++; } while (*w);
                    *p = 0;
                }

                // Start the default browser
                WIN32_BrowseURL(url);
            }
        }
        // Usually, the return value is ignored
        return FALSE;

#endif // HAVE_WLIB

#ifdef SDK_REGISTRATION

    case WM_CTLCOLORSTATIC:
        // We don't want the product key edit control to erase its
        // background because it's drawn on top of the product name
        // bitmap (white cloud).
        {
            int nDlgCtrlID = GetDlgCtrlID((HWND)lParam);
            switch (nDlgCtrlID) 
            {
            case IDC_PRODUCT_KEY:
                // Return transparent brush for the product key control
                return (BOOL)GetStockObject(HOLLOW_BRUSH);
            }
        }
        return FALSE;

    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_MBUTTONDOWN:
        // Stop scrolling on mouse click
        TitleStopScroller(hDlg);
        return FALSE;

#endif // SDK_REGISTRATION

    default:
        return FALSE;
    }
}
