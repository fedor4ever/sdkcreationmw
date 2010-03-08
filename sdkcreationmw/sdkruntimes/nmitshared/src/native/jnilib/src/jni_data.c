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
 *       Getting And Setting Fields
 ***************************************************************************/

#if DEBUG

STATIC Bool JNI_IsValidFieldSignature(JNIEnv * env, const char * sig)
{
    Bool valid = False;
    int len;
    while (sig[0] == '[') sig++;
    len = strlen(sig);
    if (len > 0) {
        switch(sig[0]) {
        case 'Z':
        case 'B':
        case 'C':
        case 'S':
        case 'I':
        case 'J':
        case 'F':
        case 'D':
            valid = (len == 1);
            break;
        case 'L':
            if (sig[len-1] == ';') {
                char * tmp = MEM_NewArray(char,len-1);
                if (tmp) {
                    strncpy(tmp, sig+1, len-2);
                    tmp[len-2] = 0;
                    valid = (JNI_FindClass(env, tmp) != NULL);
                    MEM_Free(tmp);
                }
            }
            break;
        }
    }
    return valid;
}

#endif /* DEBUG */

STATIC jfieldID JNI_GetStaticField(JNIEnv * env, JavaField * f)
{
    ASSERT(env);
    if (!f->id && !f->c) {
        f->c = JNI_FindClass(env, f->cls);
        if (f->c) {
            ASSERT(JNI_IsValidFieldSignature(env, f->sig));
            f->id = (*env)->GetStaticFieldID(env, f->c, f->name, f->sig);
            ASSERT(f->id);
        }
    }
    return f->id;
}

/**
 * Returns the value of a static object field
 */
jobject
JNI_GetStaticObjectField(
    JNIEnv * env, 
    JavaField * field)
{
    if (JNI_GetStaticField(env, field)) {
        ASSERT(strlen(field->name) > 1);
        return (*env)->GetStaticObjectField(env, field->c, field->id);
    }
    return NULL;
}

/**
 * Returns the value of a static int field
 */
jint
JNI_GetStaticIntField(
    JNIEnv * env, 
    const char * className, 
    const char * name)
{
    ASSERT(env);
    ASSERT(name);
    if (env) {
        jclass c = JNI_FindClass(env, className);
        if (c) {
            jfieldID f = (*env)->GetStaticFieldID(env, c, name, "I");
            if (f) {
                return (*env)->GetStaticIntField(env, c, f);
            } else {
                ASSMSG1("No such field: %s",name);
            }
        }
    }
    return 0;
}

/**
 * Returns the value of an int field
 */
jint
JNI_GetIntField(
    JNIEnv * env, 
    jobject obj, 
    const char * name)
{
    ASSERT(env);
    ASSERT(obj);
    ASSERT(name);
    if (env && obj) {
        jclass c = (*env)->GetObjectClass(env, obj);
        ASSERT(c);
        if (c) {
            jfieldID f = (*env)->GetFieldID(env, c, name, "I");
            if (f) {
                return (*env)->GetIntField(env, obj, f);
            } else {
                ASSMSG1("No such field: %s",name);
            }
        }
    }
    return 0;
}

/**
 * Sets the value of an int field
 */
Bool
JNI_SetIntField(
    JNIEnv * env, 
    jobject obj, 
    const char * name,
    jint value)
{
    ASSERT(env);
    ASSERT(obj);
    ASSERT(name);
    if (env && obj) {
        jclass c = (*env)->GetObjectClass(env, obj);
        ASSERT(c);
        if (c) {
            jfieldID f = (*env)->GetFieldID(env, c, name, "I");
            if (f) {
                (*env)->SetIntField(env, obj, f, value);
                return True;
            } else {
                ASSMSG1("No such field: %s",name);
            }
        }
    }
    return False;
}

/**
 * Returns the value of a long field
 */
jlong
JNI_GetLongField(
    JNIEnv * env, 
    jobject obj, 
    const char * name)
{
    ASSERT(env);
    ASSERT(obj);
    ASSERT(name);
    if (env && obj) {
        jclass c = (*env)->GetObjectClass(env, obj);
        ASSERT(c);
        if (c) {
            jfieldID f = (*env)->GetFieldID(env, c, name, "J");
            if (f) {
                return (*env)->GetLongField(env, obj, f);
            } else {
                ASSMSG1("No such field: %s",name);
            }
        }
    }
    return (jlong)0;
}

/**
 * Sets the value of a long field
 */
Bool
JNI_SetLongField(
    JNIEnv * env, 
    jobject obj, 
    const char * name,
    jlong value)
{
    ASSERT(env);
    ASSERT(obj);
    ASSERT(name);
    if (env && obj) {
        jclass c = (*env)->GetObjectClass(env, obj);
        ASSERT(c);
        if (c) {
            jfieldID f = (*env)->GetFieldID(env, c, name, "J");
            if (f) {
                (*env)->SetLongField(env, obj, f, value);
                return True;
            } else {
                ASSMSG1("No such field: %s",name);
            }
        }
    }
    return False;
}
