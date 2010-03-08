/*
* Copyright (c) 2003 - 2004 Nokia Corporation and/or its subsidiary(-ies).
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


#include <cstdlib.>
#include <iostream>
using namespace std;

#include <jni.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>


extern char dirSeparator;

extern int getExecutablePath(char* path, int maxPathLength);

extern void reportError(char *error);

extern int getJavaRuntime(char*, int);
typedef _JNI_IMPORT_OR_EXPORT_ jint (JNICALL *CREATE_JAVA_VM_FN)(JavaVM**, void**, void*);
extern void* getJavaVMCreateFn(char*);

static int getSDKHomeAndExecutableName(char* path, char*& sdkHome, char*& exeName)
    {
    int pathLength = ::strlen(path);
    int state      = 0;

    sdkHome = path;
    for (int i = pathLength - 1; i >= 0; i--)
        {
        char c = path[i];

        switch (state)
            {
        case 0:

            if (c == '.')
                {
                path[i] = 0;
                state   = 1;
                }
            break;

        case 1:

            if (c == dirSeparator)
                {
                path[i] = 0;
                exeName = path + i + 1;
                
                state = 2;
                }
            break;

        case 2:

            if (c == dirSeparator)
                {
                path[i] = 0;
                state   = 3;
                i = -1;
                }
            break;
            }
        }
    return !(state == 3);
    }

static int makeOptionString(char*& option, const char* s, int length)
    {
    option = reinterpret_cast<char*>(::malloc(length + 1));
    if (option == NULL)
        {
        return 1;
        }
    ::strcpy(option, s);
    return 0;
    }

 
int main(int argc, char* argv[])
    {
    char    path[256];
    char*   exeName;
    char*   sdkHome;

    if (getExecutablePath(path, sizeof(path)) != 0)
        {
        reportError("Unable to determine executable path");
        return -1001;
        }
    if (getSDKHomeAndExecutableName(path, sdkHome, exeName) != 0)
        {
		reportError("Unable to determine SDK Home and executable name");
        return -1002;
        }

    char* buffer = reinterpret_cast<char*>(::malloc(4096));
    int   length = 0;

    if (buffer == NULL)
        {
        reportError("Out of memory");
        return -1003;
        }

    JavaVM*         vm;
    JNIEnv*         env;
    JavaVMInitArgs  args;
    JavaVMOption    options[2];

    args.version = JNI_VERSION_1_2;
	char * envClasspath = getenv("CLASSPATH");
    length = ::sprintf(buffer, "-Djava.class.path=%s/symbian/lib/sei.jar;%s", sdkHome, envClasspath);
    if (makeOptionString(options[0].optionString, buffer, length) != 0)
        {
        reportError("Failed to initialize CLASSPATH");
        return -1004;
        }
    length = ::sprintf(buffer, "-Dsdk.home=%s", sdkHome);
    if (makeOptionString(options[1].optionString, buffer, length) != 0)
        {
        reportError("Failed to initialize -Dsdk.home param");
        return -1005;
        }
    args.nOptions = 2;
    args.options = options;

    char    jvmpath[1024]; // To store the path to the current jvm.dll

    // Find the VM
    int res = getJavaRuntime(jvmpath, sizeof(jvmpath));
    if (res == -1)
    {
       reportError("Unable to find JRE in Windows Registry");
       return -1006;
    }
    // Get function pointer to JNI_CreateJavaVM
    CREATE_JAVA_VM_FN create = (CREATE_JAVA_VM_FN)getJavaVMCreateFn(jvmpath);
    if (create == NULL)
    {
       reportError("Unable to create Java VM");
       return -1007;
    }
    // Create the VM
    if (create(
            &vm, 
            reinterpret_cast<void**>(&env), 
            reinterpret_cast<void*>(&args)) != 0)
        {
        reportError("Unable to create Java VM");
        return -1008;
        }
      // ---------------------------------------------------------------
/*  
//Static linkage with JVM.LIB
  
	if (JNI_CreateJavaVM(
            &vm, 
            reinterpret_cast<void**>(&env), 
            reinterpret_cast<void*>(&args)) != 0)
        {
        ::printf("Unable to create Java VM\n");
        return 6;
        }
*/
/*
//Dev Test to check that JadTool is loaded
	//com.sun.midp.jadtool
    ::sprintf(buffer, "com/sun/midp/jadtool/JadTool");
    jclass toolClass = env->FindClass(buffer);
	//this means the user wont be able to sign suites
    if (toolClass == NULL)
        {
        ::printf("Not Found...\n");
        return 7;
		}
	else
		{
        ::printf("Found!!\n");
		}
*/
    ::sprintf(buffer, "com/symbian/tools/j2me/sei/%s/Main", exeName);
    jclass mainClass = env->FindClass(buffer);

    if (mainClass == NULL)
        {
		char* error = reinterpret_cast<char*>(::malloc(4096));
		::sprintf(error, "Class com.symbian.tools.j2me.sei.%s.Main could not be found", exeName);
        reportError(error);
		return -1009;
        }
	else
		{
        //::printf("Found!!\n");
		}

    jmethodID mainMethod = env->GetStaticMethodID(mainClass, "main", "([Ljava/lang/String;)V");

    if (env->ExceptionOccurred())
        {
        env->ExceptionDescribe();
        vm->DestroyJavaVM();
        return -1010;
        }

    jclass stringClass = env->FindClass("java/lang/String");

    if (stringClass == NULL)
        {
        vm->DestroyJavaVM();
        return -1011;
        }

    jobjectArray jargs = env->NewObjectArray(argc - 1, stringClass, NULL);

    if (env->ExceptionOccurred())
        {
        env->ExceptionDescribe();
        vm->DestroyJavaVM();
        return -1012;
        }
    for (int i = 1; i < argc; i++)
        {
        jstring jarg = env->NewStringUTF(argv[i]);

        if (jarg == NULL)
            {
            vm->DestroyJavaVM();
            return -1013;
            }
        env->SetObjectArrayElement(jargs, i - 1, jarg);
        }
    env->CallStaticVoidMethod(mainClass, mainMethod, jargs);
    if (env->ExceptionOccurred())
        {
        env->ExceptionDescribe();
        }
    vm->DestroyJavaVM();
    return 0;
    }
