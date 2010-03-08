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


#include "s_lib.h"

#define DECLSPEC
#define _PRIMITIVE_TYPES_H

#ifndef _WIN32
typedef I32u UINT32;
typedef I16u UINT16;
typedef I8u  BYTE;
typedef int  BOOL;
#endif /* _WIN32 */

#include "wmlapi.h"

/* DTDs and token sets for supported content types */
#include "wml10dtd.h"
#include "wml10tok.h"
#include "wml11dtd.h"
#include "wml11tok.h"
#include "wml12dtd.h"
#include "wml12tok.h"
#include "wml13dtd.h"
#include "wml13tok.h"
#include "si10dtd.h"
#include "si10tok.h"
#include "sl10dtd.h"
#include "sl10tok.h"
#include "co10dtd.h"
#include "co10tok.h"

#define DEF_IN_ENC  "ISO-8859-1"
#define DEF_OUT_ENC "UTF-8"

static WML_DTD_DESCRIPTOR WML10 = {
    (char*)"-//WAPFORUM//DTD WML 1.0//EN",
    COUNT(WML10DTD) - 1,
    (BYTE*)WML10DTD
};

static WML_DTD_DESCRIPTOR WML11 = {
    (char*)"-//WAPFORUM//DTD WML 1.1//EN",
    COUNT(WML11DTD) - 1,
    (BYTE*)WML11DTD
};

static WML_DTD_DESCRIPTOR WML12 = {
    (char*)"-//WAPFORUM//DTD WML 1.2//EN",
    COUNT(WML12DTD) - 1,
    (BYTE*)WML12DTD
};

static WML_DTD_DESCRIPTOR WML13 = {
    (char*)"-//WAPFORUM//DTD WML 1.3//EN",
    COUNT(WML13DTD) - 1,
    (BYTE*)WML13DTD
};

static WML_DTD_DESCRIPTOR SI10 = {
    (char*)"-//WAPFORUM//DTD SI 1.0//EN",
    COUNT(SI10DTD) - 1,
    (BYTE*)SI10DTD
};

static WML_DTD_DESCRIPTOR SL10 = {
    (char*)"-//WAPFORUM//DTD SL 1.0//EN",
    COUNT(SL10DTD) - 1,
    (BYTE*)SL10DTD
};

STATIC WML_DTD_DESCRIPTOR CO10 = {
    (char*)"-//WAPFORUM//DTD CO 1.0//EN",
    COUNT(CO10DTD) - 1,
    (BYTE*)CO10DTD
};

static WML_DTD_DESCRIPTOR * knownDTDs[]={
    &WML10,
    &WML11,
    &WML12,
    &WML13,
    &SI10,
    &SL10,
    &CO10,
    0
};

const Str pname = "wmlenc";

static void init_encoder()
{
    WML_STATUS status = WML_StartUp(NULL,knownDTDs);
    ASSERT(status == WML_OK);
    status = WML_AddCodepage(WML10.m_publicId,COUNT(WML10TOK)-1,WML10TOK);
    ASSERT(status == WML_OK);
    status = WML_AddCodepage(WML11.m_publicId,COUNT(WML11TOK)-1,WML11TOK);
    ASSERT(status == WML_OK);
    status = WML_AddCodepage(WML12.m_publicId,COUNT(WML12TOK)-1,WML12TOK);
    ASSERT(status == WML_OK);
    status = WML_AddCodepage(WML13.m_publicId,COUNT(WML13TOK)-1,WML13TOK);
    ASSERT(status == WML_OK);
    status = WML_AddCodepage(SI10.m_publicId,COUNT(SI10TOK)-1,SI10TOK);
    ASSERT(status == WML_OK);
    status = WML_AddCodepage(SL10.m_publicId,COUNT(SL10TOK)-1,SL10TOK);
    ASSERT(status == WML_OK);
    status = WML_AddCodepage(CO10.m_publicId,COUNT(CO10TOK)-1,CO10TOK);
    ASSERT(status == WML_OK);
}

static void report_error(WML_STATUS status)
{
    switch (status) {
    case WML_ERROR_INVALID_URL: 
        Error("Invalid URL\n");
        break;
    case WML_ERROR_NO_SUCH_HOST:
        Error("Specified host does not exist\n");
        break;
    case WML_ERROR_NETWORK_ERROR:
        Error("Network error\n");
        break;
    case WML_ERROR_UNSUPPORTED_PROTOCOL:
        Error("Unsupported protocol\n");
        break;
    case WML_ERROR_DOCUMENT_NOT_FOUND:
        Error("Document not found\n");
        break;
    case WML_ERROR_CONVERSION_FAILED:
        Error("Character encoding conversion failed\n");
        break;
    case WML_ERROR_XMLPARSER_FAILED:
        Error("Unknown XML error\n");
        break;
    case XML_ERROR_GENERIC:
        Error("WFC: Unknown well-formedness error\n");
        break;
    case XML_ERROR_ONE_ROOT:
        Error("WFC: Exactly one root element allowed\n");
        break;
    case XML_ERROR_COMMENT:
        Error("WFC: Double-hyphen must not occur within comments\n");
        break;
    case XML_ERROR_VERSION:
        Error("WFC: Version number does not conform to \"1.0\n");
        break;
    case XML_ERROR_NOT_DECLARED_ROOT:
        Error("WFC: Root element not declared (download/alternative DTD failed)\n");
        break;
    case XML_ERROR_ROOT_ELEMENT_TYPE:
        Error("VC: Type of root element does not match DTD\n");
        break;
    case XML_ERROR_PE_IN_INTERNAL_SUBSET:
        Error("WFC: PE in internal subset, in markup declaration\n");
        break;
    case XML_ERROR_SA_YES_OR_NO:
        Error("VC: standalone must be 'yes' or 'no\n");
        break;
    case XML_ERROR_NOT_DECL_ELEMENT:
        Error("VC: No declaration for element\n");
        break;
    case XML_ERROR_START_END:
        Error("WFC: Name of end-tag must match start-tag\n");
        break;
    case XML_ERROR_NOT_CHILD:
        Error("VC: Not child element\n");
        break;
    case XML_ERROR_NO_CHILD:
        Error("VC: There is no child\n");
        break;
    case XML_ERROR_MUST_EMPTY:
        Error("VC: Element must be empty\n");
        break;
    case XML_ERROR_NO_CHARDATA:
        Error("VC: CharData/Space not allowed in content\n");
        break;
    case XML_ERROR_UNIQUE_ATT_SPEC:
        Error("VC: No attribute name may appear more than once\n");
        break;
    case XML_ERROR_ATTR_VALUE_TYPE:
        Error("VC: Attribute is not declared\n");
        break;
    case XML_ERROR_NO_LT_IN_ATTVAL:
        Error("WFC: No < in Attribute Values\n");
        break;
    case XML_ERROR_UNIQUE_ELEMENT_TYPE:
        Error("VC: Unique Element Type Declaration\n");
        break;
    case XML_ERROR_PROPER_GROUP:
        Error("VC: Improper Group/PE Nesting\n");
        break;
    case XML_ERROR_ID_NAME:
        Error("VC: Value of attribute type 'ID' must match name production\n");
        break;
    case XML_ERROR_IDREF_NAME:
        Error("VC: Value of attribute type 'IDREF' must match name production\n");
        break;
    case XML_ERROR_ID_EXISTS:
        Error("VC: attribute type 'ID' value must uniquely identify the element\n");
        break;
    case XML_ERROR_ID_ATT_DEFAULT:
        Error("VC: ID attribute must have a declared default of #IMPLIED or #REQUIRED\n");
        break;
    case XML_ERROR_ID_REF:
        Error("VC: 'IDREF' attribute value must match the value of an ID attribute\n");
        break;
    case XML_ERROR_ENTITY_NAME:
        Error("VC: Attribute is not a name of unparsed entity\n");
        break;
    case XML_ERROR_NMTOKEN_NAME:
        Error("VC: Attribute has no valid NMTOKEN name\n");
        break;
    case XML_ERROR_ENUMERATION:
        Error("VC: Enumeration does not match any NMTOKEN in the declaration\n");
        break;
    case XML_ERROR_REQUIRED_ATT:
        Error("VC: Required attribute missing\n");
        break;
    case XML_ERROR_FIXED_ATT_DEF:
        Error("VC: #FIXED attribute does not match default value\n");
        break;
    case XML_ERROR_LEGAL_CHAR:
        Error("WFC: Illegal character in character reference\n");
        break;
    case XML_ERROR_GE_NOT_DECLARED:
        Error("VC: General entity is not declared\n");
        break;
    case XML_ERROR_NO_RECURSION:
        Error("VC: No recursion allowed (too many nested entites)\n");
        break;
    case XML_ERROR_RECURSION_IN_PE:
        Error("VC: Recursion encountered in PEReference\n");
        break;
    case XML_ERROR_PE_NOT_DECLARED:
        Error("VC: Parameter entity is not declared\n");
        break;
    case XML_ERROR_NOTATION_DECALRED:
        Error("VC: There is no notation for external unparsed entity\n");
        break;
    case XML_ERROR_NESTING:
        Error("WFC: Entities are not properly nested\n");
        break;
    case XML_ERROR_ENCODING:
        Error("VC: Unable to process encoding\n");
        break;
    case XML_ERROR_CHAR_REF:
        Error("VC: Character reference forbidden\n");
        break;
    case XML_ERROR_NOT_REFERENCE:
        Error("VC: Found '&', but not reference\n");
        break;
    case XML_ERROR_UNPARSED_IN_CONTENT:
        Error("VC: Reference to unparsed entity in content\n");
        break;
    case XML_ERROR_EXT_IN_ATTVAL:
        Error("VC: Reference to external entity in attribute value\n");
        break;
    case XML_ERROR_UNPARSED_IN_EV:
        Error("VC: Reference to unparsed entity in entity value\n");
        break;
    case XML_ERROR_TEXT_AFTER_ROOT:
        Error("WFC: Text after the root element\n");
        break;
    case XML_WARNING_ATT_LIST_BUT_ELEMENT:
        Error("W: Attributes declared for an element type not itself declared\n");
        break;
    case XML_WARNING_MORE_ATTLIST:
        Error("W: More than one Attlist for element\n");
        break;
    case XML_WARNING_ATTDEF_IGNORED:
        Error("W: More than one definition for same attribute\n");
        break;
    case XML_WARNING_MORE_ENTITY_DECL:
        Error("W: Entity is declared more than once\n");
        break;
    case XML_WARNING_MORE_NOTA_DECL:
        Error("W: Notation is declared more than once\n");
        break;
    case XML_ERROR_STR_TOO_LONG:
        Error("String is longer than 50000 characters\n");
        break;
    case WML_ERROR_NO_TOKENS:
        Error("WML: No token set was attached to PUBID\n");
        break;
    case WML_ERROR_NUMBER:
        Error("WML: Invalid number as attribute value\n");
        break;
    case WML_ERROR_LENGTH:
        Error("WML: Invalid length as attribute value\n");
        break;
    case WML_ERROR_ONCE:
        Error("WML: Element may occur only once per document (Eg.: access)\n");
        break;
    case WML_ERROR_ZERO:
        Error("WML: Attribute value must not be '0'. (Eg.: columns of table)\n");
        break;
    case WML_ERROR_VAR:
        Error("WML: Syntax error at variable reference\n");
        break;
    case WML_ERROR_VAR_CHAR:
        Error("WML: Bad name as variable reference\n");
        break;
    case WML_ERROR_VARNAME:
        Error("WML: Illegal variable name\n");
        break;
    case WML_ERROR_VAR_ESC:
        Error("WML: Variable escape error\n");
        break;
    case WML_ERROR_VAR_KET:
        Error("WML: Missing ')' for variable\n");
        break;
    case WML_ERROR_CHAR_CONV:
        Error("WML: Character conversion failure\n");
        break;
    case WML_ERROR_ONE_TASK:
        Error("WML: Only one child task is allowed in specific element ('anchor')\n");
        break;
    case WML_ERROR_EVENT_BINDING:
        Error("WML: Conflicting event bindings\n");
        break;
    case WML_ERROR_DUP_DO:
        Error("WML: Two or more 'do' elements with the same name\n");
        break;
    case WML_ERROR_MISSING_DO_NAME:
        Error("WML: Missing name for 'do' element\n");
        break;
    case SI_ERROR_INVALID_DATETIME:
        Error("SI: Invalid datetime\n");
        break;
    default:
        break;
    }
}

static Bool encode(File * in, Buffer * out, Str inEnc, Str outEnc)
{
    Bool ok = False;
    Buffer inData;
    BUFFER_Init(&inData);
    BUFFER_EnsureCapacity(out, 1024, False);
    if (FILE_ReadData(in, &inData, -1) > 0) {
        UINT32 wmlclen;
        BYTE * wmlc;
        WML_STATUS status;
        StrBuf64 urlBuf;
        STRBUF_InitBufXXX(&urlBuf);
        STRBUF_Copy(&urlBuf.sb, "file://");
        STRBUF_Append(&urlBuf.sb, FILE_Name(in));
        wmlc = BUFFER_Access(out);
        wmlclen = out->alloc;
        while ((status = WML_Encode(
                BUFFER_Size(&inData), BUFFER_Access(&inData), inEnc,
                &wmlclen, wmlc, outEnc,
                NULL, NULL, NULL, NULL, WML11.m_publicId,
                STRBUF_Text(&urlBuf.sb))) ==
                WML_ERROR_OUTPUT_BUFFER_SMALL ||
                
                /* WML_OK and output length zero apparently means that
                 * output buffer is too small */
                (status == WML_OK && wmlclen == 0)) {

            int newsize = wmlclen;
            if (newsize <= BUFFER_Size(out)) newsize = out->alloc*2;
            if (!BUFFER_EnsureCapacity(out,newsize,False)) break;
            wmlc = BUFFER_Access(out);
            wmlclen = out->alloc;
        }
        STRBUF_Destroy(&urlBuf.sb);
        if (status == WML_OK && wmlclen) {
            ASSERT(out->start == 0);
            out->end = wmlclen;
            ok = True;
        } else {
            report_error(status);
        }
    }
    BUFFER_Destroy(&inData);
    return ok;
}

int main(int argc, char * argv[])
{
    int retval = 1;
    CmdLine * cmdline;
    cmdline = CMDLINE_Create(pname);
    if (cmdline) {
        Bool help = False;
        Bool quiet = False;
        Bool overwrite = False;
        Vector args;
        CmdOpt * helpOpt;
        Str inEnc = DEF_IN_ENC;
        Str outEnc = DEF_OUT_ENC;
        VECTOR_Init(&args,0,NULL,NULL);
        CMDLINE_SetMinArgs(cmdline,1);
        CMDLINE_SetMaxArgs(cmdline,2);
        helpOpt = CMDLINE_AddTrueOpt(cmdline,'h',"help",
            "print this help",&help);
        CMDLINE_Exclude(CMDLINE_AddTrueOpt(cmdline,'q',"quiet",
            "don't print anything to standard output",&quiet),helpOpt);
        CMDLINE_AddTrueOpt(cmdline,'f',"force",
            "overwrite output file",&overwrite);
        CMDLINE_SetParamName(CMDLINE_AddStrOpt(cmdline,'i',"input",
            "input encoding (default is "DEF_IN_ENC")",&inEnc),
            "ENC");
        CMDLINE_SetParamName(CMDLINE_AddStrOpt(cmdline,'o',"output",
            "output encoding (default is "DEF_OUT_ENC")",&outEnc),
            "ENC");
        
        if (argc > 1 &&
           CMDLINE_Parse(cmdline,argv+1,argc-1,PARSE_NO_DUP,&args) && 
            !help) {

            Str fname = (Str)VECTOR_Get(&args,0);
            File * in = FILE_Open(fname, READ_TEXT_MODE, NULL);
            if (quiet) TRACE_SetMask(0);
            if (in) {
                File * out;
                Str outName;
                StrBuf64 outNameBuf;
                STRBUF_InitBufXXX(&outNameBuf);
                if (VECTOR_Size(&args) < 2) {
                    /* automatically generate output file name */
                    STRBUF_Copy(&outNameBuf.sb, fname);
                    STRBUF_Append(&outNameBuf.sb, "c");
                    outName = STRBUF_Text(&outNameBuf.sb);
                    if (FILE_Exist(outName) && !overwrite) {
                        int i=1, len = STRBUF_Length(&outNameBuf.sb);
                        do {
                            STRBUF_SetLength(&outNameBuf.sb, len);
                            STRBUF_AppendFormat(&outNameBuf.sb,".%d",i++);
                            outName = STRBUF_Text(&outNameBuf.sb);
                        } while (FILE_Exist(outName));
                    }
                } else {
                    outName = (Str)VECTOR_Get(&args,1);
                }
                out = FILE_Open(outName, WRITE_BINARY_MODE, NULL);
                if (out) {
                    Buffer buf;
                    BUFFER_Init(&buf);
                    init_encoder();
                    if (encode(in,&buf,inEnc,outEnc)) {
                        void * data = BUFFER_Access(&buf);
                        int n = BUFFER_Size(&buf);
                        if (FILE_Write(out,data,n) == n) {
                            Output("written %d bytes to %s\n",n,outName); 
                            retval = 0;
                        } else {
                            Error("ERROR: cannot write %s\n",outName);
                        }
                    } else {
                        Error("ERROR: encoding failed\n");
                    }
                    WML_ShutDown();
                    FILE_Close(out);
                    BUFFER_Destroy(&buf);
                    if (retval) remove(outName);
                } else {
                    Error("ERROR: cannot write %s\n",outName);
                }
                STRBUF_Destroy(&outNameBuf.sb);
                FILE_Close(in);
            } else {
                Error("ERROR: file %s not found\n",fname);
            }
        } else {
            if (argc > 1 && !help) {
                Output("try '%s --help'\n",pname);
            } else {
                Output("Command Line WML Encoder (%s)\n",__DATE__);
                CMDLINE_Usage(cmdline,"INPUT [OUTPUT]",0);
            }
        }
        VECTOR_Destroy(&args);
        CMDLINE_Delete(cmdline);
    }
    MEM_Shutdown();
    return retval;
}

/* dummy entry points for linking */
void WSL_LOG_Init() {}
void WSL_LOG_Stop() {}
void WSL_STORE_Init() {}
void WSL_STORE_Stop() {}
void WSL_IMPL_Log() {}

/*
 * Local Variables:
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * compile-command: "cl /MD /I${NMIT_SHARED_ROOT}/src/native/slib/include /I${NMIT_SHARED_ROOT}/src/native/wmlenc/include /I${RAINBOW_HOME}/rb_wps/server/include wmlenc.c ${NMIT_SHARED_ROOT}/src/native/slib/slib.lib ${NMIT_SHARED_ROOT}/src/native/wmlenc/build/release/wmlenc.lib ${RAINBOW_HOME}/rb_wps/bld/vc6/nt/wsl/Release/servlib_static.lib"
 * End:
 */
