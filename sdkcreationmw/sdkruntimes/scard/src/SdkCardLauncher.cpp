/*
* Copyright (c) 2003-2005 Nokia Corporation and/or its subsidiary(-ies).
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


#include "SdkCardLauncher.h"
#include "SdkCardReader.h"

#define TRACE_PREFIX "SCARD: [Launcher] "
#include "SdkCardDebug.h"

/**
 * Constructor.
 */
SdkScardLauncher::SdkScardLauncher()
{
}

/**
 * Destructor.
 */
SdkScardLauncher::~SdkScardLauncher()
{
    if (iServiceContext)
    {
        SCardReleaseContext(iServiceContext);
        iServiceContext = NULL;
    }
    iReaders.ResetAndDestroy();
    delete [] iReaderNames;
}

/**
 * Reader service object is the one that Launcher can query needed 
 * information to connect reader for framework wide services.
 */
void SdkScardLauncher::ConstructL(MScardReaderService* aReaderService)
{
    Emulator::Escape();
    LONG err = SCardEstablishContext(SCARD_SCOPE_USER,0,0,&iServiceContext);
    Emulator::Reenter();
    if (err != SCARD_S_SUCCESS)
    {
        TRACE1("SCardEstablishContext error 0x%08x",err);
        LEAVE(KErrNotSupported);
    }

    TUint32 len = 0;
    Emulator::Escape();
    err = SCardListReaders(iServiceContext, 0, NULL, &len);
    Emulator::Reenter();
    if (err != SCARD_S_SUCCESS)
    {
        TRACE1("SCardListReaders error 0x%08x",err);
        LEAVE(KErrNotSupported);
    }
    if (len > 0)
    {
        iReaderNames = new(ELeave)TText[len];
        Emulator::Escape();
        err = SCardListReaders(iServiceContext, 0, iReaderNames, &len);
        Emulator::Reenter();
        if (err != SCARD_S_SUCCESS)
        {
            TRACE1("SCardListReaders error 0x%08x",err);
            LEAVE(KErrNotSupported);
        }
        iReaderNames[len-1] = 0; // Just in case

        // Create card readers
        const TText* p = iReaderNames;
        const TText* end = iReaderNames + len;
        while (p < end)
        {
            // Empty string usually means the end of the list
            if (*p)
            {
                CSdkCardReader* reader = CSdkCardReader::NewL(this, p);
                CleanupStack::PushL(reader);
                TPtrC name(p);
                _LIT(KVirtual,"virtual");
                if (name.FindC(KVirtual) >= 0)
                {
                    // Virtual stuff goes to the end of the list
                    LEAVE_IF_ERROR(iReaders.Append(reader));
                }
                else
                {
                    LEAVE_IF_ERROR(iReaders.Insert(reader,0));
                }
                CleanupStack::Pop(reader);
            }
            p += User::StringLength(p) + 1;
        }
    }
    iObserver = aReaderService->NotifyObserver();
}

/**
 * Create specific reader object, just create not attach the physical
 * reader device, Open() in Reader interface will do this.
 */
MScardReader* SdkScardLauncher::CreateReaderL(TReaderID aReaderID)
{
    for (TInt i=0; i<iReaders.Count(); i++)
    {
        CSdkCardReader* reader = iReaders[i];
        if (!reader->HasId())
        {
            TRACE2("reader 0x%08x (%u) created",reader,aReaderID);
            reader->SetId(aReaderID);
            reader->AddRef();
            return reader;
        }
    }
    TRACE1("can't create reader, invalid id %u",aReaderID);
    LEAVE(KErrNotSupported);
    return NULL;
}

/**
 * Delete the reader object, should be Closed() before this is called
 */
void SdkScardLauncher::DeleteReader(TReaderID aReaderID)
{
    TRACE1("DeleteReader(%u)",aReaderID);
    for (TInt i=0; i<iReaders.Count(); i++)
    {
        CSdkCardReader* reader = iReaders[i];
        if (reader->MatchId(aReaderID))
        {
            TInt ref = reader->ReleaseRef();
            if (!ref)
            {
                TRACE2("deleted reader 0x%08x (%u)",reader,aReaderID);
                iReaders.Remove(i);
            }
            else
            {
                TRACE3("reader 0x%08x(%u) ref count %d",reader,aReaderID,ref);
            }
            return;
        }
    }
    TRACE1("invalid reader id %u",aReaderID);
}

/**
 * Event notification from CSdkCardReader
 */
void SdkScardLauncher::NotifyCardEvent(TScardServiceStatus aEvent,
                                       CSdkCardReader* aReader)
{
    if (aReader->HasId())
    {
        TRACE3("reader 0x%08x (%u) event %d ",aReader,aReader->Id(),aEvent);
        iObserver->NotifyCardEvent(aEvent, aReader->Id());
    }
    else
    {
        TRACE2("reader 0x%08x event %d ",aReader,aEvent);
    }
    if (aEvent == EReaderRemoved)
    {
        TInt index = iReaders.Find(aReader);
        ASSERT(index >= 0);
        if (index >= 0)
        {
            TInt ref = aReader->ReleaseRef();
            if (!ref)
            {
                TRACE1("deleted reader 0x%08x",aReader);
                iReaders.Remove(index);
            }
            else
            {
                TRACE2("reader 0x%08x ref count %d",aReader,ref);
            }
        }
    }
}

// Entry point #1
extern "C" EXPORT_C MScardReaderLauncher * NewSdkScardLauncher()
{
    return new SdkScardLauncher();
}
