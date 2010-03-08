/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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


#include "EcmtGwP.h"
#include "GwXrpc.h"
#include "..\..\..\..\emulatorplugins\MidpDebug\inc\EcmtMidpDebugProtocol.h"
#include "..\..\..\..\emulatorplugins\MidpDebug\inc\EcmtMidpDebugPluginUids.h"

/*
 * Context for an asynchronous XRPC call.
 */
typedef struct _AsyncXRpcEntry {
    QEntry entry;
    XRpcString method;
    XRpcContainer* params;
} AsyncXRpcEntry;

/**
 * Actually submits the XRPC calls from the work thread.
 */
STATIC void GWENG_AsyncXRpc(WorkItem * w, void* arg)
{
    QEntry* e;
    MidpSession* session = arg;
    MUTEX_Lock(&session->xrpcMutex);
    while ((e = QUEUE_RemoveHead(&session->xrpcQueue)) != NULL) {
        AsyncXRpcEntry* a = QCAST(e,AsyncXRpcEntry,entry);
        MUTEX_Unlock(&session->xrpcMutex);
        XRPC_Notify(XRPC_GetClient(session->key.xrpcSession), 
            ECMTGW_SEI_PROTOCOL, a->method, 
            XRPC_ContainerToElement(a->params));
        XRPC_FreeContainer(a->params);
        MEM_Free(a);
        MUTEX_Lock(&session->xrpcMutex);
    }
    MUTEX_Unlock(&session->xrpcMutex);
}

/**
 * Submits asynchronous XRPC call. Doesn't wait until the call completes.
 * This resolves the deadlock between Ecmt Gateway and emulator.exe
 */
STATIC void GWENG_SubmitAsyncCall(MidpSession* midp, 
                                  XRpcString method,
                                  XRpcContainer* params)
{
    if (params) {
        AsyncXRpcEntry* a = MEM_New(AsyncXRpcEntry);
        if (a) {
            memset(a, 0, sizeof(*a));
            a->method = method;
            a->params = params;

            MUTEX_Lock(&midp->xrpcMutex);
            QUEUE_InsertTail(&midp->xrpcQueue, &a->entry);
            MUTEX_Unlock(&midp->xrpcMutex);

            if (!WKI_Submit(midp->xrpcWorkItem)) {

                /* 
                 * The work item is busy processing pending calls. It
                 * could be that GWENG_AsyncXRpc callback has already
                 * exited the loop but hasn't returned yet. In that case,
                 * this asynchronous call would remain in the queue until
                 * we submit the next one. That's not good. Try to "kick"
                 * it with another work item.
                 */
                WKQ_InvokeLater(midp->xrpcWorkThread, GWENG_AsyncXRpc, midp);
            }
        } else {
            XRPC_FreeContainer(params);
        }
    }
}

/**
 * Creates XRPC <params> container and adds sid and (optionally) cid 
 * parameters.
 */
STATIC XRpcContainer* GWENG_CreateXRpcParams(MidpSession* midp, 
                                             XRpcString sidElemName,
                                             XRpcString cidElemName,
                                             XRpcInt cid)
{
    XRpcContainer* params = XRPC_CreateContainer(XRpcTagParams);
    if (params) {
        XRpcElement* sidElem = _XRPC_CreateIntElement(midp->key.xrpcSid);
        if (sidElem) {
            if (XRPC_SetElementName(sidElem, sidElemName) &&
                XRPC_AddElement(params, sidElem)) {
                if (cidElemName) {
                    XRpcElement* cidElem = _XRPC_CreateIntElement(cid);
                    if (cidElem) {
                        if (XRPC_SetElementName(cidElem, cidElemName) &&
                            XRPC_AddElement(params, cidElem)) {
                            return params;
                        } else {
                            XRPC_FreeElement(cidElem);
                        }
                    }
                } else {
                    return params;
                }
            } else {
                XRPC_FreeElement(sidElem);
            }
        }
        XRPC_FreeContainer(params);
    }
    return NULL;
}

/**
 * Deallocates the MIDP session context.
 */
STATIC void GWENG_MidpFree(EcmtGateway* gw, MidpSession* midp)
{
    if (midp) {
        QEntry* e;
        VERIFY(HASH_Remove(&gw->midpSessionMap, &midp->key));
        HASH_Remove(&gw->ecmtSessionMap, (HashKey)midp->sid);
        HASH_Destroy(&midp->connMap);
        WKI_Cancel(midp->xrpcWorkItem);
        WKI_Detach(midp->xrpcWorkItem);
        WKQ_Delete(midp->xrpcWorkThread);
        MUTEX_Destroy(&midp->xrpcMutex);
        while ((e = QUEUE_RemoveHead(&midp->xrpcQueue)) != NULL) {
            AsyncXRpcEntry* a = QCAST(e,AsyncXRpcEntry,entry);
            XRPC_FreeContainer(a->params);
            MEM_Free(a);
        }
        MEM_Free(midp);
    }
}

/**
 * Fills ECMT packet header
 */
STATIC void GWENG_MidpFillHeader(char* h, I32u sid, I8u op)
{
    *((I32u*)(h + ECMT_MIDP_DEBUG_HEADER_SID_OFFSET)) = htonl(sid);
    h[ECMT_MIDP_DEBUG_HEADER_OPCODE_OFFSET] = op;
}

/**
 * Parses ECMT packet header. Returns the opcode
 */
STATIC I8u GWENG_MidpParseHeader(const char* h, I32u* sid)
{
    *sid = htonl(*((I32u*)(h + ECMT_MIDP_DEBUG_HEADER_SID_OFFSET)));
    return h[ECMT_MIDP_DEBUG_HEADER_OPCODE_OFFSET];
}

/**
 * Creates MidpSession for the specified XRPC client id. Invoked on the XRPC
 * thread under synchronization.
 */
STATIC MidpSession* GWENG_MidpCreateSession(EcmtGateway* gw, int xrpcSid)
{
    MidpSession* ses = MEM_New(MidpSession);
    if (ses) {
        LUID luid;
        /* Just in case if AllocateLocallyUniqueId fails... */
        luid.LowPart = (DWORD)ses;
        AllocateLocallyUniqueId(&luid);
        memset(ses, 0, sizeof(*ses));
        ses->key.xrpcSid = xrpcSid;
        ses->key.xrpcSession = XRPC_GetCurrentSession(gw->xrpc);
        ses->sid = luid.LowPart;
        ASSERT(!HASH_Contains(&gw->ecmtSessionMap,(HashKey)ses->sid));
        ASSERT(!HASH_Contains(&gw->midpSessionMap,&ses->key));
        if (HASH_Init(&ses->connMap, 1, NULL, NULL, hashFreeValueProc)) {
            /* Create context for the control connection (number zero) */
            MidpConnection* conn = MEM_New(MidpConnection);
            if (conn) {
                memset(conn, 0, sizeof(*conn));
                if (HASH_Put(&ses->connMap, (HashKey)0, conn)) {
                    if (HASH_Put(&gw->ecmtSessionMap,(HashKey)ses->sid,ses)) {
                        if (HASH_Put(&gw->midpSessionMap, &ses->key, ses)) {
                            if (MUTEX_Init(&ses->xrpcMutex)) {
                                ses->xrpcWorkThread = WKQ_Create();
                                if (ses->xrpcWorkThread) {
                                    ses->xrpcWorkItem = WKI_Create(
                                        ses->xrpcWorkThread, GWENG_AsyncXRpc,
                                        ses);
                                    if (ses->xrpcWorkItem) {
                                        QUEUE_Init(&ses->xrpcQueue);
                                        TRACE3("GW: new session %08x for "
                                            "%08x.%08x\n", ses->sid,
                                            ses->key.xrpcSession,
                                            ses->key.xrpcSid);
                                        return ses;
                                    }
                                    WKQ_Delete(ses->xrpcWorkThread);
                                }
                                MUTEX_Destroy(&ses->xrpcMutex);
                            }
                            HASH_Remove(&gw->midpSessionMap, &ses->key);
                        }
                        HASH_Remove(&gw->ecmtSessionMap, (HashKey)ses->sid);
                    }
                } else {
                    MEM_Free(conn);
                }
            }
            HASH_Destroy(&ses->connMap);
        }
        MEM_Free(ses);
    }
    return NULL;
}

/**
 * "open" method handler
 */
STATIC XRpcElement* GWENG_MidpOpen(void* ctx, const XRpcContainer* param)
{
    /* decode parameters */
    const XRpcIntElement* sidParam =
        XRPC_GetIntElementByName(param, 
        ECMTGW_SEI_SEND_SID_PARAM);

    if (sidParam) {
        EcmtGateway* gw = ctx;
        MidpSession* midp;
        
        MUTEX_Lock(&gw->mutex);
        TRACE2("GW: MidpOpen(%08x.%08x)\n",
            XRPC_GetCurrentSession(gw->xrpc), XRPC_GetInt(sidParam));
        midp = GWENG_MidpCreateSession(gw,XRPC_GetInt(sidParam));
        if (midp) {
            char p[ECMT_MIDP_DEBUG_OPEN_SIZE];
            GWENG_MidpFillHeader(p, midp->sid, ECMT_MIDP_DEBUG_OPCODE_OPEN);
            GWENG_QueueAdd(gw->handsetQueue,KECMT_MIDP_DEBUG_PLUGIN_UID,p,
                ECMT_MIDP_DEBUG_OPEN_SIZE);
        }
        MUTEX_Unlock(&gw->mutex);
    } else {
        TRACE("GW: open without cid!\n");
    }

    return NULL;
}

/**
 * "close" method handler
 */
STATIC XRpcElement* GWENG_MidpClose(void* ctx, const XRpcContainer* param)
{
    /* decode parameters */
    const XRpcIntElement* sidParam =
        XRPC_GetIntElementByName(param, 
        ECMTGW_SEI_SEND_SID_PARAM);

    if (sidParam) {
        EcmtGateway* gw = ctx;
        MidpSession* midp;

        MidpSessionKey key;
        key.xrpcSid = XRPC_GetInt(sidParam);
        key.xrpcSession = XRPC_GetCurrentSession(gw->xrpc);
        
        MUTEX_Lock(&gw->mutex);
        TRACE2("GW: MidpClose(%08x.%08x)\n",
            key.xrpcSession, key.xrpcSid);

        midp = HASH_Get(&gw->midpSessionMap,&key);
        if (midp) {
            char p[ECMT_MIDP_DEBUG_CLOSE_SIZE];
            GWENG_MidpFillHeader(p, midp->sid, ECMT_MIDP_DEBUG_OPCODE_CLOSE);
            GWENG_QueueAdd(gw->handsetQueue,KECMT_MIDP_DEBUG_PLUGIN_UID,p,
                ECMT_MIDP_DEBUG_CLOSE_SIZE);

            TRACE1("GW: closing session 0x%08lx (via XRPC)\n",midp->sid);
            GWENG_MidpFree(gw, midp);
        } else {
            TRACE("GW: XRPC close for non-existent session\n");
        }
        MUTEX_Unlock(&gw->mutex);

    } else {
        TRACE("GW: close without cid!\n");
    }

    return NULL;
}

/**
 * Sends "reset" event to both XRPC and ECMT clients and deallocates the 
 * connection context. Caller must hold the mutex.
 */
STATIC void GWENG_MidpResetConn(EcmtGateway* gw, MidpSession* midp, I32u cid,
                                Bool resetEcmt, Bool resetXrpc)
{
    TRACE1("GW: resetting connection %u\n",cid);
    HASH_Remove(&midp->connMap, (HashKey)cid);

    if (resetEcmt) {
        char pkt[ECMT_MIDP_DEBUG_RESET_SIZE];
        GWENG_MidpFillHeader(pkt, midp->sid, ECMT_MIDP_DEBUG_OPCODE_RESET);
        *((I32u*)(pkt+ECMT_MIDP_DEBUG_RESET_CID_OFFSET)) = htonl(cid);
        GWENG_QueueAdd(gw->handsetQueue, KECMT_MIDP_DEBUG_PLUGIN_UID, pkt,
            ECMT_MIDP_DEBUG_RESET_SIZE);
    }

    if (resetXrpc) {
        GWENG_SubmitAsyncCall(midp,
            ECMTGW_SEI_RESET_METHOD,
            GWENG_CreateXRpcParams(midp,
                ECMTGW_SEI_RESET_SID_PARAM, 
                ECMTGW_SEI_RESET_CID_PARAM, cid));
    }
}

/**
 * "send" method handler
 */
STATIC XRpcElement* GWENG_MidpSend(void* ctx, const XRpcContainer* param)
{
    /* decode parameters */
    const XRpcIntElement* sidParam =
        XRPC_GetIntElementByName(param, 
        ECMTGW_SEI_SEND_SID_PARAM);

    const XRpcIntElement* cidParam =
        XRPC_GetIntElementByName(param, 
        ECMTGW_SEI_SEND_CID_PARAM);

    const XRpcBinaryElement* dataParam =
        XRPC_GetBinaryElementByName(param, 
        ECMTGW_SEI_SEND_DATA_PARAM);

    if (sidParam && cidParam && dataParam) {
        MidpSession* midp;
        EcmtGateway* gw = ctx;
        size_t size = XRPC_GetBinaryDataSize(dataParam);
        I32u cid = XRPC_GetInt(cidParam);
        const XRpcByte* data = XRPC_GetBinaryData(dataParam);

        MidpSessionKey key;
        key.xrpcSid = XRPC_GetInt(sidParam);
        key.xrpcSession = XRPC_GetCurrentSession(gw->xrpc);

        TRACE4("GW: MidpSend(%08x.%08x.%u, %d bytes)\n",
            key.xrpcSession, key.xrpcSid, cid, size);
        DEBUG_ONLY(PRINT_Dump(DEBUG_Trace,data,size,0));

        MUTEX_Lock(&gw->mutex);
        midp = HASH_Get(&gw->midpSessionMap,&key);
        if (midp) {
            MidpConnection* conn = HASH_Get(&midp->connMap,(HashKey)cid);
            if (conn) {
                char h[ECMT_MIDP_DEBUG_SEND_DATA_OFFSET];
                I32u seq = conn->outCount++;
                GWENG_MidpFillHeader(h,midp->sid,ECMT_MIDP_DEBUG_OPCODE_SEND);
                *((I32u*)(h+ECMT_MIDP_DEBUG_SEND_CID_OFFSET)) = htonl(cid);
                *((I32u*)(h+ECMT_MIDP_DEBUG_SEND_SEQ_OFFSET)) = htonl(seq);
                GWENG_QueueAdd2(gw->handsetQueue, KECMT_MIDP_DEBUG_PLUGIN_UID,
                    h, ECMT_MIDP_DEBUG_SEND_DATA_OFFSET, data, size);
            } else {
                TRACE1("GW: invalid conn id %u\n",cid);
                GWENG_MidpResetConn(gw, midp, cid, False, True);
            }
        } else {
            TRACE2("GW: unexpected MIDP send (%08x.%08x)\n",
                key.xrpcSession, key.xrpcSid);
        }
        MUTEX_Unlock(&gw->mutex);
    }

    return NULL;
}

/**
 * "connect" method handler
 */
STATIC XRpcElement* GWENG_MidpConnect(void* ctx, const XRpcContainer* param)
{
    /* decode parameters */
    const XRpcIntElement* sidParam =
        XRPC_GetIntElementByName(param, 
        ECMTGW_SEI_CONNECT_SID_PARAM);

    const XRpcIntElement* cidParam =
        XRPC_GetIntElementByName(param, 
        ECMTGW_SEI_CONNECT_CID_PARAM);

    const XRpcShortElement* portParam =
        XRPC_GetShortElementByName(param, 
        ECMTGW_SEI_CONNECT_PORT_PARAM);

    if (sidParam && cidParam && portParam) {
        MidpSession* midp;
        EcmtGateway* gw = ctx;
        I32u cid = XRPC_GetInt(cidParam);
        Port port = XRPC_GetShort(portParam);

        MidpSessionKey key;
        key.xrpcSid = XRPC_GetInt(sidParam);
        key.xrpcSession = XRPC_GetCurrentSession(gw->xrpc);

        TRACE4("GW: MidpConnect(%08x.%08x.%u, port %hu)\n",
            key.xrpcSession, key.xrpcSid, cid, port);

        MUTEX_Lock(&gw->mutex);
        midp = HASH_Get(&gw->midpSessionMap,&key);
        if (midp) {
            MidpConnection* conn = MEM_New(MidpConnection);
            if (conn) {
                memset(conn, 0, sizeof(*conn));
                conn->connId = cid;
                if (HASH_Put(&midp->connMap, (HashKey)cid, conn)) {
                    char pkt[ECMT_MIDP_DEBUG_CONNECT_SIZE];
                    GWENG_MidpFillHeader(pkt,midp->sid,ECMT_MIDP_DEBUG_OPCODE_CONNECT);
                    *((I32u*)(pkt+ECMT_MIDP_DEBUG_CONNECT_CID_OFFSET)) = htonl(cid);
                    *((I16u*)(pkt+ECMT_MIDP_DEBUG_CONNECT_PORT_OFFSET)) = htons(port);
                    GWENG_QueueAdd(gw->handsetQueue, KECMT_MIDP_DEBUG_PLUGIN_UID, pkt,
                        ECMT_MIDP_DEBUG_CONNECT_SIZE);
                    MUTEX_Unlock(&gw->mutex);
                    return NULL;
                }
                MEM_Free(conn);
            }
            GWENG_MidpResetConn(gw, midp, cid, False, True);
        } else {
            TRACE3("GW: unexpected MIDP connect (%08x.%08x.%u)\n",
                key.xrpcSession, key.xrpcSid, cid);
        }
        MUTEX_Unlock(&gw->mutex);
    }

    return NULL;
}

/**
 * "reset" method handler
 */
STATIC XRpcElement* GWENG_MidpReset(void* ctx, const XRpcContainer* param)
{
    /* decode parameters */
    const XRpcIntElement* sidParam =
        XRPC_GetIntElementByName(param, 
        ECMTGW_SEI_CONNECT_SID_PARAM);

    const XRpcIntElement* cidParam =
        XRPC_GetIntElementByName(param, 
        ECMTGW_SEI_CONNECT_CID_PARAM);

    if (sidParam && cidParam) {
        MidpSession* midp;
        EcmtGateway* gw = ctx;
        I32u cid = XRPC_GetInt(cidParam);

        MidpSessionKey key;
        key.xrpcSid = XRPC_GetInt(sidParam);
        key.xrpcSession = XRPC_GetCurrentSession(gw->xrpc);

        TRACE3("GW: MidpReset(%08x.%08x.%u)\n",
            key.xrpcSession, key.xrpcSid, cid);

        MUTEX_Lock(&gw->mutex);
        midp = HASH_Get(&gw->midpSessionMap,&key);
        if (midp) {
            GWENG_MidpResetConn(gw, midp, cid, True, False);
        } else {
            TRACE3("GW: unexpected MIDP reset (%08x.%08x.%u)\n",
                key.xrpcSession, key.xrpcSid, cid);
        }
        MUTEX_Unlock(&gw->mutex);
    }

    return NULL;
}

/**
 * Handles ECMT packet coming from MIDP debug plugin
 */
STATIC void GWENG_MidpHandleEcmtPacket(EcmtGateway* gw,
                                       MidpSession* midp, 
                                       const I8u* pkt, 
                                       int pktlen)
{
    I8u opcode = pkt[ECMT_MIDP_DEBUG_HEADER_OPCODE_OFFSET];
    switch (opcode)
    {
    case ECMT_MIDP_DEBUG_OPCODE_SEND:
        ASSERT(pktlen >= ECMT_MIDP_DEBUG_SEND_MIN_SIZE);
        if (pktlen >= ECMT_MIDP_DEBUG_SEND_MIN_SIZE) {
            I32u cid = htonl(*((I32u*)(pkt+ECMT_MIDP_DEBUG_SEND_CID_OFFSET)));
            I32u seq = htonl(*((I32u*)(pkt+ECMT_MIDP_DEBUG_SEND_SEQ_OFFSET)));
            MidpConnection* conn = HASH_Get(&midp->connMap,(HashKey)cid);
            if (conn) {
                if (conn->inCount == seq) {
                    XRpcData data;
                    XRpcContainer* params;
                    conn->inCount++; /* increment sequence number */
                    data.ptr = (I8u*)pkt + ECMT_MIDP_DEBUG_SEND_DATA_OFFSET;
                    data.size = pktlen - ECMT_MIDP_DEBUG_SEND_DATA_OFFSET;
                    TRACE4("GW: MidpReceive(%08x.%08x.%u, %d bytes)\n",
                        midp->key.xrpcSession, midp->key.xrpcSid, cid,
                        data.size);
                    DEBUG_ONLY(PRINT_Dump(DEBUG_Trace,data.ptr,data.size,0));
                    params = GWENG_CreateXRpcParams(midp,
                        ECMTGW_SEI_SEND_SID_PARAM, 
                        ECMTGW_SEI_SEND_CID_PARAM, cid);
                    if (params) {
                        XRpcElement* dataElem = _XRPC_CreateBinaryElement(
                            data.ptr, data.size);
                        if (dataElem) {
                            if (XRPC_SetElementName(dataElem,
                                ECMTGW_SEI_SEND_DATA_PARAM) &&
                                XRPC_AddElement(params, dataElem)) {
                                /* submit the call */
                                GWENG_SubmitAsyncCall(midp, 
                                    ECMTGW_SEI_SEND_METHOD, 
                                    params);
                                break;
                            } else {
                                XRPC_FreeElement(dataElem);
                            }
                        }
                        XRPC_FreeContainer(params);
                    }
                    /* Terminate connection? That would probably fail too */
                } else {
                    TRACE2("GW: SEQ mismatch (expected %u, found %u)\n",
                        conn->inCount, seq);
                    GWENG_MidpResetConn(gw, midp, cid, True, True);
                }
            } else {
                TRACE1("GW: invalid conn id %u from Ecmt\n",cid);
                GWENG_MidpResetConn(gw, midp, cid, True, False);
            }
        }
        break;

    case ECMT_MIDP_DEBUG_OPCODE_CLOSE:
        ASSERT(pktlen == ECMT_MIDP_DEBUG_CLOSE_SIZE);
        TRACE1("GW: closing session 0x%08lx (via ECMT)\n",midp->sid);
        GWENG_SubmitAsyncCall(midp,
            ECMTGW_SEI_CLOSE_METHOD,
            GWENG_CreateXRpcParams(midp,
                ECMTGW_SEI_CLOSE_SID_PARAM, 
                NULL, 0));
        break;

    case ECMT_MIDP_DEBUG_OPCODE_RESET:
        ASSERT(pktlen == ECMT_MIDP_DEBUG_RESET_SIZE);
        if (pktlen == ECMT_MIDP_DEBUG_RESET_SIZE) {
            I32u cid = htonl(*((I32u*)(pkt+ECMT_MIDP_DEBUG_RESET_CID_OFFSET)));
            GWENG_SubmitAsyncCall(midp,
                ECMTGW_SEI_RESET_METHOD,
                GWENG_CreateXRpcParams(midp,
                    ECMTGW_SEI_RESET_SID_PARAM, 
                    ECMTGW_SEI_RESET_CID_PARAM, cid));
        }
        break;

    default:
        ASSMSG1("GW: unexpected opcode %u",opcode);
        break;
    }
}

/**
 * ECMT message filter
 */
Bool GWENG_MidpFilter(EcmtReader* reader, int uid, 
                      const void* data, int datalen)
{
    if (uid == KECMT_MIDP_DEBUG_GATEWAY_UID) {
        ASSERT(datalen >= ECMT_MIDP_DEBUG_HEADER_SIZE);
        if (datalen >= ECMT_MIDP_DEBUG_HEADER_SIZE) {
            MidpSession* midp;
            EcmtGateway* gw = reader->gw;
            I32u sid;
            I8u opcode = GWENG_MidpParseHeader(data,&sid);

            MUTEX_Lock(&gw->mutex);
            midp = HASH_Get(&gw->ecmtSessionMap,(HashKey)sid);
            if (midp) {
                GWENG_MidpHandleEcmtPacket(gw, midp, data, datalen);
            } else {
                TRACE1("GW: invalid SID 0x%08x\n",sid);
            }
            MUTEX_Unlock(&gw->mutex);
        }
        return False;
    } else {
        return True;
    }
}

/**
 * Method resolver for "com.nokia.SEI" protocol
 */
STATIC XRpcBool GWENG_MidpDebug(void* ctx, XRpcString m, XRpcMethodInfo* info)
{
    info->synchronous = XRpcTrue;
    info->context = ctx;
    if (StrCmp(m, ECMTGW_SEI_SEND_METHOD) == 0) {
        info->handler = GWENG_MidpSend;
        return XRpcTrue;
    } else if (StrCmp(m, ECMTGW_SEI_OPEN_METHOD) == 0) {
        info->handler = GWENG_MidpOpen;
        return XRpcTrue;
    } else if (StrCmp(m, ECMTGW_SEI_CLOSE_METHOD) == 0) {
        info->handler = GWENG_MidpClose;
        return XRpcTrue;
    } else if (StrCmp(m, ECMTGW_SEI_CONNECT_METHOD) == 0) {
        info->handler = GWENG_MidpConnect;
        return XRpcTrue;
    } else if (StrCmp(m, ECMTGW_SEI_RESET_METHOD) == 0) {
        info->handler = GWENG_MidpReset;
        return XRpcTrue;
    } else {
        return XRpcFalse;
    }
}

/**
 * "session finished" callback
 */
STATIC void GWENG_MidpSessionClose(void * context, XRpcSession * session)
{
    Iterator* i;
    EcmtGateway* gw = context;
    XRpcSession* xrpcSession = XRPC_GetCurrentSession(gw->xrpc);

    MUTEX_Lock(&gw->mutex);
    i = HASH_Values(&gw->ecmtSessionMap);
    if (i) {
        while (ITR_HasNext(i)) {
            MidpSession* midp = ITR_Next(i);
            if (midp->key.xrpcSession == xrpcSession) {
                char p[ECMT_MIDP_DEBUG_CLOSE_SIZE];
                TRACE1("GW: terminating session 0x%08lx\n",midp->sid);
                GWENG_MidpFillHeader(p,midp->sid,ECMT_MIDP_DEBUG_OPCODE_CLOSE);
                GWENG_QueueAdd(gw->handsetQueue,KECMT_MIDP_DEBUG_PLUGIN_UID,p,
                    ECMT_MIDP_DEBUG_CLOSE_SIZE);

                ITR_Remove(i);
                GWENG_MidpFree(gw, midp);
            }
        }
        ITR_Delete(i);
    }
    MUTEX_Unlock(&gw->mutex);
}

/** 
 * Register "com.nokia.SEI" protocol
 */
Bool GWENG_MidpRegisterXRpcProtocol(EcmtGateway* gw)
{
    XRpcProtocol protocol;
    memset(&protocol, 0, sizeof (protocol));
    protocol.getmethod = GWENG_MidpDebug;
    protocol.sclose = GWENG_MidpSessionClose;
    return XRPC_RegisterProtocol(gw->xrpc, ECMTGW_SEI_PROTOCOL,
        "EcmtGW SEI proxy", "Nokia", 1, 0, &protocol, gw);
}
