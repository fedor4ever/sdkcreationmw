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


static char rcsid[] = "$Id: ImageDataObject.cpp,v 1.2 2006/02/23 01:56:35 slava Exp $";

// INCLUDE FILES

#include "ImageDataObject.h"
#include "EcmtResource.h"
#include "EcmtOsUtils.h"
#include <ole2.h>

// ---------------------------------------------------------------------------
// Stuff for inserting images into RichEdit control. Copied from
// http://www.codeguru.com/Cpp/controls/richedit/article.php/c5383/
// ---------------------------------------------------------------------------

CImageDataObject::CImageDataObject(HBITMAP hBitmap) : 
iRefCnt(1),
iRelease(FALSE)
{
    CoInitialize(NULL);

    STGMEDIUM stgm;
    stgm.tymed = TYMED_GDI;         // Storage medium = HBITMAP handle
    stgm.hBitmap = hBitmap;
    stgm.pUnkForRelease = NULL;     // Use ReleaseStgMedium

    FORMATETC fm;
    fm.cfFormat = CF_BITMAP;        // Clipboard format = CF_BITMAP
    fm.ptd = NULL;                  // Target Device = Screen
    fm.dwAspect = DVASPECT_CONTENT; // Level of detail = Full content
    fm.lindex = -1;                 // Index = Not applicaple
    fm.tymed = TYMED_GDI;           // Storage medium = HBITMAP handle

    SetData(&fm, &stgm, TRUE);
}

void* CImageDataObject::operator new (unsigned int aSize)
{ 
    return EcmtOsUtils::Alloc(aSize); 
}

void CImageDataObject::operator delete (void* aPtr)
{ 
    return EcmtOsUtils::Free(aPtr);
}

// Inserts bitmap into the current location in the RichEdit control
void CImageDataObject::Insert(IRichEditOle* pRichEditOle)
{
    IOleClientSite *pOleSite = NULL;
    pRichEditOle->GetClientSite(&pOleSite);
    if (pOleSite) {
        LPLOCKBYTES lpLockBytes = NULL;
        SCODE sc = CreateILockBytesOnHGlobal(NULL, TRUE, &lpLockBytes);
        if (sc == S_OK && lpLockBytes) {
            IStorage *pStor = NULL;
            sc = StgCreateDocfileOnILockBytes(lpLockBytes,
                STGM_SHARE_EXCLUSIVE|STGM_CREATE|STGM_READWRITE, 0, &pStor);
            if (sc == S_OK && pStor) {
                IOleObject* pOleObj = GetOleObject(pOleSite,pStor);
                if (pOleObj) {
                    OleSetContainedObject(pOleObj, TRUE);
                    CLSID clsid;
                    sc = pOleObj->GetUserClassID(&clsid);
                    if (sc == S_OK) {
                        REOBJECT reobject;
                        ZeroMemory(&reobject, sizeof(REOBJECT));
                        reobject.cbStruct = sizeof(REOBJECT);
                        reobject.clsid = clsid;
                        reobject.cp = REO_CP_SELECTION;
                        reobject.dvaspect = DVASPECT_CONTENT;
                        reobject.poleobj = pOleObj;
                        reobject.polesite = pOleSite;
                        reobject.pstg = pStor;
                        pRichEditOle->InsertObject(&reobject);
                    }
                    pOleObj->Release();
                }
                pStor->Release();
            }
            lpLockBytes->Release();
        }
        pOleSite->Release();
    }
}

IOleObject *CImageDataObject::GetOleObject(IOleClientSite *pOleSite,
                                           IStorage *pStorage)
{
    IOleObject *pOleObject = NULL;
    // OLERENDER_FORMAT is defined as olerender_format in CW 3.1 headers 
    OleCreateStaticFromData(this, IID_IOleObject, 2 /*OLERENDER_FORMAT*/,
        &iFormat, pOleSite, pStorage, (void**)&pOleObject);
    return pOleObject;
}

// IUnknown
STDMETHODIMP(CImageDataObject::QueryInterface)(REFIID iid, void ** ppvObject)
{
    if (iid == IID_IUnknown || iid == IID_IDataObject) {
        *ppvObject = this;
        AddRef();
        return S_OK;
    } else {
        return E_NOINTERFACE;
    }
}

ULONG CImageDataObject::AddRef(void)
{
    return ++iRefCnt;
}

ULONG CImageDataObject::Release(void)
{
    ULONG ref = --iRefCnt;
    if (!ref) delete this;
    return ref;
}

// IDataObject
STDMETHODIMP(CImageDataObject::GetData)(FORMATETC *, STGMEDIUM *pmedium)
{
    HANDLE hDst = OleDuplicateData(iStgMedium.hBitmap, CF_BITMAP, NULL);
    if (hDst == NULL) {
        return E_HANDLE;
    } else {
        pmedium->tymed = TYMED_GDI;
        pmedium->hBitmap = (HBITMAP)hDst;
        pmedium->pUnkForRelease = NULL;
        return S_OK;
    }
}

STDMETHODIMP(CImageDataObject::GetDataHere)(FORMATETC*, STGMEDIUM*)
{
    return E_NOTIMPL;
}

STDMETHODIMP(CImageDataObject::QueryGetData)(FORMATETC*)
{
    return E_NOTIMPL;
}

STDMETHODIMP(CImageDataObject::GetCanonicalFormatEtc)(FORMATETC*, FORMATETC*)
{
    return E_NOTIMPL;
}

STDMETHODIMP(CImageDataObject::SetData)(FORMATETC* pf, STGMEDIUM* pm, BOOL)
{
    iFormat = *pf;
    iStgMedium = *pm;
    return S_OK;
}

STDMETHODIMP(CImageDataObject::EnumFormatEtc)(DWORD, IEnumFORMATETC**)
{
    return E_NOTIMPL;
}

STDMETHODIMP(CImageDataObject::DAdvise)(FORMATETC*, DWORD, IAdviseSink*, DWORD*)
{
    return E_NOTIMPL;
}

STDMETHODIMP(CImageDataObject::DUnadvise)(DWORD)
{
    return E_NOTIMPL;
}

STDMETHODIMP(CImageDataObject::EnumDAdvise)(IEnumSTATDATA**)
{
    return E_NOTIMPL;
}
