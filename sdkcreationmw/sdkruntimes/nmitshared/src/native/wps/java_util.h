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


#ifndef _JAVA_UTIL_H
#define _JAVA_UTIL_H

#include <jni.h>

#define JNI_FIRST_BEARER_ID 8192

/* By default, define JNI_DEBUGGING in debug build */ 
#if !defined(JNI_DEBUGGING) && !defined(NO_JNI_DEBUGGGING) && defined(_DEBUG)
#  define JNI_DEBUGGING
#endif /* !JNI_DEBUGGING && !NO_JNI_DEBUGGGING && _DEBUG */

#ifdef JNI_DEBUGGING
#  define JNI_DEBUG(msg) printf("%s\n", msg);
#  define JNI_DEBUG_ARG1(msg, arg) printf(msg, arg);
#  define JNI_DEBUG_CMD(cmd) cmd
#else /* JNI_DEBUGGING */
#  define JNI_DEBUG(msg)
#  define JNI_DEBUG_CMD(cmd)
#  define JNI_DEBUG_ARG1(msg, arg)
#endif /* !JNI_DEBUGGING */

ULONG JNI_GetByteBuffer(JNIEnv *env, BYTE *dst, UINT16 *len, UINT16 max_len, 
	jobject byte_buffer);

ULONG JNI_SetByteBuffer(JNIEnv *env, BYTE *src, UINT16 len, jobject bytebuffer);

ULONG JNI_GetByteBuffer32(JNIEnv *env, BYTE *dst, UINT32 *len, UINT32 max_len, 
	jobject byte_buffer);

ULONG JNI_SetByteBuffer32(JNIEnv *env, BYTE *src, UINT32 len, jobject bytebuffer);

/*
 * This next section has been stolen from:
 *
 *	server/jni_servicelib/jni_setup_utils.h
 */

#define ASIZE(array,type) (sizeof(array)/sizeof(type))

/* 
 * Data types 
 */

typedef struct CLASSINIT {
    const char* name;
    jclass*     ptr;
} CLASSINIT;

typedef struct METHODINIT {
    jmethodID*  ptr;
    jclass*     classPtr;
    const char* name;
    const char* signature;
} METHODINIT;

typedef struct FIELDINIT {
    jfieldID*   ptr;
    jclass*     classPtr;
    const char* name;
    const char* signature;
} FIELDINIT;

/* 
 * Function   : StartupClassEnvironment
 *
 * Parameters : - env: java environment
 *              - classArray, classSize: classes
 *              - methodArray, methodSize: member methods
 *              - stMethodArray, stMethodSize: static methods
 *              - fieldArray, fieldSize: member fields
 *              - stFieldArray, stFieldSize: static fields
 * Returns    : - 0 on success,
 *              - 1 on failure
 */

jint StartupClassEnvironment(JNIEnv *env,
                             CLASSINIT* classArray, int classSize,
                             METHODINIT* methodArray, int methodSize,
                             METHODINIT* stMethodArray, int stMethodSize,
                             FIELDINIT*  fieldArray, int fieldSize,
                             FIELDINIT*  stFieldArray, int stFieldSize);

/* 
 * Function   : ShutdownClassEnvironment
 *
 * Parameters : - env: java environment
 *              - 
 * Returns    : - 0 on success,
 *              - 1 on failure
 */

jint ShutdownClassEnvironment(JNIEnv *env,
                              CLASSINIT* classArray, int classSize);

#endif /* _JAVA_UTIL_H */
