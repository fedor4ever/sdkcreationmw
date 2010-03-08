/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Debug trace utilities
 *
*/


#include <utf.h>
#include "EcmtUids.h"
#include "EcmtServerDebugTrace.h"

#ifdef ECMT_RDEBUG

_LIT(KLogFile,"c:\\logs\\ecmt\\ecmtserver.log");

#ifdef __WINS__
#  ifdef EKA2
#    define BEGIN_WIN32() Emulator::Escape()
#    define END_WIN32() Emulator::Reenter()
#    include <emulator.h>
#  else
#    define BEGIN_WIN32() ((void)0)
#    define END_WIN32() ((void)0)
#  endif // EKA2
extern "C" int __stdcall IsDebuggerPresent(void);
#endif // __WINS__

const TInt KTls = KECMT_SERVER_UID;

class CEcmtServerDebugTrace::Truncate : public TDesOverflow
{
public:
    virtual void Overflow(TDes& /*aDes*/) {}
};

// Empty private constructor just to disallow instantiation
CEcmtServerDebugTrace::CEcmtServerDebugTrace() : iBufPtr8(NULL,0)
{
}

CEcmtServerDebugTrace::~CEcmtServerDebugTrace()
{
    if (iFlags & ELogFileOpen) iLogFile.Close();
    if (iFlags & EFsConnected) iFs.Close();
    delete iBuf8;
}

void CEcmtServerDebugTrace::Deinit()
{
    CEcmtServerDebugTrace* log = (CEcmtServerDebugTrace*)UserSvr::DllTls(KTls);
    if (log)
    {
        UserSvr::DllFreeTls(KTls);
        delete log;
    }
}

// Drop a line to the log file
void CEcmtServerDebugTrace::LogToFile(const TDesC* aString)
{
    TInt err;

    // Make sure file server is connected
    if (!(iFlags & EFsConnected))
    {
        err = iFs.Connect();
        if (err == KErrNone)
        {
            iFlags |= EFsConnected;
        }
    }    

    if (iFlags & EFsConnected)
    {

        // Make sure that log file is open
        if (!(iFlags & ELogFileOpen))
        {
            iFs.MkDirAll(KLogFile);
            err = iLogFile.Replace(iFs, KLogFile, EFileShareAny|EFileWrite);
            if (err == KErrNone)
            {
                iFlags |= ELogFileOpen;
            }
        }

        if (iFlags & ELogFileOpen)
        {
            // Check how long the text is when encoded as UTF8
            TPtr8 o(0,0);
            TInt len = CnvUtfConverter::ConvertFromUnicodeToUtf8(o,*aString);
            if (!iBuf8 || len > iBufPtr8.MaxLength())
            {
                delete iBuf8;
                iBuf8 = HBufC8::New(len);
                if (iBuf8)
                {
                    iBufPtr8.Set(iBuf8->Des());
                }
            }
            
            if (iBuf8)
            {
                // Convert USC2 string to UTF8
                iBufPtr8.SetLength(0);
                CnvUtfConverter::ConvertFromUnicodeToUtf8(iBufPtr8, *aString);

                // Actually write the file
                iLogFile.Write(iBufPtr8);
                iLogFile.Flush();
            }
        }
    }
}

void CEcmtServerDebugTrace::Print(const TText* aFormat, ...)
{
    // Send it to the debugger
    Truncate over;
    VA_LIST list;
    VA_START(list,aFormat);
    TPtrC fmt(aFormat);
    TBuf<1024> buf;
    _LIT(KEndOfLine,"\r\n");
    _LIT(KCopyFormat,"%S");
    buf.AppendFormatList(fmt,list,&over);
    buf.AppendFormat(KEndOfLine,&over);
    RDebug::Print(KCopyFormat,&buf);

    // Write it to the log file too
    CEcmtServerDebugTrace* log = (CEcmtServerDebugTrace*)UserSvr::DllTls(KTls);
    if (!log)
    {
        log = new CEcmtServerDebugTrace();
        if (log)
        {
            TInt err = UserSvr::DllSetTls(KTls, log);
            if (err != KErrNone)
            {
                delete log;
                log = NULL;
            }
        }
    }
    if (log)
    {
        log->LogToFile(&buf);
    }
}

void CEcmtServerDebugTrace::Assert(const TText* aExpression,
                                   const TText* aFileName,
                                   TInt aLine)
{
    TRACE1("ASSERT FAILED: %s", aExpression);
    TRACE1("  File: %s", aFileName);
    TRACE1("  Line: %d", aLine);
#ifdef __WINS__
    BEGIN_WIN32();
    if (IsDebuggerPresent())
    {
        __BREAKPOINT();
    }
    END_WIN32();
#endif // __WINS__
}

void CEcmtServerDebugTrace::Leave(TInt aError, 
                                  const TText* aExpression,
                                  const TText* aFileName,
                                  TInt aLineNumber)
{
    TRACE2("Leaving with error %d (%s)",aError,aExpression);
    TRACE1("File: %s",aFileName);
    TRACE1("Line: %d",aLineNumber);
#ifdef __WINS__
    BEGIN_WIN32();
    if (IsDebuggerPresent())
    {
        __BREAKPOINT();
    }
    END_WIN32();
#endif // __WINS__
    User::Leave(aError);
}
void CEcmtServerDebugTrace::LeaveIfError(TInt aError, 
                                       const TText* aExpression,
                                       const TText* aFileName,
                                       TInt aLine)
{
    if (aError != KErrNone)
    {
        TRACE1("LEAVE: %s", aExpression);
        TRACE1("  Error: %d", aError);
        TRACE1("  File: %s", aFileName);
        TRACE1("  Line: %d", aLine);
#ifdef __WINS__
        BEGIN_WIN32();
        if (IsDebuggerPresent())
        {
            __BREAKPOINT();
        }
        END_WIN32();
#endif // __WINS__
        User::Leave(aError);
    }
}

#endif // ECMT_RDEBUG

/**
 * Local Variables:
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
