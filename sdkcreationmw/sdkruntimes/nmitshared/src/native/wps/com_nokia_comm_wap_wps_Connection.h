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


/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com_nokia_comm_wap_wps_Connection */

#ifndef _Included_com_nokia_comm_wap_wps_Connection
#define _Included_com_nokia_comm_wap_wps_Connection
#ifdef __cplusplus
extern "C" {
#endif
/* Inaccessible static: cExceptionMessage */
/* Inaccessible static: lExceptionMessage */
/*
 * Class:     com_nokia_comm_wap_wps_Connection
 * Method:    wpsConnectionEqual
 * Signature: (II)Z
 */
JNIEXPORT jboolean JNICALL Java_com_nokia_comm_wap_wps_Connection_wpsConnectionEqual
  (JNIEnv *, jobject, jint, jint);

/*
 * Class:     com_nokia_comm_wap_wps_Connection
 * Method:    wpsCreateBoundConnection
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_nokia_comm_wap_wps_Connection_wpsCreateBoundConnection
  (JNIEnv *, jobject);

/*
 * Class:     com_nokia_comm_wap_wps_Connection
 * Method:    wpsCreateConnection
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_nokia_comm_wap_wps_Connection_wpsCreateConnection
  (JNIEnv *, jobject);

/*
 * Class:     com_nokia_comm_wap_wps_Connection
 * Method:    wpsIsValidConnection
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL Java_com_nokia_comm_wap_wps_Connection_wpsIsValidConnection
  (JNIEnv *, jobject, jint);

/*
 * Class:     com_nokia_comm_wap_wps_Connection
 * Method:    wpsReleaseConnection
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_comm_wap_wps_Connection_wpsReleaseConnection
  (JNIEnv *, jobject, jint);

/*
 * Class:     com_nokia_comm_wap_wps_Connection
 * Method:    wspUnitInvoke
 * Signature: (Lcom/nokia/comm/wap/wps/UnitInvokePacket;)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_comm_wap_wps_Connection_wspUnitInvoke
  (JNIEnv *, jobject, jobject);

/*
 * Class:     com_nokia_comm_wap_wps_Connection
 * Method:    wspUnitPush
 * Signature: (Lcom/nokia/comm/wap/wps/UnitPushPacket;)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_comm_wap_wps_Connection_wspUnitPush
  (JNIEnv *, jobject, jobject);

/*
 * Class:     com_nokia_comm_wap_wps_Connection
 * Method:    wspUnitResult
 * Signature: (Lcom/nokia/comm/wap/wps/UnitResultPacket;)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_comm_wap_wps_Connection_wspUnitResult
  (JNIEnv *, jobject, jobject);

/*
 * Class:     com_nokia_comm_wap_wps_Connection
 * Method:    wspUnitWaitInvoke
 * Signature: (Lcom/nokia/comm/wap/wps/UnitInvokePacket;I)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_comm_wap_wps_Connection_wspUnitWaitInvoke
  (JNIEnv *, jobject, jobject, jint);

/*
 * Class:     com_nokia_comm_wap_wps_Connection
 * Method:    wspUnitWaitPush
 * Signature: (Lcom/nokia/comm/wap/wps/UnitPushPacket;I)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_comm_wap_wps_Connection_wspUnitWaitPush
  (JNIEnv *, jobject, jobject, jint);

/*
 * Class:     com_nokia_comm_wap_wps_Connection
 * Method:    wspUnitWaitResult
 * Signature: (Lcom/nokia/comm/wap/wps/UnitResultPacket;I)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_comm_wap_wps_Connection_wspUnitWaitResult
  (JNIEnv *, jobject, jobject, jint);

/*
 * Class:     com_nokia_comm_wap_wps_Connection
 * Method:    wtlsSetAutoAccept
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL Java_com_nokia_comm_wap_wps_Connection_wtlsSetAutoAccept
  (JNIEnv *, jobject, jint);

#ifdef __cplusplus
}
#endif
#endif