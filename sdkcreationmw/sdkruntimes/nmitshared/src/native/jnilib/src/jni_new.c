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


#include "jni_libp.h"

/***************************************************************************
 *       Creating New Objects
 ***************************************************************************/

/**
 * Creates a new object using the default constructor.
 */
jobject 
JNI_CreateObject(
    JNIEnv * env, 
    const char * className)
{
    return JNI_NewObject(env, className, "()V");
}

/**
 * Creates a new object using the specified constructor and
 * arguments.
 */
jobject 
JNI_NewObject(
    JNIEnv * env, 
    const char * className, 
    const char * sig, 
    ...)
{
    jobject obj;
    va_list args;
    va_start(args, sig);
    JNI_ResetLastException();
    obj = JNI_NewObjectV(env, className, sig, args);
    JNI_ClearException(env);
    ASSERT(obj || JNI_ExceptionOccurred());
    va_end(args);
    return obj;
}

/**
 * Creates a new object using the specified constructor and
 * arguments.
 */
jobject 
JNI_NewObjectV(
    JNIEnv * env, 
    const char * className, 
    const char * sig, 
    va_list args)
{
    jclass c = JNI_FindClass(env, className);
    ASSERT(sig[strlen(sig)-1] == 'V');
    JNI_ResetLastException();
    if (c) {
        jmethodID m = (*env)->GetMethodID(env, c, "<init>", sig);
        if (m) {
            jobject obj = (*env)->NewObjectV(env, c, m, args);
            JNI_ClearException(env);
            ASSERT(obj || JNI_ExceptionOccurred());
            return obj;
        } else {
            ASSMSG2("No such constructor: %s.<init>%s",className,sig);
        }
    }
    return NULL;
}

/**
 * Creates a new object array
 */
jobjectArray
JNI_NewObjectArray(
    JNIEnv * env, 
    const char * className,
    int len)
{
    jobjectArray a = NULL;
    jclass c = JNI_FindClass(env, className);
    JNI_ResetLastException();
    if (c) {
        a = (*env)->NewObjectArray(env, len, c, NULL);
        JNI_ClearException(env);
        ASSERT(a || JNI_ExceptionOccurred());
    }
    return a;
}

/**
 * Creates a new byte array
 */
jbyteArray
JNI_NewByteArray(
    JNIEnv * env,
    const uint8_t * data,
    int len)
{
    jbyteArray byteArray;
    JNI_ResetLastException();
    byteArray = (*env)->NewByteArray(env, len);
    JNI_ClearException(env);
    ASSERT(byteArray || JNI_ExceptionOccurred());
    if (byteArray) {
        if (data && len > 0) {
            (*env)->SetByteArrayRegion(env, byteArray, 0, len, (jbyte*)data);
        }
    }
    return byteArray;
}
