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


#include "SdkCardDebug.h"

//===========================================================================
// Debugging support
//===========================================================================

#ifdef _REALLY_DEBUG

extern "C" int __stdcall IsDebuggerPresent(void);

// ASSERT
void SdkCardAssert(const TText* s, const TText* f, TInt l)
{
    TRACE1("Assertion failed: %s",s);
    TRACE1("File: %s",f);
    TRACE1("Line: %d",l);
    if (IsDebuggerPresent())
    {
        __BREAKPOINT();
    }
}

// ASSERT_SUCCESS
TInt SdkCardAssertSuccess(TInt err, const TText* s, const TText* f, TInt l)
{
    if (err != KErrNone)
    {
        TRACE1("Unexpected error: %d",err);
        TRACE1("Expression: %s",s);
        TRACE1("File: %s",f);
        TRACE1("Line: %d",l);
        if (IsDebuggerPresent())
        {
            __BREAKPOINT();
        }
    }
    return err;
}

// LEAVE
void SdkCardLeave(TInt err, const TText* s, const TText* f, TInt l)
{
    TRACE2("Leaving with error %d (%s)",err,s);
    TRACE1("File: %s",f);
    TRACE1("Line: %d",l);
    if (IsDebuggerPresent())
    {
        __BREAKPOINT();
    }
    User::Leave(err);
}

// LEAVE_IF_NULL
void SdkCardLeaveIfNull(void* ptr,const TText* s,const TText* f,TInt l)
{
    if (ptr != KErrNone)
    {
        TRACE1("Expression is NULL: %s",s);
        TRACE1("File: %s",f);
        TRACE1("Line: %d",l);
        if (IsDebuggerPresent())
        {
            __BREAKPOINT();
        }
        User::Leave(KErrNoMemory);
    }
}

// LEAVE_IF_ERROR
void SdkCardLeaveIfError(TInt err, const TText* s, const TText* f, TInt l)
{
    if (err != KErrNone)
    {
        TRACE1("Unexpected error: %d",err);
        TRACE1("Expression: %s",s);
        TRACE1("File: %s",f);
        TRACE1("Line: %d",l);
        if (IsDebuggerPresent())
        {
            __BREAKPOINT();
        }
        User::Leave(err);
    }
}

#endif // _REALLY_DEBUG
