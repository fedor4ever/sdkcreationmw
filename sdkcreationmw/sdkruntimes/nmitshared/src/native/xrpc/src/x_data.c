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
* Description:  XRPC data manipulation routines
 *
*/


#include "x_int.h"

/*
 * Values of the boolean type. Case sensitive
 */
const XRpcUcs2 _XRPC_True[]     = {_UCS2('t'), _UCS2('r'), _UCS2('u'),
                                   _UCS2('e'), _UCS2(0)};
const XRpcUcs2 _XRPC_False[]    = {_UCS2('f'), _UCS2('a'), _UCS2('l'),
                                   _UCS2('s'), _UCS2('e'), _UCS2(0)};

const XRpcStringUCS2 XRPC_True  = _XRPC_True;
const XRpcStringUCS2 XRPC_False = _XRPC_False;

/* *************************************************************************
//
//  Data types
//
// *************************************************************************/

/* 
 * Element type 
 */
typedef void (*DestroyProc)(
    ElementType * t,            /* type of 'e', or its superclass */
    XRpcElement * e             /* the element beign destoryed */
);

typedef Bool (*ParseValueProc)(
    DataElementType * t,        /* type of 'e', or its superclass */
    XRpcDataElement * e,        /* the element receiving the value */
    XRpcStringUCS2 text         /* the text to parse */ 
);

typedef XRpcUcs2 * (*GetValueTextProc)(
    DataElementType * t,        /* type of 'e', or its superclass */
    const XRpcDataElement * e   /* the element returning the value */
);

struct _ElementType {
    ElementType * super;        /* superclass */
    uint32_t signature;         /* type-specific signature, 0 if abstract */
    DestroyProc destroy;        /* destructor */
};

struct _DataElementType {
    ElementType elementType;    /* superclass */
    ParseValueProc parse;       /* parser */
    GetValueTextProc getText;   /* convert to text */
};

static void SUPER_Destroy(ElementType* t, XRpcElement * e);
static void ELEMENT_Destroy(ElementType* t, XRpcElement * e);
static void CONTAINER_Destroy(ElementType* t, XRpcElement * e);
static void STRING_Destroy(ElementType* t, XRpcElement * e);
static void BINARY_Destroy(ElementType* t, XRpcElement * e);

static Bool BOOL_Parse(DataElementType * t, XRpcDataElement * e, XRpcStringUCS2 text); 
static Bool DOUBLE_Parse(DataElementType * t, XRpcDataElement * e, XRpcStringUCS2 text); 
static Bool LONG_Parse(DataElementType * t, XRpcDataElement * e, XRpcStringUCS2 text); 
static Bool INT_Parse(DataElementType * t, XRpcDataElement * e, XRpcStringUCS2 text); 
static Bool SHORT_Parse(DataElementType * t, XRpcDataElement * e, XRpcStringUCS2 text); 
static Bool BYTE_Parse(DataElementType * t, XRpcDataElement * e, XRpcStringUCS2 text); 
static Bool STRING_Parse(DataElementType * t, XRpcDataElement * e, XRpcStringUCS2 text); 
static Bool BINARY_Parse(DataElementType * t, XRpcDataElement * e, XRpcStringUCS2 text); 

static XRpcUcs2 * BOOL_GetText(DataElementType* t,const XRpcDataElement* e);
static XRpcUcs2 * DOUBLE_GetText(DataElementType* t,const XRpcDataElement* e);
static XRpcUcs2 * LONG_GetText(DataElementType* t, const XRpcDataElement* e);
static XRpcUcs2 * INT_GetText(DataElementType* t, const XRpcDataElement* e);
static XRpcUcs2 * SHORT_GetText(DataElementType* t, const XRpcDataElement* e);
static XRpcUcs2 * BYTE_GetText(DataElementType* t, const XRpcDataElement* e);
static XRpcUcs2 * STRING_GetText(DataElementType* t,const XRpcDataElement* e);
static XRpcUcs2 * BINARY_GetText(DataElementType* t,const XRpcDataElement* e);

static ElementType ELEMENT_Type = {
    NULL,
    0,
    ELEMENT_Destroy
};

static ElementType CONTAINER_Type = {
    &ELEMENT_Type,
    _SIGNATURE32('C','O','N','T'),
    CONTAINER_Destroy
};

static DataElementType DATA_Type = {{   
        &ELEMENT_Type,
        0,
        SUPER_Destroy
    }, 
    NULL, 
    NULL
};

static DataElementType VOID_Type = {{
        &DATA_Type.elementType,
        _SIGNATURE32('V','O','I','D'),
        SUPER_Destroy
    },
    NULL, 
    NULL
};

static DataElementType NULL_Type = {{
        &DATA_Type.elementType,
        _SIGNATURE32('N','U','L','L'),
        SUPER_Destroy
    },
    NULL, 
    NULL
};

static DataElementType BOOL_Type = {{
        &DATA_Type.elementType,
        _SIGNATURE32('B','O','O','L'),
        SUPER_Destroy
    },
    BOOL_Parse,
    BOOL_GetText
};

static DataElementType DOUBLE_Type = {{
        &DATA_Type.elementType,
        _SIGNATURE32('D','B','L','E'),
        SUPER_Destroy
    },
    DOUBLE_Parse,
    DOUBLE_GetText
};

static DataElementType LONG_Type = {{
        &DATA_Type.elementType,
        _SIGNATURE32('L','O','N','G'),
        SUPER_Destroy
    },
    LONG_Parse,
    LONG_GetText
};

static DataElementType INT_Type = {{
        &DATA_Type.elementType,
        _SIGNATURE32('I','N','T',' '),
        SUPER_Destroy
    },
    INT_Parse,
    INT_GetText
};

static DataElementType SHORT_Type = {{
        &DATA_Type.elementType,
        _SIGNATURE32('S','H','R','T'),
        SUPER_Destroy
    },
    SHORT_Parse,
    SHORT_GetText
};

static DataElementType BYTE_Type = {{
        &DATA_Type.elementType,
        _SIGNATURE32('B','Y','T','E'),
        SUPER_Destroy
    },
    BYTE_Parse,
    BYTE_GetText
};

static DataElementType BINARY_Type = {{
        &DATA_Type.elementType,
        _SIGNATURE32('B','I','N',' '),
        BINARY_Destroy
    },
    BINARY_Parse,
    BINARY_GetText
};

static DataElementType STRING_Type = {{
        &DATA_Type.elementType,
        _SIGNATURE32('S','T','R',' '),
        STRING_Destroy
    },
    STRING_Parse,
    STRING_GetText
};

/* *************************************************************************
//
//  Forward declarations
//
// *************************************************************************/

static XRpcBinaryElement * _XRPC_IsBinaryData(const XRpcElement * e);
static XRpcStringElement * _XRPC_IsStringData(const XRpcElement * e);

/* *************************************************************************
//
//  Macros
//
// *************************************************************************/

#define AttributeAt(_elem,_i) ((XRpcAttr*)VECTOR_Get(&(_elem)->attr, _i))
#define IsContainer(_c) XRPC_IsInstance((XRpcElement*)(_c),&CONTAINER_Type)
#define IsDataElement(_e) XRPC_IsInstance((XRpcElement*)(_e),&DATA_Type.elementType)
#define IsBinaryData(_e) XRPC_IsInstance((XRpcElement*)(_e),&BINARY_Type.elementType)
#define IsStringData(_e) XRPC_IsInstance((XRpcElement*)(_e),&STRING_Type.elementType)

/* *************************************************************************
//
//  Internal functions
//
// *************************************************************************/

/**
 * Determines if the specified element is an instance of a subclass of the 
 * specified element class.
 */
static XRpcBool XRPC_IsInstance(const XRpcElement * e, const ElementType * t)
{
    if (e) {
        const ElementType * type;
        ASSERT(e->type->signature == e->signature); /* sanity check */
        for (type = e->type; type; type = type->super) {
            if (type == t) {
                return True;
            }
        }
    }
    return False;
}

/**
 * Deallocates the element.
 */
void XRPC_FreeElement(XRpcElement * e)
{
    if (e) {
        e->type->destroy(e->type,e);
        MEM_Free(e);
    }
}

/**
 * ListFree routine that deallocates Elements
 */
static void freeElement(VElement e)
{
    XRPC_FreeElement((XRpcElement*)e);
}

/**
 * Duplicate a wide character string.
 */
static XRpcUcs2 * dupWideString(const XRpcUcs2 * s)
{
    XRpcUcs2 * copy = NULL;
    if (s) {
        copy = MEM_Alloc(sizeof(s[0])*(UCS2_Len(s) + 1));
        if (copy) {
            UCS2_Cpy(copy,s);
        }
    }
    return copy;
}

/* *************************************************************************
//
//  DestroyProc functions
//
// *************************************************************************/

/*
 * Invokes the destroy fucntion of the superclass
 */
static void SUPER_Destroy(ElementType* t, XRpcElement * e)
{
    t->super->destroy(t->super, e);
}

/* 
 * These functions destroy the contents without deallocating the structure 
 * itself. Note that many structures are embedded into another.
 */
static void ELEMENT_Destroy(ElementType* t, XRpcElement * e)
{
    VECTOR_Destroy(&e->attr);
    e->type = NULL;
}

static void CONTAINER_Destroy(ElementType* t, XRpcElement * e)
{
    XRpcContainer * c = _XRPC_IsContainer(e);
    ASSERT(c);
    if (c) {
        VECTOR_Destroy(&c->kids);
        HASH_Destroy(&c->map);
        SUPER_Destroy(t, e);
    }
}

static void BINARY_Destroy(ElementType* t, XRpcElement * e)
{
    XRpcBinaryElement * bin = _XRPC_IsBinaryData(e);
    ASSERT(bin);
    if (bin) {
        if (bin->data.ptr) MEM_Free(bin->data.ptr);
        SUPER_Destroy(t, e);
    }
}

static void STRING_Destroy(ElementType* t, XRpcElement * e)
{
    XRpcStringElement * str = _XRPC_IsStringData(e);
    ASSERT(str);
    if (str) {
        if (str->utf) MEM_Free(str->utf);
        if (str->ws) MEM_Free(str->ws);
        if (str->s) MEM_Free(str->s);
        SUPER_Destroy(t, e);
    }
}

/* *************************************************************************
//
//  ParseValueProc functions
//
// *************************************************************************/

static Bool 
BOOL_Parse(DataElementType * t, XRpcDataElement * e, XRpcStringUCS2 text)
{
    XRpcBoolElement * elem = CAST(e,XRpcBoolElement,delement);
    ASSERT(e->element.type == &BOOL_Type.elementType);
    if (UCS2_Cmp(text,XRPC_True) == 0) {
        elem->b = XRpcTrue;
        return XRpcTrue;
    } else if (UCS2_Cmp(text,XRPC_False) == 0) {
        elem->b = XRpcFalse;
        return XRpcTrue;
    } else {
        char * s = XRPC_WideStringToASCII(text);
        if (s) {
            TRACE1("ERROR: conversion to XRpcBool failed: %s",s);
            XRPC_Free(s);
        }
        return XRpcFalse;
    }
}

static Bool 
DOUBLE_Parse(DataElementType * t, XRpcDataElement * e, XRpcStringUCS2 text)
{
    char * s = XRPC_WideStringToASCII(text);
    XRpcDoubleElement * elem = CAST(e,XRpcDoubleElement,delement);
    ASSERT(e->element.type == &DOUBLE_Type.elementType);
    if (s) {
        Bool ok = PARSE_Double(s, &elem->d);
        XRPC_Free(s);
        return ok;
    }
    return XRpcFalse;
}

static Bool 
LONG_Parse(DataElementType * t, XRpcDataElement * e, XRpcStringUCS2 text)
{
    char * s = XRPC_WideStringToASCII(text);
    XRpcLongElement * elem = CAST(e,XRpcLongElement,delement);
    ASSERT(e->element.type == &LONG_Type.elementType);
    if (s) {
        Bool ok = PARSE_I64(s, &elem->n, 0);
        XRPC_Free(s);
        return ok;
    }
    return XRpcFalse;
}

static Bool 
INT_Parse(DataElementType * t, XRpcDataElement * e, XRpcStringUCS2 text)
{
    char * s = XRPC_WideStringToASCII(text);
    XRpcIntElement * elem = CAST(e,XRpcIntElement,delement);
    ASSERT(e->element.type == &INT_Type.elementType);
    if (s) {
        Bool ok = PARSE_I32(s, &elem->n, 0);
        XRPC_Free(s);
        return ok;
    }
    return XRpcFalse;
}

static Bool 
SHORT_Parse(DataElementType * t, XRpcDataElement * e, XRpcStringUCS2 text)
{
    char * s = XRPC_WideStringToASCII(text);
    XRpcShortElement * elem = CAST(e,XRpcShortElement,delement);
    ASSERT(e->element.type == &SHORT_Type.elementType);
    if (s) {
        Bool ok = PARSE_I16(s, &elem->n, 0);
        XRPC_Free(s);
        return ok;
    }
    return XRpcFalse;
}

static Bool 
BYTE_Parse(DataElementType * t, XRpcDataElement * e, XRpcStringUCS2 text)
{
    char * s = XRPC_WideStringToASCII(text);
    XRpcByteElement * elem = CAST(e,XRpcByteElement,delement);
    ASSERT(e->element.type == &BYTE_Type.elementType);
    if (s) {
        Bool ok = PARSE_I8(s, &elem->n, 0);
        XRPC_Free(s);
        return ok;
    }
    return XRpcFalse;
}

static Bool 
STRING_Parse(DataElementType * t, XRpcDataElement * e, XRpcStringUCS2 text)
{
    XRpcStringElement * elem = CAST(e,XRpcStringElement,delement);
    return XRPC_UpdateStringElementUCS2(elem, (XRpcUcs2*)text, -1, XRpcFalse);
}

static Bool 
BINARY_Parse(DataElementType * t, XRpcDataElement * e, XRpcStringUCS2 text)
{
    XRpcBinaryElement * elem = CAST(e,XRpcBinaryElement,delement);
    XRpcBool ok = XRpcTrue;

    /* frtee existing data if any */
    if (elem->data.ptr) XRPC_Free(elem->data.ptr);
    memset(&elem->data, 0, sizeof(elem->data));

    /* convert BASE64 to binary data */
    if (text && *text) {
        char * s = XRPC_WideStringToASCII(text);
        if (s) {
            ok = XRPC_BASE64Decode(s, &elem->data);
            XRPC_Free(s);
        }
    }
    return ok;
}

/* *************************************************************************
//
//  GetText functions
//
// *************************************************************************/

static XRpcUcs2 * 
BOOL_GetText(DataElementType * t, const XRpcDataElement * e)
{
    XRpcBoolElement * elem = CAST(e,XRpcBoolElement,delement);
    ASSERT(e->element.type == &BOOL_Type.elementType);
    return dupWideString(elem->b ? XRPC_True : XRPC_False);
}

static XRpcUcs2 * 
DOUBLE_GetText(DataElementType * t, const XRpcDataElement * e)
{
    XRpcUcs2 * ws = NULL;
    StrBuf64 buf;
    XRpcDoubleElement * elem = CAST(e,XRpcDoubleElement,delement);
    ASSERT(e->element.type == &DOUBLE_Type.elementType);
    STRBUF_InitBufXXX(&buf);
    if (STRBUF_Format(&buf.sb,"%g",elem->d)) {
        ws = XRPC_ASCIIToWideString(buf.sb.s);
    }
    STRBUF_Destroy(&buf.sb);
    return ws;
}

static XRpcUcs2 * 
LONG_GetText(DataElementType * t, const XRpcDataElement * e)
{
    char buf[24];
    XRpcLongElement * elem = CAST(e,XRpcLongElement,delement);
    ASSERT(e->element.type == &LONG_Type.elementType);
    sprintf(buf,I64S_FORMAT,elem->n);
    return XRPC_ASCIIToWideString(buf);
}

static XRpcUcs2 * 
INT_GetText(DataElementType * t, const XRpcDataElement * e)
{
    char buf[16];
    XRpcIntElement * elem = CAST(e,XRpcIntElement,delement);
    ASSERT(e->element.type == &INT_Type.elementType);
    sprintf(buf,"%d",elem->n);
    return XRPC_ASCIIToWideString(buf);
}

static XRpcUcs2 * 
SHORT_GetText(DataElementType * t, const XRpcDataElement * e)
{
    char buf[16];
    XRpcShortElement * elem = CAST(e,XRpcShortElement,delement);
    ASSERT(e->element.type == &SHORT_Type.elementType);
    sprintf(buf,"%d",(int)elem->n);
    return XRPC_ASCIIToWideString(buf);
}

static XRpcUcs2 * 
BYTE_GetText(DataElementType * t, const XRpcDataElement * e)
{
    char buf[16];
    XRpcByteElement * elem = CAST(e,XRpcByteElement,delement);
    ASSERT(e->element.type == &BYTE_Type.elementType);
    sprintf(buf,"%d",(int)elem->n);
    return XRPC_ASCIIToWideString(buf);
}

static XRpcUcs2 * 
STRING_GetText(DataElementType * t, const XRpcDataElement * e)
{
    XRpcStringElement * elem = CAST(e,XRpcStringElement,delement);
    ASSERT(e->element.type == &STRING_Type.elementType);
    return dupWideString(XRPC_GetStringUCS2(elem));
}

static XRpcUcs2 * 
BINARY_GetText(DataElementType * t, const XRpcDataElement * e)
{
    XRpcBinaryElement * elem = CAST(e,XRpcBinaryElement,delement);
    char * text = XRPC_BASE64Encode(elem->data.ptr, elem->data.size);
    XRpcUcs2 * wtext = NULL;
    if (text) {
        wtext = XRPC_ASCIIToWideString(text);
        XRPC_Free(text);
    }
    return wtext;
}

/* *************************************************************************
//
//  Attributes
//
// *************************************************************************/

/**
 * Deallocates the XRpcAttr structure
 */
static void ATTR_Free(XRpcAttr* attr)
{
    if (attr) {
        if (attr->name) MEM_Free(attr->name);
        if (attr->value) MEM_Free(attr->value);
        MEM_Free(attr);
    }
}

/**
 * Creates XRpcAttr structure
 */
static XRpcAttr* ATTR_Create(XRpcString name, XRpcString value)
{
    XRpcAttr* attr = MEM_New(XRpcAttr);
    if (attr) {
        attr->name = STRING_Dup(name);
        attr->value = STRING_Dup(value);
        if (attr->name && attr->value) {
            return attr;
        }
        ATTR_Free(attr);
    }
    return NULL;
}

/**
 * ListFree routine that deallocates attributes
 */
static void freeAttr(VElement e)
{
    ATTR_Free((XRpcAttr*)e);
}

/**
 * Returns number of attributes
 */
int XRPC_AttributeCount(const XRpcElement * elem)
{
    return VECTOR_Size(&elem->attr);
}

/**
 * Returns the attribute at specified index
 */
const XRpcAttr* XRPC_GetAttributeAt(const XRpcElement * elem, int index)
{
    int n = VECTOR_Size(&elem->attr);
    ASSERT(index >= 0 && index < n);
    if (index >= 0 && index < n) {
        return AttributeAt(elem, index);
    }
    return NULL;
}

/**
 * Removes the attribute at specified index
 */
XRpcBool XRPC_RemoveAttributeAt(XRpcElement * elem, int index)
{
    int n = VECTOR_Size(&elem->attr);
    if (index >= 0 && index < n) {
        VECTOR_Remove(&elem->attr, index);
        return True;
    }
    return False; 
}

/**
 * Returns the index of the attribute with the specified name,
 * or -1 if no such attribute exists
 */
int XRPC_GetAttributeIndex(const XRpcElement * elem, XRpcString aname)
{
    int i, n = VECTOR_Size(&elem->attr);
    for (i=0; i<n; i++) {
        const XRpcAttr* attr = AttributeAt(elem, i);
        if (strcmp(aname, attr->name) == 0) {
            return i;
        }
    }
    return (-1);
}

/**
 * Returns the value of the attribute with the specified name,
 * or NULL if no such attribute exists
 */
XRpcString XRPC_GetAttribute(const XRpcElement * elem, XRpcString aname)
{
    int index = XRPC_GetAttributeIndex(elem, aname);
    if (index >= 0) {
        return AttributeAt(elem, index)->value;
    }
    return NULL;
}

/**
 * Sets the value of the attribute with the specified name. Creates the 
 * attribute if necessary. Returns True if operation has been performed 
 * successfully, False on memory allocation failure.
 */
XRpcBool XRPC_SetAttribute(XRpcElement * e, XRpcString name, XRpcString v)
{
    /* the "name" attribute is a special case */
    XRpcBool isNameAttr = (strcmp(XRPC_ATTR_NAME,name) == 0);
    XRpcAttr * attr;

    /* try to find the existing attribute */
    int index = XRPC_GetAttributeIndex(e, name);
    if (index >= 0) {
        attr = (XRpcAttr*)VECTOR_Get(&e->attr, index);
        if (strcmp(v, attr->name) != 0) {
            char* copy = STRING_Dup(v);
            if (copy) {
                MEM_Free(attr->value);
                attr->value = copy;
                if (isNameAttr) e->name = copy;
            } else {
                return False;
            }
        }
        return True;
    }

    /* need to create a new attribute */
    attr = ATTR_Create(name, v);
    if (attr) {
        int index = (isNameAttr ? 0 : VECTOR_Size(&e->attr));
        if (VECTOR_Insert(&e->attr, index, attr)) {
            if (isNameAttr) e->name = attr->value;
            return True;
        }
        ATTR_Free(attr);
    }

    /* error */
    return False;
}

/**
 * Renoves the attribute with specified name. Returns True if attribute has
 * been found and removed, False if it didn't exist.
 */
XRpcBool XRPC_RemoveAttribute(XRpcElement * elem, XRpcString name)
{
    int index = XRPC_GetAttributeIndex(elem, name);
    if (index >= 0) {
        VECTOR_Remove(&elem->attr, index);
        return True;
    }
    return False;
}

/* *************************************************************************
//
//  Element
//
// *************************************************************************/

static void 
ELEMENT_Init(XRpcElement* e, ElementType * type, XRpcTag t, XRpcDataType dt)
{
    ASSERT(type->signature);
    e->signature = type->signature;
    e->tag  = t;
    e->type = type;
    e->dataType = dt;
    e->name = NULL;
    VECTOR_Init(&e->attr, 0, NULL, freeAttr);
}

/**
 * Returns the value of the element's "name" attribute, not the tag
 */
XRpcString XRPC_GetElementName(const XRpcElement * elem)
{
    return elem->name;
}

/**
 * Returns the element's tag
 */
XRpcTag XRPC_GetElementTag(const XRpcElement * elem)
{
    return elem->tag;
}

/**
 * Returns the element's tag name
 */
XRpcString XRPC_GetElementTagName(const XRpcElement * elem)
{
    return XRPC_GetValidTagName(elem->tag);
}

/**
 * Returns the data type of the element
 */
XRpcDataType XRPC_GetDataType(const XRpcElement * elem)
{
    return elem->dataType;
}

/**
 * Sets element name.
 */
XRpcBool XRPC_SetElementName(XRpcElement * elem, XRpcString name)
{
    return XRPC_SetAttribute(elem, XRPC_ATTR_NAME, name);
}

/**
 * Returns the parent container of the element, NULL if it has no parent
 */
XRpcContainer * XRPC_GetParent(XRpcElement * element)
{
    return element->parent;
}

/* 
 * The following functions convert XRpcElement and XRpcDataElement to specific 
 * types. Each of these functions returns NULL if input parameter is NULL or 
 * if it does not point to an instance of the specific type. Each returns pointer
 * to the appropriate structure if convertion was successful.
 */

const XRpcContainer * XRPC_IsContainer(const XRpcElement * e)
{
    return _XRPC_IsContainer(e);
}

const XRpcContainer * XRPC_IsArray(const XRpcElement * e)
{
    XRpcContainer * c = _XRPC_IsContainer(e);
    return ((c && c->element.tag == XRpcTagArray) ? c : NULL);
}

const XRpcContainer * XRPC_IsStruct(const XRpcElement * e, XRpcString type)
{
    XRpcContainer * c = _XRPC_IsContainer(e);
    if (c && c->element.tag == XRpcTagStruct) {
        if (!type) {
            /* checking if this is a structure of any type */
            return c;
        } else {
            /* check if the structure type is the expected one */
            XRpcString actualType = XRPC_GetAttribute(e,XRPC_ATTR_TYPE);
            if (actualType && strcmp(actualType,type) == 0) {
                return c;
            }
        }
    }
    return NULL;
}

const XRpcDataElement * XRPC_IsDataElement(const XRpcElement * e)
{
    return _XRPC_IsDataElement(e);
}

const XRpcBoolElement * XRPC_IsBoolElement(const XRpcDataElement * elem)
{
    if (elem && XRPC_IsInstance(&elem->element,&BOOL_Type.elementType)) {
        return CAST(elem,XRpcBoolElement,delement);
    }
    return NULL;
}

const XRpcByteElement * XRPC_IsByteElement(const XRpcDataElement * elem)
{
    if (elem && XRPC_IsInstance(&elem->element,&BYTE_Type.elementType)) {
        return CAST(elem,XRpcByteElement,delement);
    }
    return NULL;
}

const XRpcShortElement * XRPC_IsShortElement(const XRpcDataElement * elem)
{
    if (elem && XRPC_IsInstance(&elem->element,&SHORT_Type.elementType)) {
        return CAST(elem,XRpcShortElement,delement);
    }
    return NULL;
}

const XRpcIntElement * XRPC_IsIntElement(const XRpcDataElement * elem)
{
    if (elem && XRPC_IsInstance(&elem->element,&INT_Type.elementType)) {
        return CAST(elem,XRpcIntElement,delement);
    }
    return NULL;
}

const XRpcLongElement * XRPC_IsLongElement(const XRpcDataElement * elem)
{
    if (elem && XRPC_IsInstance(&elem->element,&LONG_Type.elementType)) {
        return CAST(elem,XRpcLongElement,delement);
    }
    return NULL;
}

const XRpcDoubleElement * XRPC_IsDoubleElement(const XRpcDataElement * elem)
{
    if (elem && XRPC_IsInstance(&elem->element,&DOUBLE_Type.elementType)) {
        return CAST(elem,XRpcDoubleElement,delement);
    }
    return NULL;
}

const XRpcStringElement * XRPC_IsStringElement(const XRpcDataElement * elem)
{
    if (elem && XRPC_IsInstance(&elem->element,&STRING_Type.elementType)) {
        return CAST(elem,XRpcStringElement,delement);
    }
    return NULL;
}

const XRpcBinaryElement * XRPC_IsBinaryElement(const XRpcDataElement * elem)
{
    if (elem && XRPC_IsInstance(&elem->element,&BINARY_Type.elementType)) {
        return CAST(elem,XRpcBinaryElement,delement);
    }
    return NULL;
}

/**
 * These are being used internally. The differ slightly from the exported 
 * functions by the const qualifier. The intent was to minimize the number 
 * of casts necessary to eliminate all the compilation warnings.
 */
XRpcContainer * _XRPC_IsContainer(const XRpcElement * e)
{
    if (IsContainer(e)) {
        return CAST(e,XRpcContainer,element);
    }
    return NULL;
}

XRpcDataElement * _XRPC_IsDataElement(const XRpcElement * e)
{
    if (XRPC_IsInstance(e,&DATA_Type.elementType)) {
        return CAST(e,XRpcDataElement,element);
    }
    return NULL;
}

static XRpcBinaryElement * _XRPC_IsBinaryData(const XRpcElement * e)
{
    if (XRPC_IsInstance(e,&BINARY_Type.elementType)) {
        return CAST(e,XRpcBinaryElement,delement.element);
    }
    return NULL;
}

static XRpcStringElement * _XRPC_IsStringData(const XRpcElement * e)
{
    if (XRPC_IsInstance(e,&STRING_Type.elementType)) {
        return CAST(e,XRpcStringElement,delement.element);
    }
    return NULL;
}

/* *************************************************************************
//
//  Container
//
// *************************************************************************/

/**
 * Creates new container
 */
XRpcContainer * XRPC_CreateContainer(XRpcTag tag)
{
    XRpcContainer * c = MEM_New(XRpcContainer);
    if (c) {
        XRpcDataType dataType;
        switch (tag) {
        case XRpcTagStruct: 
            dataType = XRpcTypeStruct; 
            break;
        case XRpcTagArray:  
            dataType = XRpcTypeArray;  
            break; 
        default:
            dataType = XRpcTypeUnknown;
            break;
        }
        memset(c, 0, sizeof(*c));
        ELEMENT_Init(&c->element, &CONTAINER_Type, tag, dataType);
        VECTOR_Init(&c->kids, 0, NULL, freeElement);
        HASH_Init(&c->map, 0, hashCompareStringKey, stringHashProc, NULL);
    }
    return c;
}

/**
 * Convenience function that creates new container of type "struct".
 */
XRpcContainer * XRPC_CreateStruct(XRpcString type)
{
    XRpcContainer * c = XRPC_CreateContainer(XRpcTagStruct);
    if (c && type) {
        if (!XRPC_SetAttribute(&c->element, XRPC_ATTR_TYPE, type)) {
            XRPC_FreeElement(&c->element);
            c = NULL;
        }
    }
    return c;
}

/* 
 * Returns number of children elements
 */
int XRPC_ElementCount(const XRpcContainer * c)
{
    ASSERT(IsContainer(c));
    return VECTOR_Size(&c->kids);
}

/**
 * Returns element at specified position
 */
const XRpcElement * XRPC_GetElementAt(const XRpcContainer * c, int i)
{
    ASSERT(i >= 0 && i < XRPC_ElementCount(c));
    if (i >= 0 && i < VECTOR_Size(&c->kids)) {
        return (XRpcElement*)VECTOR_Get(&c->kids,i);
    }
    return NULL;
}

/**
 * Finds child element by name
 */
const XRpcElement * 
XRPC_GetElementByName(const XRpcContainer * c, XRpcString name)
{
    if (c) {
        ASSERT(IsContainer(c));
        return (XRpcElement*)HASH_Get(&c->map, name);
    }
    return NULL;
}

/**
 * Returns the index of the specified child element, or -1 if this 
 * element is not a valid child 
 */
int XRPC_GetElementIndex(const XRpcContainer * c, const XRpcElement * elem)
{
    ASSERT(IsContainer(c));
    return VECTOR_IndexOf(&c->kids, elem);
}

/**
 * Adds element to the container
 */
XRpcBool XRPC_AddElement(XRpcContainer * c, XRpcElement * elem)
{
    return XRPC_InsertElement(c, elem, XRPC_ElementCount(c));
}

/**
 * Convenience function. Adds element to the container, deallocates the 
 * element if the operation fails.
 */
XRpcBool XRPC_TryAddElement(XRpcContainer * c, XRpcElement * elem)
{
    if (c && elem) {
        if (XRPC_AddElement(c, elem)) {
            return XRpcTrue;
        }
        XRPC_FreeElement(elem);
    }
    return XRpcFalse;
}

/**
 * Inserts element at specified index. Returns True on success, False 
 * on failure (no memory, wrong index, NULL element)
 */
XRpcBool XRPC_InsertElement(XRpcContainer * c, XRpcElement * e, int index)
{
    ASSERT(IsContainer(c));
    /* ignore NULL elements */
    if (e) {
        ASSERT(!VECTOR_Contains(&c->kids, e));
        ASSERT(!e->name || !HASH_Get(&c->map, e->name));
        ASSERT(!e->parent);
        ASSERT(e != &c->element);

        if (VECTOR_Insert(&c->kids, index, e)) {
            e->parent = c;

            /* if it has no name, we are all set */
            if (!e->name) {
                return True;
            }

            /* if it does have a name, put it into the map */
            if (HASH_Put(&c->map, e->name, e)) {
                return True;
            }

            /* cannot put, no more memory */
            VECTOR_Remove(&c->kids, index);
            e->parent = NULL;
        }
    }
    return False;
}

/**
 * Removes element at specified index. Returns True if element has been 
 * deleted, False if index if out of bounds. 
 */
XRpcBool XRPC_RemoveElementAt(XRpcContainer * container, int index)
{
    if (index >= 0 && index < VECTOR_Size(&container->kids)) {
        XRpcElement * e = (XRpcElement*)VECTOR_Get(&container->kids,index);
        VERIFY(HASH_Remove(&container->map, e->name));
        VECTOR_Remove(&container->kids, index);
        return XRpcTrue;
    }
    return XRpcFalse; 
}

/**
 * Removes element at specified index but does not deallocate it. 
 * Returns the removed element, NULL if index if out of bounds. 
 */
XRpcElement * XRPC_DetachElementAt(XRpcContainer * container, int index)
{
    XRpcElement * element = NULL;
    if (index >= 0 && index < VECTOR_Size(&container->kids)) {
        VectorFree freeProc;
        element = (XRpcElement*)VECTOR_Get(&container->kids,index);
        freeProc = container->kids.free;
        container->kids.free = NULL;
        VECTOR_Remove(&container->kids, index);
        container->kids.free = freeProc;
    }
    return element; 
}

/**
 * Removes the specified element from the container but does not 
 * deallocate it. Returns True if element has been detached, False 
 * if it's not a part of the container
 */
XRpcBool XRPC_DetachElement(XRpcContainer * container, XRpcElement * elem)
{
    int pos = XRPC_GetElementIndex(container, elem);
    if (pos >= 0) {
        XRPC_DetachElementAt(container, pos);
        return XRpcTrue;
    }
    return XRpcFalse;
}

/* *************************************************************************
//
//  Adding elements to a structure
//
// *************************************************************************/

/**
 * Adds a named data element to the container of type "struct" or "params".
 * Fails if target container is an array. If operation fails, deallocates 
 * the data element
 */
STATIC
XRpcBool 
XRPC_AddDataField(
    XRpcContainer *     c, 
    XRpcString          name,
    XRpcDataElement *   data)
{
    if (XRPC_AddField(c, name, &data->element)) {
        return XRpcTrue;
    } else {
        XRPC_FreeElement(&data->element);
        return XRpcFalse;
    }
}

/**
 * Adds a named double element to the container. Fails if target container
 * is an array
 */
XRpcBool 
XRPC_API
XRPC_AddDoubleField(
    XRpcContainer * container, 
    XRpcString      name,
    XRpcDouble      data)
{
    XRpcDoubleElement * d = XRPC_CreateDoubleElement(data);
    return d? XRPC_AddDataField(container, name, &d->delement) : XRpcFalse;
}

/**
 * Adds a named long element to the container. Fails if target container
 * is an array
 */
XRpcBool 
XRPC_API
XRPC_AddLongField(
    XRpcContainer * container, 
    XRpcString      name,
    XRpcLong        data)
{
    XRpcLongElement * l = XRPC_CreateLongElement(data);
    return l? XRPC_AddDataField(container, name, &l->delement) : XRpcFalse;
}

/**
 * Adds a named int element to the container. Fails if target container
 * is an array
 */
XRpcBool 
XRPC_API
XRPC_AddIntField(
    XRpcContainer * container, 
    XRpcString      name,
    XRpcInt         data)
{
    XRpcIntElement * i = XRPC_CreateIntElement(data);
    return i? XRPC_AddDataField(container, name, &i->delement) : XRpcFalse;
}

/**
 * Adds a named short element to the container. Fails if target container
 * is an array
 */
XRpcBool 
XRPC_API
XRPC_AddShortField(
    XRpcContainer * container, 
    XRpcString      name,
    XRpcShort       data)
{
    XRpcShortElement * s = XRPC_CreateShortElement(data);
    return s? XRPC_AddDataField(container, name, &s->delement) : XRpcFalse;
}

/**
 * Adds a named byte element to the container. Fails if target container
 * is an array
 */
XRpcBool 
XRPC_API
XRPC_AddByteField(
    XRpcContainer * container, 
    XRpcString      name,
    XRpcByte        data)
{
    XRpcByteElement * b = XRPC_CreateByteElement(data);
    return b? XRPC_AddDataField(container, name, &b->delement) : XRpcFalse;
}

/**
 * Adds a named boolean element to the container. Fails if target container
 * is an array
 */
XRpcBool 
XRPC_API
XRPC_AddBoolField(
    XRpcContainer * container, 
    XRpcString      name,
    XRpcBool        data)
{
    XRpcBoolElement * b = XRPC_CreateBoolElement(data);
    return b? XRPC_AddDataField(container, name, &b->delement) : XRpcFalse;
}

/**
 * Adds a named binary element to the container. Fails if target container
 * is an array
 */
XRpcBool 
XRPC_API
XRPC_AddBinaryField(
    XRpcContainer * container, 
    XRpcString      name,
    const XRpcByte* data,
    size_t          size)
{
    XRpcBinaryElement * b = XRPC_CreateBinaryElement(data, size);
    return b? XRPC_AddDataField(container, name, &b->delement) : XRpcFalse;
}

/**
 * Adds a named string element to the container. Fails if target container
 * is an array or the data parameter is NULL
 */
XRpcBool 
XRPC_API
XRPC_AddStringField(
    XRpcContainer * container, 
    XRpcString      name,
    XRpcString      data)
{
    if (data) {
        XRpcStringElement * s = XRPC_CreateStringElement(data, -1);
        if (s) return XRPC_AddDataField(container, name, &s->delement);
    }
    return XRpcFalse;
}

/**
 * Adds a named string element to the container. Fails if target container
 * is an array or the data parameter is NULL
 */
XRpcBool 
XRPC_API
XRPC_AddStringFieldUCS2(
    XRpcContainer * container, 
    XRpcString      name,
    XRpcStringUCS2  data)
{
    if (data) {
        XRpcStringElement * s = XRPC_CreateStringElementUCS2(data, -1);
        if (s) return XRPC_AddDataField(container, name, &s->delement);
    }
    return XRpcFalse;
}

/**
 * Adds a named string element to the container. Fails if target container
 * is an array or the data parameter is NULL
 */
XRpcBool 
XRPC_API
XRPC_AddStringFieldUTF8(
    XRpcContainer * container,
    XRpcString      name,
    XRpcStringUTF8  data,
    int             nbytes)
{
    if (data) {
        XRpcStringElement * s = XRPC_CreateStringElementUTF8(data, nbytes);
        if (s) return XRPC_AddDataField(container, name, &s->delement);
    }
    return XRpcFalse;
}

/**
 * Adds a named container element to the container of type "struct" or 
 * "params". Fails if target container is an array. 
 */
XRpcBool 
XRPC_API
XRPC_AddContainerField(
    XRpcContainer *         c, 
    XRpcString              name,
    XRpcContainer *         field)
{
    return (field ? XRPC_AddField(c, name, &field->element) : XRpcFalse);
}

/**
 * Adds a named element to the container of type "struct" or "params".
 * Fails if target container is an array. 
 */
XRpcBool 
XRPC_AddField(
    XRpcContainer *     c, 
    XRpcString          name,
    XRpcElement *       field)
{
    if (field && c && 
       (c->element.tag == XRpcTagStruct || c->element.tag == XRpcTagParams) &&
       (!name || XRPC_SetElementName(field,name))) {
        return XRPC_AddElement(c, field);
    }
    return XRpcFalse;
}

/* *************************************************************************
//
//  Finding named data elements of various types in a container
//
// *************************************************************************/

const XRpcDoubleElement *
XRPC_API
XRPC_GetDoubleElementByName(
    const XRpcContainer *   container,
    XRpcString              name)
{
    const XRpcElement * e = XRPC_GetElementByName(container, name);
    if (e && XRPC_IsInstance(e,&DOUBLE_Type.elementType)) {
        return CAST(e,XRpcDoubleElement,delement.element);
    }
    return NULL;
}

const XRpcLongElement *
XRPC_API
XRPC_GetLongElementByName(
    const XRpcContainer *   container, 
    XRpcString              name)
{
    const XRpcElement * e = XRPC_GetElementByName(container, name);
    if (e && XRPC_IsInstance(e,&LONG_Type.elementType)) {
        return CAST(e,XRpcLongElement,delement.element);
    }
    return NULL;
}

const XRpcIntElement *
XRPC_API
XRPC_GetIntElementByName(
    const XRpcContainer *   container, 
    XRpcString              name)
{
    const XRpcElement * e = XRPC_GetElementByName(container, name);
    if (e && XRPC_IsInstance(e,&INT_Type.elementType)) {
        return CAST(e,XRpcIntElement,delement.element);
    }
    return NULL;
}

const XRpcShortElement *
XRPC_API
XRPC_GetShortElementByName(
    const XRpcContainer *   container, 
    XRpcString              name)
{
    const XRpcElement * e = XRPC_GetElementByName(container, name);
    if (e && XRPC_IsInstance(e,&SHORT_Type.elementType)) {
        return CAST(e,XRpcShortElement,delement.element);
    }
    return NULL;
}

const XRpcByteElement *
XRPC_API
XRPC_GetByteElementByName(
    const XRpcContainer *   container, 
    XRpcString              name)
{
    const XRpcElement * e = XRPC_GetElementByName(container, name);
    if (e && XRPC_IsInstance(e,&BYTE_Type.elementType)) {
        return CAST(e,XRpcByteElement,delement.element);
    }
    return NULL;
}

const XRpcBoolElement *
XRPC_API
XRPC_GetBoolElementByName(
    const XRpcContainer *   container, 
    XRpcString              name)
{
    const XRpcElement * e = XRPC_GetElementByName(container, name);
    if (e && XRPC_IsInstance(e,&BOOL_Type.elementType)) {
        return CAST(e,XRpcBoolElement,delement.element);
    }
    return NULL;
}

const XRpcBinaryElement *
XRPC_API
XRPC_GetBinaryElementByName(
    const XRpcContainer *   container, 
    XRpcString              name)
{
    const XRpcElement * e = XRPC_GetElementByName(container, name);
    if (e && XRPC_IsInstance(e,&BINARY_Type.elementType)) {
        return CAST(e,XRpcBinaryElement,delement.element);
    }
    return NULL;
}

const XRpcStringElement *
XRPC_API
XRPC_GetStringElementByName(
    const XRpcContainer *   container, 
    XRpcString              name)
{
    const XRpcElement * e = XRPC_GetElementByName(container, name);
    if (e && XRPC_IsInstance(e,&STRING_Type.elementType)) {
        return CAST(e,XRpcStringElement,delement.element);
    }
    return NULL;
}

const XRpcContainer *
XRPC_API
XRPC_GetArrayByName(
    const XRpcContainer *   container, 
    XRpcString              name)
{
    const XRpcElement * elem = XRPC_GetElementByName(container, name);
    return (elem ? XRPC_IsArray(elem) : NULL);
}

const XRpcContainer *
XRPC_API
XRPC_GetStructByName(
    const XRpcContainer *   container, 
    XRpcString              name,
    XRpcString              type)
{
    const XRpcElement * elem = XRPC_GetElementByName(container, name);
    return (elem ? XRPC_IsStruct(elem,type) : NULL);
}

/* *************************************************************************
//
//  Data Element
//
// *************************************************************************/

static 
void 
DATA_ELEMENT_Init(XRpcDataElement* elem, DataElementType * type, XRpcDataType t) 
{
    XRpcTag tag = XRpcTagUnknown;
    switch (t) {
    case XRpcTypeDouble: tag = XRpcTagDouble;     break;
    case XRpcTypeLong:   tag = XRpcTagLong;       break;
    case XRpcTypeInt:    tag = XRpcTagInt;        break;
    case XRpcTypeShort:  tag = XRpcTagShort;      break;
    case XRpcTypeByte:   tag = XRpcTagByte;       break;
    case XRpcTypeBool:   tag = XRpcTagBoolean;    break;
    case XRpcTypeBinary: tag = XRpcTagBase64;     break;
    case XRpcTypeString: tag = XRpcTagString;     break;
    case XRpcTypeNull:   tag = XRpcTagNull;       break;
    case XRpcTypeVoid:   tag = XRpcTagVoid;       break;
    default: ASSMSG1("DATA_ELEMENT_Init: invalid data type %d",t);
    }
    memset(elem, 0, sizeof(*elem));
    ELEMENT_Init(&elem->element, &type->elementType, tag, t);
}

/**
 * Returns the name of the data type 
 */
const char * XRPC_GetTypeName(XRpcDataType type) {
    static char unknown[32];
    static const char * names [] = {
        "XRpcTypeDouble",
        "XRpcTypeLong",
        "XRpcTypeInt",
        "XRpcTypeShort",
        "XRpcTypeByte",
        "XRpcTypeBool",
        "XRpcTypeBinary",
        "XRpcTypeString",
        "XRpcTypeStruct",
        "XRpcTypeArray",
        "XRpcTypeNull",
        "XRpcTypeVoid"
    };

    int n = COUNT(names);
    int index = (type - XRpcTypeDouble);
    ASSERT(n == (XRpcTypeUnknown-1));
    if (index >= 0 && index < n) {
        return names[index];
    } else {
        sprintf(unknown,"XRpcTypeUnknown(%d)",type);
        return unknown;
    }
}

/**
 * Returns a text representation of a data element. The returned string
 * must be deallocated with XRPC_Free()
 */
XRpcUcs2 * XRPC_GetAsText(const XRpcDataElement * elem)
{
    DataElementType * dataType = (DataElementType *)elem->element.type;
    ASSERT(XRPC_IsInstance(&elem->element, &DATA_Type.elementType));
    if (dataType->getText) {
        return (*dataType->getText)(dataType, elem);
    } else {
        return NULL;
    }
}

/**
 * Simple accessor functions
 */
XRpcBool XRPC_GetBool(const XRpcBoolElement * be)
{
    ASSERT(!be || XRPC_IsBoolElement(&be->delement));
    return be ? be->b : XRpcFalse;
}

XRpcByte XRPC_GetByte(const XRpcByteElement * num)
{
    ASSERT(!num || XRPC_IsByteElement(&num->delement));
    return num ? num->n : 0;
}

XRpcShort XRPC_GetShort(const XRpcShortElement * num)
{
    ASSERT(!num || XRPC_IsShortElement(&num->delement));
    return num ? num->n : 0;
}

XRpcInt XRPC_GetInt(const XRpcIntElement *  num)
{
    ASSERT(!num || XRPC_IsIntElement(&num->delement));
    return num ? num->n : 0;
}

XRpcLong XRPC_GetLong(const XRpcLongElement * num)
{
    ASSERT(!num || XRPC_IsLongElement(&num->delement));
    return num ? num->n : 0;
}

XRpcDouble XRPC_GetDouble(const XRpcDoubleElement * num)
{
    ASSERT(!num || XRPC_IsDoubleElement(&num->delement));
    return num ? num->d : 0;
}

XRpcString XRPC_GetString(const XRpcStringElement * str)
{
    if (str) {
        ASSERT(XRPC_IsStringElement(&str->delement));
        if (!str->s && str->len ) {
            XRpcStringElement * elem = (XRpcStringElement*)str;
            if (str->ws) {
                elem->s = XRPC_WideStringToASCII(str->ws);
                ASSERT(strlen(elem->s) == str->len);
            } else if (str->utf) {
                elem->s = XRPC_UTF8ToASCIIString(str->s,-1);
                ASSERT(strlen(elem->s) == str->len);
            } else {
                ASSERT(False);
            }
        }
        return str->s;
    } else {
        return NULL;
    }
}

XRpcStringUTF8 XRPC_GetStringUTF8(const XRpcStringElement * str)
{
    if (str) {
        ASSERT(XRPC_IsStringElement(&str->delement));
        if (!str->utf && str->len ) {
            XRpcStringElement * elem = (XRpcStringElement*)str;
            if (!str->ws && str->s) {
                elem->ws = XRPC_ASCIIToWideString(str->s);
            }
            ASSERT(str->ws);
            if (str->ws) {
                elem->utf = XRPC_WideStringToUTF8(str->ws);
                ASSERT(XRPC_LengthUTF8(elem->utf) == (int)str->len);
            }
        }
        return str->s;
    } else {
        return NULL;
    }
}

XRpcStringUCS2 XRPC_GetStringUCS2(const XRpcStringElement * str)
{
    if (str) {
        ASSERT(XRPC_IsStringElement(&str->delement));
        if (!str->ws && str->len) {
            XRpcStringElement * elem = (XRpcStringElement*)str;
            if (str->utf) {
                elem->ws = XRPC_UTF8ToWideString(str->utf,-1);
                ASSERT(UCS2_Len(elem->ws) == (int)str->len);
            } else if (str->s) {
                elem->ws = XRPC_ASCIIToWideString(str->s);
                ASSERT(UCS2_Len(elem->ws) == (int)str->len);
            } else {
                ASSERT(False);
            }
        }
        return str->ws;
    } else {
        return NULL;
    }
}

size_t XRPC_GetStringLength(const XRpcStringElement * str)
{
    ASSERT(!str || XRPC_IsStringElement(&str->delement));
    return str ? str->len : 0;
}

const XRpcByte * XRPC_GetBinaryData(const XRpcBinaryElement * bin)
{
    ASSERT(!bin || XRPC_IsBinaryElement(&bin->delement));
    return bin ? bin->data.ptr : NULL;
}

size_t XRPC_GetBinaryDataSize(const XRpcBinaryElement * bin)
{
    ASSERT(!bin || XRPC_IsBinaryElement(&bin->delement));
    return bin ? bin->data.size : 0;
}

/* *************************************************************************
//
//  Create Data Element
//
// These functions (of course!) return NULL if memory allocation fails, 
// otherwise they return the created element
//
// *************************************************************************/

XRpcVoidElement * XRPC_CreateVoidElement()
{
    XRpcVoidElement * elem = MEM_New(XRpcVoidElement);
    if (elem) {
        memset(elem, 0, sizeof(*elem));
        DATA_ELEMENT_Init(elem, &VOID_Type, XRpcTypeVoid); 
    }
    return elem;
}

XRpcNullElement * XRPC_CreateNullElement()
{
    XRpcNullElement * elem = MEM_New(XRpcNullElement);
    if (elem) {
        memset(elem, 0, sizeof(*elem));
        DATA_ELEMENT_Init(elem, &NULL_Type, XRpcTypeNull); 
    }
    return elem;
}

XRpcBoolElement * XRPC_CreateBoolElement(XRpcBool b)
{
    XRpcBoolElement * be = MEM_New(XRpcBoolElement);
    if (be) {
        memset(be, 0, sizeof(*be));
        DATA_ELEMENT_Init(&be->delement, &BOOL_Type, XRpcTypeBool); 
        be->b = b;
    }
    return be;
}

XRpcByteElement * XRPC_CreateByteElement(XRpcByte n)
{
    XRpcByteElement * num = MEM_New(XRpcByteElement);
    if (num) {
        memset(num, 0, sizeof(*num));
        DATA_ELEMENT_Init(&num->delement, &BYTE_Type, XRpcTypeByte); 
        num->n = n;
    }
    return num;
}

XRpcShortElement * XRPC_CreateShortElement(XRpcShort n)
{
    XRpcShortElement * num = MEM_New(XRpcShortElement);
    if (num) {
        memset(num, 0, sizeof(*num));
        DATA_ELEMENT_Init(&num->delement, &SHORT_Type, XRpcTypeShort);
        num->n = n;
    }
    return num;
}

XRpcIntElement * XRPC_CreateIntElement(XRpcInt n)
{
    XRpcIntElement * num = MEM_New(XRpcIntElement);
    if (num) {
        memset(num, 0, sizeof(*num));
        DATA_ELEMENT_Init(&num->delement, &INT_Type, XRpcTypeInt);
        num->n = n;
    }
    return num;
}

XRpcLongElement * XRPC_CreateLongElement(XRpcLong n)
{
    XRpcLongElement * num = MEM_New(XRpcLongElement);
    if (num) {
        memset(num, 0, sizeof(*num));
        DATA_ELEMENT_Init(&num->delement, &LONG_Type, XRpcTypeLong);
        num->n = n;
    }
    return num;
}

XRpcDoubleElement * XRPC_CreateDoubleElement(XRpcDouble d)
{
    XRpcDoubleElement * num = MEM_New(XRpcDoubleElement);
    if (num) {
        memset(num, 0, sizeof(*num));
        DATA_ELEMENT_Init(&num->delement, &DOUBLE_Type, XRpcTypeDouble);
        num->d = d;
    }
    return num;
}

XRpcStringElement * XRPC_CreateStringElementUCS2(XRpcStringUCS2 ws, int len)
{
    XRpcStringElement * str = MEM_New(XRpcStringElement);
    if (str) {
        memset(str, 0, sizeof(*str));
        DATA_ELEMENT_Init(&str->delement, &STRING_Type, XRpcTypeString);
        if (ws) {
            if (!XRPC_UpdateStringElementUCS2(str,(XRpcUcs2*)ws,len,XRpcFalse)) {
                XRPC_FreeElement(&str->delement.element);
                str = NULL;
            }
        }
    }
    return str;
}

XRpcStringElement * 
XRPC_CreateStringElementUTF8(XRpcStringUTF8 s, int nbytes)
{
    XRpcStringElement * str = MEM_New(XRpcStringElement);
    if (str) {
        memset(str, 0, sizeof(*str));
        DATA_ELEMENT_Init(&str->delement, &STRING_Type, XRpcTypeString);
        if (s) {
            if (!XRPC_UpdateStringElementUTF8(str,(char*)s,nbytes)) {
                XRPC_FreeElement(&str->delement.element);
                str = NULL;
            }
        }
    }
    return str;
}

XRpcStringElement * XRPC_CreateStringElement(XRpcString s, int len)
{
    XRpcStringElement * str = MEM_New(XRpcStringElement);
    if (str) {
        memset(str, 0, sizeof(*str));
        DATA_ELEMENT_Init(&str->delement, &STRING_Type, XRpcTypeString);
        if (s) {
            if (!XRPC_UpdateStringElement(str, (char*)s, len, XRpcFalse)) {
                XRPC_FreeElement(&str->delement.element);
                str = NULL;
            }
        }
    }
    return str;
}

XRpcBinaryElement * 
XRPC_CreateBinaryElement(const XRpcByte * data, size_t size)
{
    XRpcBinaryElement * bin = MEM_New(XRpcBinaryElement);
    if (bin) {
        memset(bin, 0, sizeof(*bin));
        DATA_ELEMENT_Init(&bin->delement, &BINARY_Type, XRpcTypeBinary);
        if (data) {
            bin->data.size = size;
            bin->data.ptr = MEM_NewArray(XRpcByte, size);
            if (bin->data.ptr) {
                memcpy(bin->data.ptr, data, size);
            } else {
                XRPC_FreeElement(&bin->delement.element);
                bin = NULL;
            }
        }
    }
    return bin;
}

/* *************************************************************************
//
//  Update Data Element
//
// XRPC_UpdateBinaryElement returns False if 'keep' parameter is False 
// *and* memory allocation fails. If 'keep'  parameter is True, this 
// function always returns True
//
// *************************************************************************/

XRpcBool XRPC_SetAsText(XRpcDataElement * elem, XRpcStringUCS2 text)
{
    DataElementType * dataType = (DataElementType *)elem->element.type;
    ASSERT(XRPC_IsInstance(&elem->element, &DATA_Type.elementType));
    if (dataType->parse) {
        return (*dataType->parse)(dataType, elem, text);
    } else {
        return XRpcFalse;
    }
}

void XRPC_UpdateBoolElement(XRpcBoolElement * elem, XRpcBool b)
{
    ASSERT(elem->delement.element.type == &BOOL_Type.elementType);
    elem->b = b;
}

void XRPC_UpdateByteElement(XRpcByteElement * elem, XRpcByte n)
{
    ASSERT(elem->delement.element.type == &BYTE_Type.elementType);
    elem->n = n;
}

void XRPC_UpdateShortElement(XRpcShortElement * elem, XRpcShort n)
{
    ASSERT(elem->delement.element.type == &SHORT_Type.elementType);
    elem->n = n;
}

void XRPC_UpdateIntElement(XRpcIntElement * elem, XRpcInt n)
{
    ASSERT(elem->delement.element.type == &INT_Type.elementType);
    elem->n = n;
}

void XRPC_UpdateLongElement(XRpcLongElement * elem, XRpcLong n)
{
    ASSERT(elem->delement.element.type == &LONG_Type.elementType);
    elem->n = n;
}

void XRPC_UpdateDoubleElement(XRpcDoubleElement * elem, XRpcDouble d)
{
    ASSERT(elem->delement.element.type == &DOUBLE_Type.elementType);
    elem->d = d;
}

static void XRPC_FreeStringData(XRpcStringElement * elem) 
{
    if (elem->utf) {
        MEM_Free(elem->utf);
        elem->utf = NULL;
    }
    if (elem->ws) {
        MEM_Free(elem->ws);
        elem->ws = NULL;
    }
    if (elem->s) {
        MEM_Free(elem->s);
        elem->s = NULL;
    }
}

XRpcBool
XRPC_UpdateStringElement(
    XRpcStringElement *     elem,
    char *                  s,
    int                     len,
    XRpcBool                keep) 
{
    char * data;
    ASSERT(elem->delement.element.type == &STRING_Type.elementType);

    /* calculate the length if necessary */
    if (len < 0 && s) {
        len = strlen(s);
    }

    /* 
     * Validate the string. Note that invalid XML characters can still be 
     * encoded by the use of character references, but that may break the
     * parser on the other end of the connection. It sure does break the
     * Java XML parser.
     */
    if (XRPC_IsValidXMLString(s, len)) {
        /* if 'keep' is True, then we don't have to copy the string */
        if (keep) {
            data = s;
            ASSERT(len == (int)strlen(s));
        } else {
            data = MEM_NewArray(char,len + 1);
            if (data) {
                strncpy(data, s, len);
                data[len] = 0;
            }
        }
    } else {
        ASSMSG("Invalid XML string");
        return XRpcFalse;
    }

    if (data) {

        /* deallocate the old stuff */
        XRPC_FreeStringData(elem);

        /* keep the new stuff */
        elem->s = data;
        elem->len = len;
        return XRpcTrue;
    } else {
        return XRpcFalse;
    }
}

XRpcBool
XRPC_UpdateStringElementUCS2(
    XRpcStringElement *     elem,
    XRpcUcs2 *              ws,
    int                     len,
    XRpcBool                keep) 
{
    XRpcUcs2 * data;
    ASSERT(elem->delement.element.type == &STRING_Type.elementType);

    /* calculate the length if necessary */
    if (len < 0 && ws) {
        len = UCS2_Len(ws);
    }

    /* 
     * Validate the string. Note that invalid XML characters can still be 
     * encoded by the use of character references, but that may break the
     * parser on the other end of the connection. It sure does break the
     * Java XML parser.
     */
    if (XRPC_IsValidXMLStringUCS2(ws, len)) {
        /* if 'keep' is True, then we don't have to copy the string */
        if (keep) {
            data = ws;
            ASSERT(len == (int)UCS2_Len(ws));
        } else {
            data = MEM_NewArray(XRpcUcs2,len + 1);
            if (data) {
                UCS2_Ncpy(data, ws, len);
                data[len] = 0;
            }
        }
    } else {
        ASSMSG("Invalid XML string");
        return XRpcFalse;
    }

    if (data) {

        /* deallocate the old stuff */
        XRPC_FreeStringData(elem);

        /* store the new stuff */
        elem->ws = data;
        elem->len = len;

        return XRpcTrue;
    } else {
        return XRpcFalse;
    }
}

/*
 * Note: nbytes parameter specifies number of bytes (not characters!)
 * in the UTF8 buffer. It can be -1 if the buffer is NULL terminated.
 */
XRpcBool
XRPC_UpdateStringElementUTF8(
    XRpcStringElement *     elem,
    XRpcStringUTF8          utf,
    int                     nbytes) 
{
    ASSERT(elem->delement.element.type == &STRING_Type.elementType);

    /* calculate number of bytes if necessary */
    if (nbytes < 0 && utf) nbytes = strlen(utf);

    /* 
     * Validate the string. Note that invalid XML characters can still be 
     * encoded by the use of character references, but that may break the
     * parser on the other end of the connection. It sure does break the
     * Java XML parser.
     */
    if (XRPC_IsValidXMLStringUTF8(utf, nbytes)) {
        char * data = MEM_NewArray(char,nbytes+1);
        if (data) {
            int len;

            /* copy the data, make it NULL terminated */
            memcpy(data, utf, nbytes);
            data[nbytes] = 0;

            /* calculate the length and test UTF8 string is valid */
            len = XRPC_LengthUTF8(data);
            ASSERT(len >= 0);
            if (len >= 0) {

                /* deallocate the old stuff */
                XRPC_FreeStringData(elem);

                /* keep the new stuff */
                elem->utf = data;
                elem->len = len;
                return XRpcTrue;
            }

            /* cleanup */
            MEM_Free(data);
        }
    }

    return XRpcFalse;
}

XRpcBool
XRPC_UpdateBinaryElement(
    XRpcBinaryElement *     elem,
    XRpcByte *              b,
    size_t                  size,
    XRpcBool                keep) {

    XRpcByte * data = NULL;
    ASSERT(elem->delement.element.type == &BINARY_Type.elementType);

    /* if 'keep' is True, then we don't have to copy the data */
    if (keep) {
        data = b;
    } else if (size) {
        ASSERT(b);
        ASSERT(size > 0);
        data = MEM_NewArray(XRpcByte,size);
        if (data) {
            memcpy(data, b, size);
        }
    }

    if (data || !size) {

        /* deallocate the old stuff */
        if (elem->data.ptr) {
            MEM_Free(elem->data.ptr);
        }

        /* store the new stuff */
        elem->data.ptr = data;
        elem->data.size = size;

        return XRpcTrue;
    } else {
        return XRpcFalse;
    }
}

/* *************************************************************************
//
//  Debug functions
//
//  Normally, these shouldn't be called in release build (see macros in 
//  xrpc.h), but we still have to keep them around in case if someone
//  links debug code with release version of XRPC library
//
// *************************************************************************/

#undef XRPC_ContainerToElement
#undef XRPC_FreeContainer

XRpcElement * XRPC_API XRPC_ContainerToElement(XRpcContainer * container)
{
    if (container) {
        ASSERT(IsContainer(container));
        return &container->element;
    }
    return NULL;
}

void XRPC_API XRPC_FreeContainer(XRpcContainer * container)
{
    if (container) {
        ASSERT(IsContainer(container));
        XRPC_FreeElement(&container->element);
    }
}

/*
 * Local Variables:
 * c-basic-offset:4
 * indent-tabs-mode: nil
 * End:
 */
