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


#ifndef _JAVA_WPS_CLIENT_H_
#define _JAVA_WPS_CLIENT_H_ 1

#include <jni.h>
#include <stdlib.h>
#include <string.h>

/* WPS includes */
#include "wps_type.h"	/* WPS pseudo types */
#include "wdp_api.h"	/* WDP interface structure */
#include "wps_conn.h"
#include "wps_wspclapic.h"
#include "wps_wspcoapi.h"
#include "wps_mgmt.h"
#include "wps_slmgmt.h"
#include "wtls.h"
#include "wtperror.h"
#include "wdp_bearer_interface.h"	/* WDP interface structure */
#include "hc_api.h"			/* header encoding */
#include "wspconstants.h"

/* javah -jni generated files */
#include "com_nokia_comm_wap_wps_Wtls.h"
#include "com_nokia_comm_wap_wps_Stack.h"
#include "com_nokia_comm_wap_wps_Bearer.h"
#include "com_nokia_comm_wap_wps_Session.h"
#include "com_nokia_comm_wap_wps_Connection.h"
#include "com_nokia_comm_wap_wps_Transaction.h"
#include "com_nokia_comm_wap_wps_ExceptionEvent.h"
#include "com_nokia_wap_sdk_http_GatewayLoader.h"

/* other includes */
#include "java_bearer_adapter.h"
#include "java_util.h"

/* 
 * In different versions of WPS the different types for length of buffer.
 * It used to be UINT16, then (of course) it became UINT32. To be flexible,
 * let's define it here
 */ 
#ifdef WPS_1_4
typedef UINT16 Length;
#else
typedef UINT32 Length;
#endif /* WPSC_1_4 */

#endif /* _JAVA_WPS_CLIENT_H_ */
