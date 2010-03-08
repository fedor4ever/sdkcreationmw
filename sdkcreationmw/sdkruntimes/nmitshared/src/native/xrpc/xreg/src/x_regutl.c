/*
* Copyright (c) 2000 - 2005 Nokia Corporation and/or its subsidiary(-ies).
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


#include "x_reg.h"

/* *************************************************************************
//
//  Utilities shared by server and client code
//
// *************************************************************************/

/*
 * Finds the value of the property with the specified name. Returns NULL
 * if property is not found
 */
XRpcString
XREG_API
XREG_GetPropValue(
    const XRegProperty * props, /* properties to scan */
    int nprops,                 /* number of properties */
    XRpcString name)            /* name of the property */
{
    int i;
    for (i=0; i<nprops; i++) {
        if (strcmp(props[i].name, name) == 0) {
            return props[i].value;
        }
    }
    return NULL;
}

/*
 * Finds the value of the property with the specified name. Returns NULL
 * if property is not found
 */
XRpcString
XREG_API
XREG_GetServiceProp(
    const XRegServiceInfo* svc, /* service info */
    XRpcString name)            /* name of the property */
{
    return XREG_GetPropValue(svc->props, svc->nprops, name);
}

/**
 * Compares two XRegService structures, ignoring the properties
 */
Bool XREG_EqualService(const XRegService * s1, const XRegService * s2)
{
    if (!s1 && !s2) {
        return True;
    } else if (s1 && s2) {
        if (s1->port == s2->port && 
           !strcmp(s1->id, s2->id) && 
           !strcmp(s1->name, s2->name)) {
            if ((!s1->host && !s2->host) ||
                (s1->host && s2->host && !strcmp(s1->host,s2->host)) ||
                (s1->host && !s2->host && !strcmp(s1->host,"localhost")) ||
                (s2->host && !s1->host && !strcmp(s2->host,"localhost"))) {
                return True;
            }
        }
    }
    return False;
}

/* *************************************************************************
//
//  XRPC encoding
//
// *************************************************************************/

/**
 * Encodes property and adds it to the container
 */
STATIC XRpcBool XREG_EncodeProperty(XRpcContainer * a, Str name, Str value)
{
    XRpcContainer * c = XRPC_CreateStruct(XREG_STRUCT_PROPERTY);
    ASSERT(name && name[0] && value);
    if (c) {
        if (XRPC_AddStringField(c, XREG_STRUCT_PROPERTY_NAME, name) &&
            XRPC_AddStringField(c, XREG_STRUCT_PROPERTY_VALUE, value)) {
            if (XRPC_AddElement(a, XRPC_ContainerToElement(c))) {
                return XRpcTrue;
            }
        }
        XRPC_FreeContainer(c);
    }
    return XRpcFalse;
}

/**
 * Callback used by XREG_EncodeProp 
 */
STATIC Bool XREG_EncodePropCB(const PropData * entry, void * ctx)
{
    XRpcContainer * a = (XRpcContainer*)ctx;
    return XREG_EncodeProperty(a, entry->key, entry->value);
}

/**
 * Encodes Prop structure
 */
XRpcElement * XREG_EncodeProp(Prop * prop)
{
    XRpcContainer * a = XRPC_CreateArray();
    if (a) {
        if (PROP_Examine(prop, XREG_EncodePropCB, a)) {
            return XRPC_ContainerToElement(a);
        }
        XRPC_FreeContainer(a);
    }
    return NULL;
}

/**
 * Encodes two sets of properties
 */
XRpcElement * XREG_EncodeProps(Prop * p1, Prop * p2)
{
    XRpcContainer * a = XRPC_CreateArray();
    if (a) {
        if (PROP_Examine(p1, XREG_EncodePropCB, a) &&
            PROP_Examine(p2, XREG_EncodePropCB, a)) {
            return XRPC_ContainerToElement(a);
        }
        XRPC_FreeContainer(a);
    }
    return NULL;
}

/**
 * Encodes array of XRegProperties
 */
XRpcElement * XREG_EncodeProperties(const XRegProperty * p, int np)
{
    XRpcContainer * a = XRPC_CreateArray();
    if (a) {
        int i;
        for (i=0; i<np; i++) {
            if (!XREG_EncodeProperty(a, p[i].name, p[i].value)) {
                XRPC_FreeContainer(a);
                return NULL;
            }
        }
        return XRPC_ContainerToElement(a);
    }
    return NULL;
}

/* *************************************************************************
//
//  XRPC decoding
//
// *************************************************************************/

/**
 * Initializes XRegServiceInfo with service information. Caller must call
 * VECTOR_Destroy when it's done with XRegServiceInfo. The vector contains
 * the stuff dynamically allocated by this function 
 */
Bool 
XREG_InitRemoteServiceInfo(
    const XRpcContainer * svc,  /* XRPC-encoded xrpc.reg.ServiceInfo */
    XRegServiceInfo * info,     /* the structure to be initialized */
    Vector * propVector)        /* vector to store XRegProperty structures */
{
    const XRpcStringElement * se;
    memset(info, 0, sizeof(*info));
    if (VECTOR_Init(propVector,0,NULL,vectorFreeValueProc)) {
        se = XRPC_GetStringElementByName(svc,XREG_STRUCT_SERVICE_INFO_NAME);
        if (se) {
            info->name = XRPC_GetString(se);
            se = XRPC_GetStringElementByName(svc,XREG_STRUCT_SERVICE_INFO_ID);
            if (se) {
                const XRpcIntElement * ie;
                info->id = XRPC_GetString(se);
                ie = XRPC_GetIntElementByName(svc,
                     XREG_STRUCT_SERVICE_INFO_PORT);

                if (ie) {
                    int port = XRPC_GetInt(ie);
                    if (!(port & ~0xffff)) {
                        const XRpcContainer * a;
                        info->port = (XRpcPort)(port & 0xffff);
                        a = XRPC_IsArray(
                            XRPC_GetElementByName(svc,
                            XREG_STRUCT_SERVICE_INFO_PROP));

                        /* read the properties */
                        if (a) {
                            int i, n = XRPC_ElementCount(a);
                            for (i=0; i<n; i++) {
                                const XRpcContainer * pe = 
                                    XRPC_IsStruct(
                                    XRPC_GetElementAt(a, i), 
                                    XREG_STRUCT_PROPERTY);
                                if (pe) {
                                    XRegProperty * p = MEM_New(XRegProperty);
                                    if (p) {
                                        se = XRPC_GetStringElementByName(pe,
                                             XREG_STRUCT_PROPERTY_NAME);
                                        if (se) p->name = XRPC_GetString(se);
                                        se = XRPC_GetStringElementByName(pe,
                                             XREG_STRUCT_PROPERTY_VALUE);
                                        if (se) p->value = XRPC_GetString(se);
                                        if (p->name && p->value) {
                                            if (VECTOR_Add(propVector,p)) {
                                                continue;
                                            }
                                        }
                                        MEM_Free(p);
                                    }
                                }
                            }
                        }
                        info->nprops = VECTOR_Size(propVector);
                        if (info->nprops) {
                            info->props = MEM_NewArray(XRegProperty,info->nprops);
                            if (info->props) {
                                int i;
                                for (i=0; i<info->nprops; i++) {
                                    XRegProperty * p;
                                    p = (XRegProperty*)VECTOR_Get(propVector,i);
                                    info->props[i] = *p;
                                }

                                /* VECTOR_Add must succeed because we already
                                 * have something there, so after VECTOR_Clear
                                 * we will have at least two available slots 
                                 * in the buffer, because VECTOR_Clear doesn't
                                 * deallocate the buffer.
                                 */
                                VECTOR_Clear(propVector);
                                VERIFY(VECTOR_Add(propVector, info->props));
                            }
                        }
                        return True;
                    }
                }
            }
        }
        VECTOR_Destroy(propVector);
    }
    return False;
}

/* *************************************************************************
//
//  Event queue
//
// *************************************************************************/

/**
 * Initializes XRegWorkQueue
 */
Bool XREG_InitWorkQueue(XRegWorkQueue * q, Lock * lock)
{
    QUEUE_Init(&q->items);
    q->lock = lock;
    q->posted = False;
    q->q = WKQ_Create();
    if (q->q) {
        if (EVENT_Init(&q->event)) {
            EVENT_Set(&q->event);
            return True;
        }
        WKQ_Delete(q->q);
    }
    return False;
}

/**
 * Deinitializes XRegWorkQueue
 */
void XREG_DestroyWorkQueue(XRegWorkQueue * q)
{
    WKQ_Delete(q->q);
    EVENT_Destroy(&q->event);
}

/**
 * This callback invokes prevously submitted work items
 */
STATIC void XREG_WorkItemCB(WorkItem * w, void * arg)
{
    XRegWorkQueue * q = (XRegWorkQueue *)arg;
    QEntry * e;
    LOCK_WriteLock(q->lock);
    ASSERT(q->posted);
    while ((e = QUEUE_RemoveHead(&q->items)) != NULL) {
        XRegWorkItem * item = QCAST(e,XRegWorkItem,entry);
        EVENT_Reset(&q->event);
        LOCK_Unlock(q->lock);
        item->cb->run(item);
        EVENT_Set(&q->event);
        LOCK_WriteLock(q->lock);
    }
    q->posted = False;
    LOCK_Unlock(q->lock);
}

/**
 * Post a work item
 */
void XREG_PostWorkItem(XRegWorkQueue * q, XRegWorkItem * w)
{
    LOCK_WriteLock(q->lock);
    QUEUE_InsertTail(&q->items, &w->entry);
    if (!q->posted) {
        q->posted = WKQ_InvokeLater(q->q, XREG_WorkItemCB, q);
    }
    LOCK_Unlock(q->lock);
}

/**
 * Cancel pending work items
 */
void XREG_ResetEventQueue(XRegWorkQueue * q)
{
    QEntry * e;
    WKQ_Cancel(q->q);
    LOCK_WriteLock(q->lock);
    while ((e = QUEUE_RemoveHead(&q->items)) != NULL) {
        XRegWorkItem * item = QCAST(e,XRegWorkItem,entry);
        item->cb->cancel(item);
    }
    q->posted = False;
    LOCK_Unlock(q->lock);

    /* we should not be holding the lock when we wait */
    ASSERT(LOCK_CanRead(q->lock));
    EVENT_Wait(&q->event);
}

/*
 * Local Variables:
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
