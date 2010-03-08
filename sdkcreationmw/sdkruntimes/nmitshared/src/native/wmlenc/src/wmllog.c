/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
/* File    : U:\wpsvob\wmlenc\src\common\wmllog.c
   Version : \main\8
   Date    : 17-Oct-00.12:47:29
*/

/*
 * ========================================================================
 *  Name        : wmllog.c
 *  Part of     : 
 *  Description : 
 *  Version     : 
 *
 *  Copyright (c) 2000 - 2005 Nokia Corporation.
 *  This material, including documentation and any related
 *  computer programs, is protected by copyright controlled by
 *  Nokia Corporation. All rights are reserved. Copying,
 *  including reproducing, storing, adapting or translating, any
 *  or all of this material requires the prior written consent of
 *  Nokia Corporation. This material also contains confidential
 *  information which may not be disclosed to others without the
 *  prior written consent of Nokia Corporation.
 * ========================================================================
 */



/*

    1  ABSTRACT

    1.1 Module Type

    type  : subroutines

    1.2 Functional Description

        Logging.


    1.3 Specification/Design Reference

        doc/wmlarch.doc

    1.4 Module Test Specification Reference

        none

    1.5 Compilation Information

        See: building.txt

    1.6 Notes

        None

*/



/*  2  CONTENTS

        1  ABSTRACT

        2  CONTENTS

        3  GLOSSARY

        4  { NAL RESOURCES
        4.1  Include Files
        4.2  External Data Structures
        4.3  External Function Prototypes

        5  LOCAL CONSTANTS AND MACROS

        6  MODULE DATA STRUCTURES
        6.1  Local Data Structures
        6.2  Local Function Prototypes

        7  MODULE CODE

        7.1   wmlLOGs
        7.2   wmlLOGss
        7.3   wmlLOGsss
        7.4   wmlLOGssss
        7.5   error2str

*/



/*  3  GLOSSARY

        None

*/


/*  4  { NAL RESOURCES  */

    /* None */

/*  4.1  Include Files  */

#include "xmlstr.h"
#include "wmlapi.h"
#include "wps_service.h"

/*  4.2 External Data Structures  */

    /* None */


/*  4.3 External Function Prototypes  */

    /* None */


/*  5  LOCAL CONSTANTS AND MACROS  */

    /* None */



/*  6  MODULE DATA STRUCTURES  */


/*  6.1 Local Data Structures  */


typedef struct SStatus
{
  WML_STATUS    m_status;
  const char *  m_ascii;
}SStatus;

static SStatus s_status[]={

{ WML_OK,
  "No error" },
{ WML_ERROR_CACHE_FULL,
  "No more free buffers in cache" },
{ WML_ERROR_LARGE_DOC,
  "Document size is too large" },
{ WML_ERROR_NOT_INITIALIZED,
  "Startup has not yet been called" },
{ WML_ERROR_EMPTY_ARG,
  "Expected real data" },
{ WML_ERROR_INVALID_BUFFER_SIZE,
  "==0  or >XML_BUFFER_SIZE" },
{ WML_ERROR_IS_STARTED,
  "Don't call startup twice" },
{ WML_ERROR_INVALID_MAP,
  "character code < 128" },
{ WML_ERROR_ENCODING_EXISTS,
  "at WML_AddCharEnc" },
{ WML_ERROR_NO_MIBENUM,
  "at WML_AddCharEnc" },
{ WML_ERROR_INVALID_ENCODING,
  "encoding does not exists" },
{ WML_ERROR_INVALID_CONTENT,
  "error of WML_CodeLoad" },
{ WML_ERROR_OUTPUT_BUFFER_SMALL,
  "binary output too long" },
{ WML_ERROR_MANY_PROPERTY,
  "Only one property (http-equiv | name) allowed for meta element" },
{ WML_ERROR_NO_PROPERTY,
  "meta element needs exactly one property attribute (http-equiv | name)" },
{ WML_ERROR_MANY_META,
  "Insufficient number of buffers for meta-s /WML_Encode(...arraySize...)/" },
{ WML_ERROR_LONG_META,
  "Meta string is too long /WML_Encode(...stringLength...)/" },
{ WML_ERROR_SPEC_FAILURE,
  "WML_AddCodepage(): token-set specification failure" },
{ WML_ERROR_INVALID_URL,
  "Invalid URL" },
{ WML_ERROR_NO_SUCH_HOST,
  "Specified host does not exist" },
{ WML_ERROR_NETWORK_ERROR,
  "Network error" },
{ WML_ERROR_UNSUPPORTED_PROTOCOL,
  "Unsupported protocol" },
{ WML_ERROR_DOCUMENT_NOT_FOUND,
  "Document not found" },
{ WML_ERROR_CONVERSION_FAILED,
  "Character encoding conversion failed" },
{ WML_ERROR_XMLPARSER_FAILED,
  "Unknown XML error" },
{ XML_ERROR_GENERIC,
  "WFC: Unknown well-formedness error" },
{ XML_ERROR_ONE_ROOT,
  "WFC: Exactly one root element allowed" },
{ XML_ERROR_COMMENT,
  "WFC: Double-hyphen must not occur within comments" },
{ XML_ERROR_VERSION,
  "WFC: Version number does not conform to \"1.0\"" },
{ XML_ERROR_NOT_DECLARED_ROOT,
  "WFC: Root element not declared (download/alternative DTD failed?)" },
{ XML_ERROR_ROOT_ELEMENT_TYPE,
  "VC: Type of root element does not match DTD" },
{ XML_ERROR_PE_IN_INTERNAL_SUBSET,
  "WFC: PE in internal subset, in markup declaration" },
{ XML_ERROR_SA_YES_OR_NO,
  "VC: standalone must be 'yes' or 'no'" },
{ XML_ERROR_NOT_DECL_ELEMENT,
  "VC: No declaration for element" },
{ XML_ERROR_START_END,
  "WFC: Name of end-tag must match start-tag" },
{ XML_ERROR_NOT_CHILD,
  "VC: Not child element" },
{ XML_ERROR_NO_CHILD,
  "VC: There is no child" },
{ XML_ERROR_MUST_EMPTY,
  "VC: Element must be empty" },
{ XML_ERROR_NO_CHARDATA,
  "VC: CharData/Space not allowed in content" },
{ XML_ERROR_UNIQUE_ATT_SPEC,
  "VC: No attribute name may appear more than once" },
{ XML_ERROR_ATTR_VALUE_TYPE,
  "VC: Attribute is not declared" },
{ XML_ERROR_NO_LT_IN_ATTVAL,
  "WFC: No < in Attribute Values" },
{ XML_ERROR_UNIQUE_ELEMENT_TYPE,
  "VC: Unique Element Type Declaration" },
{ XML_ERROR_PROPER_GROUP,
  "VC: Improper Group/PE Nesting" },
{ XML_ERROR_ID_NAME,
  "VC: Value of attribute type 'ID' must match name production" },
{ XML_ERROR_IDREF_NAME,
  "VC: Value of attribute type 'IDREF' must match name production" },
{ XML_ERROR_ID_EXISTS,
  "VC: attribute type 'ID' value must uniquely identify the element" },
{ XML_ERROR_ID_ATT_DEFAULT,
  "VC: ID attribute must have a declared default of #IMPLIED or #REQUIRED" },
{ XML_ERROR_ID_REF,
  "VC: 'IDREF' attribute value must match the value of an ID attribute" },
{ XML_ERROR_ENTITY_NAME,
  "VC: Attribute is not a name of unparsed entity" },
{ XML_ERROR_NMTOKEN_NAME,
  "VC: Attribute has no valid NMTOKEN name" },
{ XML_ERROR_ENUMERATION,
  "VC: Enumeration does not match any NMTOKEN in the declaration" },
{ XML_ERROR_REQUIRED_ATT,
  "VC: Required attribute missing" },
{ XML_ERROR_FIXED_ATT_DEF,
  "VC: #FIXED attribute does not match default value" },
{ XML_ERROR_LEGAL_CHAR,
  "WFC: Illegal character in character reference" },
{ XML_ERROR_GE_NOT_DECLARED,
  "VC: General entity is not declared" },
{ XML_ERROR_NO_RECURSION,
  "VC: No recursion allowed (too many nested entites)" },
{ XML_ERROR_RECURSION_IN_PE,
  "VC: Recursion encountered in PEReference" },
{ XML_ERROR_PE_NOT_DECLARED,
  "VC: Parameter entity is not declared" },
{ XML_ERROR_NOTATION_DECALRED,
  "VC: There is no notation for external unparsed entity" },
{ XML_ERROR_NESTING,
  "WFC: Entities are not properly nested" },
{ XML_ERROR_ENCODING,
  "VC: Unable to process encoding" },
{ XML_ERROR_CHAR_REF,
  "VC: Character reference forbidden" },
{ XML_ERROR_NOT_REFERENCE,
  "VC: Found '&', but not reference" },
{ XML_ERROR_UNPARSED_IN_CONTENT,
  "VC: Reference to unparsed entity in content" },
{ XML_ERROR_EXT_IN_ATTVAL,
  "VC: Reference to external entity in attribute value" },
{ XML_ERROR_UNPARSED_IN_EV,
  "VC: Reference to unparsed entity in entity value" },
{ XML_ERROR_TEXT_AFTER_ROOT,
  "WFC: Text after the root element" },
{ XML_WARNING_ATT_LIST_BUT_ELEMENT,
  "W: Attributes declared for an element type not itself declared" },
{ XML_WARNING_MORE_ATTLIST,
  "W: More than one Attlist for element" },
{ XML_WARNING_ATTDEF_IGNORED,
  "W: More than one definition for same attribute" },
{ XML_WARNING_MORE_ENTITY_DECL,
  "W: Entity is declared more than once" },
{ XML_WARNING_MORE_NOTA_DECL,
  "W: Notation is declared more than once" },
{ XML_ERROR_STR_TOO_LONG,
  "String is longer than 50000 characters" },
{ WML_ERROR_NO_TOKENS,
  "No token set was attached to PUBID" },
{ WML_ERROR_NUMBER,
  "Invalid number as attr. value" },
{ WML_ERROR_LENGTH,
  "Invalid length as attr. value" },
{ WML_ERROR_ONCE,
  "Element may occur only once per document (Eg.: access)" },
{ WML_ERROR_ZERO,
  "Attribute value must not be '0'. (Eg.: columns of table)" },
{ WML_ERROR_VAR,
  "Syntax error at variable reference" },
{ WML_ERROR_VAR_CHAR,
  "Bad name as variable reference." },
{ WML_ERROR_VARNAME,
  "Illegal variable name" },
{ WML_ERROR_VAR_ESC,
  "Variable escape error" },
{ WML_ERROR_VAR_KET,
  "Missing ')' for variable" },
{ WML_ERROR_CHAR_CONV,
  "Character conversion failure" },
{ WML_ERROR_ONE_TASK,
  "Only one child task is allowed in specific element ('anchor')" },
{ WML_ERROR_EVENT_BINDING,
  "Conflicting event bindings" },
{ WML_ERROR_DUP_DO,
  "Two or more 'do' elements with the same name" },
{ WML_ERROR_MISSING_DO_NAME,
  "Missing name for 'do' element" },
{ SI_ERROR_INVALID_DATETIME,
  "Invalid datetime" }
};

/*  6.2 Local Function Prototypes  */

    /* None */


/*  7 MODULE CODE  */

#ifdef _DEBUG

/*
=============================================================================
*/
/*  7.1  */
void       wmlLOGs  (int         level,
                     const char *s1)
{
  wmlLOGssss(level, s1, 0,0,0);
}

/*
=============================================================================
*/
/*  7.2  */
void       wmlLOGss (int         level,
                     const char *s1,
                     const char *s2)
{
  wmlLOGssss(level, s1, s2,0,0);
}

/*
=============================================================================
*/
/*  7.3  */
void       wmlLOGsss(int         level,
                     const char *s1,
                     const char *s2,
                     const char *s3)
{
  wmlLOGssss(level, s1, s2, s3, 0);
}

/*
=============================================================================
*/
/*  7.4  */
void       wmlLOGssss(int level,
                      const char *s1,
                      const char *s2,
                      const char *s3,
                      const char *s4)
{
/*
-----------------------------------------------------------------------------
*/
  if(s1 && s2 && s3 && s4) {
      WSL_IMPL_Log(WSL_GLOBAL, "%s %s %s %s", s1,s2,s3,s4);
  }
  else if(s1 && s2 && s3) {
      WSL_IMPL_Log(WSL_GLOBAL, "%s %s %s", s1,s2,s3);
  }
  else if(s1 && s2) {
      WSL_IMPL_Log(WSL_GLOBAL, "%s %s", s1,s2);
  }
  else if(s1) {
      WSL_IMPL_Log(WSL_GLOBAL, "%s", s1);
  }
}

#endif /* _DEBUG */

/*
=============================================================================
*/
/*  7.5  */
const char *error2str(WML_STATUS status)
{
/*  Convert XML_STATUS to ASCII string
 *
 *---------------------------------------------------------------------------
 */
/*  Data Structures  */
  int pos = (int)status;
  const int max_pos = sizeof(s_status)/sizeof(s_status[0]);

/*  Code  */
  if(pos<0 || pos>=max_pos)
      pos = (int)WML_ERROR_NOT_INITIALIZED;

  return s_status[pos].m_ascii;
}



/*
 * The rest of the file will be included when 'WML_DEBUG' is defined
 */

#ifdef WML_DEBUG

UINT32 s_gdbFlags=0;

/*
=============================================================================
*/
/*  7.3  */
void dPrint(int flg,
            char *fmt,
            ...)
{
/*
-----------------------------------------------------------------------------
*/
/*  Data structures */

  char _buf[WSL_MAX_LOG_LEN];
  va_list marker;

/* Code */

  va_start(marker, fmt);
  vsprintf(_buf, fmt, marker);
  va_end( marker);

  WSL_LOG_Log(1,WSL_LOG_DEBUG,(UINT8*)"%s", _buf);
}


/*
=============================================================================
*/
/*  7.4  */
void dPrint2(int        flg,
             PString    str,
             char *     fmt,
             ...)
{
/*
-----------------------------------------------------------------------------
*/
/*  Data structures */

  char _buf[WSL_MAX_LOG_LEN];
  char *_ptr = _buf;
  int  _len;
  va_list marker;

/* Code */

  va_start(marker, fmt);
  vsprintf(_buf, fmt, marker);
  va_end( marker);
  _len = strlen(_buf);

  if(str) {
      wPrint(STR_LEN(str),STR_DATA(str),
	      (WSL_MAX_LOG_LEN-_len-1), _ptr+_len);
  }
  else {
      strcat(_buf,">NULL pointer<");
  }

  WSL_LOG_Log(1,WSL_LOG_DEBUG,(UINT8*)"%s", _buf);
}
#endif

/*
=============================================================================
*/


/*  End of File  */
