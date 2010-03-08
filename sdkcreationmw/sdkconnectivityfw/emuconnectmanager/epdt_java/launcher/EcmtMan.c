/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Launcher for the Ecmt Manager Java application
 *
*/


static char rcsid[] = "$Id: EcmtMan.c,v 1.9 2006/05/04 22:50:03 slava Exp $";

#include <jni_lib.h>
#include <w_lib.h>
#include "EcmtMan.h"

/* we require at least JRE 1.4, but prefer 1.5 */
static JavaVersion javaVersionRequired = {{1,4,0,0}};
static JavaVersion javaVersionPreferred = {{1,5,0,0}};

/* the application title is replaced with localized version when app starts */
static Str appTitle = "ECMT Manager";

#ifdef PREFS
#define EPDT_DIRECTORY  "..\\epoc32\\tools\\ecmt\\"
#endif //PREFS

#define ECMTMAN_JAR "epdt.jar"
static Str requiredFiles[] = { ECMTMAN_JAR };
static Str defaultOptions[] = {
    "-Dstart=core",
#ifndef PREFS
    "-Dcommand=open",
#else
    "-Dcommand=show_preferences",
#endif
    "-Dhost=127.0.0.1",
    "-Dport=49359",
#if DEBUG
    "-DlogLevel=DEBUG",
    "-Dnokia.wtk.verbose=true",
#endif
};

#define MAIN_CLASS      "com.nokia.epdt.Main"
#define MAIN_CLASS_JNI  "com/nokia/epdt/Main"

#define EPOC_ROOT_OPT   "-Depoc.root="
#define SDK_HOME_OPT    "-Dsdk.home="
#define CLASS_PATH_OPT  "-Djava.class.path="
#define LIB_PATH_OPT    "-Djava.library.path="
#define JAVA_URL        "http://java.sun.com/"

//Tempovo:
//#define SDK_HOME_DIR    "tools\\java\\sei\\winscw\\udeb\\hmidps60v30"
#define SDK_HOME_DIR        "..\\.."

/* *************************************************************************
 *  Error reporting
 * *************************************************************************/

STATIC void ECMTMAN_ErrorMessage(Str msg)
{
    PRINT_Error("ERROR: %s\n", msg);
#ifndef _CONSOLE
    MessageBox(NULL, msg, appTitle, MB_OK|MB_ICONINFORMATION);
#endif
}

/**
 * Format system error message and show it
 */
STATIC void ECMTMAN_SysError(ULONG err)
{
    LPSTR lpMsgBuf = NULL;
    if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                      FORMAT_MESSAGE_FROM_SYSTEM |     
                      FORMAT_MESSAGE_IGNORE_INSERTS,    
                      NULL, err, 
                      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                      (LPTSTR)&lpMsgBuf, 0, NULL )) {
        ECMTMAN_ErrorMessage(lpMsgBuf);
        LocalFree(lpMsgBuf);
    } else {
        char szMsg[64];
        sprintf(szMsg,"ERROR %lu",err);
        ECMTMAN_ErrorMessage(szMsg);
    }
}

/**
 * Format error message and show it
 */
STATIC void ECMTMAN_FormatError(UINT uMsgID, ...)
{
    StrBuf64 buf;
    Str msg;
    va_list va;
    va_start(va, uMsgID);
    STRBUF_InitBufXXX(&buf);
    msg = WIN32_FormatMessageVa(GetModuleHandle(NULL), uMsgID, &buf.sb, va);
    if (!msg) {
        STRBUF_Format(&buf.sb, "ERROR %u", uMsgID);
        msg = STRBUF_Text(&buf.sb);
    }
    ECMTMAN_ErrorMessage(msg);
    STRBUF_Destroy(&buf.sb);
}

/***************************************************************************
 * Allocates memory. Never returns NULL. If memory allocation fails, pops 
 * up an error message and exits. Hopfully, this never happens...
 **************************************************************************/

struct _MemContext {
    const MemHook* nextHook;
};

STATIC void* ECMTMAN_Malloc(MemContext* ctx, size_t size)
{
    void* ptr = MEM_Alloc2(ctx->nextHook, size);
    if (!ptr) {
        ECMTMAN_ErrorMessage("out of memory!");
        exit(-1);
    }
    return ptr;
}

/***************************************************************************
 * JVM selection criteria
 **************************************************************************/

STATIC int ECMTMAN_SortVM(const JVM * vm1, const JVM * vm2)
{
    const JavaVersion * v1 = JVM_GetVersion(vm1);
    const JavaVersion * v2 = JVM_GetVersion(vm2);
    Bool match1 = (v1->v[0] == javaVersionPreferred.v[0] && 
                   v1->v[1] == javaVersionPreferred.v[1]);
    Bool match2 = (v2->v[0] == javaVersionPreferred.v[0] && 
                   v2->v[1] == javaVersionPreferred.v[1]);
    if (match1 && !match2) return -1;
    if (match2 && !match1) return 1;
    return JVM_CompareVersions(v2, v1);
}

/***************************************************************************
 * main
 **************************************************************************/

int main(int argc, char* argv[])
{
    int i;
    StrBuf buf;
    StrBuf titleBuf;
    char appDir[MAX_PATH];
    char appPath[MAX_PATH];
    JVMSet* jvms;

    /* 
     * install memory allocation hook which will exit the application if
     * memory allocation fails. with this hook, MEM_Alloc never returns NULL.
     */
    MemProc memHook;
    MemContext memContext;
    memset(&memHook,0,sizeof(memHook));
    memHook.memAlloc = ECMTMAN_Malloc;
    memContext.nextHook = MEM_NextHook(MEM_Hook(&memHook,&memContext));

    /* Initialize JNILIB */
    JNI_Init();

    /* initialize local variables */
    STRBUF_Init(&buf);
    STRBUF_Init(&titleBuf);

    /* load the application title from the resources */
    if (WIN32_LoadString(GetModuleHandle(NULL), IDS_APP_TITLE, &titleBuf)) {
        appTitle = STRBUF_Text(&titleBuf);
    }

    /* get the directory where the executable is located */
    if (GetModuleFileName(NULL, appPath, COUNT(appPath))) {
        Str szFilePart = FILE_FilePart(appPath);
        if (szFilePart > appPath) {
            int dirLen = szFilePart - appPath;
            StrnCpy(appDir, appPath, dirLen);
            appDir[dirLen] = 0;
            TRACE1("app dir: %s\n",appDir);
            ASSERT(FILE_IsDir(appDir));
        } else {
            appDir[0] = 0;
        }
    } else {
        appDir[0] = 0;
    }
#ifdef PREFS
    // Ecmulator.exe and Prefs.exe are located in <EPOCROOT>
    // whereas epdt.jar and its resources are located in 
    // <EPOCROOT>\\epoc32\\tools\\ecmt changing cur. dir,
    // because epdt uses relative paths to refer to its resources
    if (strlen(appDir)+strlen(EPDT_DIRECTORY)+1<MAX_PATH){
        strcat(appDir, EPDT_DIRECTORY);
	} 
#endif //PREFS
    SetCurrentDirectory(appDir);

    jvms = JVM_Find();
    if (jvms) {
        JVM* jvm;
        JVM_Sort(jvms, ECMTMAN_SortVM); 
        jvm = JVM_Select(jvms, &javaVersionRequired, False);
        if (jvm) {
            Bool ok = True;

            /* make sure that required files exist */
            for (i=0; i<COUNT(requiredFiles); i++) {
                Str required;
                STRBUF_Format(&buf,"%s%s",appDir,requiredFiles[i]);
                required = STRBUF_Text(&buf);
                if (!FILE_Exist(required)) {
                    ECMTMAN_FormatError(IDS_ERR_MISSING_FILE,required);
                    ok = False;
                    break;
                }
            }

            if (ok) {
                Bool defaultOptUsed[COUNT(defaultOptions)];
                Bool defaultSdkHome = True;
                Bool defaultEpocRoot = True;
                DirEntry* e;
                Char* sdkHome = NULL;
                Char* epocRoot = NULL;
                Char* libPath = NULL;
                Char* classPath = NULL;
                Iterator* jarFiles;
                JavaVM * vm;
                Vector jvmOpt;
                Vector appOpt;
                VECTOR_Init(&jvmOpt,0,vectorEqualsString,NULL);
                VECTOR_Init(&appOpt,0,vectorEqualsString,NULL);

                /* Java classpath */
                STRBUF_Copy(&buf, CLASS_PATH_OPT);

                /* jars in the current directory */
                jarFiles = FILE_ListDir(".");
                while (ITR_HasNext(jarFiles)) {
                    e = ITR_Next(jarFiles);
                    if (e->type == DTypeFile &&
                        STRING_EndsWithNoCase(e->name,".jar")) {
                        STRBUF_Append(&buf, appDir);
                        STRBUF_Append(&buf, e->name);
                        STRBUF_Append(&buf, ";");
                    }
                }
                ITR_Delete(jarFiles);

                /* plus jars in the lib directory */
                jarFiles = FILE_ListDir("lib");
                while (ITR_HasNext(jarFiles)) {
                    e = ITR_Next(jarFiles);
                    if (e->type == DTypeFile &&
                        STRING_EndsWithNoCase(e->name,".jar")) {
                        STRBUF_Append(&buf, appDir);
                        STRBUF_Append(&buf, e->dir);
                        STRBUF_Append(&buf, "\\");
                        STRBUF_Append(&buf, e->name);
                        STRBUF_Append(&buf, ";");
                    }
                }
                ITR_Delete(jarFiles);
                VECTOR_Add(&jvmOpt, classPath = STRBUF_Dup(&buf));

                /* Java library path */
                STRBUF_Copy(&buf,LIB_PATH_OPT);
                STRBUF_Append(&buf,appDir);
                if (strlen(appDir) > 3 && STRBUF_EndsWith(&buf,"\\")) {
                    STRBUF_SetLength(&buf, STRBUF_Length(&buf)-1);
                }
                VECTOR_Add(&jvmOpt, libPath = STRBUF_Dup(&buf));

                /* Split options into JVM and application options */
                memset(defaultOptUsed, 0, sizeof(defaultOptUsed));
                for (i=1; i<argc; i++) {
                    Str arg = argv[i];
                    int k;
                    for (k=0; k<COUNT(defaultOptions); k++) {
                        Str defOpt = defaultOptions[k];
                        int sep = STRING_IndexOf(defOpt,'=');
                        ASSERT(sep > 2);
                        if (strncmp(defOpt,arg,sep+1) == 0) {
                            defaultOptUsed[k] = True;
                            TRACE2("user option %s replaces default %s\n",
                                arg,defOpt);
                            break;
                        }
                    }

                    if (STRING_StartsWith(arg, "-D")) {
                        if (STRING_StartsWith(arg, LIB_PATH_OPT) ||
                            STRING_StartsWith(arg, CLASS_PATH_OPT)) {
                            TRACE1("ignoring option: %s\n",arg);
                        } else {
                            if (STRING_StartsWith(arg, SDK_HOME_OPT)) {
                                TRACE("user defined sdk.home - good luck!\n");
                                defaultSdkHome = False;
                            } else if (STRING_StartsWith(arg,EPOC_ROOT_OPT)) {
                                TRACE("user defined epoc.home\n");
                                defaultEpocRoot = False;
                            }
                            VECTOR_Add(&jvmOpt, arg);
                        }
                    } else {
                        VECTOR_Add(&appOpt, arg);
                    }
                }

                /* add default options */
                for (i=0; i<COUNT(defaultOptions); i++) {
                    if (!defaultOptUsed[i]) {
                        VECTOR_Add(&jvmOpt, defaultOptions[i]);
                    }
                }

                if (defaultSdkHome || defaultEpocRoot) {
                    int pos;
                    static const char epoc32[] = "\\epoc32\\";
                    STRBUF_Copy(&buf,appDir);
                    pos = STRBUF_FindLastNoCase(&buf,epoc32);
                    ASSERT(pos >= 0);
                    if (pos >= 0) {
                        StrBuf opt;
                        STRBUF_Init(&opt);
                        if (defaultSdkHome) {
                            STRBUF_SetLength(&buf, pos+COUNT(epoc32)-1);
                            STRBUF_Copy(&opt, SDK_HOME_OPT);
                            STRBUF_AppendBuf(&opt, &buf);
                            STRBUF_Append(&opt, SDK_HOME_DIR);
                            VECTOR_Add(&jvmOpt, sdkHome = STRBUF_Dup(&opt));
                        }
                        if (defaultEpocRoot) {
                            STRBUF_SetLength(&buf, pos+1);
                            STRBUF_Copy(&opt, EPOC_ROOT_OPT);
                            STRBUF_AppendBuf(&opt, &buf);
                            VECTOR_Add(&jvmOpt, epocRoot = STRBUF_Dup(&opt));
                        }
                        STRBUF_Destroy(&opt);
                    }
                }

                /* dump the command line equivalent of what we are doing */ 
#if DEBUG
                STRBUF_Copy(&buf, JVM_GetHome(jvm));
                STRBUF_Append(&buf, "\\bin\\java.exe");
                for (i=0; i<VECTOR_Size(&jvmOpt); i++) {
                    STRBUF_Append(&buf, " ");
                    STRBUF_Append(&buf, VECTOR_Get(&jvmOpt,i));
                }
                STRBUF_Append(&buf, " " MAIN_CLASS);
                for (i=0; i<VECTOR_Size(&appOpt); i++) {
                    STRBUF_Append(&buf, " ");
                    STRBUF_Append(&buf, VECTOR_Get(&appOpt,i));
                }
                TRACE1("%s\n",STRBUF_Text(&buf));
#endif /* DEBUG */
    
                /* start Java */
                vm = JVM_Create(jvm, VECTOR_Size(&jvmOpt), 
                    (Str*)VECTOR_GetElements(&jvmOpt));
                if (!vm || 
                    !JVM_Main(vm, MAIN_CLASS_JNI, VECTOR_Size(&appOpt),
                        (Str*)VECTOR_GetElements(&appOpt), True)) {
                    ECMTMAN_FormatError(IDS_STARTUP_ERR, appTitle);
                }

                VECTOR_Destroy(&jvmOpt);
                VECTOR_Destroy(&appOpt);
                MEM_Free(classPath);
                MEM_Free(libPath);
                MEM_Free(sdkHome);
                MEM_Free(epocRoot);
            }
        }
        JVM_Delete(jvms);
    } else {
        ECMTMAN_FormatError(IDS_JRE_NOT_FOUND, 
                            javaVersionRequired.v[0],
                            javaVersionRequired.v[1],
                            JAVA_URL,
                            appTitle);
    }

    STRBUF_Destroy(&titleBuf);
    STRBUF_Destroy(&buf);
    JNI_Deinit();
    return 0;
}
