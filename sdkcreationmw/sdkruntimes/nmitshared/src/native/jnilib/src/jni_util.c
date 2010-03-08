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
 *       Global data
 ***************************************************************************/

typedef struct _JniGlobalData {
    int initCount;          /* how many times we have been initialized */
    ThrKey key;             /* TLS key with cleanup */
    ThrKey key2;            /* TLS key without cleanup */
    Mutex mutex;            /* synchronizes access to the queue */
    Queue queue;            /* queue of JniThreadData2 structures */
    Bool keepLastException; /* True if the caller needs last exceptions */
#if DEBUG
    Bool assertOnException; /* True to ASSERT on each exception */
#endif /* DEBUG */
} JniGlobalData;

/**
 * Thread local data for storing exception information
 */
typedef struct _JniThreadData {
    Bool shitHappened;      /* True if exception has occured */
    JNIEnv * env;           /* JNI environment for this thread */
    jthrowable exception;   /* The exception object (globally referenced) */
} JniThreadData;

/**
 * Thread local data for which we don't have a cleanup procedure
 */
typedef struct _JniThreadData2 {
    JniThreadData data;     /* The common thread local data */
    QEntry entry;           /* Entry attached to JniGlobalData::queue */
} JniThreadData2;

STATIC JniGlobalData jniLibData = {0};

/***************************************************************************
 *       Initialization/Deinitialization
 ***************************************************************************/

/**
 * Cleanup procedure for thread local data
 */
STATIC void JNI_CleanThreadData(void * value)
{
    JniThreadData * data = value;
    if (data->shitHappened) {
        ASSERT(data->exception);
        (*data->env)->DeleteGlobalRef(data->env, data->exception);
    }
    MEM_Free(data);
}

/**
 * Global initialization. You must invoke this function before invoking any
 * other JNILIB function. It's OK to invoke it more than once.
 */
Bool JNI_Init(void)
{
    if (!(jniLibData.initCount++)) {
        THREAD_InitModule();
        QUEUE_Init(&jniLibData.queue);
        jniLibData.keepLastException = False;
#if DEBUG
        jniLibData.assertOnException = True;
#endif /* DEBUG */
        if (MUTEX_Init(&jniLibData.mutex)) {
            jniLibData.key = THREAD_CreateKey(JNI_CleanThreadData);
            if (jniLibData.key) {
                jniLibData.key2 = NULL;
                return True;
            }
            MUTEX_Destroy(&jniLibData.mutex);
        }
        THREAD_Shutdown();
        return False;
    }
    return True;
}

/**
 * Global deinitialization. Should be called during shutdown procedure (if 
 * your program has one). For each JNI_Init call there should be a matching 
 * JNI_Deinit call. 
 */
void JNI_Deinit(void)
{
    ASSERT(jniLibData.initCount > 0);
    if (!(--jniLibData.initCount)) {
        /* 
         * When JNI_Deinit is called, there must be only one thread using
         * JNILIB, and that's the one calling this function. Therefore, it 
         * makes no sense to synchronize anything.
         */
        MUTEX_Destroy(&jniLibData.mutex);
        THREAD_DeleteKey(jniLibData.key);
        if (jniLibData.key2) {
            THREAD_DeleteKey(jniLibData.key2);
            if (!QUEUE_IsEmpty(&jniLibData.queue)) {
                JavaVM * jvm = JNI_GetCurrentVM();
                if (jvm) {
                    JNIEnv * env = NULL;
                    (*jvm)->AttachCurrentThread(jvm,(void**)&env, NULL);
                    if (env) {
                        while (!QUEUE_IsEmpty(&jniLibData.queue)) {
                            QEntry* e = QUEUE_RemoveHead(&jniLibData.queue);
                            JniThreadData2* d = QCAST(e,JniThreadData2,entry);
                            if (d->data.shitHappened && d->data.exception) {
                                (*env)->DeleteGlobalRef(env,d->data.exception);
                            }
                            MEM_Free(d);
                        }
                    }
                }
            }
        }
        THREAD_Shutdown();
    }
}

/***************************************************************************
 *       Miscellaneous utilities
 ***************************************************************************/

/**
 * Finds a class given its name.
 */
jclass
JNI_FindClass(
    JNIEnv * env, 
    const char * className)
{
    jclass c = (*env)->FindClass(env, className);
    if (c) {
        return c;
    }
    ASSMSG1("No such class: %s",className);
    return NULL;
}

/**
 * Checks if the object if an instance of the class specified in the
 * method descriptor
 */
Bool 
JNI_IsInstance(
    JNIEnv * env, 
    jobject obj, 
    const char * className)
{
    if (obj) {
        jclass c = JNI_FindClass(env, className);
        if (c) {
            return (*env)->IsInstanceOf(env,obj,c);
        }
    }
    return False;
}

/**
 * The return value must be deallocated with JNI_Free
 */
jbyte * 
JNI_GetByteArray(
    JNIEnv * env,
    jbyteArray byteArray,
    jint * len)
{
    jboolean isCopy;
    jbyte * copy = NULL;
    jbyte * bytes;
    ASSERT(JNI_IsInstance(env, byteArray, "[B"));
    bytes = (*env)->GetByteArrayElements(env, byteArray, &isCopy);
    if (bytes) {
        *len = (*env)->GetArrayLength(env, byteArray);
        copy = MEM_NewArray(jbyte, *len);
        if (copy) {
            memcpy(copy, bytes, *len);
        }
        (*env)->ReleaseByteArrayElements(env, byteArray, bytes, 0);
    } else {
        *len = 0;
    }
    return copy;        
}

/**
 * Deallocates the memory allocated by jnilib
 */
void JNI_Free(void * ptr)
{
    MEM_Free(ptr);
}

/***************************************************************************
 *       Exceptions
 ***************************************************************************/

/**
 * Gets the thread local data for the calling thread. Returns NULL if no data
 * is associated with the current thread.
 */
STATIC JniThreadData * JNI_GetThreadData()
{
    /* Make sure that JNILIB library has been initialized */
    JniThreadData * data;
    ASSERT(jniLibData.initCount > 0); /* Have you called JNI_Init()? */
    if (!jniLibData.initCount) {
        if (!JNI_Init()) {
            return NULL;
        }
    }

    /* Lookup the thread local value. This requires no synchronization */
    data = THREAD_GetValue(jniLibData.key);
    if (!data) {

        /* Check out the second thread local storage */
        if (jniLibData.key2) {
            JniThreadData2 * data2 = THREAD_GetValue(jniLibData.key2);
            if (data2) {
                data = &data2->data;
            }
        }
    }

    return data;
}

/**
 * Creates thread local data for the calling thread.
 */
STATIC JniThreadData * JNI_CreateThreadData()
{
    JniThreadData * data = NULL;
    ASSERT(!JNI_GetThreadData());

    /* On Unix, THREAD_CanSetValue always returns True */
    if (THREAD_CanSetValue(jniLibData.key)) {
        data = MEM_New(JniThreadData);
        if (data) {
            memset(data, 0, sizeof(*data));
            if (!THREAD_SetValue(jniLibData.key, data)) {
                MEM_Free(data);
                return NULL;
            }
        }
    } else {
        if (!jniLibData.key2) {
            /* create the second thread local key on demand */
            jniLibData.key2 = THREAD_CreateKey(NULL);
        }
        if (jniLibData.key2) {
            JniThreadData2 * data2 = MEM_New(JniThreadData2);
            if (data2) {
                memset(data2, 0, sizeof(*data2));
                if (THREAD_SetValue(jniLibData.key2, data2)) {
                    if (MUTEX_Lock(&jniLibData.mutex)) {
                        QUEUE_InsertTail(&jniLibData.queue, &data2->entry);
                        MUTEX_Unlock(&jniLibData.mutex);
                        return &data2->data;
                    }
                    THREAD_SetValue(jniLibData.key2, NULL);
                }
                MEM_Free(data2);
            }
        }
    }

    return data;
}

/**
 * Resets the last exception for the calling thread
 */
void JNI_ResetLastException()
{
    JniThreadData* data = JNI_GetThreadData();
    if (data && data->shitHappened) {
        if (data->exception) {
            (*data->env)->DeleteGlobalRef(data->env, data->exception);
            data->exception = NULL;
        }
        data->shitHappened = False;
    }
}

/**
 * Sets the last exception for the calling thread
 */
STATIC void JNI_SetLastException(JNIEnv * env)
{
    JniThreadData* data = JNI_GetThreadData();
    if (!data) data = JNI_CreateThreadData();
    if (data) {
        if (data->exception) {
            ASSERT(data->shitHappened);
            (*data->env)->DeleteGlobalRef(data->env, data->exception);
            data->exception = NULL;
        }
        ASSERT(env);
        data->shitHappened = True;
        data->env = env;
        if (env && jniLibData.keepLastException) {
            jthrowable exception = (*env)->ExceptionOccurred(env);
            ASSERT(exception); /* why call us if there's no exception? */
            if (exception) {
                data->exception = (*env)->NewGlobalRef(env, exception);
                ASSERT(data->exception);
            }
        }
    }
}

/**
 * Clears pending Java exception
 */
Bool JNI_ClearException(JNIEnv * env)
{
    if ((*env)->ExceptionCheck(env)) {
        JNI_SetLastException(env);
#if DEBUG
        (*env)->ExceptionDescribe(env);
        if (jniLibData.assertOnException) {
            ASSMSG("Exception in Java code!");
        }
#endif /* DEBUG */
        (*env)->ExceptionClear(env);
        return True;
    } else {
        JNI_ResetLastException();
        return False;
    }
}

/**
 * Returns True if exception occured last time a JNI call was made by the
 * current thread using one of the JNILIB functions. This works regardless
 * of whether the "last exception" feature is on or off.
 */
Bool JNI_ExceptionOccurred()
{
    JniThreadData* data = JNI_GetThreadData();
    return (data && data->shitHappened);
}

/**
 * Returns the exception that occured when last JNI call was made by the 
 * current thread using one of the JNILIB functions, NULL if no exception
 * occured or this feature has been turned off. By default it's turned off.
 * To turn this feature on, call JNI_KeepLastException(True)
 */
jthrowable JNI_GetLastException()
{
    JniThreadData* data = JNI_GetThreadData();
    if (data && data->shitHappened) {
        return data->exception;
    } else {
        return NULL;
    }
}

/**
 * If the parameter is True, then JNILIB will store the last exception thrown
 * by the Java code, so that it can be later obtained by JNI_GetLastException.
 * By default, this feature if off to avoid unnecessary overhead, because it 
 * involves some additional and (relatively) expensive JNI calls on each
 * exception thrown by the Java code. It's recommended that you don't turn
 * it on unless you really need it.
 */
void JNI_KeepLastException(Bool keep)
{
    ASSERT(jniLibData.initCount > 0);   /* call JNI_Init() first */
    jniLibData.keepLastException = keep;
}

/**
 * The JNI_AssertOnException function is only available in debug build. If 
 * the parameter if True, then the ASSERT message box will pop up each time 
 * an exception is thrown by the Java code. By default, this feature is on.
 *
 * You may want to call JNI_AssertOnException(False) if you intentionally
 * throw exceptions in Java code and catch them in native code. I can't think
 * of any good reason to use such a sophisticated method of returning the 
 * status of the call to the native code, but apparently some peope are 
 * doing it. Oh well...
 */
#if DEBUG
void JNI_AssertOnException(Bool assertPlease)
{
    ASSERT(jniLibData.initCount > 0);   /* call JNI_Init() first */
    jniLibData.assertOnException = assertPlease;
}
#endif /* DEBUG */
