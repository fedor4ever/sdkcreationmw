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


#include "EcmtMidpDebugTrace.h"
#include "EcmtMessageEvent.h"
#include "EcmtUids.h"
#include <e32svr.h>
#include <utf.h>

#ifdef _MIDPDEBUG_TRACE

#ifdef __WINS__
#  ifdef EKA2
#    define BEGIN_WIN32() Emulator::Escape()
#    define END_WIN32() Emulator::Reenter()
#    include <emulator.h>
#    undef SendMessage
#  else
#    define BEGIN_WIN32() ((void)0)
#    define END_WIN32() ((void)0)
#  endif // EKA2
extern "C" int __stdcall IsDebuggerPresent(void);
#endif // __WINS__

const TInt KTls = KJAVA_SYSTEMOUT_UID;

class CEcmtMidpDebugTrace::Truncate : public TDesOverflow
{
public:
    virtual void Overflow(TDes& /*aDes*/) {}
};

CEcmtMidpDebugTrace::CEcmtMidpDebugTrace(MEcmtMessaging* aMessaging) :
iMessaging(aMessaging)
{
}

void CEcmtMidpDebugTrace::Init(MEcmtMessaging* aMessaging)
{
    if (aMessaging && !UserSvr::DllTls(KTls))
    {
        CEcmtMidpDebugTrace* trace = new CEcmtMidpDebugTrace(aMessaging);
        if (trace)
        {
            TInt err = UserSvr::DllSetTls(KTls, trace);
            if (err != KErrNone)
            {
                delete trace;
            }
        }
    }
}

void CEcmtMidpDebugTrace::Deinit()
{
    CEcmtMidpDebugTrace* trace = (CEcmtMidpDebugTrace*)UserSvr::DllTls(KTls);
    if (trace)
    {
        UserSvr::DllFreeTls(KTls);
        delete trace;
    }
}

// Send a text message to the SystemOut window in the Diagnostics
void CEcmtMidpDebugTrace::EcmtPrint(const TDesC* aString)
{
    // Check how long the text is when encoded as UTF8
    TPtr8 empty(0,0);
    TInt utf8len = CnvUtfConverter::ConvertFromUnicodeToUtf8(empty, *aString);

    // Allocate an Ecmt message that is long enough
    CEcmtMessageEvent* msg = iMessaging->NewMessageEvent(
        TUid::Uid(KJAVA_SYSTEMOUT_UID), utf8len);
    if (msg)
    {
        TPtr8 ptr(iMessaging->Message(*msg));
        CnvUtfConverter::ConvertFromUnicodeToUtf8(ptr, *aString);
        iMessaging->GetMessageSender()->SendMessage(msg);
    }
}

void CEcmtMidpDebugTrace::Print(const TText* aFormat, ...)
{
    Truncate over;
    VA_LIST list;
    VA_START(list,aFormat);
    TPtrC fmt(aFormat);
    TBuf<1024> buf;
    _LIT(KEndOfLine,"\n");
    _LIT(KCopyFormat,"%S");
    buf.AppendFormatList(fmt,list,&over);
    buf.AppendFormat(KEndOfLine,&over);
    RDebug::Print(KCopyFormat,&buf);

    // Send it to SystemOut window too
    CEcmtMidpDebugTrace* trace = (CEcmtMidpDebugTrace*)UserSvr::DllTls(KTls);
    if (trace)
    {
        trace->EcmtPrint(&buf);
    }
}

void CEcmtMidpDebugTrace::Assert(const TText* aExpression,
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

void CEcmtMidpDebugTrace::LeaveIfError(TInt aError, 
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

#endif // _MIDPDEBUG_TRACE

/**
 * Local Variables:
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
