/*
* Copyright (c) 2003-2005 Nokia Corporation and/or its subsidiary(-ies).
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


#include "s_lib.h"
#include "instreg.h"

/* Exit codes */
#define EXITCODE_SUCCESS    0   /* successful completion */
#define EXITCODE_HELP       1   /* help printed, nothing done */
#define EXITCODE_CMDLINE    2   /* command line parse error */
#define EXITCODE_IOERR      3   /* cannot write installation registry */
#define EXITCODE_NOMEM      4   /* out of memory (very unlikely) */

/* Data structures */
struct _MemContext {
    const MemHook * next;
};

typedef struct _AppContext {
    Bool unregister;            /* true if we are unregistering the product */
    StrBuf sb;                  /* temporary string buffer */
    DOMNode* root;              /* root node of the DOM tree */
    Str name;                   /* product name */
    Str ver;                    /* product version, NULL if unknown */
    Str loc;                    /* product location */
    Str exe;                    /* product executable */
    Str type;                   /* product type */
    Str nameEsc;                /* XML-escaped product name */
    Str typeEsc;                /* XML-escaped product type */
    Str verEsc;                 /* XML-escaped version */
    Str exeEsc;                 /* XML-escaped path */
    Str locEsc;                 /* XML-escaped location */
} AppContext;

/* Tags in the installation registry file */
#define REGISTRY_TAG    "registry"
#define PRODUCT_TAG     "product"
#define NAME_TAG        "name"
#define VERSION_TAG     "version"
#define EXECUTABLE_TAG  "executable"
#define LOCATION_TAG    "location"
#define TYPE_ATTR       "type"
#define EMULATOR_TYPE   "emulator"

#define ROOT_TAG        REGISTRY_TAG

/*
 * Some default values. Note that we are making an assumption that the
 * default values don't need to be XML-escaped.
 */
#define DEFAULT_TYPE    EMULATOR_TYPE
#define DEFAULT_VERSION "1.0"

/* Desctiption of the command line parameters */
#define CMDLINE_PARAM   "PRODUCT"

/* Modifiable global data */
static Char* pname = "instreg";
static Char regfile[] = "C:\\Nokia\\Registry\\installationRegistry.xml";

/* Constants */
static const Str commonHeader = "\
<?xml version=\"1.0\" standalone=\"yes\"?>\n\
\n\
<!DOCTYPE "REGISTRY_TAG" [\n\
<!ELEMENT "REGISTRY_TAG" ("PRODUCT_TAG")*>\n\
<!ELEMENT "PRODUCT_TAG" ("NAME_TAG", "VERSION_TAG", "EXECUTABLE_TAG", "\
LOCATION_TAG")>\n\
<!ELEMENT "NAME_TAG" (#PCDATA)>\n\
<!ELEMENT "VERSION_TAG" (#PCDATA)>\n\
<!ELEMENT "EXECUTABLE_TAG" (#PCDATA)>\n\
<!ELEMENT "LOCATION_TAG" (#PCDATA)>\n\
<!ATTLIST "PRODUCT_TAG" "TYPE_ATTR" CDATA #IMPLIED>\n\
]>\n\
\n\
";

/* This template is used when we are creating a new file */
static const Str newTemplate = "%s\
<"REGISTRY_TAG">\n\
    <"PRODUCT_TAG" "TYPE_ATTR"=\"%s\">\n\
        <"NAME_TAG">\n\
            %s\n\
        </"NAME_TAG">\n\
        <"VERSION_TAG">\n\
            %s\n\
        </"VERSION_TAG">\n\
        <"EXECUTABLE_TAG">\n\
            %s\n\
        </"EXECUTABLE_TAG">\n\
        <"LOCATION_TAG">\n\
            %s\n\
        </"LOCATION_TAG">\n\
    </"PRODUCT_TAG">\n\
</"REGISTRY_TAG">\n\
";

/* This template is used when we are updating the existing entry */
static const Str updateTemplate = "\
<"PRODUCT_TAG" "TYPE_ATTR"=\"%s\">\n\
        <"NAME_TAG">\n\
            %s\n\
        </"NAME_TAG">\n\
        <"VERSION_TAG">\n\
            %s\n\
        </"VERSION_TAG">\n\
        <"EXECUTABLE_TAG">\n\
            %s\n\
        </"EXECUTABLE_TAG">\n\
        <"LOCATION_TAG">\n\
            %s\n\
        </"LOCATION_TAG">\n\
    </"PRODUCT_TAG">\
";

/* Template for appending a new entry to the existing file */
static const Str appendTemplate = "\
    <"PRODUCT_TAG" "TYPE_ATTR"=\"%s\">\n\
        <"NAME_TAG">\n\
            %s\n\
        </"NAME_TAG">\n\
        <"VERSION_TAG">\n\
            %s\n\
        </"VERSION_TAG">\n\
        <"EXECUTABLE_TAG">\n\
            %s\n\
        </"EXECUTABLE_TAG">\n\
        <"LOCATION_TAG">\n\
            %s\n\
        </"LOCATION_TAG">\n\
    </"PRODUCT_TAG">\n\
";

/**
 * Memory hooks. If memory allocation fails, we exit the program with
 * EXITCODE_NOMEM status. Therefore, we don't need for check for memory
 * allocation failures in the code.
 */
static Bool APP_MemInit(MemContext* ctx, const MemHook* next)
{
    ctx->next = next;
    return True;
}

static void* APP_MemAlloc(MemContext* ctx, size_t size)
{
    void* ptr = MEM_Alloc2(ctx->next, size);
    if (!ptr) {
        TRACE_Error("Out of memory!\n");
        exit(EXITCODE_NOMEM);
    }
    return ptr;
}

static Bool APP_PrintTag(File* out, Str tag, const XMLAttr* a, StrBuf* sb)
{
    if (FILE_Puts(out, TEXT("<")) && FILE_Puts(out, tag)) {
        int i;
        int n = XML_AttrCount(a);
        for (i=0; i<n; i++) {
            Str name = XML_AttrNameAt(a, i);
            Str value = XML_AttrValueAt(a, i);
            Str esc = XML_Escape(NULL, value);
            if (!esc) esc = XML_Escape(sb, value);
            if (FILE_Printf(out, TEXT(" %s=\"%s\""), name, esc) < 0) {
                return False;
            }
        }
        return (FILE_Puts(out, TEXT(">")) > 0);
    }
    return False;
}

/**
 * Prints the DOM node to the specified stream.
 */
static Bool APP_PrintDOM(File* out, DOMNode* node, StrBuf* sb)
{
    if (APP_PrintTag(out, DOM_TagName(node),DOM_Attr(node), sb)) {
        if (DOM_ChunkCount(node) == 0) {
            Str charData = DOM_CharData(node);
            if (charData && FILE_Printf(out, TEXT("%s"), charData) < 0) {
                return False;
            }
        } else {
            DOMChunk* chunk = DOM_FirstChunk(node);
            while (chunk) {
                DOMNode* child = DOM_ChunkNode(chunk);
                Str text = DOM_ChunkText(chunk);
                ASSERT(text || child);
                if ((text && !FILE_Puts(out, text)) ||
                    (child && !APP_PrintDOM(out, child, sb))) {
                    return False;
                }
                chunk=DOM_NextChunk(chunk);
            }
        }
        return (FILE_Printf(out, TEXT("</%s>"),DOM_TagName(node)) > 0);
    }
    return False;
}

/**
 * Matches the child tag of the product node against the expected value
 */
static Bool APP_MatchInfo(DOMNode* p, Str tag, Str expect, StrBuf* sb)
{
    if (expect) {
        DOMNode* node = DOM_FindFirst(p, tag);
        if (node) {
            Str data = DOM_CharData(node);
            if (data && StrStr(data, expect)) {
                STRBUF_Copy(sb, data);
                STRBUF_Trim(sb);
                return STRBUF_EqualsTo(sb, expect);
            }
        }
        /* no match */
        return False;
    } else {
        /* nothing is expected, anything will do */
        return True;
    }
}

/**
 * Checks whether the node matches the product criteria specified on
 * the command line. This serves dual purpose. When registering the
 * product, this is how we find the existing entry to update. The
 * first entry that matches the criteria will be updated, all others
 * ignored. When unregistering the product, all matching entries are
 * removed.
 */
static Bool APP_MatchProduct(DOMNode* product, AppContext* app)
{
    Bool match = False;
    if (product && StrCmp(DOM_TagName(product),PRODUCT_TAG) == 0) {
        Str type = XML_AttrValue(DOM_Attr(product),TYPE_ATTR);
        if (!app->type || (type && StrCmp(type,app->type) == 0)) {
            /* compare product information */
            if (APP_MatchInfo(product, NAME_TAG,  app->name, &app->sb) &&
                APP_MatchInfo(product, VERSION_TAG,  app->ver, &app->sb) &&
                APP_MatchInfo(product, LOCATION_TAG,  app->loc, &app->sb) &&
                APP_MatchInfo(product, EXECUTABLE_TAG,  app->exe, &app->sb)) {
                match = True;
            }
        }
    }
    return match;
}

/**
 * FILE_Save callback. If invoked from FILE_Save, this callback is actually
 * writing to a temporary file. The temporary file then gets renamed into the
 * actual output file. This guarantees that we either successfully update the
 * file, or don't change it at all. Note that we also invoke this callback 
 * directly, to write to stdout.
 */
static Bool APP_FileSaveCB(File* out, Str fname, void * ctx)
{
    AppContext* app = ctx;
    Str tag = DOM_TagName(app->root);
    if (FILE_Puts(out, commonHeader) &&
        APP_PrintTag(out, tag, DOM_Attr(app->root), &app->sb)) {
        Bool updated = False;
        DOMChunk* chunk = DOM_FirstChunk(app->root);
        while (chunk) {
            DOMNode* child = DOM_ChunkNode(chunk);
            Str text = DOM_ChunkText(chunk);
            if (child) {
                if (APP_MatchProduct(child, app)) {

                    /* 
                     * If unregistering, skip this entry. Also, when 
                     * we find more that one matching entry when we are
                     * registering the product, we update the first one
                     * and remove the others.
                     */
                    if (!app->unregister && !updated) {
                        /* update the existing entry */
                        if (!FILE_Puts(out, text) ||
                             FILE_Printf(out, updateTemplate, 
                                        app->typeEsc, app->nameEsc, 
                                        app->verEsc, app->exeEsc, 
                                        app->locEsc) > 0) {
                            updated = True;
                        } else {
                            return False;
                        }
                    }
                } else if (!FILE_Puts(out, text) ||
                           !APP_PrintDOM(out, child, &app->sb)) {
                    return False;
                }
            } else if (text) {
                if (!FILE_Puts(out, text)) {
                    return False;
                }
            }
            chunk = DOM_NextChunk(chunk);
        }
        if (!updated) {
            if (DOM_ChunkCount(app->root) == 0) {
                Str charData = DOM_CharData(app->root);
                if (charData && FILE_Printf(out, TEXT("%s"), charData) < 0) {
                    return False;
                }
            }
            if (!app->unregister) {
                /* append a new entry */
                if (FILE_Printf(out,appendTemplate, app->typeEsc, 
                                app->nameEsc, app->verEsc, app->exeEsc,
                                app->locEsc) < 0) {
                    return False;
                }
            }
        }
        return (FILE_Printf(out, TEXT("</%s>\n"),tag) > 0);
    }

    /* some kind of I/O error */
    return False;
}

/**
 * The program entry point.
 */
int main(int argc, char* argv[]) 
{
    AppContext app;
    Bool toStdout = False;
    Bool help = False;
    Vector params;
    CmdLine* c;
    Str prog;
    int status;
    Char sysDirBuf[MAX_PATH +1];

    MemProc mp;
    MemContext mc;

    /* install memory hook so that we don't have to check for NULL */
    MEM_InitModule();    
    memset(&mc, 0, sizeof(mc));
    memset(&mp, 0, sizeof(mp));
    mp.memInit = APP_MemInit;
    mp.memAlloc = APP_MemAlloc;
    MEM_Hook(&mp, &mc);
    RANDOM_InitModule();    

    /* figure out the program name */
    prog = FILE_FilePart(argv[0]);
#ifdef _WIN32
    if (STRING_EndsWith(prog, ".exe")) {
        int n = StrLen(prog)-4;
        pname = MEM_NewArray(Char, n+1);
        StrnCpy(pname, prog, n);
        pname[n] = 0;
    } else 
#endif /* _WIN32 */
    pname = STRING_Dup(prog);

    /* 
     * determine the drive where Windows is installed. That's the drive
     * where installationRegistry.xml file is located.
     */
#ifdef _WIN32
    if (GetSystemDirectory(sysDirBuf, COUNT(sysDirBuf))) {
        TRACE1("system drive is %c:\n",sysDirBuf[0]);
        regfile[0] = sysDirBuf[0];
    }
#endif /* _WIN32 */

    memset(&app, 0, sizeof(app));
    c = CMDLINE_Create(pname);

#ifdef _CONSOLE
    CMDLINE_AddTrueOpt(c,'h',"help","print this help and exit",&help);
    CMDLINE_AddTrueOpt(c,'c',"stdout","write updated installation registry "
        "to stdout",&toStdout);
#endif /* _CONSOLE */

    CMDLINE_SetParamName(CMDLINE_AddStrOpt(c,'e',"executable",
        "path to the executable (required to register)",&app.exe), "FILE");
    CMDLINE_SetParamName(CMDLINE_AddStrOpt(c,'v',"version",
        "product version (default is "DEFAULT_VERSION")",&app.ver),"VERSION");
    CMDLINE_SetParamName(CMDLINE_AddStrOpt(c,'l',"location",
        "product location (default is the executable folder)",&app.loc),"DIR");
    CMDLINE_SetParamName(CMDLINE_AddStrOpt(c,'t',"type",
        "product type (default is "DEFAULT_TYPE")",&app.type),"TYPE");
    CMDLINE_AddTrueOpt(c,'u',"unregister","unregister the product "
        "(default is to register)",&app.unregister);

    VECTOR_Init(&params, 0, vectorEqualsString, NULL);
    if (!CMDLINE_Parse(c,argv+1,argc-1,PARSE_NO_DUP,&params) || 
        VECTOR_IsEmpty(&params) || help) {
        TRACE_Output(PRODUCT_NAME " Version %d.%d.%d\nCopyright (C) "
            PRODUCT_COPYRIGHT ". All rights reserved.\n\n", 
            PRODUCT_VERSION_MAJOR, PRODUCT_VERSION_MINOR,
            PRODUCT_VERSION_MICRO);
        CMDLINE_Usage(c, CMDLINE_PARAM, 0);
        status = (help ? EXITCODE_HELP : EXITCODE_CMDLINE);
    } else if (VECTOR_Size(&params) > 1) {
        TRACE_Error("%s: unexpected command line parameters\n", pname);
        CMDLINE_Usage(c, CMDLINE_PARAM, 0);
        status = EXITCODE_CMDLINE;
    } else if (!app.exe && !app.unregister) {
        TRACE_Error("%s: product executable is required\n", pname);
        CMDLINE_Usage(c, CMDLINE_PARAM, 0);
        status = EXITCODE_CMDLINE;
    } else {

        StrBuf typeBuf;
        StrBuf nameBuf;
        StrBuf verBuf;
        StrBuf exeBuf;
        StrBuf locBuf;
        Char* dir = NULL;

        if (app.exe && !app.loc) {
            /* default location */
            if (FILE_FilePart(app.exe) == app.exe) {
                int n = GetCurrentDirectory(0,NULL)+1;
                dir = MEM_NewArray(Char,n);
                dir[GetCurrentDirectory(n,dir)] = 0;
            } else {
                dir = FILE_DirName(app.exe, 0);
            }
            if (STRING_EndsWith(dir, FILE_SEPARATOR)) {
                dir[StrLen(dir)-1] = 0;
            }
            app.loc = dir;
        }

        status = EXITCODE_SUCCESS;

        /* XML escape the strings */
        STRBUF_Init(&typeBuf);
        STRBUF_Init(&nameBuf);
        STRBUF_Init(&verBuf);
        STRBUF_Init(&exeBuf);
        STRBUF_Init(&locBuf);
        STRBUF_Init(&app.sb);
        app.name = VECTOR_Get(&params, 0);
        app.nameEsc = XML_Escape(&nameBuf, app.name);
        if (app.exe) app.exeEsc = XML_Escape(&exeBuf, app.exe);
        if (app.loc) app.locEsc = XML_Escape(&locBuf, app.loc);
        
        if (app.type) {
            app.typeEsc = XML_Escape(&typeBuf, app.type);
        } else {
            /* we know there's nothing to escape here */
            app.typeEsc = DEFAULT_TYPE;
        }

        if (app.ver) {
            app.verEsc = XML_Escape(&verBuf, app.ver);
        } else {
            /* we know there's nothing to escape here */
            app.verEsc = DEFAULT_VERSION;
        }

        app.root = DOM_Load(regfile);
        if (app.root && StrCmp(DOM_TagName(app.root),ROOT_TAG) == 0) {
            if (toStdout) {
                File* out = FILE_AttachToFile(stdout,"stdout");
                APP_FileSaveCB(out, FILE_Name(out), &app);
                FILE_Close(out);
            } else {
                if (!FILE_Save(regfile, APP_FileSaveCB, &app, NULL)) {
                    status = EXITCODE_IOERR;
                }
            }
        } else if (!app.unregister) {

            /*
             * either the file does not exist, or else it exists but 
             * contains some garbage (root tag does not match the 
             * expectation). Create a new file, unless the user has
             * given us -u (unregister) option in which case we have
             * nothing to do.
             */
            if (toStdout) {
                printf(newTemplate, commonHeader, app.typeEsc, app.nameEsc,
                    app.verEsc, app.exeEsc, app.locEsc);
            } else {

                /* create a new file */
                File* f = FILE_Open(regfile, WRITE_TEXT_MODE, NULL);
                if (!f) {
                    /* perhaps, the directory does not exist? */
                    Char* regdir = FILE_DirName(regfile, 0);
                    FILE_MkDir(regdir);
                    MEM_Free(regdir);
                }

                /* try again */
                f = FILE_Open(regfile, WRITE_TEXT_MODE, NULL);
                if (f) {
                    if (FILE_Printf(f, newTemplate, commonHeader, 
                                    app.typeEsc, app.nameEsc, 
                                    app.verEsc, app.exeEsc, 
                                    app.locEsc) < 0) {
                        status = EXITCODE_IOERR;
                    }
                    FILE_Close(f);
                } else {
                    status = EXITCODE_IOERR;
                }
            }
        }

        STRBUF_Destroy(&app.sb);
        DOM_Delete(app.root);
        STRBUF_Destroy(&typeBuf);
        STRBUF_Destroy(&nameBuf);
        STRBUF_Destroy(&verBuf);
        STRBUF_Destroy(&exeBuf);
        STRBUF_Destroy(&locBuf);
        MEM_Free(dir);
    }

    /* Cleanup */
    MEM_Free(pname);
    VECTOR_Destroy(&params);
    CMDLINE_Delete(c);
    RANDOM_Shutdown();
    MEM_Shutdown();
    return status;
}
