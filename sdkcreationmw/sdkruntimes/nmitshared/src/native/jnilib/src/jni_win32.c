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
* Description:  Win32 specific JNI utilities
 *
*/


#include "jni_libp.h"

/* functions exported from JVM.DLL */
typedef jint (JNICALL * CreateJavaVM)(JavaVM **, void **penv, void *args);
typedef jint (JNICALL * GetCreatedJavaVMs)(JavaVM **, jsize, jsize *);

/* set of discovered JVMs */
struct _JVMSet {
    Vector found;           /* discovered JVMs */
    JVM * defaultVM;        /* pointer to the default VM */
    JVM * specialVM;        /* pointer to the special VM */
};

/* JVM context */
struct _JVM {
    JavaVersion version;    /* parsed version of the JVM */
    int nVersionDigits;     /* number of significant digits in version */
    Char * versionString;   /* the original version string */
    Char * javaHome;        /* JRE home directory */
    Char * javaLib;         /* path to jvm.dll */
    HMODULE hModule;        /* handle to the loaded jvm.dll */
    CreateJavaVM createVM;  /* CreateJavaVM pointer */
    int flags;              /* flags, see below */

#define JVM_FLAG_JDK     1  /* JRE is found under JDK */
#define JVM_FLAG_DEFAULT 2  /* JRE is marked as default in the registry */
#define JVM_FLAG_SPECIAL 4  /* JRE is found in one of the special dirs */
};

/* Type of path to additional JVMs */
typedef enum _JvmPathType {
    JvmPathInvalid,
    JvmPathRelative,
    JvmPathAbsolute,
    JvmPathSystem
} JvmPathType;

/* registry keys */
#define HKLM            HKEY_LOCAL_MACHINE
#define JRE_REG_KEY     "SOFTWARE\\JavaSoft\\Java Runtime Environment"
#define JVM_REG_KEY     "SOFTWARE\\JavaSoft\\Java Development Kit"
#define CURRENT_VERSION "CurrentVersion"
#define JAVA_HOME       "JavaHome"
#define RUNTIME_LIB     "RuntimeLib"

/* directories and files */
#define JRE_UNDER_JDK   "\\jre"
#define JVM_DLL_1        "bin\\client\\jvm.dll"
#define JVM_DLL_2        "bin\\hotspot\\jvm.dll"

/* java class names */
#define JAVA_STRING     "java/lang/String"

/***************************************************************************
 *       Creating Java Vitual Machine (Win32 specific)
 ***************************************************************************/

/**
 * Deallocates JVM context
 */
STATIC void JVM_Free(JVM * jvm)
{
    if (jvm) {
        if (jvm->hModule) FreeLibrary(jvm->hModule);
        MEM_Free(jvm->versionString);
        MEM_Free(jvm->javaHome);
        MEM_Free(jvm->javaLib);
        MEM_Free(jvm);
    }
}

/**
 * Free routine for the vector containing JVM structures
 */
STATIC void JVM_VectorFree(VElement elem)
{
    JVM_Free(elem);
}

/**
 * Equals routine for the vector containing JVM structures
 */
STATIC Bool JVM_VectorEquals(VElementC e1, VElementC e2)
{
    const JVM * jvm1 = e1;
    const JVM * jvm2 = e2;
    if (jvm1->javaLib && jvm2->javaLib) {
        return !StrCaseCmp(jvm1->javaLib, jvm2->javaLib);
    } else {
        return (!JVM_CompareVersions(&jvm1->version, &jvm2->version) &&
                !strcmp(jvm1->versionString, jvm2->versionString) &&
                !strcmp(jvm1->javaHome, jvm2->javaHome) &&
                !strcmp(jvm1->javaLib, jvm2->javaLib));
    }
}

/**
 * Adds JVM to the list unless it's already there. If JVM is not added to
 * the list, it gets deallocated.
 */
STATIC Bool JVM_Add(JVMSet * jvms, JVM * jvm)
{
    if (jvm) {
        int pos = VECTOR_IndexOf(&jvms->found, jvm);
        if (pos < 0) {
            return VECTOR_TryAdd(&jvms->found, jvm);
        } else {
            /* if the new JVM has better flags, replace the old one */
            Bool replace = False;
            JVM * jvm2 = VECTOR_Get(&jvms->found, pos);
            if (!(jvm2->flags & JVM_FLAG_SPECIAL) && 
                 (jvm->flags & JVM_FLAG_SPECIAL)) {
                /* copy version information from the existing JRE */
                Char * tmp = jvm->versionString;
                jvm->version = jvm2->version;
                jvm->nVersionDigits = jvm2->nVersionDigits;
                jvm->versionString = jvm2->versionString;
                jvm2->versionString = tmp;
                replace = True;
            } else if ((jvm2->flags & JVM_FLAG_JDK) && 
                      !(jvm->flags & JVM_FLAG_JDK)) {
                /* we prefer JRE over JDK */
                replace = True;
            } else if (!(jvm2->flags & JVM_FLAG_DEFAULT) && 
                        (jvm->flags & JVM_FLAG_DEFAULT)) {
                /* new JVM is the default one */
                replace = True;
            }

            if (replace) {
                if (jvm2->nVersionDigits > jvm->nVersionDigits) {
                    /* existing JVM has more detailed version information */
                    Char * tmp = jvm->versionString;
                    jvm->version = jvm2->version;
                    jvm->nVersionDigits = jvm2->nVersionDigits;
                    jvm->versionString = jvm2->versionString;
                    jvm2->versionString = tmp;
                }
                TRACE1("JNILIB: duplicate JVM %s\n",jvm->versionString);
                VECTOR_Set(&jvms->found, pos, jvm);
                return True;
            } else {
                if (jvm->nVersionDigits > jvm2->nVersionDigits) {
                    /* new JVM has more detailed version information */
                    Char * tmp = jvm2->versionString;
                    jvm2->version = jvm->version;
                    jvm2->nVersionDigits = jvm->nVersionDigits;
                    jvm2->versionString = jvm->versionString;
                    jvm->versionString = tmp;
                }
                TRACE1("JNILIB: duplicate JVM %s\n",jvm2->versionString);
                JVM_Free(jvm);
            }
        }
    }
    return False;
}

/**
 * Attempts to find java.exe and Jvm.dll path for the specified JVM and
 * updates javaHome and javaLib fields
 */
STATIC Bool JVM_Check(HKEY hJavaKey, JVM * jvm)
{
    Bool ok = False;
    HKEY hVer = NULL; 
    ULONG err;

    /* open the subkey for specific version */
    TRACE2("JNILIB: checking %s %s\n",(jvm->flags & JVM_FLAG_JDK)?"JDK":"JRE",
        jvm->versionString);

    err = RegOpenKeyEx(hJavaKey, jvm->versionString, 0, KEY_READ, &hVer);
    if (err == NO_ERROR) {
        char s[MAX_PATH];
        ULONG t = REG_SZ;
        ULONG n = COUNT(s);
        
        /* "JavaHome" must point to Java home directory */
        err = RegQueryValueEx(hVer,JAVA_HOME,0,&t,(LPBYTE)s,&n);
        if (err == NO_ERROR) {
            s[COUNT(s)-1] = 0;
            if (jvm->flags & JVM_FLAG_JDK) {
                strncat(s, JRE_UNDER_JDK, COUNT(s)-strlen(s));
                s[COUNT(s)-1] = 0;
            }
            if (FILE_IsDir(s)) {
                jvm->javaHome = STRING_Dup(s);

                /* "RuntimeLib" must point to jvm.dll unless this is a JDK */
                s[0] = 0;
                if (!(jvm->flags & JVM_FLAG_JDK)) {
                    n = COUNT(s);
                    err = RegQueryValueEx(hVer,RUNTIME_LIB,0,&t,(LPBYTE)s,&n);
                }
                if ((jvm->flags & JVM_FLAG_JDK) || err == NO_ERROR) {
                    /* If this is a JDK, we have to guess where jvm.dll is */
                    if (jvm->flags & JVM_FLAG_JDK) {
                        snprintf(s,COUNT(s),"%s\\%s",jvm->javaHome,JVM_DLL_1);
                        s[COUNT(s)-1] = 0;
                    }
                    if (FILE_IsFile(s)) {
                        jvm->javaLib = STRING_Dup(s);
                        if (jvm->javaLib) {
                            TRACE1("JNILIB: Java runtime lib: %s\n",s);
                            ok = True;
                        }
                    } else {
                        TRACE1("JNILIB: no such file: %s\n",s);
                        if (jvm->flags & JVM_FLAG_JDK) {
                            snprintf(s,COUNT(s),"%s\\%s",jvm->javaHome,JVM_DLL_2);
                            s[COUNT(s)-1] = 0;
                            if (FILE_IsFile(s)) {
                                jvm->javaLib = STRING_Dup(s);
                                if (jvm->javaLib) {
                                    TRACE1("JNILIB: Java runtime lib: %s\n",s);
                                    ok = True;
                                }
                            } else {
                                TRACE1("JNILIB: no such file: %s\n",s);
                            }
                        }
                    }
                } else {
                    TRACE3("JNILIB: can't read %s from %s\\%s registry key\n",
                        RUNTIME_LIB, (jvm->flags & JVM_FLAG_JDK) ? 
                        JVM_REG_KEY : JRE_REG_KEY, jvm->versionString);
                }
            } else {
                TRACE1("JNILIB: no such directory: %s\n",s);
            }
        } else {
            TRACE3("JNILIB: can't read %s from the %s\\%s\n", JAVA_HOME, 
                (jvm->flags & JVM_FLAG_JDK) ? JVM_REG_KEY : JRE_REG_KEY, 
                jvm->versionString);
        }
        RegCloseKey(hVer);
    }
    return ok;
}

/**
 * Creates JVM context given the JRE or JDK registry key.
 */
STATIC JVM * JVM_CreateRegContext(HKEY hJavaKey, Str szVersion, Bool isJdk)
{
    int n;
    JavaVersion version;
    if (JVM_ParseVersion2(szVersion,&version,&n)) {
        JVM * jvm = MEM_New(JVM);
        if (jvm) {
            memset(jvm, 0, sizeof(*jvm));
            if (isJdk) jvm->flags |= JVM_FLAG_JDK;
            jvm->version = version;
            jvm->versionString = STRING_Dup(szVersion);
            jvm->nVersionDigits = n;
            if (jvm->versionString) {
                if (JVM_Check(hJavaKey, jvm)) {
                    return jvm;
                }
            }
            JVM_Free(jvm);
        }
    } else {
        TRACE1("JNILIB: unparsable Java version: %s\n",szVersion);
    }
    return NULL;
}

/**
 * Creates JVM context given the JRE home dir. We assume that the parameter
 * points to existing directory. Note that in this case we don't know the
 * version of JRE. We could figure it out by reading the manifest from rt.jar
 * but that might take considerable amount of time. A typical size of rt.jar
 * is over 25MB. Instead, we assume that if the caller has given us a path
 * to its own JRE installation, then this is the preferred version of JRE.
 */
STATIC JVM * JVM_CreateDirContext(Str home)
{
    JVM * jvm = MEM_New(JVM);
    if (jvm) {
        memset(jvm, 0, sizeof(*jvm));
        jvm->versionString = STRING_Dup("UNKNOWN");
        jvm->javaHome = STRING_Dup(home);
        if (jvm->versionString && jvm->javaHome) {
            char s[MAX_PATH];

            /* Normalize the Java home path */
            Char* p;
            int len = StrLen(jvm->javaHome);
            for (p=StrChr(jvm->javaHome,'/'); p; p=strchr(p,'/')) *p = '\\';
            while (len > 0 && jvm->javaHome[len-1] == '\\') {
                jvm->javaHome[--len] = 0;
            }
            
            /* find jvm.dll */
            snprintf(s,COUNT(s),"%s\\%s",jvm->javaHome,JVM_DLL_1);
            s[COUNT(s)-1] = 0;

            if (FILE_IsFile(s)) {
                jvm->javaLib = STRING_Dup(s);
                if (jvm->javaLib) {
                    TRACE1("JNILIB: Java runtime lib: %s\n",s);
                    return jvm;
                }
            } else {
                TRACE1("JNILIB: no such file: %s\n",s);
                snprintf(s,COUNT(s),"%s\\%s",jvm->javaHome,JVM_DLL_2);
                s[COUNT(s)-1] = 0;
                if (FILE_IsFile(s)) {
                    jvm->javaLib = STRING_Dup(s);
                    if (jvm->javaLib) {
                        TRACE1("JNILIB: Java runtime lib: %s\n",s);
                        return jvm;
                    }
                } else {
                    TRACE1("JNILIB: no such file: %s\n",s);
                }
            }
        }
        JVM_Free(jvm);
    }
    return NULL;
}

/**
 * JVM discovery. Returns number of discovered JVMs
 */
STATIC int JVM_Discover(JVMSet * jvms)
{
    HKEY hRoot = NULL; 
    ULONG err = RegOpenKeyEx(HKLM, JRE_REG_KEY, 0, KEY_READ, &hRoot);
    char curVer[32];
    char ver[COUNT(curVer)];
    if (err == NO_ERROR) {
        ULONG nIndex = 0;

        /* determine the current version */
        ULONG t = REG_SZ;
        ULONG n = COUNT(ver);
        TRACE("JNILIB: checking current version of JRE...\n");
        err = RegQueryValueEx(hRoot,CURRENT_VERSION,0,&t,(LPBYTE)curVer,&n);
        if (err == NO_ERROR) {            
            JVM * jvm;
            curVer[COUNT(curVer)-1] = 0;  /* just in case */
            TRACE1("JNILIB: current Java version: %s\n", curVer);
            jvm = JVM_CreateRegContext(hRoot, curVer, False);
            if (jvm) {
                jvm->flags |= JVM_FLAG_DEFAULT;
                if (JVM_Add(jvms, jvm)) {
                    jvms->defaultVM = jvm;
                }
            }
        }

        TRACE("JNILIB: trying to find other versions of JRE...\n");
        while (RegEnumKey(hRoot, nIndex, ver, COUNT(ver)) == NO_ERROR) {
            if (StrCmp(ver, curVer)) {
                JVM_Add(jvms, JVM_CreateRegContext(hRoot, ver, False));
            }
            nIndex++;
        }
        RegCloseKey(hRoot);
    }

    err = RegOpenKeyEx(HKLM, JVM_REG_KEY, 0, KEY_READ, &hRoot);
    if (err == NO_ERROR) {
        ULONG nIndex = 0;

        /* determine the current version */
        curVer[0] = 0;
        if (!jvms->defaultVM) {
            ULONG t = REG_SZ;
            ULONG n = COUNT(ver);
            TRACE("JNILIB: checking current version of JVM...\n");
            err = RegQueryValueEx(hRoot,CURRENT_VERSION,0,&t,(LPBYTE)curVer,&n);
            if (err == NO_ERROR) {            
                JVM * jvm;
                curVer[COUNT(curVer)-1] = 0;  /* just in case */
                TRACE1("JNILIB: current Java version: %s\n", curVer);
                jvm = JVM_CreateRegContext(hRoot, curVer, True);
                if (jvm) {
                    jvm->flags |= JVM_FLAG_DEFAULT;
                    if (JVM_Add(jvms, jvm)) {
                        jvms->defaultVM = jvm;
                    }
                }
            }
        }

        TRACE("JNILIB: trying to find other versions of JVM...\n");
        while (RegEnumKey(hRoot, nIndex, ver, COUNT(ver)) == NO_ERROR) {
            if (StrCmp(ver, curVer)) {
                JVM_Add(jvms, JVM_CreateRegContext(hRoot, ver, True));
            }
            nIndex++;
        }
        RegCloseKey(hRoot);
    }

    return VECTOR_Size(&jvms->found);
}

/**
 * Compares two Java versions. Returns a negative number, zero or a
 * positive number if v1 is less than, equal or greater than v2,
 * respectively. Version 0.0.0.0 is considered greater than any other 
 * version (other than 0.0.0.0)
 */
int JVM_CompareVersions(const JavaVersion * v1, const JavaVersion * v2)
{
    int i;
    for (i=0; i<JAVA_VERSION_SIZE; i++) {
        if (v1->v[i] > v2->v[i]) {
            return 1;
        } else if (v1->v[i] < v2->v[i]) {
            return -1;
        }
    }
    return 0;
}

/**
 * Sorts JVMs in descending order
 */
STATIC int JVM_DefaultSort(const JVM * vm1, const JVM * vm2)
{
    if ((vm1->flags & JVM_FLAG_SPECIAL) && !(vm2->flags & JVM_FLAG_SPECIAL)) {
        return -1;
    }
    if (!(vm1->flags & JVM_FLAG_SPECIAL) && (vm2->flags & JVM_FLAG_SPECIAL)) {
        return 1;
    }
    return JVM_CompareVersions(&vm2->version, &vm1->version);
}

/**
 * Checks whether the path is relative or absolute. Assume 
 */
STATIC JvmPathType JVM_GetPathType(Str path)
{
    Char drv;
    if (!path || !path[0]) return JvmPathInvalid;
    if (path[0] == '.') return JvmPathRelative;
    drv = ToUpper(path[0]); /* this can be be a drive letter */
    if (drv >= 'A' && drv <= 'Z' && path[1] == ':') return JvmPathAbsolute;
    if (path[0] == '/' || path[0] == '\\') {
        /* reject network (UNC) paths */
        if (path[1] == '/' || path[1] == '\\') return JvmPathInvalid;
        return JvmPathSystem;
    }
    return JvmPathRelative;
}

/**
 * Returns the module file name, using the specified buffer to actually store
 * the file name. Returns NULL in case of any problem, such as wrong module
 * handle or memory allocation failure.
 */
STATIC Str JVM_GetModuleFileName(HMODULE hModule, StrBuf * sb)
{
    size_t maxlen = (sb->alloc ? ((sb->alloc-1)/2) : 0);
    do {
        STRBUF_Clear(sb);
        maxlen = MAX(2*maxlen,31);
        if (!STRBUF_Alloc(sb, maxlen)) {
            /* cannot allocate the memory */
            return NULL;
        }
        sb->len = GetModuleFileName(hModule, sb->s, maxlen);
        ASSERT(sb->len <= maxlen);
        sb->s[maxlen] = 0;
        ASSERT(!sb->s[sb->len]);
        if (!sb->len) {
            return NULL;    /* bad module handle, I guess */
        }
    } while (sb->len >= (maxlen-1));
    return STRBUF_GetString(sb);
}

/**
 * Discovers all available JVMs. If no JVMs are discovered, returns NULL.
 * In addition to the standard directories, also looks in the additional
 * directories specified by the dirs array. Note that this directory list
 * replaces the default list ("jre","../jre") used by JVM_Find, it does not
 * adds new directories to the list.
 */
JVMSet * JVM_Find2(const Str dirs[], int n)
{
    JVMSet * jvms = MEM_New(JVMSet);
    if (jvms) {
        memset(jvms, 0, sizeof(*jvms));
        if (VECTOR_Init(&jvms->found, 0, JVM_VectorEquals, JVM_VectorFree)) {

            /* Look for JVMs in the Windows registry */
            JVM_Discover(jvms);

            /* Look for JVMs in the additional directories */
            if (n > 0) {
                int i;
                StrBuf sb,sb2;
                Char* baseDir = NULL;
                STRBUF_Init(&sb);
                STRBUF_Init(&sb2);
                TRACE("JNILIB: checking special directories\n");
                for (i=0; i<n; i++) {
                    Str javaHome = NULL;
                    JvmPathType pathType = JVM_GetPathType(dirs[i]);
                    if (pathType == JvmPathRelative) {
                        LPTSTR filePath;
                        TRACE1("JNILIB: relative path: %s\n",dirs[i]);
                        if (baseDir) {
                            STRBUF_Copy(&sb, baseDir);
                        } else {
                            int separator;
                            JVM_GetModuleFileName(NULL,&sb);
                            STRBUF_Replace(&sb, '/', '\\');
                            separator = STRBUF_LastIndexOf(&sb,'\\');
                            STRBUF_SetLength(&sb,separator+1);
                            baseDir = STRBUF_Dup(&sb);
                            if (!baseDir) continue;
                            TRACE1("JNILIB: base dir: %s\n",baseDir);
                        }
                        STRBUF_Append(&sb, dirs[i]);
                        STRBUF_Replace(&sb, '/', '\\');
                        STRBUF_Alloc(&sb2, STRBUF_Length(&sb));
                        sb2.len = GetFullPathName(STRBUF_Text(&sb), 
                            sb2.alloc, sb2.s, &filePath);
                        ASSERT(sb2.len && sb2.s[0]);
                        javaHome = STRBUF_Text(&sb2);
                    } else if (pathType == JvmPathAbsolute) {
                        TRACE1("JNILIB: absolute path: %s\n",dirs[i]);
                        javaHome = dirs[i];
                    } else if (pathType == JvmPathSystem) {
                        /* directory on the system drive */
                        TRACE1("JNILIB: system path: %s\n",dirs[i]);
                        STRBUF_Alloc(&sb,GetSystemDirectory(NULL,0)+1);
                        STRBUF_SetLength(&sb,GetSystemDirectory(sb.s,sb.alloc));
                        STRBUF_Clear(&sb2);
                        STRBUF_AppendChar(&sb2,STRBUF_CharAt(&sb,0));
                        STRBUF_AppendChar(&sb2,':');
                        STRBUF_Append(&sb2, dirs[i]);
                        javaHome = STRBUF_Text(&sb2);
                    } else {
                        TRACE1("JNILIB: invalid path: %s\n",dirs[i]);
                        continue;
                    }
                    if (javaHome) {
                        TRACE1("JNILIB: Java home: %s\n",javaHome);
                        if (FILE_IsDir(javaHome)) {
                            JVM* jvm = JVM_CreateDirContext(javaHome);
                            if (jvm) {
                                jvm->flags |= JVM_FLAG_SPECIAL;
                                if (JVM_Add(jvms, jvm) && !jvms->specialVM) {
                                    jvms->specialVM = jvm;
                                }
                            }
                        } else {
                            TRACE1("JNILIB: no such directory: %s\n",javaHome);
                        }
                    }
                }
                MEM_Free(baseDir);
                STRBUF_Destroy(&sb);
                STRBUF_Destroy(&sb2);
            }

            /* Did we find anything? */
            if (!VECTOR_IsEmpty(&jvms->found)) {
                JVM_Sort(jvms, JVM_DefaultSort);
                TRACE1("JNILIB: found %d JVM(s)\n",VECTOR_Size(&jvms->found));
                return jvms;
            }
            TRACE("JNILIB: found no JVMs\n, sorry");
            VECTOR_Destroy(&jvms->found);
        }
        MEM_Free(jvms);
    }
    return NULL;
}

/**
 * Discovers all available JVMs. If no JVMs are discovered, returns NULL.
 * In addition to the standard directories, also looks in the additional
 * directories specified by the dirs array. Note that this directory list
 * replaces the default list ("jre","../jre") used by JVM_Find, it does not
 * adds new directories to the list.
 */
JVMSet * JVM_Find()
{
    static const Str defaultExtraDirs[] = {
        TEXT("jre"),
        TEXT("..\\jre")
    };
    return JVM_Find2(defaultExtraDirs, COUNT(defaultExtraDirs));
}

/**
 * This does essentially the same as JVM_Find2, only additional directories
 * are passed in as a variable list of strings. The list of directories
 * must be NULL terminated, for example:
 *
 *   JVM_FindVa("../lib/jre", NULL);
 */
JVMSet * JVM_FindVa(Str dir1, ...)
{
    if (!dir1) {
        return JVM_Find2(NULL, 0);
    } else {
        JVMSet* jvms;
        Str arg = dir1;
        va_list va;
        Vector dirs;
        VECTOR_Init(&dirs, 0, NULL, NULL);
        va_start(va, dir1);
        while (arg) {
            if (!VECTOR_Add(&dirs, arg)) {
                VECTOR_Destroy(&dirs);
                return NULL;
            }
            arg = va_arg(va, Str);
        }
        va_end(va);
        jvms = JVM_Find2((Str*)VECTOR_GetElements(&dirs), VECTOR_Size(&dirs));
        VECTOR_Destroy(&dirs);
        return jvms;
    }
}

/**
 * Parses Java version string. Also returns the number of digits in the
 * version string that have been successfully parsed. All non-parsed digits
 * are filled with zeros. The ndigits pointer may be NULL.
 */
Bool JVM_ParseVersion2(Str strVersion, JavaVersion * version, int* ndigits)
{
    int i;
    Str p = strVersion;
    StrBuf16 buf;
    STRBUF_InitBufXXX(&buf);

    memset(version, 0, sizeof(*version));
    for (i=0; i<JAVA_VERSION_SIZE; i++) {
        STRBUF_Clear(&buf.sb);
        while (*p && !IsDigit(*p)) p++;
        if (*p) {
            while (IsDigit(*p)) STRBUF_AppendChar(&buf.sb, *p++);
            if (STRBUF_Length(&buf.sb) > 0) {
                if (PARSE_Int(STRBUF_Text(&buf.sb),&(version->v[i]),10)) {
                    continue;
                }
            }
        }
        break;
    }

    if (ndigits) *ndigits = i;
    STRBUF_Destroy(&buf.sb);
    return (i >= 2);
}

/**
 * Parses Java version string. 
 */
Bool JVM_ParseVersion(Str strVersion, JavaVersion * version)
{
    return JVM_ParseVersion2(strVersion, version, NULL);
}

/**
 * Checks if this JRE is the default one. It's not necessary the one with
 * the highest version. It's probably the most recently installed one.
 */
Bool JVM_IsDefault(const JVM * jvm)
{
    return ((jvm->flags & JVM_FLAG_DEFAULT) != 0);
}

/**
 * Checks if this JRE is the special one, i.e. the one loaded from one of the
 * special directories given to JVM_Find2 or JVM_FindVa, or from one of the
 * default special directories used by JVM_Find ("jre" and "../jre")
 */
Bool JVM_IsSpecial(const JVM * jvm)
{
    return ((jvm->flags & JVM_FLAG_SPECIAL) != 0);
}

/**
 * Returns the JVM version
 */
const JavaVersion * JVM_GetVersion(const JVM * jvm)
{
    return &jvm->version;
}

/**
 * Returns the original Java version string
 */
Str JVM_GetVersionString(const JVM * jvm)
{
    return jvm->versionString;
}

/**
 * Returns the JVM home path
 */
Str JVM_GetHome(const JVM * jvm)
{
    return jvm->javaHome;
}

/**
 * Returns the path of the JVM dynamically loadable library.
 */
Str JVM_GetPath(const JVM * jvm)
{
    return jvm->javaLib;
}

/**
 * Sorts the JVMs according to the provided rule.
 */
void JVM_Sort(JVMSet * jvms, JvmSortProc proc)
{
    VECTOR_Sort(&jvms->found, proc);
}

/**
 * Returns the default JVM. It's not necessary the one with highest version.
 * It's probably the most recently installed one.
 */
JVM * JVM_GetDefault(JVMSet * jvms)
{
    return jvms->defaultVM;
}

/**
 * Returns the special JVM found in one of the special directories given to 
 * JVM_Find2 or JVM_FindVa, or from one of the default special directories
 * used by JVM_Find ("jre" and "../jre"). Returns NULL if none were found.
 */
JVM * JVM_GetSpecial(JVMSet * jvms)
{
    return jvms->specialVM;
}

/**
 * Returns the number of discovered JVMs. The return value is always > 0
 */
int JVM_GetCount(const JVMSet * jvms)
{
    return VECTOR_Size(&jvms->found);
}

/**
 * Returns i-th discovered JVM. By default, JVMs are sorted by version, 
 * i.e. the first JVM in the list has the most recent version.
 */
JVM * JVM_Get(JVMSet * jvms, int i)
{
    return (JVM *)VECTOR_Get(&jvms->found, i);
}

/**
 * Selects the JVM that has the specific version. If the last paramter is
 * True, returns the JVM with version exactly matching the requested one.
 * If the last parameter is False, returns the JVM that has the highest 
 * version greater or equal the requested version (recommended use).
 * If no JVMs match the criteria, return NULL.
 */
JVM * JVM_Select(JVMSet * jvms, const JavaVersion * v, Bool exact)
{
    int i, n = JVM_GetCount(jvms);
    JVM * selected = NULL;
    for(i=0; i<n; i++) {
        JVM * jvm = JVM_Get(jvms, i);
        if (jvm->flags & JVM_FLAG_SPECIAL) {
            /* special JVMs match any criteria and terminate the search */
            selected = jvm;
            break;
        } else if (!selected) {
            int c = JVM_CompareVersions(v, &jvm->version);
            if ((exact && c == 0) || (!exact && c <= 0)) {
                /* we have found first JVM that matches the criteria 
                 * but don't break the look just yet, keep looking 
                 * for a special JVM */
                selected = jvm;
            }
        }
    }
    return selected;
}

/**
 * Loads the JVM. Every time this method is invoked, it attempts to create 
 * a new JavaVM context. 
 */
JavaVM * JVM_Create(JVM * jvm, int argc, Str args[])
{
    static Str szCreateVM = "JNI_CreateJavaVM";

    /* load the JVM library */
    if (!jvm->hModule) {
        jvm->hModule = LoadLibrary(jvm->javaLib);
        if (jvm->hModule) {
            FARPROC proc = GetProcAddress(jvm->hModule,szCreateVM);
            if (proc) {
                jvm->createVM = (CreateJavaVM)proc;
            } else {
                Error("ERROR: %s not found in %s\n",szCreateVM,jvm->javaLib);
                FreeLibrary(jvm->hModule);
                jvm->hModule = NULL;
            }
        }
    }

    /* create new VM */
    if (jvm->createVM) {
        JavaVMInitArgs vm_args;
        memset(&vm_args, 0, sizeof(vm_args));
        vm_args.version = JNI_VERSION_1_2;
        vm_args.ignoreUnrecognized = JNI_TRUE;
        vm_args.nOptions = argc;
        if (argc) {
            vm_args.options = MEM_NewArray(JavaVMOption,argc);
            if (vm_args.options) {
                int i;
                for (i=0; i<vm_args.nOptions; i++) {
                    JavaVMOption * option = vm_args.options + i;
                    memset(option, 0, sizeof(*option));
                    option->optionString = (char*)args[i];
                }
            }
        }

        if (!argc || vm_args.options) {
            __try {
                JNIEnv * env = NULL;
                JavaVM * vm = NULL;
                int status = jvm->createVM(&vm, (void**)&env, &vm_args);
                ASSERT(status >= 0);
                if (status >= 0) {
                    MEM_Free(vm_args.options);
                    return vm;
                }
            } __except(EXCEPTION_EXECUTE_HANDLER) {
                ASSMSG2("EXCEPTION %08lX in %s",GetExceptionCode(),szCreateVM);
            }
            MEM_Free(vm_args.options);
        }
    }
    return NULL;
}

/**
 * Calls the main method of the specified class with the provided arguments.
 * If the last parameter is True, invokes (*vm)->DestroyJavaVM(vm) which
 * waits until the only threads running are all daemon threads and destroys
 * JavaVM. If you plan to reuse this JavaVM, the last parameter must be False.
 */
Bool JVM_Main(JavaVM * vm, Str cls, int argc, Str args[], Bool wait)
{
    Bool ok = False;
    jobjectArray javaArgs = NULL;

    /* obtain JNIEnv pointer */
    JNIEnv * env = NULL;
    (*vm)->AttachCurrentThread(vm,(void**)&env, NULL);
    ASSERT(env);

    /* prepare arguments */
    if (env) {
        javaArgs = JNI_NewObjectArray(env,JAVA_STRING,argc);
        if (javaArgs) {
            int i;
            for (i=0; i<argc; i++) {
                jstring arg = JNI_NewString(env,args[i]);
                if (arg) {
                    (*env)->SetObjectArrayElement(env,javaArgs,i,arg);
                    (*env)->DeleteLocalRef(env, arg);
                } else {
                    (*env)->DeleteLocalRef(env, javaArgs);
                    javaArgs = NULL;
                    break;
                }
            }
        }
    }

    if (javaArgs) {
        JavaMethod main;

        /* call main method */
        memset(&main,0,sizeof(main));
        main.cls = cls;
        main.met = "main";
        main.sig = "([L"JAVA_STRING";)V";
        ok = JNI_CallStaticVoidMethod(env,&main,javaArgs) && 
            !JNI_ExceptionOccurred();

        /* release references to the arguments */
        (*env)->DeleteLocalRef(env, javaArgs);
        
        if (ok) {

            /*
             * Detach the current thread so that it appears
             * to have exited when the application's main 
             * method exits. 
             */
            (*vm)->DetachCurrentThread(vm);

            /*
             * DestroyJavaVM will wait until the only threads 
             * running are all daemon threads
             */
            if (wait) (*vm)->DestroyJavaVM(vm);
        }
    }

    return ok;
}

/**
 * Deletes the JVM context. Do not invoke this function until after you have
 * destroyed all JavaVMs created by JVM_Start function, because this function
 * will unload all loaded JVM libraries and unmap them from the process 
 * address space
 */
void JVM_Delete(JVMSet * jvms)
{
    if (jvms) {
        VECTOR_Destroy(&jvms->found);
        MEM_Free(jvms);
    }
}

/**
 * Detects the presence of JVM in the context of the current process and
 * returns its JavaVM context. If no JVM has been detected, returns NULL
 */
JavaVM * JNI_GetCurrentVM()
{
    JavaVM * vm = NULL;
    HMODULE hMod = GetModuleHandle("jvm");
    if (hMod) {
        FARPROC proc = GetProcAddress(hMod,"JNI_GetCreatedJavaVMs");
        ASSERT(proc);
        if (proc) {
            JavaVM * jvms[1];
            jsize max = COUNT(jvms);
            jsize n = 0;
            memset((void*)jvms, 0, sizeof(jvms));
            ((GetCreatedJavaVMs)proc)(jvms, max, &n);
            if (n > 0) {
#if DEBUG
                Char path[MAX_PATH];
                GetModuleFileName(hMod, path, COUNT(path));
                TRACE1("JNILIB: detected running JVM from %s\n",path);
#endif /* DEBUG */
                vm = jvms[0];
            } else {
                TRACE("JNILIB: JVM is loaded, but no running VM found\n");
            }
        }
    } else {
        TRACE("JNILIB: could not detect running JVM\n");
    }
    return vm;
}
