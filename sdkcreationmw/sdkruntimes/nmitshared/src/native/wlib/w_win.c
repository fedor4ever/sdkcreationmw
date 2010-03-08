/*
 * $Id: w_win.c,v 1.1 2005/04/21 23:54:39 slava Exp $
 *
 * Copyright (C) 2000-2005 by Slava Monich
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions 
 * are met: 
 *
 *   1.Redistributions of source code must retain the above copyright 
 *     notice, this list of conditions and the following disclaimer. 
 *   2.Redistributions in binary form must reproduce the above copyright 
 *     notice, this list of conditions and the following disclaimer 
 *     in the documentation and/or other materials provided with the 
 *     distribution. 
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
 * IN NO EVENT SHALL THE CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) ARISING 
 * IN ANY WAY OUT OF THE USE OR INABILITY TO USE THIS SOFTWARE, EVEN 
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 *
 * The views and conclusions contained in the software and documentation 
 * are those of the authors and should not be interpreted as representing 
 * any official policies, either expressed or implied.
 */

#include "w_lib.h"

/*==========================================================================*
 *      W I N D O W    U T I L I T I E S
 *==========================================================================*/

/**
 * Loads window text into a string buffer. Returns NULL if window handle
 * is invalid or memory allocation fails. Uses provided string buffer for 
 * storing the string. In case of success, returns pointer to the StrBuf's 
 * buffer
 */
Str WIN32_GetWindowText(HWND hwnd, StrBuf * sb) 
{
    /* we will multiply by 2 when we enter the loop...  divide by 2 now to 
     * avoid unnecessary reallocation */
    if (IsWindow(hwnd)) {
        size_t maxlen = (sb->alloc ? ((sb->alloc-1)/2) : 0);
        do {
            STRBUF_Clear(sb);
            maxlen = MAX(2*maxlen,32);
            if (!STRBUF_Alloc(sb, maxlen)) {
                /* cannot allocate the memory */
                return NULL;
            }
            SetLastError(NO_ERROR);
            sb->len = GetWindowText(hwnd, sb->s, maxlen);
            ASSERT(sb->len <= maxlen);
            ASSERT(!sb->s[sb->len]);
            if (!sb->len && GetLastError() != NO_ERROR) {
                return NULL;    /* bad window handle, I guess */
            }
        } while (sb->len >= (maxlen-1));
        return STRBUF_GetString(sb);
    }
    return NULL;
}

/**
 * A halftone brush shows pixels that are alternately foreground and 
 * background colors to create a dithered pattern.
 */
HBRUSH WIN32_CreateHalftoneBrush()
{
    int i;
    HBITMAP grayBitmap;
    WORD grayPattern[8];
    for (i = 0; i < COUNT(grayPattern); i++) {
        grayPattern[i] = (WORD)(0x5555 << (i & 1));
    }
    grayBitmap = CreateBitmap(8, 8, 1, 1, grayPattern);
    ASSERT(grayBitmap);
    if (grayBitmap) {
        HBRUSH hBrush = CreatePatternBrush(grayBitmap);
        DeleteObject(grayBitmap);
        return hBrush;
    }
    return NULL;
}

/**
 * Centers a window relative to its parent. Usually called from the 
 * WM_INITDIALOG to center dialog boxes relative to the main window 
 * of the application. By default, the function centers child windows 
 * relative to their parent window, and pop-up windows relative to their 
 * owner. If the pop-up window is not owned, it is centered relative to 
 * the screen. To center a window relative to a specific window which 
 * is not the owner or parent, the hwndCenter parameter may be set to 
 * a valid window. 
 */
Bool WIN32_CenterWindow(HWND hwnd, HWND hwndCenter)
{
    DWORD dwStyle;
    RECT rcWnd, rcArea, rcCenter;
    int x, y;

    if (!IsWindow(hwnd)) return False;

    /* determine owner window to center against*/
    dwStyle = GetWindowLong(hwnd, GWL_STYLE);
    if (!hwndCenter) {
         hwndCenter = ((dwStyle & WS_CHILD) ?
                       GetParent(hwnd) :
                       GetWindow(hwnd,GW_OWNER));
    }

    /* get coordinates of the window relative to its parent */
    memset(&rcWnd, 0, sizeof(rcWnd));
    VERIFY(GetWindowRect(hwnd, &rcWnd));
    if (!(dwStyle & WS_CHILD)) {

        /* don't center against invisible or minimized windows */
        if (hwndCenter) {
            DWORD dwStyle = GetWindowLong(hwndCenter, GWL_STYLE);
#ifdef _WIN32_WCE
            if (!(dwStyle & WS_VISIBLE)) {
#else  /* _WIN32_WCE */
            if (!(dwStyle & WS_VISIBLE) || (dwStyle & WS_MINIMIZE)) {
#endif /* !_WIN32_WCE */
                hwndCenter = NULL;
            }
        }

        /* center within screen coordinates */
        memset(&rcArea, 0, sizeof(RECT));
        VERIFY(SystemParametersInfo(SPI_GETWORKAREA, 0, &rcArea, 0));
        if (!hwndCenter) {
            rcCenter = rcArea;
        } else {
            memset(&rcCenter, 0, sizeof(RECT));
            VERIFY(GetWindowRect(hwndCenter, &rcCenter));
        }

#ifdef _WIN32_WCE
        /* special fix for PocketPCs */
        if (memcmp(&rcCenter,&rcArea,sizeof(RECT)) == 0 && rcArea.top > 0) {
            rcArea.top = 0;
            rcCenter.top = 0;
        }
#endif /* _WIN32_WCE */

    } else {

        HWND hwndParent = GetParent(hwnd);

        /* center within parent client coordinates */
        if (!IsWindow(hwndParent)) return False;
        if (!IsWindow(hwndCenter)) return False;

        VERIFY(GetClientRect(hwndParent, &rcArea));
        VERIFY(GetClientRect(hwndCenter, &rcCenter));
        MapWindowPoints(hwndCenter, hwndParent, (POINT*)&rcCenter, 2);
    }

    /* find upper left based on rcCenter */
    x = (rcCenter.left + rcCenter.right - RECT_Width(&rcWnd))/2;
    y = (rcCenter.top + rcCenter.bottom - RECT_Height(&rcWnd))/ 2;

    /* if the window is outside the screen, move it inside */
    if (x < rcArea.left) {
        x = rcArea.left;
    } else if (x + RECT_Width(&rcWnd) > rcArea.right) {
        x = rcArea.right - RECT_Width(&rcWnd);
    }

    if (y < rcArea.top) {
        y = rcArea.top;
    } else if (y + RECT_Height(&rcWnd) > rcArea.bottom) {
        y = rcArea.bottom - RECT_Height(&rcWnd);
    }

    /* move the window */
    TRACE3("Moving window 0x%04lX to (%d,%d)\n", hwnd, x, y);
    if (!SetWindowPos(hwnd, NULL, x, y, -1, -1, 
         SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE)) {
        ASSMSG1("SetWindowPos failed, err", GetLastError());
        return False;
    }

    return True;
}

/**
 * This function draws a transparent bitmap (one of the colors is taken
 * as a "transparent" color).
 */
void WIN32_DrawTransparentBitmap(HDC hdc, HBITMAP hBitmap, int x,int y, 
    COLORREF cTransparentColor)
{
    BITMAP   bm;
    COLORREF cColor;
    HBITMAP  bmAndBack, bmAndObject, bmAndMem, bmSave;
    HBITMAP  bmBackOld, bmObjectOld, bmMemOld, bmSaveOld;
    POINT    ptSize;

    /* create some DCs to hold temporary data. */
    HDC hdcBack   = CreateCompatibleDC(hdc);
    HDC hdcObject = CreateCompatibleDC(hdc);
    HDC hdcMem    = CreateCompatibleDC(hdc);
    HDC hdcSave   = CreateCompatibleDC(hdc);
    HDC hdcTemp   = CreateCompatibleDC(hdc);

    /* select temporary bitmap */
    SelectObject(hdcTemp, hBitmap); 

    /* convert from device to logical points */
    GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&bm);
    ptSize.x = bm.bmWidth;
    ptSize.y = bm.bmHeight;
#ifndef _WIN32_WCE
    DPtoLP(hdcTemp, &ptSize, 1);
#endif /* _WIN32_WCE */

    /* create a bitmap for each DC */
    bmAndBack   = CreateBitmap(ptSize.x, ptSize.y, 1, 1, NULL);
    bmAndObject = CreateBitmap(ptSize.x, ptSize.y, 1, 1, NULL);
    bmAndMem    = CreateCompatibleBitmap(hdc, ptSize.x, ptSize.y);
    bmSave      = CreateCompatibleBitmap(hdc, ptSize.x, ptSize.y);

    /* each DC must select a bitmap object to store pixel data. */
    bmBackOld   = SelectObject(hdcBack, bmAndBack);
    bmObjectOld = SelectObject(hdcObject, bmAndObject);
    bmMemOld    = SelectObject(hdcMem, bmAndMem);
    bmSaveOld   = SelectObject(hdcSave, bmSave);

    /* set proper mapping mode. */
#ifndef _WIN32_WCE
    SetMapMode(hdcTemp, GetMapMode(hdc));
#endif /* _WIN32_WCE */

    /* save the bitmap sent here, because it will be overwritten. */
    BitBlt(hdcSave, 0, 0, ptSize.x, ptSize.y, hdcTemp, 0, 0, SRCCOPY);

    /*
     * set the background color of the source DC to the color.
     * contained in the parts of the bitmap that should be transparent
     */
    cColor = SetBkColor(hdcTemp, cTransparentColor);

    /*
     * create the object mask for the bitmap by performing a BitBlt
     * from the source bitmap to a monochrome bitmap.
     */
    BitBlt(hdcObject, 0, 0, ptSize.x, ptSize.y, hdcTemp, 0, 0, SRCCOPY);

    /*
     * set the background color of the source DC back to the original
     * color.
     */
    SetBkColor(hdcTemp, cColor);

    /*
     * 1. create the inverse of the object mask.
     * 2. copy the background of the main DC to the destination.
     * 3. mask out the places where the bitmap will be placed.
     * 4. mask out the transparent colored pixels on the bitmap.
     * 5. XOR the bitmap with the background on the destination DC.
     * 6. copy the destination to the screen.
     * 7. place the original bitmap back into the bitmap sent here.
     */
    BitBlt(hdcBack, 0, 0, ptSize.x, ptSize.y, hdcObject, 0, 0,NOTSRCCOPY);
    BitBlt(hdcMem, 0, 0, ptSize.x, ptSize.y, hdc, x, y,SRCCOPY);
    BitBlt(hdcMem, 0, 0, ptSize.x, ptSize.y, hdcObject, 0, 0, SRCAND);
    BitBlt(hdcTemp, 0, 0, ptSize.x, ptSize.y, hdcBack, 0, 0, SRCAND);
    BitBlt(hdcMem, 0, 0, ptSize.x, ptSize.y, hdcTemp, 0, 0, SRCPAINT);
    BitBlt(hdc, x, y, ptSize.x, ptSize.y, hdcMem, 0, 0,SRCCOPY);
    BitBlt(hdcTemp, 0, 0, ptSize.x, ptSize.y, hdcSave, 0, 0, SRCCOPY);

    /* delete the memory bitmaps. */
    DeleteObject(SelectObject(hdcBack, bmBackOld));
    DeleteObject(SelectObject(hdcObject, bmObjectOld));
    DeleteObject(SelectObject(hdcMem, bmMemOld));
    DeleteObject(SelectObject(hdcSave, bmSaveOld));

    /* delete the memory DCs. */
    DeleteDC(hdcMem);
    DeleteDC(hdcBack);
    DeleteDC(hdcObject);
    DeleteDC(hdcSave);
    DeleteDC(hdcTemp);
}
