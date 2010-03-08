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
* Description:  Functions exported from jnilib
 *
*/


#ifndef _NOKIA_JNI_LIB_H_
#define _NOKIA_JNI_LIB_H_ 1

#include <s_lib.h>
#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/* small piece of platform dependent code */
#ifdef _WIN32
#  define JNI_API __cdecl
#else
#  define JNI_API
#endif

/* information about a Java method */
typedef struct _JavaMethod {
    const char * cls;       /* the class name */
    const char * met;       /* the method name */
    const char * sig;       /* the method signature */
    jmethodID id;           /* cached method id */
} JavaMethod;

/* information about a Java field */
typedef struct _JavaField {
    const char * cls;       /* the class name */
    const char * name;      /* the field name */
    const char * sig;       /* the field signature */
    jfieldID id;            /* cached field id */
    jclass c;               /* class id (for static fields) */
} JavaField;

/* how many numbers in Java version */
#define JAVA_VERSION_SIZE 4

/* version of java */
typedef struct _JavaVersion {
    int v[JAVA_VERSION_SIZE];
} JavaVersion;

/* opaque structures */
typedef struct _JVMSet  JVMSet;     /* a set of JVMs */
typedef struct _JVM     JVM;        /* particular JVM */

/* routine to sort JVMs */
typedef int (*JvmSortProc)(const JVM * vm1, const JVM * vm2);

/***************************************************************************
 *       Initialization/Deinitialization
 ***************************************************************************/

/**
 * Global initialization. You must invoke this function before invoking any
 * other JNILIB function. It's OK to invoke it more than once.
 */
Bool
JNI_API
JNI_Init(void);

/**
 * Global deinitialization. Should be called during shutdown procedure (if 
 * your program has one). For each JNI_Init call there should be a matching 
 * JNI_Deinit call. 
 */
void
JNI_API
JNI_Deinit(void);

/***************************************************************************
 *       Creating New Objects
 ***************************************************************************/

jobject 
JNI_API
JNI_CreateObject(
    JNIEnv * env, 
    const char * className);

jobject 
JNI_API
JNI_NewObject(
    JNIEnv * env, 
    const char * className, 
    const char * sig, 
    ...);

jobject 
JNI_API
JNI_NewObjectV(
    JNIEnv * env, 
    const char * className, 
    const char * sig, 
    va_list args);

jobjectArray
JNI_API
JNI_NewObjectArray(
    JNIEnv * env, 
    const char * className,
    int len);

jbyteArray
JNI_API
JNI_NewByteArray(
    JNIEnv * env,
    const uint8_t * data,
    int len);

/***************************************************************************
 *       Calling Static Methods
 ***************************************************************************/

Bool 
JNI_API
JNI_CallStaticVoidMethod(
    JNIEnv * env, 
    JavaMethod * method, 
    ...);

Bool 
JNI_API
JNI_CallStaticBooleanMethod(
    JNIEnv * env, 
    JavaMethod * method, 
    ...);

jint
JNI_API
JNI_CallStaticIntMethod(
    JNIEnv * env, 
    JavaMethod * method, 
    ...);

jobject 
JNI_API
JNI_CallStaticObjectMethod(
    JNIEnv * env, 
    JavaMethod * method, 
    ...);

/***************************************************************************
 *       Calling Virtual Methods
 ***************************************************************************/

Bool 
JNI_API
JNI_CallVoidMethod(
    JNIEnv * env, 
    jobject obj, 
    JavaMethod * method, 
    ...);

Bool 
JNI_API
JNI_CallBooleanMethod(
    JNIEnv * env, 
    jobject obj, 
    JavaMethod * method, 
    ...);

jint
JNI_API
JNI_CallIntMethod(
    JNIEnv * env, 
    jobject obj, 
    JavaMethod * method, 
    ...);

jobject
JNI_API
JNI_CallObjectMethod(
    JNIEnv * env, 
    jobject obj, 
    JavaMethod * method, 
    ...);

Str
JNI_API
JNI_CallStringMethod(
    JNIEnv * env, 
    jobject obj, 
    StrBuf * sb,
    JavaMethod * method,
    ...);

/***************************************************************************
 *       Getting And Setting Fields
 ***************************************************************************/

jobject
JNI_API
JNI_GetStaticObjectField(
    JNIEnv * env, 
    JavaField * field);

jint
JNI_API
JNI_GetStaticIntField(
    JNIEnv * env, 
    const char * className, 
    const char * name);

jint
JNI_API
JNI_GetIntField(
    JNIEnv * env, 
    jobject obj, 
    const char * name);

Bool
JNI_API
JNI_SetIntField(
    JNIEnv * env, 
    jobject obj, 
    const char * name,
    jint value);

jlong
JNI_API
JNI_GetLongField(
    JNIEnv * env, 
    jobject obj, 
    const char * name);

Bool
JNI_API
JNI_SetLongField(
    JNIEnv * env, 
    jobject obj, 
    const char * name,
    jlong value);

/***************************************************************************
 *       String Utilities
 ***************************************************************************/

char * 
JNI_API
JNI_ConvertToUTF8(
    const wchar_t * ws);
 
jstring
JNI_API
JNI_NewString(
    JNIEnv * env, 
    const char * str);

jstring
JNI_API
JNI_NewStringUCS2(
    JNIEnv * env, 
    const jchar * ucs2);

jstring
JNI_API
JNI_NewUnicodeString(
    JNIEnv * env, 
    const wchar_t * ws);

Str
JNI_API
JNI_GetString(
    JNIEnv * env, 
    jstring str, 
    StrBuf * sb);

Str
JNI_API
JNI_GetStringUTF8(
    JNIEnv * env, 
    jstring str, 
    StrBuf * sb);

jchar *
JNI_API
JNI_GetStringUCS2(
    JNIEnv * env, 
    jstring str);

wchar_t * 
JNI_API
JNI_GetUnicodeString(
    JNIEnv * env, 
    jstring str);

size_t
JNI_API
JNI_CopyString(
    JNIEnv * env, 
    jstring str, 
    Char * buf,
    size_t bufsize);

Bool 
JNI_API
JNI_StringArrayToVector(
    JNIEnv * env, 
    jobjectArray in, 
    Vector * out);

/***************************************************************************
 *       Miscellaneous
 ***************************************************************************/

jclass
JNI_API
JNI_FindClass(
    JNIEnv * env, 
    const char * className);

Bool 
JNI_API
JNI_ClearException(
    JNIEnv * env);

Bool 
JNI_API
JNI_IsInstance(
    JNIEnv * env, 
    jobject obj, 
    const char * className);

jbyte * 
JNI_API
JNI_GetByteArray(
    JNIEnv * env,
    jbyteArray byteArray,
    jint * len);

void
JNI_API
JNI_Free(
    void * ptr);

/**
 * Returns True if exception occured last time a JNI call was made by the
 * current thread using one of the JNILIB functions. This works regardless
 * of whether the "last exception" feature is on or off.
 */
Bool 
JNI_API
JNI_ExceptionOccurred(void);

/**
 * Returns the exception that occured when last JNI call was made by the 
 * current thread using one of the JNILIB functions, NULL if no exception
 * occured or this feature has been turned off. By default it's turned off.
 * To turn this feature on, call JNI_KeepLastException(True)
 */
jthrowable 
JNI_API
JNI_GetLastException(void);

/**
 * If the parameter is True, then JNILIB will store the last exception thrown
 * by the Java code, so that it can be later obtained by JNI_GetLastException.
 * By default, this feature if off to avoid unnecessary overhead, because it 
 * involves some additional and (relatively) expensive JNI calls on each
 * exception thrown by the Java code. It's recommended that you don't turn
 * it on unless you really need it.
 */
void
JNI_API
JNI_KeepLastException(
    Bool keep);

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
void
JNI_API
JNI_AssertOnException(
    Bool assertPlease);
#else
#  define JNI_AssertOnException(_foo) ((void)(_foo,0))
#endif

/***************************************************************************
 *              Creating Java Vitual Machine
 *
 * Here is a simple example:
 *
 *  JVMSet * jvms = JVM_Find();
 *  if (jvms) {
 *      JVM * jvm;
 *      JavaVersion v131;
 *      memset(&v131, 0, sizeof(v131));
 *      v131.v[0] = 1;
 *      v131.v[1] = 3;
 *      v131.v[2] = 1;
 *      if ((jvm = JVM_Select(jvms, &v131, False)) != NULL) {
 *          JavaVM * vm;
 *          Str vmParams[1];
 *          vmParams[0] = "-Djava.class.path=c:\nokia\foo\bar.jar");
 *          if ((vm = JVM_Create(jvm, COUNT(vmParams), vmParams)) != NULL) {
 *              Str params[1];
 *              params[0] = "please work";
 *              JVM_Main(vm, "foo/Bar", COUNT(params), params, True);
 *          } else {
 *              Error("Failed to start JVM!");
 *          }
 *      } else {
 *          Error("Need JRE 1.3.1 or better!");
 *      }
 *      JVM_Delete(jvms);
 *  } else {
 *      Error("No JRE installed!");
 *  }
 *
 ***************************************************************************/

/**
 * Discovers all available JVMs. If no JVMs are discovered, returns NULL.
 * This functions will find all JREs and JDKs installed on the system. It
 * will also check ./jre and ../jre directories relative to the path to the
 * executable that created the current process.
 */
JVMSet * 
JNI_API
JVM_Find(void);

/**
 * Discovers all available JVMs. If no JVMs are discovered, returns NULL.
 * In addition to the standard directories, also looks in the additional
 * directories specified by the dirs array. Note that this directory list
 * replaces the default list ("./jre","../jre") used by JVM_Find, it does not
 * adds new directories to the list.
 */
JVMSet * 
JNI_API
JVM_Find2(
    const Str dirs[],
    int n);

/**
 * This does essentially the same as JVM_Find2, only additional directories
 * are passed in as a variable list of strings. The list of directories
 * must be NULL terminated, for example:
 *
 *   JVM_FindVa("../lib/jre", NULL);
 */
JVMSet * 
JNI_API
JVM_FindVa(
    Str dir1, 
    ...);

/**
 * Returns the number of discovered JVMs. The return value is always > 0
 */
int 
JNI_API
JVM_GetCount(
    const JVMSet * jvms);

/**
 * Returns i-th discovered JVM. By default, JVMs are sorted by version, 
 * i.e. the first JVM in the list has the most recent version.
 */
JVM * 
JNI_API
JVM_Get(
    JVMSet * jvms, 
    int i);

/**
 * Returns the default JVM. It's not necessary the one with the highest
 * version. It's probably the most recently installed one.
 */
JVM * 
JNI_API
JVM_GetDefault(
    JVMSet * jvms);

/**
 * Returns the special JVM found in one of the special directories given to 
 * JVM_Find2 or JVM_FindVa, or from one of the default special directories
 * used by JVM_Find ("./jre" and "../jre"). Returns NULL if none were found.
 */
JVM * 
JNI_API
JVM_GetSpecial(
    JVMSet * jvms);

/**
 * Selects the JVM that has the specific version. If the last paramter is
 * True, returns the JVM with version exactly matching the requested one.
 * If the last parameter is False, returns the JVM that has the highest 
 * version greater or equal the requested version (recommended use).
 * If no JVMs match the criteria, return NULL.
 */
JVM * 
JNI_API
JVM_Select(
    JVMSet * jvms, 
    const JavaVersion * version, 
    Bool exact);

/**
 * Parses Java version string, like "1.4.1_b32". 
 */
Bool
JNI_API
JVM_ParseVersion(
    Str strVersion, 
    JavaVersion * version);

/**
 * Parses Java version string, like "1.4.1_b32". Also returns the number
 * of digits in the version string that have been successfully parsed.
 * All non-parsed digits are filled with zeros. The ndigits pointer may 
 * be NULL.
 */
Bool
JNI_API
JVM_ParseVersion2(
    Str strVersion, 
    JavaVersion * version,
    int* ndigits);

/**
 * Compares two Java versions. Returns a negative number, zero or a
 * positive number if v1 is less than, equal or greater than v2,
 * respectively.
 */
int 
JNI_API
JVM_CompareVersions(
    const JavaVersion * v1, 
    const JavaVersion * v2);

/**
 * Checks if this JRE is the default one. It's not necessary the one with
 * the highest version. It's probably the most recently installed one.
 */
Bool 
JNI_API
JVM_IsDefault(
    const JVM * jvm);

/**
 * Checks if this JRE is the special one, i.e. the one loaded from one of the
 * special directories given to JVM_Find2 or JVM_FindVa, or from one of the
 * default special directories used by JVM_Find ("jre" and "../jre")
 */
Bool 
JNI_API
JVM_IsSpecial(
    const JVM * jvm);

/**
 * Returns the parsed JVM version
 */
const JavaVersion * 
JNI_API
JVM_GetVersion(
    const JVM * jvm);

/**
 * Returns the original Java version string
 */
Str 
JNI_API
JVM_GetVersionString(
    const JVM * jvm);

/**
 * Returns the JVM home path
 */
Str 
JNI_API
JVM_GetHome(
    const JVM * jvm);

/**
 * Returns the path of the JVM dynamically loadable library.
 */
Str 
JNI_API
JVM_GetPath(
    const JVM * jvm);

/**
 * Sorts the JVMs according to the provided rule.
 */
void
JNI_API
JVM_Sort(
    JVMSet * jvms, 
    JvmSortProc proc);

/**
 * Loads the JVM. Every time this method is invoked, it attempts to create 
 * a new JavaVM context. 
 */
JavaVM *
JNI_API
JVM_Create(
    JVM * jvm, 
    int argc, 
    Str args[]);

/**
 * Calls the main method of the specified class with the provided arguments.
 * If the last parameter is True, invokes (*jvm)->DestroyJavaVM(jvm) which
 * waits until the only threads running are all daemon threads and destroys
 * JavaVM. If you plan to reuse this JavaVM context, the last parameter must
 * be NULL.
 */
Bool
JNI_API
JVM_Main(
    JavaVM * vm, 
    Str cls, 
    int argc, 
    Str args[], 
    Bool wait);

/**
 * Deletes the JVM context. Do not invoke this function until after you have
 * destroyed all JavaVMs created by JVM_Create function, because this function
 * will unload all loaded JVM libraries and unmap them from the process 
 * address space
 */
void
JNI_API
JVM_Delete(
    JVMSet * jvms);

/**
 * Detects the presence of JVM in the context of the current process and
 * returns its JavaVM context. If no JVM has been detected, returns NULL
 */
JavaVM * 
JNI_API
JNI_GetCurrentVM(void);

/***************************************************************************
 * Unicode mapping for prototypes that use Str or Char types. If the UNICODE
 * settings of the application that uses the library don't match the UNICODE
 * settings used by the library when it was compiled, the application won't
 * link. That's better than debug the problem at run time.
 ***************************************************************************/
#ifdef _UNICODE

/* jni_call.c */
#define JNI_CallStringMethod    JNI_CallStringMethodU

/* jni_str.c */
#define JNI_NewString           JNI_NewStringU
#define JNI_GetString           JNI_GetStringU
#define JNI_GetStringUTF8       JNI_GetStringUTF8U
#define JNI_CopyString          JNI_CopyStringU
#define JNI_StringArrayToVector JNI_StringArrayToVectorU

#endif /* _UNICODE */

#ifdef __cplusplus
} /* and of extern "C" */
#endif  /* __cplusplus */

#endif /* _NOKIA_JNI_LIB_H_ */
