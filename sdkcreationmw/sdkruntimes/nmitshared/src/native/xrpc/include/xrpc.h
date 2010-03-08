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
* Description:  Pubic API for the XRPC library
 *
*/


#ifndef _XRPC_H_
#define _XRPC_H_

#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#if !defined(_WIN32) && defined(__WINS__)
/* Symbian development environment? */
#  define _WIN32 
#endif /* !_WIN32 && __WINS__ */

/* small piece of platform dependent code */
#ifdef _WIN32
#  define XRPC_API __cdecl

typedef char    int8_t;
typedef short   int16_t;
typedef int     int32_t;
typedef __int64 int64_t;

typedef unsigned char    uint8_t;
typedef unsigned short   uint16_t;
typedef unsigned int     uint32_t;
typedef unsigned __int64 uint64_t;

#else

#  define XRPC_API 
#  include <inttypes.h>

#endif 

typedef int XRpcSocket;
typedef uint16_t XRpcPort;

/* *************************************************************************
//
//  Attributes
//
// *************************************************************************/

#define XRPC_ATTR_NAME          "name"
#define XRPC_ATTR_TYPE          "type"
#define XRPC_ATTR_ID            "id"
#define XRPC_ATTR_METHOD        "method"
#define XRPC_ATTR_PROTOCOL      "target"
#define XRPC_ATTR_STATUS        "status"

/* *************************************************************************
//
//  Tags
//
// *************************************************************************/

/* root element */
#define XRPC_TAG_ROOT           "xrpc"

/* primitive data types */
#define XRPC_TAG_DOUBLE         "double"
#define XRPC_TAG_LONG           "long"
#define XRPC_TAG_INT            "int"
#define XRPC_TAG_SHORT          "short"
#define XRPC_TAG_BYTE           "byte"
#define XRPC_TAG_BOOLEAN        "bool"
#define XRPC_TAG_BASE64         "base64"
#define XRPC_TAG_STRING         "string"

/* aggregates */
#define XRPC_TAG_STRUCT         "struct"
#define XRPC_TAG_ARRAY          "array"

/* special values */
#define XRPC_TAG_NULL           "null"
#define XRPC_TAG_VOID           "void"

/* call tags */
#define XRPC_TAG_CALL           "call"
#define XRPC_TAG_PARAMS         "params"

/* response tags */
#define XRPC_TAG_RESPONSE       "response"
#define XRPC_TAG_RESULT         "result"

/* enumeration for XRPC tags */
typedef enum _XRpcTag {
    XRpcTagNone = 0,    /* must be first and must be zero */
    XRpcTagRoot,        /* must be second */
    XRpcTagDouble,
    XRpcTagLong,
    XRpcTagInt,
    XRpcTagShort,
    XRpcTagByte,
    XRpcTagBoolean,
    XRpcTagBase64,
    XRpcTagString,
    XRpcTagStruct,
    XRpcTagArray,
    XRpcTagNull,
    XRpcTagVoid,
    XRpcTagCall,
    XRpcTagParams,
    XRpcTagResponse,
    XRpcTagResult,
    XRpcTagUnknown      /* must be last */
} XRpcTag;

/* *************************************************************************
//
//  XRPC status codes
//
// *************************************************************************/

#define XRPC_STATUS_OK          "ok"
#define XRPC_STATUS_NOPROTO     "notarget"
#define XRPC_STATUS_NOMETHOD    "nomethod"
#define XRPC_STATUS_SYSERR      "syserr"

/* enumeration for XRPC status attributes */
typedef enum _XRpcStatus {
    XRpcStatusInProgress = 0,
    XRpcStatusNoTarget,
    XRpcStatusNoMethod,
    XRpcStatusSysErr,
    XRpcStatusOK
} XRpcStatus;

/* *************************************************************************
//
//  Data types
//
// *************************************************************************/

/* basic data types */
typedef enum _XRpcBool {
    XRpcFalse = 0,
    XRpcTrue  = 1
} XRpcBool;

typedef double   XRpcDouble;
typedef int8_t   XRpcByte;
typedef int16_t  XRpcShort;
typedef int32_t  XRpcInt;
typedef int64_t  XRpcLong;
typedef uint16_t XRpcUcs2;

typedef const char     * XRpcString;
typedef const char     * XRpcStringUTF8;
typedef const XRpcUcs2 * XRpcStringUCS2;

/* XRPC related data types */
typedef enum _XRpcDataType {

    XRpcTypeDouble = 1, 
    XRpcTypeLong,           /* -9223372036854775808 ... 9223372036854775807 */
    XRpcTypeInt,            /* -2147483648 ... 2147483647 */
    XRpcTypeShort,          /* -32768 to 32767 */
    XRpcTypeByte,           /* -128 ... 127 */
    XRpcTypeBool,           /* True/False */
    XRpcTypeBinary,         /* arbitrary binary data */
    XRpcTypeString,         /* string */

    /* aggregate types */
    XRpcTypeStruct,
    XRpcTypeArray,
    
    /* special types */
    XRpcTypeNull,
    XRpcTypeVoid,

    /* even more special type (must be last) */
    XRpcTypeUnknown,
} XRpcDataType;

/* names and values of the attributes *must* be US-ASCII */
typedef struct _XRpcAttr {
    char * name;            /* attribute's name */
    char * value;           /* attribute's value */
} XRpcAttr;

/*  a binary buffer */
typedef struct _XRpcData {
    XRpcByte * ptr;         /* binary data */
    size_t size;            /* size, in bytes */
} XRpcData;

/* 
 * elements 
 *
 * "inheritance" graph:
 *
 * XRpcElement
 * |
 * + -- XRpcContainer
 * |
 * + -- XRpcDataElement
 *      |
 *      +-- XRpcVoidElement
 *      |
 *      +-- XRpcNullElement
 *      |
 *      +-- XRpcBoolElement
 *      |
 *      +-- XRpcByteElement
 *      |
 *      +-- XRpcShortElement
 *      |
 *      +-- XRpcIntElement
 *      |
 *      +-- XRpcLongElement
 *      |
 *      +-- XRpcDoubleElement
 *      |
 *      +-- XRpcBinaryElement
 *      |
 *      +-- XRpcStringElement
 *
 * You can safely cast each type to its "parent" (or "base class") 
 * To perform a cast in another direction you have to use the 
 * convertion functions such as XRPC_IsDataElement(), etc.
 */
typedef struct _XRpcElement         XRpcElement;
typedef struct _XRpcContainer       XRpcContainer;
typedef struct _XRpcDataElement     XRpcDataElement;

typedef struct _XRpcDataElement     XRpcVoidElement;
typedef struct _XRpcDataElement     XRpcNullElement;

typedef struct _XRpcBoolElement     XRpcBoolElement;
typedef struct _XRpcByteElement     XRpcByteElement;
typedef struct _XRpcShortElement    XRpcShortElement;
typedef struct _XRpcIntElement      XRpcIntElement;
typedef struct _XRpcLongElement     XRpcLongElement;
typedef struct _XRpcDoubleElement   XRpcDoubleElement;
typedef struct _XRpcBinaryElement   XRpcBinaryElement;
typedef struct _XRpcStringElement   XRpcStringElement;

/* *************************************************************************
//
//  Protocol information
//
// *************************************************************************/

typedef struct _XRpcProtocolInfo {
    XRpcString name;        /* protocol name */
    XRpcString descr;       /* protocol description */
    XRpcString vendor;      /* protocol vendor */
    int vmajor;             /* major version of the protocol */
    int vminor;             /* minor version of the protocol */
} XRpcProtocolInfo;

/* callback to receive protocol information */
typedef XRpcBool (
XRPC_API * 
XRpcProtInfoProc)(
    const XRpcProtocolInfo * info, 
    void * context
);

/* *************************************************************************
//
//  Functions
//
// *************************************************************************/
 
XRpcString
XRPC_API
XRPC_GetTagName(
    XRpcTag                 tag
);

XRpcString
XRPC_API
XRPC_GetValidTagName(
    XRpcTag                 tag
);

/* *************************************************************************
 *  XRpcElement access
 * *************************************************************************/

const XRpcDataElement *
XRPC_API
XRPC_IsDataElement(
    const XRpcElement *     element
);

const XRpcContainer *
XRPC_API
XRPC_IsContainer(
    const XRpcElement *     element
);

const XRpcContainer *
XRPC_API
XRPC_IsArray(
    const XRpcElement *     element
);

const XRpcContainer *
XRPC_API
XRPC_IsStruct(
    const XRpcElement *     element,
    XRpcString              type
);

XRpcTag
XRPC_API
XRPC_GetElementTag(
    const XRpcElement *     element
);

XRpcString
XRPC_API
XRPC_GetElementTagName(
    const XRpcElement *     element
);

/* macros */

#define _XRPC_GetElementTag(_e) \
         XRPC_GetElementTag((XRpcElement*)(_e))
#define _XRPC_GetElementTagName(_e) \
         XRPC_GetElementTagName((XRpcElement*)(_e))

/* *************************************************************************
 *  Global (de)initialization
 * *************************************************************************/

XRpcBool
XRPC_API
XRPC_Init(
    void
);

void
XRPC_API
XRPC_Deinit(
    void
);

/* *************************************************************************
 *  Attributes
 * *************************************************************************/

XRpcString
XRPC_API
XRPC_GetElementName(
    const XRpcElement *     element
);

XRpcBool
XRPC_API
XRPC_SetElementName(
    XRpcElement *           element,
    XRpcString              name
);

XRpcContainer * 
XRPC_API
XRPC_GetParent(
    XRpcElement *           element
);

int
XRPC_API
XRPC_AttributeCount(
    const XRpcElement *     element
);

const XRpcAttr *
XRPC_API
XRPC_GetAttributeAt(
    const XRpcElement *     element,
    int                     index
);

XRpcBool
XRPC_API
XRPC_RemoveAttributeAt(
    XRpcElement *           element,
    int                     index
);

XRpcString
XRPC_API
XRPC_GetAttribute(
    const XRpcElement *     element,
    XRpcString              name
);

int
XRPC_API
XRPC_GetAttributeIndex(
    const XRpcElement *     element,
    XRpcString              name
);

XRpcBool
XRPC_API
XRPC_SetAttribute(
    XRpcElement *           element,
    XRpcString              name,
    XRpcString              value
);

XRpcBool
XRPC_API
XRPC_RemoveAttribute(
    XRpcElement *           element,
    XRpcString              name
);

/* macros */

#define _XRPC_GetElementName(_e) \
         XRPC_GetElementName((XRpcElement*)(_e))
#define _XRPC_SetElementName(_e,_s) \
         XRPC_SetElementName((XRpcElement*)(_e),_s)
#define _XRPC_AttributeCount(_e) \
         XRPC_AttributeCount((XRpcElement*)(_e))
#define _XRPC_GetAttributeAt(_e,_i) \
         XRPC_GetAttributeAt((XRpcElement*)(_e), _i)
#define _XRPC_RemoveAttributeAt(_e,_i) \
         XRPC_RemoveAttributeAt((XRpcElement*)(_e),_i)
#define _XRPC_GetAttribute(_e,_n) \
         XRPC_GetAttribute((XRpcElement*)(_e),_n)
#define _XRPC_GetAttributeIndex(_e,_n) \
         XRPC_GetAttributeIndex((XRpcElement*)(_e),_n)
#define _XRPC_SetAttribute(_e,_n,_v) \
         XRPC_SetAttribute((XRpcElement*)(_e),_n,_v) 
#define _XRPC_RemoveAttribute(_e,_n) \
         XRPC_RemoveAttribute((XRpcElement*)(_e),_n)

/* *************************************************************************
 *  XRpcContainer access
 * *************************************************************************/

/* container element */
int
XRPC_API
XRPC_ElementCount(
    const XRpcContainer *   container
);

const XRpcElement *
XRPC_API
XRPC_GetElementAt(
    const XRpcContainer *   container,
    int                     index
);

const XRpcElement *
XRPC_API
XRPC_GetElementByName(
    const XRpcContainer *   container,
    XRpcString              name
);

int 
XRPC_API
XRPC_GetElementIndex(
    const XRpcContainer *   container, 
    const XRpcElement *     elem
);

XRpcBool
XRPC_API
XRPC_AddElement(
    XRpcContainer *         container,
    XRpcElement *           element
);

XRpcBool
XRPC_API
XRPC_TryAddElement(
    XRpcContainer *         container,
    XRpcElement *           element
);

XRpcBool
XRPC_API
XRPC_InsertElement(
    XRpcContainer *         container,
    XRpcElement *           element,
    int                     index
);

XRpcBool
XRPC_API
XRPC_RemoveElementAt(
    XRpcContainer *         container,
    int                     index
);

/*
 * const XRpcDataElement *
 * XRPC_API
 * XRPC_GetDataElementAt(
 *     const XRpcContainer *   container,
 *     int                     index
 * );
 */

#define XRPC_GetDataElementAt(_c,_i) \
        XRPC_IsDataElement(XRPC_GetElementAt(_c, _i))

/*
 * const XRpcDataElement *
 * XRPC_API
 * XRPC_GetDataElementByName(
 *     const XRpcContainer *   container,
 *     XRpcString              name
 * );
 */

#define XRPC_GetDataElementByName(_c,_s) \
        XRPC_IsDataElement(XRPC_GetElementByName(_c, _s))

/* macros */

#define _XRPC_AddElement(_c,_e) \
         XRPC_AddElement(_c, (XRpcElement*)(_e))
#define _XRPC_InsertElement(_c,_e,_i) \
         XRPC_InsertElement(_c, (XRpcElement*)(_e), _i)

/* *************************************************************************
 *  XRpcDataElement access
 *
 *  Recommended use:
 *
 *  const XRpcContainer * params;
 *  const XIntElement * arg5;
 *  int n;
 *  ...
 *  arg5 = XRPC_IsIntElement(XRPC_GetDataElementAt(params, 4));
 *  if (arg5) {
 *      n = XRPC_GetInt(arg5);
 *  } else {
 *      Error("ERROR: Type mismatch for parameter 5 (%s, expected int)\n",
 *            XRPC_GetTypeName(_XRPC_GetDataType(arg5)));
 *      return ERROR;
 *  }
 *
 * *************************************************************************/

XRpcUcs2 * 
XRPC_API
XRPC_GetAsText(
    const XRpcDataElement * elem
);

XRpcDataType
XRPC_API
XRPC_GetDataType(
    const XRpcElement *     element
);

XRpcString
XRPC_API
XRPC_GetTypeName(
    XRpcDataType            type
);

#define _XRPC_GetDataType(_e) \
         XRPC_GetDataType((XRpcElement*)(_e))

/* 
 * Convertion from XRpcDataElement to specific types. Each of these functions
 * returns NULL if input parameter is NULL.
 */
const XRpcBoolElement *
XRPC_API
XRPC_IsBoolElement(
    const XRpcDataElement * element
);

const XRpcByteElement *
XRPC_API
XRPC_IsByteElement(
    const XRpcDataElement * element
);

const XRpcShortElement *
XRPC_API
XRPC_IsShortElement(
    const XRpcDataElement * element
);

const XRpcIntElement *
XRPC_API
XRPC_IsIntElement(
    const XRpcDataElement * element
);

const XRpcLongElement *
XRPC_API
XRPC_IsLongElement(
    const XRpcDataElement * element
);

const XRpcDoubleElement *
XRPC_API
XRPC_IsDoubleElement(
    const XRpcDataElement * element
);

const XRpcStringElement *
XRPC_API
XRPC_IsStringElement(
    const XRpcDataElement * element
);

const XRpcBinaryElement *
XRPC_API
XRPC_IsBinaryElement(
    const XRpcDataElement * element
);

/* access to actual data */

XRpcBool
XRPC_API
XRPC_GetBool(
    const XRpcBoolElement * element
);

XRpcByte
XRPC_API
XRPC_GetByte(
    const XRpcByteElement * element
);

XRpcShort
XRPC_API
XRPC_GetShort(
    const XRpcShortElement * element
);

XRpcInt
XRPC_API
XRPC_GetInt(
    const XRpcIntElement *  element
);

XRpcLong
XRPC_API
XRPC_GetLong(
    const XRpcLongElement * element
);

XRpcDouble
XRPC_API
XRPC_GetDouble(
    const XRpcDoubleElement * element
);

XRpcString
XRPC_API
XRPC_GetString(
    const XRpcStringElement * element
);

XRpcStringUTF8
XRPC_API
XRPC_GetStringUTF8(
    const XRpcStringElement * element
);

XRpcStringUCS2
XRPC_API
XRPC_GetStringUCS2(
    const XRpcStringElement * element
);

size_t
XRPC_API
XRPC_GetStringLength(
    const XRpcStringElement * element
);

const XRpcByte *
XRPC_API
XRPC_GetBinaryData(
    const XRpcBinaryElement * element
);

size_t
XRPC_API
XRPC_GetBinaryDataSize(
    const XRpcBinaryElement * element
);

/* creation of data elements */

XRpcContainer * 
XRPC_API
XRPC_CreateContainer(
    XRpcTag         tag
);

/*
 * XRpcContainer * 
 * XRPC_API
 * XRPC_CreateArray();
 */

#define XRPC_CreateArray() \
        XRPC_CreateContainer(XRpcTagArray)

XRpcContainer * 
XRPC_API
XRPC_CreateStruct(
    XRpcString      type
);

XRpcVoidElement *
XRPC_API
XRPC_CreateVoidElement(
    void
);

XRpcNullElement *
XRPC_API
XRPC_CreateNullElement(
    void
);

XRpcBoolElement *
XRPC_API
XRPC_CreateBoolElement(
    XRpcBool                data
);

XRpcByteElement *
XRPC_API
XRPC_CreateByteElement(
    XRpcByte                data
);

XRpcShortElement *
XRPC_API
XRPC_CreateShortElement(
    XRpcShort               data
);

XRpcIntElement *
XRPC_API
XRPC_CreateIntElement(
    XRpcInt                 data
);

XRpcLongElement *
XRPC_API
XRPC_CreateLongElement(
    XRpcLong                data
);

XRpcDoubleElement *
XRPC_API
XRPC_CreateDoubleElement(
    XRpcDouble              data
);

XRpcStringElement *
XRPC_API
XRPC_CreateStringElement(
    XRpcString              data,
    int                     len
);

XRpcStringElement *
XRPC_API
XRPC_CreateStringElementUCS2(
    XRpcStringUCS2          data,
    int                     len
);

/*
 * Note: nbytes parameter specifies number of bytes (not characters!)
 * in the UTF8 buffer. It can be -1 if the buffer is NULL terminated.
 */
XRpcStringElement *
XRPC_API
XRPC_CreateStringElementUTF8(
    XRpcStringUTF8          data,
    int                     nbytes
);

XRpcBinaryElement *
XRPC_API
XRPC_CreateBinaryElement(
    const XRpcByte *        data,
    size_t                  size
);

/* updating existing elements */

XRpcBool 
XRPC_API
XRPC_SetAsText(
    XRpcDataElement *       elem, 
    XRpcStringUCS2          text
);

void
XRPC_API
XRPC_UpdateBoolElement(
    XRpcBoolElement *       element,
    XRpcBool                data
);

void
XRPC_API
XRPC_UpdateByteElement(
    XRpcByteElement *       element,
    XRpcByte                data
);

void
XRPC_API
XRPC_UpdateShortElement(
    XRpcShortElement *      element,
    XRpcShort               data
);

void
XRPC_API
XRPC_UpdateIntElement(
    XRpcIntElement *        element,
    XRpcInt                 data
);

void
XRPC_API
XRPC_UpdateLongElement(
    XRpcLongElement *       element,
    XRpcLong                data
);

void
XRPC_API
XRPC_UpdateDoubleElement(
    XRpcDoubleElement *     element,
    XRpcDouble              data
);

/*
 * Note: if you use 'keep' == True in any of the functions below, the memory 
 * must be allocated with XRPC_Malloc() call. 
 */

XRpcBool
XRPC_API
XRPC_UpdateStringElement(
    XRpcStringElement *     element,
    char *                  data,
    int                     len,
    XRpcBool                keep
);

XRpcBool
XRPC_API
XRPC_UpdateStringElementUCS2(
    XRpcStringElement *     element,
    XRpcUcs2 *              data,
    int                     len,
    XRpcBool                keep
);

/*
 * Note: nbytes parameter specifies number of bytes (not characters!)
 * in the UTF8 buffer. It can be -1 if the buffer is NULL terminated.
 */
XRpcBool
XRPC_API
XRPC_UpdateStringElementUTF8(
    XRpcStringElement *     element,
    XRpcStringUTF8          data,
    int                     nbytes
);

XRpcBool
XRPC_API
XRPC_UpdateBinaryElement(
    XRpcBinaryElement *     element,
    XRpcByte *              data,
    size_t                  size,
    XRpcBool                keep
);

/* deleting elements */
void 
XRPC_API
XRPC_FreeElement(
    XRpcElement *   e
);

/* adding data elements to a container of type "struct" or "params" */

XRpcBool 
XRPC_API
XRPC_AddDoubleField(
    XRpcContainer *         structure, 
    XRpcString              name,
    XRpcDouble              data
);

XRpcBool 
XRPC_API
XRPC_AddLongField(
    XRpcContainer *         structure, 
    XRpcString              name,
    XRpcLong                data
);

XRpcBool 
XRPC_API
XRPC_AddIntField(
    XRpcContainer *         structure, 
    XRpcString              name,
    XRpcInt                 data
);

XRpcBool 
XRPC_API
XRPC_AddShortField(
    XRpcContainer *         structure, 
    XRpcString              name,
    XRpcShort               data
);

XRpcBool 
XRPC_API
XRPC_AddByteField(
    XRpcContainer *         structure, 
    XRpcString              name,
    XRpcByte                data
);

XRpcBool 
XRPC_API
XRPC_AddBoolField(
    XRpcContainer *         structure, 
    XRpcString              name,
    XRpcBool                data
);

XRpcBool 
XRPC_API
XRPC_AddBinaryField(
    XRpcContainer *         structure, 
    XRpcString              name,
    const XRpcByte *        data,
    size_t                  size
);

XRpcBool 
XRPC_API
XRPC_AddStringField(
    XRpcContainer *         structure, 
    XRpcString              name,
    XRpcString              data
);

XRpcBool 
XRPC_API
XRPC_AddStringFieldUCS2(
    XRpcContainer *         structure, 
    XRpcString              name,
    XRpcStringUCS2          data
);

XRpcBool 
XRPC_API
XRPC_AddStringFieldUTF8(
    XRpcContainer *         structure, 
    XRpcString              name,
    XRpcStringUTF8          data,
    int                     nbytes
);

XRpcBool 
XRPC_API
XRPC_AddContainerField(
    XRpcContainer *         structure, 
    XRpcString              name,
    XRpcContainer *         field 
);

XRpcBool 
XRPC_API
XRPC_AddField(
    XRpcContainer *         structure, 
    XRpcString              name,
    XRpcElement *           field 
);

/* finding named data elements of various types in a container */

const XRpcDoubleElement *
XRPC_API
XRPC_GetDoubleElementByName(
    const XRpcContainer *   container,
    XRpcString              name
);

const XRpcLongElement *
XRPC_API
XRPC_GetLongElementByName(
    const XRpcContainer *   container, 
    XRpcString              name
);

const XRpcIntElement *
XRPC_API
XRPC_GetIntElementByName(
    const XRpcContainer *   container, 
    XRpcString              name
);

const XRpcShortElement *
XRPC_API
XRPC_GetShortElementByName(
    const XRpcContainer *   container, 
    XRpcString              name
);

const XRpcByteElement *
XRPC_API
XRPC_GetByteElementByName(
    const XRpcContainer *   container, 
    XRpcString              name
);

const XRpcBoolElement *
XRPC_API
XRPC_GetBoolElementByName(
    const XRpcContainer *   container, 
    XRpcString              name
);

const XRpcBinaryElement *
XRPC_API
XRPC_GetBinaryElementByName(
    const XRpcContainer *   container, 
    XRpcString              name
);

const XRpcStringElement *
XRPC_API
XRPC_GetStringElementByName(
    const XRpcContainer *   container, 
    XRpcString              name
);

const XRpcContainer *
XRPC_API
XRPC_GetArrayByName(
    const XRpcContainer *   container, 
    XRpcString              name
);

const XRpcContainer *
XRPC_API
XRPC_GetStructByName(
    const XRpcContainer *   container, 
    XRpcString              name,
    XRpcString              type
);

/* macros */

#define _XRPC_FreeElement(_e) \
         XRPC_FreeElement((XRpcElement*)(_e))
#define _XRPC_CreateArray() ((XRpcElement*)\
         XRPC_CreateArray())
#define _XRPC_CreateContainer(_tag) ((XRpcElement*)\
         XRPC_CreateContainer(_tag))
#define _XRPC_CreateStruct(_type) ((XRpcElement*)\
         XRPC_CreateStruct(_type))
#define _XRPC_CreateVoidElement() ((XRpcElement*)\
         XRPC_CreateVoidElement())
#define _XRPC_CreateNullElement() ((XRpcElement*)\
         XRPC_CreateNullElement())
#define _XRPC_CreateBoolElement(_b) ((XRpcElement*)\
         XRPC_CreateBoolElement(_b))
#define _XRPC_CreateByteElement(_b) ((XRpcElement*)\
         XRPC_CreateByteElement(_b))
#define _XRPC_CreateShortElement(_h) ((XRpcElement*)\
         XRPC_CreateShortElement(_h))
#define _XRPC_CreateIntElement(_i) ((XRpcElement*)\
         XRPC_CreateIntElement(_i))
#define _XRPC_CreateLongElement(_l) ((XRpcElement*)\
         XRPC_CreateLongElement(_l))
#define _XRPC_CreateDoubleElement(_d) ((XRpcElement*)\
         XRPC_CreateDoubleElement(_d))
#define _XRPC_CreateStringElement(_s) ((XRpcElement*)\
         XRPC_CreateStringElement(_s,-1))
#define _XRPC_CreateStringElementUCS2(_s) ((XRpcElement*)\
         XRPC_CreateStringElementUCS2(_s,-1))
#define _XRPC_CreateStringElementUTF8(_s) ((XRpcElement*)\
         XRPC_CreateStringElementUTF8(_s,-1))
#define _XRPC_CreateBinaryElement(_data,_size) ((XRpcElement*)\
         XRPC_CreateBinaryElement(_data,_size))

/* 
 * These macros translate into type-safe functions in debug build.
 * In release build they are quick macros. Please use these as much
 * as possible to prevent invalid casts.
 */
#ifdef _DEBUG

XRpcElement * 
XRPC_API 
XRPC_ContainerToElement(
    XRpcContainer * container
);

void 
XRPC_API 
XRPC_FreeContainer(
    XRpcContainer * container
);

#else /* _DEBUG */

#define XRPC_ContainerToElement(_c)     ((XRpcElement*)(_c))
#define XRPC_FreeContainer(_c)          _XRPC_FreeElement(_c)

#endif /* !_DEBUG */

/* *************************************************************************
 *  Client API
 * *************************************************************************/

typedef struct _XRpcCall   XRpcCall;
typedef struct _XRpcClient XRpcClient;
typedef struct _XRpcServer XRpcServer;
typedef struct _XRpcSession XRpcSession;

/**
 * Sends message to the remote side and returns waitable context.
 * The context must be eventually deallocated with XRPC_FreeCall
 */
XRpcCall * 
XRPC_API
XRPC_Call(
    XRpcClient * client,
    XRpcString protocol,
    XRpcString method,
    const XRpcElement * args    /* element, <struct>, <array> or <params> */
);

/**
 * Waits for the results of the pending XRPC call. Does not return until
 * the call is complete
 */
void
XRPC_API
XRPC_Wait(
    XRpcCall * call
);

/**
 * Waits for the results of the pending XRPC call. 
 * Timeout is given in milliseconds.
 * Returns True if call completes, False if the call is still pending
 */
XRpcBool
XRPC_API
XRPC_TimeWait(
    XRpcCall * call,
    XRpcInt timeout
);

/**
 * Returns the status of the completed call, or XRpcStatusInProgress if the
 * call is still in progress.
 */
XRpcStatus
XRPC_API
XRPC_CallStatus(
    XRpcCall * call
);

/**
 * Returns the result of the completed call, or NULL if the call is still 
 * in progress.
 */
const XRpcElement *
XRPC_API
XRPC_CallResult(
    XRpcCall * call
);

/**
 * Deallocates the call context
 */
void
XRPC_API
XRPC_FreeCall(
    XRpcCall * call
);

/**
 * Sends a message to the remote side which does not require a confirmation.
 * Returns True if message has been successfully sent. There's no guarantee
 * that it has been (or will ever be) processed by the other side. This is
 * OK for many kinds of asynchronous notifications.
 */
XRpcBool
XRPC_API
XRPC_Notify(
    XRpcClient * client,
    XRpcString protocol,
    XRpcString method,
    const XRpcElement * args    /* element, <struct>, <array> or <params> */
);

/**
 * The next two functions are same as XRPC_Notify and XRPC_Call but take a 
 * format specification and variable number of arguments. The format is a 
 * sequence of format specifications each of which looks as follows
 * 
 *     %<name>!<type> 
 *     %%<type> 
 *
 * The latter means that the name of the argument preceeds the parameter
 * in the argument list. See the source code for detail.
 *
 * The types are
 *
 *     s   - ASCII string
 *     u   - UCS2 string
 *     S   - wide string
 *     c   - XRpcByte
 *     h   - XRpcShort
 *     i   - XRpcInt
 *     l   - XRpcLong
 *     b   - XRpcBool
 *     d   - XRpcData (binary)
 *     x   - XRpcElement (will be deleted)
 *     n   - null
 *
 * other types are not supported at the moment
 */
XRpcBool
XRPC_API
XRPC_FormatNotify(
    XRpcClient * client,
    XRpcString protocol,
    XRpcString method,
    XRpcString format,
    ...
);

XRpcCall *
XRPC_API
XRPC_FormatCall(
    XRpcClient * client,
    XRpcString protocol,
    XRpcString method,
    XRpcString format,
    ...
);

/**
 * Call the specified callback once per each protocol registered on the 
 * other side of the connection, assuming that the other side implements
 * "xrpc.core" protocol (which will be one of the protocols). Returns 
 * True if the remote side supports "xrpc.core" protocol, False if the 
 * other side does not support it or if the remote call failed.
 */
XRpcBool
XRPC_API
XRPC_ListRemoteProtocols(
    XRpcClient * client,                /* XRPC client */
    XRpcProtInfoProc proc,              /* the callback to invoke */
    void * ctx                          /* context passed to the callback */
);

/**
 * Tests whether the connection is alive
 */
XRpcBool 
XRPC_API 
XRPC_IsAlive(
    XRpcClient * client                 /* XRPC client */
);

/* *************************************************************************
 *  Server API
 * *************************************************************************/

/* call handler */
typedef 
XRpcElement * (XRPC_API * XRpcHandler)(
    void *                  context,    /* method specific context */
    const XRpcContainer *   params      /* parameters of the call */
);

typedef struct _XRpcMethodInfo {
    XRpcHandler handler;                /* method callback */
    XRpcBool    synchronous;            /* method is synchronous */
    void *      context;                /* method specific context */
} XRpcMethodInfo;

/* 
 * "get method" protocol callback. It's invoked when server starts 
 * parsing a <call> element. If this method returns True and fills 
 * out the XRpcMethodInfo structure, that method will be invoked when
 * parsing of the <call> block is complete. If an error occurs while 
 * parsing the <call> tag, the server will invoke XRpcProtoCancel
 * function if such was provided by the protocol.
 */
typedef 
XRpcBool (XRPC_API * XRpcProtoGetMethod)(
    void *                  context,    /* protocol specific context */
    XRpcString              name,       /* method name */
    XRpcMethodInfo *        info        /* receives handler and context */
);

/* "cancel call" protocol callback */
typedef 
void (XRPC_API * XRpcProtoCancel)(
    void *                  context,    /* protocol specific context */
    XRpcString              name,       /* method name */
    XRpcMethodInfo *        info        /* handler and context */
);

/* "session started" callback */
typedef 
void (XRPC_API * XRpcSessionOpen)(
    void *                  context,    /* protocol or server context */
    XRpcSession *           session     /* new session */
);

/* "session finished" callback */
typedef 
void (XRPC_API * XRpcSessionClose)(
    void *                  context,    /* protocol or server context */
    XRpcSession *           session     /* dead session */
);

/* protocol descriptor */
typedef struct _XRpcProtocol {
    XRpcProtoGetMethod      getmethod;  /* returns method information */
    XRpcProtoCancel         cancel;     /* cancels call */
    XRpcSessionOpen         sopen;      /* session started */
    XRpcSessionClose        sclose;     /* session finished */
} XRpcProtocol;

XRpcServer *
XRPC_API
XRPC_CreateServer2(
    XRpcString              parser,     /* parser to use (NULL = default) */
    XRpcSessionOpen         sopen,      /* session started */
    XRpcSessionClose        sclose,     /* session finished */
    void *                  context     /* context for callbacks */
);

/* backward compatibility */
#define XRPC_CreateServer(_parser) \
        XRPC_CreateServer2(_parser,NULL,NULL,NULL)

void
XRPC_API
XRPC_FreeServer(
    XRpcServer *            server      /* server to destory */
);

XRpcBool 
XRPC_API
XRPC_RegisterProtocol(
    XRpcServer *            server,     /* server instance */
    XRpcString              name,       /* protocol name */
    XRpcString              descr,      /* protocol description */
    XRpcString              vendor,     /* protocol vendor */
    int                     vmajor,     /* major version of the protocol */
    int                     vminor,     /* minor version of the protocol */
    const XRpcProtocol *    protocol,   /* protocol callbacks */
    void *                  context     /* protocol context */
);

XRpcBool 
XRPC_API
XRPC_UnregisterProtocol(
    XRpcServer *            server,     /* server instance */
    XRpcString              name        /* protocol name */
);

XRpcBool
XRPC_API
XRPC_ServerListen(
    XRpcServer *            server,     /* server instance */
    XRpcPort                port,       /* TCP port */
    XRpcBool                once        /* accept one connection */
);

XRpcBool
XRPC_API
XRPC_ServerListenSocket(
    XRpcServer *            server,     /* server instance */
    XRpcSocket              socket,     /* TCP socket (not listening) */
    XRpcBool                once        /* accept one connection */
);

XRpcBool
XRPC_API
XRPC_ServerAccept(
    XRpcServer *            server,     /* server instance */
    XRpcSocket              socket,     /* listening TCP socket */
    XRpcBool                once        /* accept one connection */
);

XRpcPort
XRPC_API
XRPC_ServerSpawn(
    XRpcServer *            server,     /* server instance */
    XRpcPort                port        /* listening TCP port (0 for any) */
);

/* 
 * Thread manipulation API
 */
typedef struct _XRpcThread XRpcThread;
typedef void (XRPC_API * XRpcThreadProc)(void * arg);
typedef void (XRPC_API * XRpcThreadJoin)(XRpcThread * thread);
typedef void (XRPC_API * XRpcThreadDetach)(XRpcThread * thread);
typedef XRpcThread * (XRPC_API * XRpcThreadCreate)(
    XRpcThreadProc proc,                /* thread function */
    void * arg                          /* thread function parameter */
);

void
XRPC_API
XRPC_ServerSetThreadAPI(
    XRpcServer *            server,     /* server instance */
    XRpcThreadCreate        create,     /* thread creation routine */
    XRpcThreadJoin          join,       /* waits until thread is done */
    XRpcThreadDetach        detach      /* makes thread free-running */
);

void
XRPC_API
XRPC_ServerResetThreadAPI(              /* sets default thread API */
    XRpcServer *            server      /* server instance */
);

/* *************************************************************************
 *  Session API
 * *************************************************************************/

typedef
void (XRPC_API * XRpcSessionProc)(
    XRpcSession *           session,    /* the session */
    void *                  context     /* context parameter */
);

XRpcSession *
XRPC_API
XRPC_CreateStreamSession(
    XRpcServer *            server,     /* server instance (may be NULL) */
    FILE *                  in,         /* input stream */
    FILE *                  out         /* output stream */
);

XRpcSession *
XRPC_API
XRPC_CreateSocketSession(
    XRpcServer *            server,     /* server instance (may be NULL) */
    XRpcSocket              in          /* connected TCP sockets */
);

XRpcSession *
XRPC_API
XRPC_Connect(
    XRpcServer *            server,     /* server instance (may be NULL) */
    XRpcString              host,       /* host name (NULL for localhost) */
    XRpcPort                port        /* TCP port (native byte order) */
);

XRpcSession *
XRPC_API
XRPC_GetCurrentSession(
    XRpcServer *            server      /* server instance */
);

void *
XRPC_API
XRPC_SetSessionContext(
    XRpcSession *           session,    /* the session */
    void *                  context     /* associated context */
);

void *
XRPC_API
XRPC_GetSessionContext(
    XRpcSession *           session     /* the session */
);

XRpcBool 
XRPC_API 
XRPC_CreateSessionThread(
    XRpcSession *           session,    /* the session */
    XRpcSessionProc         proc,       /* session proc, may be NULL */
    void *                  context     /* context parameter */
);

XRpcBool 
XRPC_API 
XRPC_CreateSessionThread2(
    XRpcSession *           session,    /* the session */
    XRpcSessionProc         proc,       /* session proc, may be NULL */
    void *                  context,    /* context parameter */
    XRpcThreadCreate        create,     /* thread creation routine */
    XRpcThreadJoin          join,       /* waits until thread is done */
    XRpcThreadDetach        detach      /* makes thread free-running */
);

void
XRPC_API 
XRPC_UseExternalThread(
    XRpcSession *           session     /* the session */
);

XRpcClient *
XRPC_API
XRPC_GetClient(
    XRpcSession *           session     /* the session */
);

XRpcServer *
XRPC_API
XRPC_GetServer(
    XRpcSession *           session     /* the session */
);

void
XRPC_API
XRPC_RunSession(
    XRpcSession *           session     /* the session */
);

void
XRPC_API
XRPC_CloseSession(
    XRpcSession *           session     /* the session */
);

void
XRPC_API
XRPC_FreeSession(
    XRpcSession *           session     /* the session */
);

/* macros */

#define XRPC_CloseCurrentSession(_srv) \
        XRPC_CloseSession(XRPC_GetCurrentSession(_srv))

/* *************************************************************************
 *  Utilities
 * *************************************************************************/

/**
 * Reads one UTF8 character. 
 * On input, *len contains number of bytes available in the buffer
 *
 * If succeeds, returns numeber of bytes used, puts the wide character 
 * into *wc, and  updates *len subtracting the number of bytes used from 
 * the original value. Returns zero if there's not enough data in the input 
 * buffer to read the next UTF8 character. If there's an error in UTF8
 * sequence, returns -1
 *
 * If the UTF8 character cannot be converted into a single byte character 
 * in active code page, the *ch character is set to zero
 */
int
XRPC_API
XRPC_GetUTF8Char(
    XRpcStringUTF8  utf,        /* input UTF8 string */
    XRpcUcs2 *      wc,         /* receives the wide character */
    char *          c,          /* 1-byte character in active code page */
    int *           len         /* available/remaining bytes */
);

/**
 * Returns number of bytes used by a UTF8 character (1...3), -1 if 'utf' 
 * parameter does not point to a valid UTF8 sequence or zero if size of the 
 * character appears to be larger than the amount of data available. 
 *
 * If 'maxsize' parameter is negative, the string is assumed to be null 
 * terminated. Note: if *utf points to a null character, this function 
 * returns 1.
 */
int
XRPC_API
XRPC_GetUTF8CharSize(
    XRpcStringUTF8  utf,        /* input UTF8 string */
    int             maxsize     /* number of bytes available, or -1 */
);

/**
 * Converts a UTF-8 encoded string into null terminated UCS-2 string
 * If number of characters of -1, the input string is assumed to be null
 * terminated. The returned string must be deallocated with XRPC_Free
 */
XRpcUcs2 *
XRPC_API
XRPC_UTF8ToWideString(
    XRpcStringUTF8  utf,        /* input UTF8 string */
    int             size        /* number of bytes */
);

/**
 * Converts a UTF-8 encoded string into null terminated ASCII string
 * If number of characters of -1, the input string is assumed to be null
 * terminated. The returned string must be deallocated with XRPC_Free
 */
char *
XRPC_API
XRPC_UTF8ToASCIIString(
    XRpcStringUTF8  utf,        /* input UTF8 string */
    int             size        /* number of bytes */
);

/**
 * Converts a null terminated wide string to UTF-8. The returned string
 * must be deallocated with XRPC_Free
 */
char *
XRPC_API
XRPC_WideStringToUTF8(
    XRpcStringUCS2  ws          /* input wide string */
);

/**
 * Converts a wide char to UTF-8 chars. Returns the length of the UTF8 char
 */
int
XRPC_API
XRPC_WideCharToUTF8(
    XRpcUcs2        wc,         /* input wide char*/
    char *          dest        /* receives UTF chars, at least 4 bytes */
);

/**
 * Converts a wide char to a single byte character in active code page.
 * Returns zero if conversion is impossible
 */
char
XRPC_API
XRPC_WideCharToASCII(
    XRpcUcs2        wc          /* input wide char*/
);

/**
 * Converts a null terminated wide string into a 8-bit US-ASCII string. 
 * If either memory allocation or convertion fails, returns NULL.
 * The returned string must be deallocated with XRPC_Free
 */
char *
XRPC_API
XRPC_WideStringToASCII(
    XRpcStringUCS2 ws           /* input wide string */
);

/**
 * Converts a null terminated 8-bit US-ASCII string into a wide string.
 * If memory allocation fails, returns NULL. The returned string must be 
 * deallocated with XRPC_Free
 */
XRpcUcs2 *
XRPC_API
XRPC_ASCIIToWideString(
    XRpcString      s           /* input US-ASCII string */
);

/**
 * Converts an 8-bit US-ASCII character into a wide character.
 */
XRpcUcs2
XRPC_API
XRPC_ASCIIToWideChar(
    char c                      /* input US-ASCII character */
);

/**
 * Returns number of characters in a null terminated UTF8 string
 * -1 if it's not a valid UTF8 string
 */
int
XRPC_API
XRPC_LengthUTF8(
    XRpcStringUTF8 utf           /* input UTF8 string */
);

/**
 * Translates a binary buffer into a BASE64 encoded string. The returned 
 * string is guaranteed to be a 7-bit ASCII string which also happens to 
 * be a valid UTF-8 string. If buffer is empty, returns an empty string. 
 * Only returns NULL if memory allocation fails. The caller will have to 
 * deallocate the returned string with XRPC_Free
 */
char *
XRPC_API
XRPC_BASE64Encode(
    const void *    data,       /* buffer to encode */
    size_t          size        /* number of bytes to encode */
);

/**
 * Decodes a BASE64 encoded data block. Returns XRpcTrue if string has been 
 * successfully decoded, or XRpcFalse if it cannot be decoded (or memory
 * allocation failes). The buffer returned by this function must be 
 * deallocated with XRPC_Free
 */
XRpcBool 
XRPC_API
XRPC_BASE64Decode(
    XRpcString      base64,     /* BASE64 encoded data */
    XRpcData *      data        /* receives the decoded data */
);

/**
 * Returns True if the specified character is valid in XML documents. 
 */
XRpcBool
XRPC_API
XRPC_IsValidXMLChar(
    char            c           /* the character to test */
);

/**
 * Returns True if the specified UCS2 character is valid in XML documents. 
 */
XRpcBool
XRPC_API
XRPC_IsValidXMLCharUCS2(
    XRpcUcs2        wc          /* the character to test */
);

/**
 * Returns True if the string doesn't contains characters that are invalid 
 * in XML documents. If the 'len' parameter is negative, the string is
 * assumed to be NULL terminated.
 */
XRpcBool
XRPC_API
XRPC_IsValidXMLString(
    XRpcString      s,          /* the ASCII string to test */
    int             len         /* string length, -1 if zero terminated */
);

/**
 * Returns True if the UCS2 string doesn't contains characters that are 
 * invalid in XML documents. If the 'len' parameter is negative, the 
 * string is assumed to be NULL terminated.
 */
XRpcBool
XRPC_API
XRPC_IsValidXMLStringUCS2(
    XRpcStringUCS2  ws,         /* the UCS2 string to test */
    int             len         /* string length, -1 if zero terminated */
);

/**
 * Returns True if the UTF8 string doesn't contains characters that are 
 * invalid in XML documents. If the 'len' parameter is negative, the 
 * string is assumed to be NULL terminated.
 */
XRpcBool
XRPC_API
XRPC_IsValidXMLStringUTF8(
    XRpcStringUTF8  uft,        /* the UTF8 string to test */
    int             nbytes      /* number of bytes, -1 if zero terminated */
);

/**
 * Allocates memory using XRPC memory allocator.
 */
void * 
XRPC_API
XRPC_Malloc(
    size_t          size        /* number of bytes to allocate */
);

/**
 * Deallocates memory allocated with XRPC_Malloc
 * This functions ignores NULL pointers.
 */
void 
XRPC_API
XRPC_Free(
    void *          ptr         /* memory block to deallocate */
);

/**
 * Returns a string that more or less uniquely identifies a user's logon
 * session. This function is not being used by the XRPC library itself,
 * but it's the recommended way for the parties communicating via XRPC
 * to find out whether they are running in context of the same login 
 * session (whether they are running on the same machine, can be 
 * determined by comparing local/remote addresses of the socket).
 * The returned string has to be deallocated with XRPC_Free when 
 * it's no longer needed. The contents of the returned string is
 * very platform specific.
 */
char * 
XRPC_API 
XRPC_GetLogonID(
    void
);

/**
 * Note: this functions is only present in debug build of the XRPC library.
 * On Unix systems it's equivalent to fprintf(stderr,format,...), but on
 * Win32 it has a useful feature - in addition to doing fprintf(), it also
 * sends all the output to the debugger by invoking OutputDebugString
 * Returns number of characters dumped to the debug output
 */
int 
XRPC_API
XRPC_DebugTrace(
    const char * format,        /* format */
    ...                         /* arguments */
);

#ifdef __cplusplus
} /* and of extern "C" */
#endif  /* __cplusplus */

#endif /* _XRPC_H_ */

/*
 * Local Variables:
 * mode:C
 * c-basic-offset:4
 * indent-tabs-mode: nil
 * End:
 */
