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
* Description:  Helper object for inserting images into RichEdit control
*
*/


#ifndef _MENUBAR_IMAGE_DATA_OBJECT_H_
#define _MENUBAR_IMAGE_DATA_OBJECT_H_

#include "EcmtWin32.h"
#include <richedit.h>

#include <wtypes.h>
#include <unknwn.h>
#include <objidl.h>
#include <oleidl.h>
#include <richole.h>

// ---------------------------------------------------------------------------
// Stuff for inserting images into RichEdit control. Copied from
// http://www.codeguru.com/Cpp/controls/richedit/article.php/c5383/
// ---------------------------------------------------------------------------
class CImageDataObject : IDataObject
{
private:
    ULONG   iRefCnt;
    BOOL    iRelease;
    STGMEDIUM iStgMedium;
    FORMATETC iFormat;

public:
    CImageDataObject(HBITMAP hBitmap);

    // Inserts bitmap into the current location in the RichEdit control
    void Insert(IRichEditOle* pRichEditOle);

    // Redefine default new and delete operators to use memory allocator
    // that works outside of Symbian emulation environment. They are
    // invoked on GUI thread.
    void* operator new (unsigned int aSize);
    void operator delete (void* aPtr);

    // IUnknown
    STDMETHOD(QueryInterface)(REFIID iid, void ** ppvObject);
    STDMETHOD_(ULONG, AddRef)(void);
    STDMETHOD_(ULONG, Release)(void);

    // IDataObject
    STDMETHOD(GetData)(FORMATETC *, STGMEDIUM *pmedium);
    STDMETHOD(GetDataHere)(FORMATETC*, STGMEDIUM*);
    STDMETHOD(QueryGetData)(FORMATETC*);
    STDMETHOD(GetCanonicalFormatEtc)(FORMATETC*, FORMATETC*);
    STDMETHOD(SetData)(FORMATETC* pformatetc, STGMEDIUM*  pmedium, BOOL);
    STDMETHOD(EnumFormatEtc)(DWORD, IEnumFORMATETC**);
    STDMETHOD(DAdvise)(FORMATETC*, DWORD, IAdviseSink*, DWORD*);
    STDMETHOD(DUnadvise)(DWORD);
    STDMETHOD(EnumDAdvise)(IEnumSTATDATA**);

private:
    IOleObject* GetOleObject(IOleClientSite *pOleSite, IStorage *pStorage);
};

#endif // _MENUBAR_IMAGE_DATA_OBJECT_H_
