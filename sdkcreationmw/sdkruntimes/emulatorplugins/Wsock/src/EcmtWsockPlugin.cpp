/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Wsock configuration plugin 
*
*/


#include <ecom.h>
#include <ImplementationProxy.h>
#include "EcmtServerDebug.h"
#include "EcmtMessagingIf.h"
#include "EcmtWsockPlugin.h"
#include "EcmtWsockPluginUids.h"

// UIDs

const TUid KWsockPrefsUid = {KWSOCK_PREFS_UID};
const TUid KWsockPluginUid = {KWSOCKPLUGIN_UID};

//===========================================================================
// Respect Ecmt server-wide debug trace configuration
//===========================================================================

#ifdef ECMT_RDEBUG
#  define _REALLY_DEBUG
#else // !ECMT_RDEBUG
#  undef _REALLY_DEBUG
#endif // !ECMT_RDEBUG

//===========================================================================
// Debug trace
//===========================================================================

#define TRACE_PREFIX "EcmtWsockPlugin: "

#ifdef _REALLY_DEBUG

#  ifdef TRACE_PREFIX
#    if defined(_UNICODE)
#      define ___L(a,b) _L(a L##b)
#    else  // _UNICODE
#      define ___L(a,b) _L(a##b)
#    endif // _UNICODE
#    define __L(z) ___L(TRACE_PREFIX,z)
#  else
#    define __L(z) _L(z)
#  endif // TRACE_PREFIX

#  define TRACE(s)                  RDebug::Print(__L("%s"),_S(s))
#  define TRACE0(s)                 RDebug::Print(__L("%s"),_S(s))
#  define TRACE1(s,p1)              RDebug::Print(__L(s),p1)
#  define TRACE2(s,p1,p2)           RDebug::Print(__L(s),p1,p2)
#  define TRACE3(s,p1,p2,p3)        RDebug::Print(__L(s),p1,p2,p3)
#  define TRACE4(s,p1,p2,p3,p4)     RDebug::Print(__L(s),p1,p2,p3,p4)
#  define TRACE5(s,p1,p2,p3,p4,p5)  RDebug::Print(__L(s),p1,p2,p3,p4,p5)

#  define DEBUG_ONLY(x)             x

#else  /* !_DEBUG */

#  define TRACE(s)                  ((void)0)
#  define TRACE0(s)                 ((void)0)
#  define TRACE1(s,p1)              ((void)0)
#  define TRACE2(s,p1,p2)           ((void)0)
#  define TRACE3(s,p1,p2,p3)        ((void)0)
#  define TRACE4(s,p1,p2,p3,p4)     ((void)0)
#  define TRACE5(s,p1,p2,p3,p4,p5)  ((void)0)

#  define DEBUG_ONLY(x)

#endif /* !_DEBUG */

//===========================================================================
// Protocol constants
//===========================================================================

#define WSOCK_MESSAGE_TYPE_CONFIG  0
#define WSOCK_MESSAGE_TYPE_CONFIG_REQUEST 1
#define WSOCK_MESSAGE_TYPE_PROXY_CONFIG_REQUEST 2

_LIT(KWsockEnabledKey, "useWinsock");
_LIT(KWsockUseProxyKey, "winsockUseProxy");
_LIT(KWsockProxyServerKey, "winsockProxyServerName");
_LIT(KWsockProxyPortKey, "winsockProxyServerPort");

enum TKeyword {
    EKeywordNone,
    EKeywordUseWinsock,
    EKeywordUseProxy,
    EKeywordProxyHost,
    EKeywordProxyPort
};

_LIT(KBoolValueTrue, "True");
_LIT(KBoolValueFalse, "False");

static TKeyword WsockKeyword(const TDesC& aKeyword)
{
    if (aKeyword == KWsockEnabledKey()) return EKeywordUseWinsock;
    if (aKeyword == KWsockUseProxyKey()) return EKeywordUseProxy;
    if (aKeyword == KWsockProxyServerKey()) return EKeywordProxyHost;
    if (aKeyword == KWsockProxyPortKey()) return EKeywordProxyPort;
    TRACE1("unrecognized keyword: %S",&aKeyword);
    return EKeywordNone;
}

//===========================================================================
// Debug assert
//===========================================================================

#undef ASSERT
#ifdef _REALLY_DEBUG
void EcmtWsockAssert(const TText* s,const TText* file,TInt line);
void EcmtWsockLeaveIfError(TInt err,const TText* s,const TText* file,TInt line);
#  define __S(x) _S(x)
#  define ASSERT(x) ((void)((x)||(EcmtWsockAssert(_S(#x),__S(__FILE__),__LINE__),0),0))
#  define LEAVE_IF_ERROR(x) EcmtWsockLeaveIfError(x,_S(#x),__S(__FILE__),__LINE__)
#  define VERIFY_SUCCESS(x) ASSERT((x) == KErrNone)
#else  // _DEBUG
#  define ASSERT(x)         ((void)0)
#  define VERIFY_SUCCESS(x) (x)
#  define LEAVE_IF_ERROR(x) User::LeaveIfError(x)
#endif // _DEBUG

#ifdef _REALLY_DEBUG
extern "C" int __stdcall IsDebuggerPresent(void);
void EcmtWsockAssert(const TText* s,const TText* file,TInt line)
{
    TRACE1("Assertion failed: %s",s);
    TRACE1("File: %s",file);
    TRACE1("Line: %d",line);
    if (IsDebuggerPresent()) {
        __BREAKPOINT();
    }
}
void EcmtWsockLeaveIfError(TInt err,const TText* s,const TText* file,TInt line)
{
    if (err != KErrNone) {
        TRACE2("Error %d in %s",err,s);
        TRACE1("File: %s",file);
        TRACE1("Line: %d",line);
        if (IsDebuggerPresent()) {
            __BREAKPOINT();
        }
        User::Leave(err);
    }
}
#endif // _REALLY_DEBUG

//===========================================================================
// ESK files
//===========================================================================

_LIT8(KWsockDll8, "wsock.prt");
_LIT(KWsockDll, "wsock.prt");
_LIT(KEskFilePath, "c:\\private\\101f7989\\ESock\\ip.tcpip.esk");
_LIT(KOrigEskFilePath, "c:\\private\\101f7989\\ESock\\ip.tcpip.esk.orig");

_LIT8(KWsockEskData,"\
; This file is automatically generated by the SDK.\n\
; Do not edit!\n\
[sockman]\n\
protocols= tcp,udp,icmp,icmp6,tcp.ip6,udp.ip6,icmp.ip6,icmp6.ip6\n\
\n\
[tcp]\n\
filename= wsock.prt\n\
index= 1\n\
\n\
[udp]\n\
filename= wsock.prt\n\
index= 2\n\
\n\
[icmp]\n\
filename= wsock.prt\n\
index= 3\n\
\n\
[icmp6]\n\
filename= wsock.prt\n\
index= 4\n\
\n\
[tcp.ip6]\n\
filename= wsock6.prt\n\
index= 1\n\
\n\
[udp.ip6]\n\
filename= wsock6.prt\n\
index= 2\n\
\n\
[icmp.ip6]\n\
filename= wsock6.prt\n\
index= 3\n\
\n\
[icmp6.ip6]\n\
filename= wsock6.prt\n\
index= 4\n\
\n\
[nifman]\n\
default= wsocka\n\
\n\
[interfaces]\n\
wsocki= wsocki.nif\n\
");

_LIT8(KIpTcpEskData, "\
; This file is automatically generated by the SDK.\n\
; Do not edit!\n\
[sockman]\n\
protocols= tcp,udp,icmp,icmp6,ip,ip6,resolver\n\
\n\
[icmp]\n\
filename= tcpip6.prt\n\
bindto= ip6\n\
index= 7\n\
\n\
[ip]\n\
filename= tcpip6.prt\n\
bindto= ip6,resolver\n\
index= 6\n\
\n\
[resolver]\n\
filename= tcpip6.prt\n\
index= 5\n\
\n\
[tcp]\n\
filename= tcpip6.prt\n\
bindto= ip6\n\
index= 4\n\
\n\
[udp]\n\
filename= tcpip6.prt\n\
bindto= ip6\n\
index= 3\n\
\n\
[icmp6]\n\
filename= tcpip6.prt\n\
bindto= ip6\n\
index= 2\n\
\n\
[ip6]\n\
filename= tcpip6.prt\n\
index= 1\n\
bindto= resolver\n\
\n\
[nifman]\n\
default= csd\n\
\n\
[interfaces]\n\
ppp= ppp.nif\n\
");

//===========================================================================
// CEcmtWsockPlugin
//===========================================================================

CEcmtWsockPlugin::CEcmtWsockPlugin() : iMessaging(NULL)
{
}

CEcmtWsockPlugin::~CEcmtWsockPlugin()
{
    iRfs.Close();
    TRACE1("[%08X] destroyed",this);
}

// Symbian style constructor
MEcmtPlugin* CEcmtWsockPlugin::NewL()
{
    CEcmtWsockPlugin* self = new(ELeave)CEcmtWsockPlugin;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    TRACE1("[%08X] created",self);
    return self;
}

// Second stage construction
void CEcmtWsockPlugin::ConstructL()
{
    LEAVE_IF_ERROR(iRfs.Connect(1));
}

// Called by ECMT to initialize the plugin
void CEcmtWsockPlugin::BindMessagingL(MEcmtMessaging* aMessaging)
{
    iMessaging = aMessaging;
    iMessageSender = aMessaging->GetMessageSender();
    iMessaging->AddMessageEventObserverL(*this, KWsockPluginUid);
    SendCurrentSettings(EFalse);
    TRACE1("[%08X] initialized",this);
}

// Reads the ESK file, returns NULL on failure
HBufC8* CEcmtWsockPlugin::ReadEskFile()
{
    HBufC8* data = NULL;
    RFile file;
    TInt err = file.Open(iRfs, KEskFilePath(), EFileRead | EFileShareAny);
    ASSERT(err == KErrNone);
    if (err == KErrNone)
    {
        TInt size = 0;
        err = file.Size(size);
        ASSERT(err == KErrNone);
        if (err == KErrNone)
        {
            data = HBufC8::New(size);
            ASSERT(data);
            if (data)
            {
                TPtr8 des = data->Des();
                err = file.Read(des);
                ASSERT(err == KErrNone);
                if (err != KErrNone)
                {
                    delete data;
                    data = NULL;
                }
            }
        }
        file.Close();
    }
    return data;
}

// Writes the ESK file
TInt CEcmtWsockPlugin::WriteEskFile(const TDesC8& aData)
{
    // Create the backup file if it does not exist. Ignore errors
    iRfs.Rename(KEskFilePath(), KOrigEskFilePath());

    // Overwrite the existing file
    RFile file;
    TInt err = file.Replace(iRfs, KEskFilePath(), EFileWrite);
    ASSERT(err == KErrNone);
    if (err == KErrNone)
    {
        err = file.Write(aData);
        ASSERT(err == KErrNone);
        file.Close();
    }

    return err;
}

// Queries currentWsock settings, returns NULL on failure
// If the parameter is True, always reads proxy settings, regardless
// of whether WSOCK is enabled or not. Otherwise, only reads proxy
// settings if it finds WSOCK in the ESK file.
typedef TInt (*TQueryConfigFunction)(TBool&, HBufC*&, TUint&);
CEcmtWsockSettings* CEcmtWsockPlugin::QuerySettings(TBool aAlwaysReadProxy)
{
    CEcmtWsockSettings* settings = NULL;
    TInt flags = 0;

    // Determine what kind of ESK file is currently installed
    HBufC8* esk = ReadEskFile();
    if (esk)
    {
        if (esk->FindF(KWsockDll8()) > 0) 
        {
            TRACE1("[%08X] Winsock is enabled",this);
            flags |= CEcmtWsockSettings::EUseWinsock;
        }
        else
        {
            TRACE1("[%08X] Winsock is NOT enabled",this);
        }
        delete esk;
    }

    if (aAlwaysReadProxy || (flags & CEcmtWsockSettings::EUseWinsock))
    {
        // Now ask wsock.prt to read current proxy configuration from CommDb
        RLibrary wsock;
        TInt err = wsock.Load(KWsockDll());
        if (err == KErrNone)
        {
            const int KOrd = 4;
            TQueryConfigFunction q = (TQueryConfigFunction)wsock.Lookup(KOrd);
            if (q)
            {
                TBool useProxy = EFalse;
                HBufC* proxyHost = NULL;
                TUint proxyPort = 0;
                err = (*q)(useProxy, proxyHost, proxyPort);
                if (err == KErrNone)
                {
                    if (useProxy) flags |= CEcmtWsockSettings::EUseProxy;
                    settings = CEcmtWsockSettings::New(flags, 
                                                       proxyHost, 
                                                       proxyPort);
                }
            }
            else
            {
                TRACE3("[%08X] ordinal %d not in %S",this,KOrd,&KWsockDll());
            }
            wsock.Close();
        }
        else
        {
            TRACE3("[%08X] failed to load %S, err %d",this,&KWsockDll(),err);
        }
    }
    else
    {
        settings = CEcmtWsockSettings::New(flags, NULL, 0);
    }

    return settings;
}

// Applies the specified settings
typedef TInt (*SetConfigFunction)(TBool, TBool, const TDesC*, TUint);
void CEcmtWsockPlugin::ApplySettings(const CEcmtWsockSettings* aParam)
{
    // First write the ESK file
    WriteEskFile(aParam->UseWinsock() ? KWsockEskData() : KIpTcpEskData());

    // Now ask wsock.prt to update proxy configuration in CommDb
    RLibrary wsock;
    TInt err = wsock.Load(KWsockDll());
    if (err == KErrNone)
    {
        const int KOrd = 5;
        SetConfigFunction set = (SetConfigFunction)wsock.Lookup(KOrd);
        if (set)
        {
            (*set)(aParam->UseWinsock(),aParam->UseProxy(),
                   aParam->ProxyHost(),aParam->ProxyPort());
        }
        else
        {
            TRACE3("[%08X] ordinal %d not found in %S",this,KOrd,&KWsockDll());
        }
        wsock.Close();
    }
    else
    {
        TRACE3("[%08X] failed to load %S, err %d",this,&KWsockDll(),err);
    }
}

//===========================================================================
// ECMT protocol stuff
//===========================================================================

// Appends bytes to the buffer. Returns possibly reallocated buffer
const TInt KBufChunk = 512;
HBufC8* CEcmtWsockPlugin::Append(HBufC8* aBuf, const TDesC8& aData)
{
    TInt len = (aBuf ? aBuf->Length() : 0);
    TInt newLen = len + aData.Length();
    if (!aBuf || aBuf->Des().MaxLength() < newLen)
    {
        // Allocate buffer space in chunks
        TInt maxLen = ((newLen + KBufChunk - 1)/KBufChunk) * KBufChunk;
        HBufC8* aNewBuf = aBuf ? aBuf->ReAlloc(maxLen) : HBufC8::New(maxLen);
        ASSERT(aNewBuf);
        if (!aNewBuf) return aBuf;
        aBuf = aNewBuf;
    }

    aBuf->Des().Append(aData);
    return aBuf;
}

// The same thing as above but for 16-bit descriptors 
HBufC16* CEcmtWsockPlugin::Append(HBufC16* aBuf, const TDesC16& aData)
{
    TInt len = (aBuf ? aBuf->Length() : 0);
    TInt newLen = len + aData.Length();
    if (!aBuf || aBuf->Des().MaxLength() < newLen)
    {
        // Allocate buffer space in chunks
        TInt maxLen = ((newLen + KBufChunk - 1)/KBufChunk) * KBufChunk;
        HBufC16* aNewBuf = aBuf ? aBuf->ReAlloc(maxLen) : HBufC16::New(maxLen);
        ASSERT(aNewBuf);
        if (!aNewBuf) return aBuf;
        aBuf = aNewBuf;
    }

    aBuf->Des().Append(aData);
    return aBuf;
}

// Appends NULL terminated UCS2 string into a buffer in network (big endian,
// MSB first) byte order.
HBufC8* CEcmtWsockPlugin::AppendString(HBufC8* aBuf, const TDesC& aString)
{
    TText8 bytes[2];
    TPtrC8 des(bytes,2);
    TInt n = aString.Length();
    for (TInt i=0; i<n; i++)
    {
        TText16 c = aString[i];
        bytes[0] = (TText8)((c >> 8) & 0xff);
        bytes[1] = (TText8)(c & 0xff);
        aBuf = Append(aBuf, des);
    }
    bytes[0] = 0;
    bytes[1] = 0;
    return Append(aBuf, des);
}

// Appends a name-value pair for a boolean parameter
HBufC8* CEcmtWsockPlugin::AppendBool(HBufC8* aBuf,
                                     const TDesC& aName,
                                     TBool aValue)
{
    const TDesC& value = aValue ? KBoolValueTrue() : KBoolValueFalse();
    return AppendString(AppendString(aBuf, aName), value);
}

// Parsing helpers
TBool CEcmtWsockPlugin::ParseBool(const TDesC& aString, TBool& aBool)
{
    // Assume that string is already trimmed
    if (aString.CompareF(KBoolValueTrue()) == 0)
    {
        aBool = ETrue;
        return ETrue;
    }

    if (aString.CompareF(KBoolValueFalse()) == 0)
    {
        aBool = EFalse;
        return ETrue;
    }

    TRACE1("not a boolean value: %S",&aString);
    return EFalse;
}

TBool CEcmtWsockPlugin::ParseInt(const TDesC& aString, TUint& aNum)
{
    TLex lex(aString);
    return lex.Val(aNum) == KErrNone;
}

// Sends current settings to the preferences plugin. If the parameter is
// False, the information may or may not include the proxy settings, 
// depending on whether Winsock is enabled
void CEcmtWsockPlugin::SendCurrentSettings(TBool aProxyOnly)
{
    CEcmtWsockSettings* settings = QuerySettings(aProxyOnly);
    if (settings)
    {
        SendSettings(settings, aProxyOnly);
        delete settings;
    }
}

// Sends specified settings to the preferences plugin
void CEcmtWsockPlugin::SendSettings(const CEcmtWsockSettings* aSettings, 
                                    TBool aProxyOnly)
{
    ASSERT(iMessaging);
    ASSERT(iMessageSender);
    if (iMessaging && iMessageSender)
    {
        TText8 type = WSOCK_MESSAGE_TYPE_CONFIG;
        TPtrC8 typePtr(&type,1);
        HBufC8* buf = Append(NULL, typePtr);
        if (!aProxyOnly)
        {
            buf = AppendBool(buf, KWsockEnabledKey(), aSettings->UseWinsock());
            buf = AppendBool(buf, KWsockUseProxyKey(), aSettings->UseProxy());
        }

        const TDesC* proxy = aSettings->ProxyHost();
        if (proxy) {
            buf = AppendString(buf, KWsockProxyServerKey());
            buf = AppendString(buf, *proxy);
        }

        TUint port = aSettings->ProxyPort();
        if (port) {
            TBuf<16> portBuf;
            portBuf.Num(port);
            buf = AppendString(buf, KWsockProxyPortKey());
            buf = AppendString(buf, portBuf);
        }

        if (buf)
        {
            CEcmtMessageEvent* message;
            message = iMessaging->NewMessageEvent(KWsockPrefsUid,*buf);
            if (message)
            {
                TRACE1("[%08X] sending settings",this);
                iMessageSender->SendMessage(message);
            }
            delete buf;
        }
    }
}

// Parses WSOCK_MESSAGE_TYPE_CONFIG message
CEcmtWsockSettings* CEcmtWsockPlugin::ParseConfigMessage(const TPtrC8& aData)
{
    TBool useWinsock = ETrue;
    TBool useProxy = EFalse;
    HBufC* host = NULL;
    TUint port = 80;

    // The body contains null separated key-value pairs of UCS2
    // strings in network (big endian, MSB first) byte order
    TParseState state = EParseStart; // 0 = nothing, 1 = key, 2 = value
    TKeyword keyword = EKeywordNone;
    HBufC* buf = NULL;
    TInt len = aData.Length();

    TText16 c;
    TPtrC16 ptr(&c, 1);

    for (TInt i=1; i<len; i+=2) {
        TUint msb = aData[i-1];
        TUint lsb = aData[i];
        c = (TText16)((msb << 8) | lsb);
        if (c == 0) {
            if (buf) 
            {
                switch (state)
                {
                case EParseKey:
                    // We have received the keyword
                    keyword = WsockKeyword(*buf);
                    break;

                case EParseValue:
                    // Parse the value
                    switch (keyword)
                    {
                    case EKeywordUseWinsock:
                        buf->Des().Trim();
                        ParseBool(*buf, useWinsock);
                        break;
                    case EKeywordUseProxy:
                        buf->Des().Trim();
                        ParseBool(*buf, useProxy);
                        break;
                    case EKeywordProxyHost:
                        delete host;
                        host = buf->Alloc();
                        break;
                    case EKeywordProxyPort:
                        buf->Des().Trim();
                        ParseInt(*buf, port);
                        break;
                    }
                    break;
                }

                // Reset the buffer to start receiving next word
                buf->Des().SetLength(0);
            }

            // Switch to the next state
            state = (TParseState)((state+1) % EParseStates);
        } else {
            switch (state) {
            case EParseStart:  state = EParseKey; // No break
            case EParseKey:    // No break
            case EParseValue:  buf = Append(buf, ptr); break;
            }
        }
    }

    if (buf)
    {
        if (state == EParseValue)
        {
            // Handle the final key-value pair
            switch (keyword)
            {
            case EKeywordUseWinsock: 
                buf->Des().Trim();
                ParseBool(*buf, useWinsock);
                break;
            case EKeywordUseProxy:
                buf->Des().Trim();
                ParseBool(*buf, useProxy);
                break;
            case EKeywordProxyHost:
                delete host;
                host = buf->Alloc();
                break;
            case EKeywordProxyPort:
                buf->Des().Trim();
                ParseInt(*buf, port);
                break;
            }
        }
        delete buf;
    }

    // Construct the CEcmtWsockSettings object
    TInt flags = 0;
    if (useWinsock) flags |= CEcmtWsockSettings::EUseWinsock;
    if (useProxy) flags |= CEcmtWsockSettings::EUseProxy;
    CEcmtWsockSettings* settings = CEcmtWsockSettings::New(flags, host, port);
    if (!settings) delete host; // CEcmtWsockSettings takes ownership
    return settings;
}

// Handles WSOCK_MESSAGE_TYPE_CONFIG message
void CEcmtWsockPlugin::HandleConfigMessage(const TPtrC8& aData)
{
    CEcmtWsockSettings* settings = ParseConfigMessage(aData);
    if (settings)
    {
        ApplySettings(settings);
        delete settings;
    }
    SendCurrentSettings(EFalse);
}

// MEcmtMessageEventObserver
void CEcmtWsockPlugin::HandleNotifyL(const CEcmtMessageEvent& aEvent)
{
    const TPtrC8 msg = iMessaging->Message(aEvent);
    TInt len = msg.Length();
    if (len > 0)
    {
        TInt type = msg[0];
        TRACE3("[%08X] message type %d, %d byte(s)",this,type,len);
        switch (type)
        {
        case WSOCK_MESSAGE_TYPE_CONFIG:
            TRACE1("[%08X] config message received",this);
            HandleConfigMessage(msg.Right(len-1));
            break;
        case WSOCK_MESSAGE_TYPE_CONFIG_REQUEST:
            TRACE1("[%08X] config request received",this);
            ASSERT(len == 1);
            SendCurrentSettings(EFalse);
            break;
        case WSOCK_MESSAGE_TYPE_PROXY_CONFIG_REQUEST:
            TRACE1("[%08X] proxy config request received",this);
            ASSERT(len == 1);
            SendCurrentSettings(ETrue);
            break;
        default:
            TRACE1("[%08X] oops, unexpected message type",this);
            break;
        }
    }
}

//===========================================================================
// ECom stuff
//===========================================================================

static const TImplementationProxy ImplementationTable[] = 
{
    {{KWSOCKPLUGIN_IMPL_UID}, CEcmtWsockPlugin::NewL}
};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aCount)
{
    aCount = sizeof(ImplementationTable)/sizeof(ImplementationTable[0]);
    return ImplementationTable;
}

/*
 * Local Variables:
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
