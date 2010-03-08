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

#if DEBUG

/**
 * Signature validation functions used strictly for debugging purposes
 */
STATIC Bool JNI_IsObjectMethodSignature(const char * sig)
{
    int len = strlen(sig);
    if (len > 2) {
        switch(sig[len-1]) {
        case ';': 
            return True;
        case 'Z':
        case 'B':
        case 'C':
        case 'S':
        case 'I':
        case 'J':
        case 'F':
        case 'D':
            return sig[len-2] == '[';
        }
    }
    return False;
}

#endif /* DEBUG */

/**
 * Returns non-static method ID. ASSERTs if method does not exist.
 */
STATIC jmethodID JNI_GetMethodID(JNIEnv * env, jclass c, JavaMethod * m)
{
    if (!m->id) {
        m->id = (*env)->GetMethodID(env, c, m->met, m->sig);
        if (!m->id) {
            ASSMSG3("No such method: %s.%s%s", m->cls, m->met, m->sig);
            return NULL;
        }
    }
    return m->id;
}

/**
 * Returns static method ID. ASSERTs if method does not exist.
 */
STATIC jmethodID JNI_GetStaticMethodID(JNIEnv * env, jclass c, JavaMethod * m)
{
    if (!m->id) {
        m->id = (*env)->GetStaticMethodID(env, c, m->met, m->sig);
        if (!m->id) {
            ASSMSG3("No such static method: %s.%s%s", m->cls, m->met, m->sig);
            return  NULL;
        }
    }
    return m->id;
}

/***************************************************************************
 *       Calling Static Methods
 ***************************************************************************/

/**
 * Calls a static void method. Nothing else.
 * Returns True if method has been invoked, False if method descriptor 
 * is invalid or there's no environment.
 */
Bool 
JNI_CallStaticVoidMethod(
    JNIEnv * env, 
    JavaMethod * method, 
    ...) 
{
    ASSERT(env);
    ASSERT(method->sig[strlen(method->sig)-1] == 'V');
    JNI_ResetLastException();
    if (env) {
        jclass c = JNI_FindClass(env, method->cls);
        if (c) {
            jmethodID m = JNI_GetStaticMethodID(env, c, method);
            if (m) {
                va_list args;
                va_start(args, method);
                (*env)->CallStaticVoidMethodV(env, c, m, args);
                JNI_ClearException(env);
                va_end(args);
                return True;
            } else {
                ASSMSG3("No such method: %s.%s%s",method->cls,
                    method->met,method->sig);
            }
        }
    }
    return False;
}

/**
 * Calls a static boolean method. Nothing else.
 * Returns whatever method has returned, False if method descriptor 
 * is invalid or there's no environment.
 */
Bool 
JNI_CallStaticBooleanMethod(
    JNIEnv * env, 
    JavaMethod * method, 
    ...) 
{
    ASSERT(env);
    ASSERT(method->sig[strlen(method->sig)-1] == 'Z');
    JNI_ResetLastException();
    if (env) {
        jclass c = JNI_FindClass(env, method->cls);
        if (c) {
            jmethodID m = JNI_GetStaticMethodID(env, c, method);
            if (m) {
                jboolean result;
                va_list args;
                va_start(args, method);
                result = (*env)->CallStaticBooleanMethodV(env, c, m, args);
                JNI_ClearException(env);
                va_end(args);
                return (result == JNI_TRUE);
            } else {
                ASSMSG3("No such method: %s.%s%s",method->cls,
                    method->met,method->sig);
            }
        }
    }
    return False;
}

/**
 * Calls a static boolean method. Nothing else.
 * Returns whatever method has returned, zero if method descriptor 
 * is invalid or there's no environment.
 */
jint
JNI_CallStaticIntMethod(
    JNIEnv * env, 
    JavaMethod * method, 
    ...) 
{
    ASSERT(env);
    ASSERT(method->sig[strlen(method->sig)-1] == 'I');
    JNI_ResetLastException();
    if (env) {
        jclass c = JNI_FindClass(env, method->cls);
        if (c) {
            jmethodID m = JNI_GetStaticMethodID(env, c, method);
            if (m) {
                jint result;
                va_list args;
                va_start(args, method);
                result = (*env)->CallStaticIntMethodV(env, c, m, args);
                JNI_ClearException(env);
                va_end(args);
                return result;
            } else {
                ASSMSG3("No such method: %s.%s%s",method->cls,
                    method->met,method->sig);
            }
        }
    }
    return False;
}

/**
 * Calls a static object method. Nothing else.
 * Returns whatever method has returned, NULL if method descriptor 
 * is invalid or there's no environment.
 */
jobject 
JNI_CallStaticObjectMethod(
    JNIEnv * env, 
    JavaMethod * method, 
    ...)
{
    ASSERT(env);
    ASSERT(JNI_IsObjectMethodSignature(method->sig));
    JNI_ResetLastException();
    if (env) {
        jclass c = JNI_FindClass(env, method->cls);
        if (c) {
            jmethodID m = JNI_GetStaticMethodID(env, c, method);
            if (m) {
                jobject result;
                va_list args;
                va_start(args, method);
                result = (*env)->CallStaticObjectMethodV(env, c, m, args);
                JNI_ClearException(env);
                va_end(args);
                return result;
            }
        }
    }
    return NULL;
}

/***************************************************************************
 *       Calling Virtual Methods
 ***************************************************************************/

/**
 * Calls a non-static void method. Nothing else.
 * Returns True if method has been invoked, False if method descriptor 
 * is invalid or there's no environment.
 */
Bool 
JNI_CallVoidMethod(
    JNIEnv * env, 
    jobject obj, 
    JavaMethod * method, 
    ...) 
{
    ASSERT(env);
    ASSERT(obj);
    ASSERT(method->sig[strlen(method->sig)-1] == 'V');
    JNI_ResetLastException();
    if (env && obj) {
        jclass c = (*env)->GetObjectClass(env, obj);
        ASSERT(JNI_IsInstance(env, obj, method->cls));
        if (c) {
            jmethodID m = JNI_GetMethodID(env, c, method);
            if (m) {
                va_list args;
                va_start(args, method);
                (*env)->CallVoidMethodV(env, obj, m, args);
                JNI_ClearException(env);
                va_end(args);
                return True;
            }
        } else {
            ASSMSG1("No such class: %s",method->cls);
        }
    }
    return False;
}

/**
 * Calls a non-static boolean method. Nothing else.
 * Returns whatever method has returned, False if method descriptor 
 * is invalid or there's no environment.
 */
Bool 
JNI_CallBooleanMethod(
    JNIEnv * env, 
    jobject obj, 
    JavaMethod * method,
    ...) 
{
    ASSERT(env);
    ASSERT(obj);
    ASSERT(method->sig[strlen(method->sig)-1] == 'Z');
    JNI_ResetLastException();
    if (env && obj) {
        jclass c = (*env)->GetObjectClass(env, obj);
        ASSERT(JNI_IsInstance(env, obj, method->cls));
        if (c) {
            jmethodID m = JNI_GetMethodID(env, c, method);
            if (m) {
                jboolean result;
                va_list args;
                va_start(args, method);
                result = (*env)->CallBooleanMethodV(env, obj, m, args);
                JNI_ClearException(env);
                va_end(args);
                return (result == JNI_TRUE);
            }
        } else {
            ASSMSG1("No such class: %s",method->cls);
        }
    }
    return False;
}

/**
 * Calls a non-static int method. Nothing else.
 * Returns whatever method has returned, False if method descriptor 
 * is invalid or there's no environment.
 */
jint 
JNI_CallIntMethod(
    JNIEnv * env, 
    jobject obj, 
    JavaMethod * method,
    ...) 
{
    ASSERT(env);
    ASSERT(obj);
    ASSERT(method->sig[strlen(method->sig)-1] == 'I');
    JNI_ResetLastException();
    if (env && obj) {
        jclass c = (*env)->GetObjectClass(env, obj);
        ASSERT(JNI_IsInstance(env, obj, method->cls));
        if (c) {
            jmethodID m = JNI_GetMethodID(env, c, method);
            if (m) {
                jint result;
                va_list args;
                va_start(args, method);
                result = (*env)->CallIntMethodV(env, obj, m, args);
                JNI_ClearException(env);
                va_end(args);
                return result;
            }
        } else {
            ASSMSG1("No such class: %s",method->cls);
        }
    }
    return False;
}

/**
 * Calls a non-static object method. Nothing else.
 * Returns whatever method has returned, NULL if method descriptor 
 * is invalid or there's no environment.
 */
jobject 
JNI_CallObjectMethod(
    JNIEnv * env, 
    jobject obj, 
    JavaMethod * method, 
    ...)
{
    ASSERT(env);
    ASSERT(obj);
    ASSERT(JNI_IsObjectMethodSignature(method->sig));
    JNI_ResetLastException();
    if (env && obj) {
        jclass c = (*env)->GetObjectClass(env, obj);
        ASSERT(JNI_IsInstance(env, obj, method->cls));
        if (c) {
            jmethodID m = JNI_GetMethodID(env, c, method);
            if (m) {
                jobject result;
                va_list args;
                va_start(args, method);
                result = (*env)->CallObjectMethodV(env, obj, m, args);
                JNI_ClearException(env);
                va_end(args);
                return result;
            }
        }
    }
    return NULL;
}

/**
 * Calls a method that returns a string.
 */
Str
JNI_CallStringMethod(
    JNIEnv * env, 
    jobject obj, 
    StrBuf * sb,
    JavaMethod * method,
    ...)
{
    Str result = NULL;
    ASSERT(env);
    ASSERT(obj);
    ASSERT(!strcmp(method->sig+strlen(method->sig)-18,"Ljava/lang/String;"));
    JNI_ResetLastException();
    STRBUF_Clear(sb);
    if (env && obj) {
        jclass c = (*env)->GetObjectClass(env, obj);
        ASSERT(JNI_IsInstance(env, obj, method->cls));
        if (c) {
            jmethodID m = JNI_GetMethodID(env, c, method);
            if (m) {
                jstring str;
                va_list args;
                va_start(args, method);
                str = (*env)->CallObjectMethodV(env, obj, m, args);
                JNI_ClearException(env);
                if (str) {
                    result = JNI_GetString(env, str, sb);
                    (*env)->DeleteLocalRef(env,str);
                }
            }
        }
    }
    return result;
}
