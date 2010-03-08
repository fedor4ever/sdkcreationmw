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

#define STRING "java/lang/String"

static JavaMethod STRING_GetBytes = {
    STRING,
    "getBytes",
    "()[B",
    (jmethodID)0};

static JavaMethod STRING_GetBytesEnc = {
    STRING,
    "getBytes",
    "(L"STRING";)[B",
    (jmethodID)0};

/***************************************************************************
 *       String Utilities
 ***************************************************************************/

/**
 * Converts a null terminated Unicode string into UTF-8. This algorithm 
 * works under the assumption that all surrogate pairs have already been 
 * converted into scalar code point values within the argument.
 */
char * JNI_ConvertToUTF8(const wchar_t * ws) 
{
    char * utf8;

    /* determine how many bytes are needed for the complete conversion */
    int len = 0;
    int bytesNeeded = 1;
    while (ws[len]) {
        if (ws[len] < 0x80) {
            ++bytesNeeded;
        } else if (ws[len] < 0x0800) {
            bytesNeeded += 2;
        } else if (ws[len] < 0x10000) {
            bytesNeeded += 3;
        } else {
            bytesNeeded += 4;
        }
        len++;
    }

    /* allocate a byte array of the necessary size */
    utf8 = MEM_NewArray(char,bytesNeeded);
    if (utf8) {
        /* do the conversion from character code points to utf-8 */
        int i, bytes = 0;
        for(i=0; i<len; i++) {
            if(ws[i] < 0x80) {
                utf8[bytes++] = (char)ws[i];
            } else if (ws[i] < 0x0800) {
                utf8[bytes++] = (char)(( ws[i] >> 6)          | 0xC0);
                utf8[bytes++] = (char)(( ws[i]        & 0x3F) | 0x80);
            } else if (ws[i] < 0x10000) {
                utf8[bytes++] = (char)(( ws[i] >> 12)         | 0xE0);
                utf8[bytes++] = (char)(((ws[i] >> 6)  & 0x3F) | 0x80);
                utf8[bytes++] = (char)(( ws[i]        & 0x3F) | 0x80);
            } else {
                utf8[bytes++] = (char)(( ws[i] >> 18)         | 0xF0);
                utf8[bytes++] = (char)(((ws[i] >> 12) & 0x3F) | 0x80);
                utf8[bytes++] = (char)(((ws[i] >> 6)  & 0x3F) | 0x80);
                utf8[bytes++] = (char)(( ws[i]        & 0x3F) | 0x80);
            }
        }
        utf8[bytes] = '\0';
    }
    return utf8;
}

/**
 * Creates a new Java string from a NULL terminated string
 */
jstring
JNI_NewString(
    JNIEnv * env, 
    const char * str)
{
    jstring s = NULL;
    wchar_t * unicode = STRING_ToUnicode(str);
    if (unicode) {
        s = JNI_NewUnicodeString(env, unicode);
        MEM_Free(unicode);
    }
    return s;
}

/**
 * Creates a new Java string from a NULL terminated UCS2 string
 */
jstring
JNI_NewStringUCS2(
    JNIEnv * env, 
    const jchar * ucs2)
{
    ASSERT(env);
    if (env && ucs2) {
        jstring str;
        jsize len = 0;
        const jchar * p = ucs2;
        while (*p++) len++;
        str = (*env)->NewString(env, ucs2, len);
        ASSERT(str);
        return str;
    }
    return NULL;
}

/**
 * Creates a new Java string from a NULL terminated UNICODE string
 */
jstring
JNI_NewUnicodeString(
    JNIEnv * env, 
    const wchar_t * ws)
{
    ASSERT(env);
    if (env && ws) {
        char * utf8 = JNI_ConvertToUTF8(ws);
        if (utf8) {
            jstring str = (*env)->NewStringUTF(env, utf8);
            ASSERT(str);
            MEM_Free(utf8);
            return str;
        }
    }
    return NULL;
}

/**
 * Converts Java string into a NULL terminated string using the default
 * character encoding.
 */
Str
JNI_GetString(
    JNIEnv * env, 
    jstring str, 
    StrBuf * sb)
{
    Str result = NULL;
    STRBUF_Clear(sb);
    if (str) {
        jbyteArray array = JNI_CallObjectMethod(env, str, &STRING_GetBytes);
        if (array) {
            jboolean copy;
            jint len = (*env)->GetArrayLength(env, array);
            jbyte * bytes = (*env)->GetByteArrayElements(env, array, &copy);
            if (bytes) {
                if (STRBUF_CopyN(sb, bytes, len)) {
                    result = STRBUF_GetString(sb);
                }
                (*env)->ReleaseByteArrayElements(env, array, bytes, 0);
            }
            (*env)->DeleteLocalRef(env, array);
        }
    }
    return result;
}

/**
 * Converts Java string into a NULL terminated UTF-8 string
 */
Str
JNI_GetStringUTF8(
    JNIEnv * env, 
    jstring str, 
    StrBuf * sb)
{
    Str result = NULL;
    STRBUF_Clear(sb);
    if (str) {
        jbyteArray utf8 = NULL;

        /* allocate static instance of the "UTF-8" string */
        static jstring enc = NULL;
        if (!enc) {
            enc = (*env)->NewStringUTF(env, "UTF-8");
            if (enc) {
                enc = (*env)->NewGlobalRef(env, enc);
            }
        }

        if (enc) {
            utf8 = JNI_CallObjectMethod(env, str, &STRING_GetBytesEnc, enc);
        }

        if (utf8) {
            jboolean copy;
            jint len = (*env)->GetArrayLength(env, utf8);
            jbyte * bytes = (*env)->GetByteArrayElements(env, utf8, &copy);
            if (bytes) {
                if (STRBUF_CopyN(sb, bytes, len)) {
                    result = STRBUF_GetString(sb);
                }
                (*env)->ReleaseByteArrayElements(env, utf8, bytes, 0);
            }
            (*env)->DeleteLocalRef(env, utf8);
        }
    }
    return result;
}

/**
 * Converts Java string to NULL-terminated UCS2 string.
 * Returns NULL is input string is NULL or either JNI call or 
 * memory allocation fails. The returned string must be deallocated
 * with MEM_Free
 */
jchar * JNI_GetStringUCS2(JNIEnv * env, jstring str)
{
    jchar * ws = NULL;
    if (str) {
        jsize len = (*env)->GetStringLength(env, str);
        if (len == 0) {
            ws = MEM_NewArray(jchar,1);
            if (ws) ws[0] = (jchar)0;
        } else {
            jboolean copy;
            const jchar * chars = (*env)->GetStringChars(env, str, &copy);
            if (chars) {
                ws = MEM_NewArray(jchar,len+1);
                if (ws) {
                    jsize i;
                    for (i=0; i<len; i++) ws[i] = chars[i];
                    ws[len] = (jchar)0;
                }
                (*env)->ReleaseStringChars(env, str, chars);
            }
        }
    }
    return ws;
}

/**
 * Converts Java string to NULL-terminated Unicode string.
 * Returns NULL is input string is NULL or either JNI call or 
 * memory allocation fails. The returned string must be deallocated
 * with MEM_Free
 *
 * NOTE: wchar_t isn't necessarily the same thing as jchar. On most
 * Unix platforms it's defined as a 4-byte value. 
 */
wchar_t * JNI_GetUnicodeString(JNIEnv * env, jstring str)
{
    wchar_t * ws = NULL;
    if (str) {
        jsize len = (*env)->GetStringLength(env, str);
        if (len == 0) {
            ws = MEM_NewArray(wchar_t,1);
            if (ws) ws[0] = (wchar_t)0;
        } else {
            jboolean copy;
            const jchar * chars = (*env)->GetStringChars(env, str, &copy);
            if (chars) {
                ws = MEM_NewArray(wchar_t,len+1);
                if (ws) {
                    jsize i;
                    for (i=0; i<len; i++) ws[i] = chars[i];
                    ws[len] = (wchar_t)0;
                }
                (*env)->ReleaseStringChars(env, str, chars);
            }
        }
    }
    return ws;
}

/**
 * Copies Java string into a fixed size buffer. Makes sure it's 
 * NULL terminated. Returns the length of the string copied into
 * the buffer
 */
size_t
JNI_CopyString(
    JNIEnv * env, 
    jstring str, 
    Char * buf,
    size_t bufsize)
{
    size_t len = 0;
    ASSERT(buf);
    ASSERT(bufsize);
    if (bufsize && buf) {
        jbyteArray array = JNI_CallObjectMethod(env, str, &STRING_GetBytes);
        if (array) {
            len = (*env)->GetArrayLength(env, array);
            if (len > 0) {
                jboolean copy;
                jbyte * bytes = (*env)->GetByteArrayElements(env, array, &copy);
                if (bytes) {
                    len = MAX(len,bufsize-1);
                    strncpy(buf, bytes, len);
                    (*env)->ReleaseByteArrayElements(env, array, bytes, 0);
                }
            }
            (*env)->DeleteLocalRef(env, array);
        }
        buf[len] = 0;
    }
    return len;
}

/** 
 * Converts Java String [] into StringArray.
 * Returns False if either JNI call of memory allocation fails
 */
Bool 
JNI_StringArrayToVector(
    JNIEnv * env, 
    jobjectArray in, 
    Vector * out)
{
    ASSERT(env);
    VECTOR_Clear(out);
    out->free = vectorFreeValueProc;
    if (in) {
        Bool ok = True;
        jsize n = (*env)->GetArrayLength(env, in);
        jsize i;

        for (i=0; i<n && ok; i++) {
            Char * s = NULL;
            jobject str = (*env)->GetObjectArrayElement(env,in,i);
            if (str) {
                jbyteArray array = JNI_CallObjectMethod(env, str, &STRING_GetBytes);
                if (array) {
                    jboolean copy;
                    jint len = (*env)->GetArrayLength(env, array);
                    jbyte * bytes = (*env)->GetByteArrayElements(env, array, &copy);
                    if (bytes) {
                        s = MEM_NewArray(Char,len+1);
                        if (s) {
                            memcpy(s, bytes, len);
                            s[len] = 0;
                        }
                        (*env)->ReleaseByteArrayElements(env, array, bytes, 0);
                    }
                    (*env)->DeleteLocalRef(env, array);
                }
                (*env)->DeleteLocalRef(env, str);
            }
            ok = VECTOR_Add(out,s);
            if (!ok) MEM_Free(s);
        }

        if (ok) {
            return True;
        }

        /* cleanup */
        VECTOR_Clear(out);
    }
    return False;
}
