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



/*
 * WDP bearer adapter for Java. Uses Java JNI to call java methods.
 *
 * NB: bearer adapter include files are devided into two parts: 
 *
 * 1) private include file and 
 * 2) public include file that can be distributed. 
 * In public include file only the bearer adapter factory should be shown.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>

#include "wps_mgmt.h"
#include "wps_type.h"	/* WPS pseudo types */
#include "wdp_api.h"	/* WDP interface structure */
#include "udp_bearer_adapter.h"	/* WDP interface structure */
#include "java_util.h"

#include "com_nokia_wap_sdk_http_GatewayLoader.h"	

JNIEXPORT void JNICALL Java_com_nokia_wap_sdk_http_GatewayLoader_installCBearer(JNIEnv *env, jclass clz) {
	WPS_STATUS status;

	JNI_DEBUG("Call   -> BearerInstall")	
	status = WPS_AttachBearer(udp_bearer_adapter_factory());	
	JNI_DEBUG_ARG1("  Return -> BearerInstall %d", status)
}
