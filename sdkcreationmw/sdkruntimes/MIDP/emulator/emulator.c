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
* Description:  UEI executable for Series 60 SDK
 *
*/


static char rcsid[] = "$Id: emulator.c,v 1.10 2006/05/04 22:51:04 slava Exp $";

#include <jni_lib.h>
#include <w_lib.h>
#include <xrpc.h>
#include "emulator.h"

/* we require at least JRE 1.4, but prefer 1.5 */
static JavaVersion javaVersionRequired = {{1,4,0,0}};
static JavaVersion javaVersionPreferred = {{1,5,0,0}};

/* the application title is replaced with localized version when app starts */
static Str appTitle = "Series 60 SDK for MIDP";

#define SEI_JAR             "..\\epoc32\\tools\\ecmt\\nei.jar"

/* Don't require nei.exe - error message will be provided by Java code */
static Str requiredFiles[] = { SEI_JAR };

/* Possible values of SEI_PACKAGE are emulator and utils */
#ifndef SEI_PACKAGE
#  define SEI_PACKAGE emulator
#endif /* SEI_PACKAGE */
#define T__(x) #x
#define T_(x) T__(x)

#define MAIN_CLASS_JNI      "com/symbian/tools/j2me/sei/" T_(SEI_PACKAGE) "/Main"

#define CLASS_PATH_OPT      "-Djava.class.path="
#define LOGON_ID_OPT        "-Dxrpc.logon-id="
#define SDK_HOME_OPT        "-Dsdk.home="
//#define SDK_HOME_DIR        "epoc32\\tools\\java\\sei\\winscw\\udeb\\hmidps60v30"
#define SDK_HOME_DIR        ".."

#define JAVA_URL            "http://java.sun.com/"

/* *************************************************************************
 *  Error reporting
 * *************************************************************************/

STATIC void LAUNCH_ErrorMessage(Str msg)
{
    PRINT_Error("ERROR: %s\n", msg);
    MessageBox(NULL, msg, appTitle, MB_OK|MB_ICONINFORMATION);
}

/**
 * Format system error message and show it
 */
STATIC void LAUNCH_SysError(ULONG err)
{
    LPSTR lpMsgBuf = NULL;
    if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                      FORMAT_MESSAGE_FROM_SYSTEM |     
                      FORMAT_MESSAGE_IGNORE_INSERTS,    
                      NULL, err, 
                      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                      (LPTSTR)&lpMsgBuf, 0, NULL )) {
        LAUNCH_ErrorMessage(lpMsgBuf);
        LocalFree(lpMsgBuf);
    } else {
        char szMsg[64];
        sprintf(szMsg,"ERROR %lu",err);
        LAUNCH_ErrorMessage(szMsg);
    }
}

/**
 * Format error message and show it
 */
STATIC void LAUNCH_FormatError(UINT uMsgID, ...)
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
    LAUNCH_ErrorMessage(msg);
    STRBUF_Destroy(&buf.sb);
}

/***************************************************************************
 * Allocates memory. Never returns NULL. If memory allocation fails, pops 
 * up an error message and exits. Hopfully, this never happens...
 **************************************************************************/

struct _MemContext {
    const MemHook* nextHook;
};

STATIC void* LAUNCH_Malloc(MemContext* ctx, size_t size)
{
    void* ptr = MEM_Alloc2(ctx->nextHook, size);
    if (!ptr) {
        LAUNCH_ErrorMessage("out of memory!");
        exit(-1);
    }
    return ptr;
}

/***************************************************************************
 * JVM selection criteria
 **************************************************************************/

STATIC int LAUNCH_SortVM(const JVM * vm1, const JVM * vm2)
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
    JVMSet* jvms;

    /* 
     * install memory allocation hook which will exit the application if
     * memory allocation fails. with this hook, MEM_Alloc never returns NULL.
     */
    MemProc memHook;
    MemContext memContext;
    memset(&memHook,0,sizeof(memHook));
    memHook.memAlloc = LAUNCH_Malloc;
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
    if (GetModuleFileName(NULL, appDir, COUNT(appDir))) {
        Str szFilePart = FILE_FilePart(appDir);
        if (szFilePart > appDir) {
            int len = szFilePart - appDir;
            appDir[len] = 0;
            while (STRING_EndsWith(appDir,"\\")) appDir[--len] = 0;
            TRACE1("app dir: %s\n",appDir);
            ASSERT(FILE_IsDir(appDir));
        } else {
            appDir[0] = 0;
        }
    } else {
        appDir[0] = 0;
    }

    SetCurrentDirectory(appDir);

    jvms = JVM_Find();
    if (jvms) {
        JVM* jvm;
        JVM_Sort(jvms, LAUNCH_SortVM); 
        jvm = JVM_Select(jvms, &javaVersionRequired, False);
        if (jvm) {
            Bool ok = True;

            /* make sure that required files exist */
            for (i=0; i<COUNT(requiredFiles); i++) {
                Str required;
                STRBUF_Format(&buf,"%s\\%s",appDir,requiredFiles[i]);
                required = STRBUF_Text(&buf);
                if (!FILE_Exist(required)) {
                    LAUNCH_FormatError(IDS_ERR_MISSING_FILE,required);
                    ok = False;
                    break;
                }
            }

            if (ok) {
                Bool defaultSdkHome = True;
                Char* sdkHomeOpt = NULL;
                Char* classPathOpt;
                Char* logonIdOpt = NULL;
                JavaVM * vm;
                char* logonId;
                Vector jvmOpt;
                Vector appOpt;
                VECTOR_Init(&jvmOpt,0,vectorEqualsString,NULL);
                VECTOR_Init(&appOpt,0,vectorEqualsString,NULL);

                /* Java classpath */
                STRBUF_Copy(&buf, CLASS_PATH_OPT);
                STRBUF_Append(&buf, appDir);
                STRBUF_Append(&buf, ";");    // Petchatnikov: in order to read .properties 
                STRBUF_Append(&buf, appDir); // from a file in FS, not from .jar only
                STRBUF_Append(&buf, "\\");
                STRBUF_Append(&buf, SEI_JAR);

                /* store the pointer so that we can later deallocate it */
                VECTOR_Add(&jvmOpt, classPathOpt = STRBUF_Dup(&buf));

                /* XRPC logon ID */
                logonId = XRPC_GetLogonID();
                if (logonId) {
                    STRBUF_Copy(&buf, LOGON_ID_OPT);
                    STRBUF_Append(&buf, logonId);
                    VECTOR_Add(&jvmOpt, logonIdOpt = STRBUF_Dup(&buf));
                }

                /* Split options into JVM and application options */
                for (i=1; i<argc; i++) {
                    Str arg = argv[i];
                    if (STRING_StartsWith(arg, "-D")) {
                        if (STRING_StartsWith(arg, CLASS_PATH_OPT)) {
                            TRACE1("ignoring option: %s\n",arg);
                        } else if (STRING_StartsWith(arg, LOGON_ID_OPT)) {
                            TRACE1("ignoring option: %s\n",arg);
                        } else if (STRING_StartsWith(arg, SDK_HOME_OPT)) {
                            defaultSdkHome = False;
                        } else {
                            VECTOR_Add(&jvmOpt, arg);
                        }
                    } else {
                        VECTOR_Add(&appOpt, arg);
                    }
                }

                if (defaultSdkHome) {
                    STRBUF_Copy(&buf, SDK_HOME_OPT);
                    STRBUF_Append(&buf, appDir);
                    STRBUF_Append(&buf, "\\");
                    STRBUF_Append(&buf, SDK_HOME_DIR);
                    VECTOR_Add(&jvmOpt, sdkHomeOpt = STRBUF_Dup(&buf));
                }

#if DEBUG /* dump the command line equivalent of what we are doing */ 
                STRBUF_Copy(&buf, JVM_GetHome(jvm));
                STRBUF_Append(&buf, "\\bin\\java.exe");
                for (i=0; i<VECTOR_Size(&jvmOpt); i++) {
                    STRBUF_Append(&buf, " ");
                    STRBUF_Append(&buf, VECTOR_Get(&jvmOpt,i));
                }
                i = STRBUF_Length(&buf);
                STRBUF_Append(&buf, " " MAIN_CLASS_JNI);
                for (;i < (int)STRBUF_Length(&buf); i++) {
                    if (buf.s[i] == '/') buf.s[i] = '.';
                }
                for (i=0; i<VECTOR_Size(&appOpt); i++) {
                    STRBUF_Append(&buf, " ");
                    STRBUF_Append(&buf, VECTOR_Get(&appOpt,i));
                }
                TRACE1("%s\n",STRBUF_Text(&buf));
#endif  /* DEBUG */
    
                /* start Java */
                vm = JVM_Create(jvm, VECTOR_Size(&jvmOpt), 
                    (Str*)VECTOR_GetElements(&jvmOpt));
                if (!vm || 
                    !JVM_Main(vm, MAIN_CLASS_JNI, VECTOR_Size(&appOpt),
                        (Str*)VECTOR_GetElements(&appOpt), True)) {
                    LAUNCH_FormatError(IDS_STARTUP_ERR, appTitle);
                }

                VECTOR_Destroy(&jvmOpt);
                VECTOR_Destroy(&appOpt);
                MEM_Free(logonIdOpt);
                MEM_Free(classPathOpt);
                MEM_Free(sdkHomeOpt);
            }
        }
        JVM_Delete(jvms);
    } else {
        LAUNCH_FormatError(IDS_JRE_NOT_FOUND, 
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
