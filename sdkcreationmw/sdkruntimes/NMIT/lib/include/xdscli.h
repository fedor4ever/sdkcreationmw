/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  XRPC interface
 *
*/


#ifndef _NMIT_XRPC_DSCLI_H_
#define _NMIT_XRPC_DSCLI_H_

/* DSCLI over XRPC protocol definition */
#define DSCLI_PROTOCOL              "com.nokia.DSCLI"
#define DSCLI_LOAD_METHOD           "load"
#define DSCLI_LOAD_URL_PARAM        "url"       /* string */
#define DSCLI_LOAD_BROWSER_PARAM    "browser"   /* int */
#define DSCLI_TO_FRONT_METHOD       "toFront"
#define DSCLI_RUN_METHOD            "run"
#define DSCLI_RUN_EXE_PARAM         "exe"       /* string */
#define DSCLI_RUN_ARGS_PARAM        "args"      /* string[] */
#define DSCLI_EXIT_METHOD           "exit"

/* Ecmt Gateway protocol definition */
#define ECMTGW_PROTOCOL             "com.nokia.EcmtGw"
#define ECMTGW_LOAD_METHOD          DSCLI_LOAD_METHOD
#define ECMTGW_LOAD_URL_PARAM       DSCLI_LOAD_URL_PARAM
#define ECMTGW_LOAD_PORT_PARAM      "port"      /* int */
#define ECMTGW_LOAD_BROWSER_PARAM   DSCLI_LOAD_BROWSER_PARAM
#define ECMTGW_TO_FRONT_METHOD      DSCLI_TO_FRONT_METHOD
#define ECMTGW_EXIT_METHOD          DSCLI_EXIT_METHOD

/* Browser IDs */
#define ECMTGW_BROWSER_NAME_KIMONO  "services"
#define ECMTGW_BROWSER_NAME_KHTML   "web"

/* Extensions */
#define ECMTGW_REGISTER_LISTENER_METHOD "registerListener"
#define ECMTGW_UNREGISTER_LISTENER_METHOD "unregisterListener"
#define ECMTGW_LISTENER_PROTOCOL_PARAM "protocol" /* string */
#define ECMTGW_LISTENER_MASK_PARAM "mask" /* int */

#define ECMTGW_LISTEN_MASK_SEND     0x00000002
#define ECMTGW_LISTEN_MASK_RECEIVE  0x00000001
#define ECMTGW_LISTEN_MASK_ALL      0xffffffff
#define ECMTGW_LISTEN_DEFAULT_MASK \
       (ECMTGW_LISTEN_MASK_SEND | \
        ECMTGW_LISTEN_MASK_RECEIVE)

/* com.nokia.EcmtGwListener */
#define ECMTGW_LISTENER_PROTOCOL "com.nokia.EcmtGwListener"

/* Invoked when messages are being sent to or received from the handset */
#define ECMTGW_LISTENER_SEND_METHOD "send"
#define ECMTGW_LISTENER_RECV_METHOD "receive"
/* both methods take the same parameters */
#define ECMTGW_LISTENER_UID_PARAM "uid"
#define ECMTGW_LISTENER_DATA_PARAM "data"

/* XRPC protocol properties */
#define DSCLI_LOGON_ID_PROP         "logon-id"
#define DSCLI_PROCESS_ID_PROP       "process-id"
#define DSCLI_PRODUCT_ID_PROP       "product-id"
#define DSCLI_INSTANCE_ID_PROP      "instance-id"
#define DSCLI_DEFAULT_INSTANCE_PROP "default-instance"
#define DSCLI_DEFAULT_INSTANCE_VALUE "true"

#endif /* _NMIT_XRPC_DSCLI_H_ */
