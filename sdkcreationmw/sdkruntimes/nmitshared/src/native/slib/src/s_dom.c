/*
 * $Id: s_dom.c,v 1.12 2006/03/29 02:08:26 slava Exp $
 *
 * Copyright (C) 2002-2006 by Slava Monich
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions 
 * are met: 
 *
 *   1.Redistributions of source code must retain the above copyright 
 *     notice, this list of conditions and the following disclaimer. 
 *   2.Redistributions in binary form must reproduce the above copyright 
 *     notice, this list of conditions and the following disclaimer 
 *     in the documentation and/or other materials provided with the 
 *     distribution. 
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
 * IN NO EVENT SHALL THE CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) ARISING 
 * IN ANY WAY OUT OF THE USE OR INABILITY TO USE THIS SOFTWARE, EVEN 
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 *
 * The views and conclusions contained in the software and documentation 
 * are those of the authors and should not be interpreted as representing 
 * any official policies, either expressed or implied.
 */

#include "s_buf.h"
#include "s_mem.h"
#include "s_util.h"
#include "s_queue.h"
#include "s_dom.h"
#include "s_xmlp.h"

/*==========================================================================*
 *                  D O M   T R E E
 *==========================================================================*/

struct _DOMNode {
    QEntry entry;           /* entry in DOMNode->children */
    Queue children;         /* list of children */
    DOMNode * parent;       /* the parent node */
    DOMNode * root;         /* the root node */
    Char * tagName;         /* the tag name */
    wchar_t * wchars;       /* character data, NULL if none */
    int len;                /* total number of characters */
    int pos;                /* position of this node (see chunks) */
    Queue chunks;           /* list of chunks */
    XMLAttr a;              /* tag attributes */
#ifndef UNICODE
    Char * chars;           /* translated character data, NULL if none */
#endif /* UNICODE */
};

struct _DOMChunk {          /* text + following child node */
    QEntry entry;           /* entry in DOMNode->chunks */
    Char * chars;           /* translated chunk data, NULL if none */
    DOMNode * node;         /* node that terminates this chunk */
};

typedef struct _DOMTag {    /* parsing context */
    XMLTag tag;             /* required by the API */
    DOMNode * node;         /* node being created for this tag */
    Buffer buf;             /* buffer for loading character data */
} DOMTag;

/* turns QEntry pointer into DOMNode pointer */
#define DOM_CastNode(_e)        QCAST(_e,DOMNode,entry)
#define DOM_CastChunk(_e)       QCAST(_e,DOMChunk,entry)
#define DOM_NodeFromEntry(_e)   (e ? DOM_CastNode(e) : ((DOMNode*)NULL))
#define DOM_ChunkFromEntry(_e)  (e ? DOM_CastChunk(e) : ((DOMChunk*)NULL))

/**
 * Return the parent node, NULL if this is the root node
 */
DOMNode * DOM_Parent(DOMNode * node)
{
    return node->parent;
}

/**
 * Returns the first child node, NULL if none
 */
DOMNode * DOM_FirstNode(DOMNode * node)
{
    QEntry * e = QUEUE_First(&node->children);
    return DOM_NodeFromEntry(e);
}

/**
 * Returns the last child node, NULL if none
 */
DOMNode * DOM_LastNode(DOMNode * node)
{
    QEntry * e = QUEUE_Last(&node->children);
    return DOM_NodeFromEntry(e);
}

/**
 * Finds the first child node that has the specified tag
 */
DOMNode * DOM_FindFirst(DOMNode * node, Str tag)
{
    QEntry * e = QUEUE_First(&node->children);
    while (e) {
        DOMNode * child = DOM_CastNode(e);
        if (child->tagName && StrCmp(tag, child->tagName) == 0) return child;
        e = QUEUE_Next(e);
    }
    return NULL;
}

/**
 * Finds the first child node that has the specified tag
 */
DOMNode * DOM_FindLast(DOMNode * node, Str tag)
{
    QEntry * e = QUEUE_Last(&node->children);
    while (e) {
        DOMNode * child = DOM_CastNode(e);
        if (child->tagName && StrCmp(tag, child->tagName) == 0) return child;
        e = QUEUE_Prev(e);
    }
    return NULL;
}

/**
 * Returns the next sibling node, NULL if none
 */
DOMNode * DOM_Next(DOMNode * node)
{
    QEntry * e = QUEUE_Next(&node->entry);
    return DOM_NodeFromEntry(e);
}

/**
 * Returns the previous sibling node, NULL if none
 */
DOMNode * DOM_Prev(DOMNode * node)
{
    QEntry * e = QUEUE_Prev(&node->entry);
    return DOM_NodeFromEntry(e);
}

/**
 * Finds the next sibling node that has the specified tag
 */
DOMNode * DOM_FindNext(DOMNode * node, Str tag)
{
    QEntry * e = &node->entry;
    while ((e = QUEUE_Next(e)) != NULL) {
        DOMNode * next = DOM_CastNode(e);
        if (next->tagName && StrCmp(tag, next->tagName) == 0) return next;
    }
    return NULL;
}

/**
 * Finds the previous sibling node that has the specified tag
 */
DOMNode * DOM_FindPrev(DOMNode * node, Str tag)
{
    QEntry * e = &node->entry;
    while ((e = QUEUE_Prev(e)) != NULL) {
        DOMNode * next = DOM_CastNode(e);
        if (next->tagName && StrCmp(tag, next->tagName) == 0) return next;
    }
    return NULL;
}

/**
 * Returns the number of child nodes
 */
int DOM_NodeCount(const DOMNode * node)
{
    return node->children.size;
}

/**
 * Returns the character data for this node, NULL if none
 */
Str DOM_CharData(const DOMNode * node)
{
#ifdef UNICODE
    return node->wchars;
#else /* !UNICODE */
    if (!node->chars && node->len && node->wchars) {
        ((DOMNode*)node)->chars = STRING_ToMultiByte(node->wchars);
    }
    return node->chars;
#endif /* !UNICODE */
}

/**
 * Returns the tag name
 */
Str DOM_TagName(const DOMNode * node)
{
    return node->tagName;
}

/**
 * Returns the dom attributes
 */
const XMLAttr * DOM_Attr(const DOMNode * node)
{
    return &node->a;
}

/**
 * Deletes one chunk
 */
STATIC Bool DOM_DeleteChunkCB(QEntry * e, void * ctx)
{
    DOMChunk * chunk = QCAST(e,DOMChunk,entry);
    UNREF(ctx);
    QUEUE_RemoveEntry(e);
    MEM_Free(chunk->chars);
    MEM_Free(chunk);
    return True;
}

/**
 * Deletes the chunks
 */
STATIC Bool DOM_DeleteChunks(DOMNode * node)
{
    QUEUE_Examine(&node->chunks, DOM_DeleteChunkCB, NULL);
    return True;
}

/**
 * Creates the chunks if they have not been created yet
 */
STATIC Bool DOM_CreateChunks(DOMNode * node)
{
    if (QUEUE_IsEmpty(&node->chunks) && !QUEUE_IsEmpty(&node->children)) {
        Str chars = DOM_CharData(node);
        DOMNode * child = DOM_FirstNode(node);
        int lastPos = 0;

        /* chunks terminated by child nodes */
        while (child) {
            DOMChunk * chunk = MEM_New(DOMChunk);
            if (chunk) {
                memset(chunk, 0, sizeof(*chunk));
                if (child->pos > lastPos) {
                    int nchars = child->pos - lastPos;
                    chunk->chars = MEM_NewArray(Char,nchars + 1);
                    if (chunk->chars) {
                        StrnCpy(chunk->chars, chars + lastPos, nchars);
                        chunk->chars[nchars] = 0;
                    }
                }
                QUEUE_InsertTail(&node->chunks, &chunk->entry);
                lastPos = child->pos;
                chunk->node = child;
            } else {
                DOM_DeleteChunks(node);
                return False;
            }
            child = DOM_Next(child);
        }

        /* the last chunk (may also be the first one) */
        if (lastPos < node->len) {
            DOMChunk * chunk = MEM_New(DOMChunk);
            if (chunk) {
                int nchars = node->len - lastPos;
                memset(chunk, 0, sizeof(*chunk));
                chunk->chars = MEM_NewArray(Char,nchars + 1);
                if (chunk->chars) {
                    StrnCpy(chunk->chars, chars + lastPos, nchars);
                    chunk->chars[nchars] = 0;
                }
                QUEUE_InsertTail(&node->chunks, &chunk->entry);
            }
        }
    }
    return True;
}

/**
 * Creates number of chunks
 */
int DOM_ChunkCount(DOMNode * node)
{
    DOM_CreateChunks(node);
    return QUEUE_Size(&node->chunks);
}

/**
 * Returns the first chunk of text, NULL if none
 */
DOMChunk * DOM_FirstChunk(DOMNode * node)
{
    if (DOM_CreateChunks(node)) {
        QEntry * e = QUEUE_First(&node->chunks);
        return DOM_ChunkFromEntry(e);
    }
    return NULL;
}

/**
 * Returns the next chunk of text, NULL if none
 */
DOMChunk * DOM_NextChunk(DOMChunk * chunk)
{
    QEntry * e = QUEUE_Next(QUEUE_CastEntry(&chunk->entry));
    return DOM_ChunkFromEntry(e);
}

/**
 * Returns the node that terminates this chunk
 */
DOMNode * DOM_ChunkNode(DOMChunk * chunk)
{
    return chunk->node;
}

/**
 * Returns the text in this chunk
 */
Str DOM_ChunkText(const DOMChunk * chunk)
{
    return chunk->chars;
}

/*==========================================================================*
 *                  P A R S I N G
 *==========================================================================*/

/**
 * Creates a new node
 */
STATIC DOMNode * DOM_CreateNode(DOMNode * parent, Str tag, const XMLAttr * a)
{
    DOMNode * node = MEM_New(DOMNode);
    if (node) {
        memset(node, 0, sizeof(*node));
        node->tagName = STRING_Dup(tag);
        if (!tag || node->tagName) {
            if (XML_AttrCopy(&node->a, a)) {
                QUEUE_Init(&node->children);
                QUEUE_Init(&node->chunks);
                if (parent) {
                    node->parent = parent;
                    node->root = parent->root;
                    QUEUE_InsertTail(&parent->children, &node->entry);
                }
                return node;
            }
            MEM_Free(node->tagName);
        }
        MEM_Free(node);
    }
    return NULL;
}

/**
 * Deletes a DOM node
 */
void DOM_Delete(DOMNode * node)
{
    if (node) {
        QEntry * e;
        while ((e = QUEUE_RemoveTail(&node->children)) != NULL) {
            DOM_Delete(QCAST(e,DOMNode,entry));
        }
        if (node->parent) {
            DOM_DeleteChunks(node->parent);
            QUEUE_RemoveEntry(&node->entry);
        }
        MEM_Free(node->tagName);
#ifndef UNICODE
        MEM_Free(node->chars);
#endif /* UNICODE */
        MEM_Free(node->wchars);
        XML_AttrDestroy(&node->a);
        MEM_Free(node);
    }
}

/**
 * Stores character data for this tag in the temporary buffer
 */
STATIC Bool DOM_HandleChars(XMLTag * tag, const wchar_t * s, int n)
{
    DOMTag * domTag = CAST(tag,DOMTag,tag);
    return BoolValue(BUFFER_Put(&domTag->buf,s,sizeof(wchar_t)*n,False) > 0);
}

/**
 * Deallocates the tag loading context, finishes the initialization 
 * of DOMNode (copies the character data)
 */
STATIC Bool DOM_EndTag(XMLTag * tag, XMLTag * parent)
{
    DOMTag * domTag = CAST(tag,DOMTag,tag);
    DOMNode * node = domTag->node;
    int size;
    UNREF(parent);

    /* copy character data */
    size = BUFFER_Size(&domTag->buf);
    ASSERT((size%sizeof(wchar_t)) == 0);
    ASSERT(!node->wchars);
    if (size > 0) {
        int len = size/sizeof(wchar_t);
        node->wchars = MEM_NewArray(wchar_t,len+1);
        if (node->wchars) {
            node->len = len;
            memcpy(node->wchars, BUFFER_Access(&domTag->buf), size);
            node->wchars[len] = 0;
        }
    }

    /* deallocate everything except the node */
    BUFFER_Destroy(&domTag->buf);
    MEM_Free(domTag);
    return True;
}

/**
 * Helper for DOM_HandleTag and DOM_HandleRoot 
 */
STATIC XMLTag * DOM_HandleTag(XMLTag * p, Str name, const XMLAttr * a);
STATIC XMLTag * DOM_HandleTag2(DOMTag * parent, Str name, 
                               const XMLAttr * a, XMLEndTag end)
{
    DOMNode * node = DOM_CreateNode(parent ? parent->node : NULL, name, a);
    if (node) {
        DOMTag * domTag = MEM_New(DOMTag);
        if (domTag) {
            memset(domTag, 0, sizeof(*domTag));
            BUFFER_Init(&domTag->buf);
            domTag->node = node;
            domTag->tag.handleTag = DOM_HandleTag; 
            domTag->tag.endTag = end;
            if (parent) {
                node->pos = BUFFER_Size(&parent->buf)/sizeof(wchar_t);
                domTag->tag.handleChars = parent->tag.handleChars;
            } else {
                domTag->tag.handleChars = DOM_HandleChars; 
            }
            return &domTag->tag;
        }
        DOM_Delete(node);
    }
    return NULL;
}

/**
 * Handles a child tag. The root tag is handled by DOM_RootHandler
 */
STATIC XMLTag * DOM_HandleTag(XMLTag * p, Str name, const XMLAttr * a)
{
    return DOM_HandleTag2(CAST(p,DOMTag,tag), name, a, DOM_EndTag);
}

/**
 * Deallocates the tag loading context as well as the node
 */
STATIC Bool DOM_EndRoot(XMLTag * tag, XMLTag * parent)
{
    DOMNode * node = CAST(tag,DOMTag,tag)->node;
    DOM_EndTag(tag, parent);
    DOM_Delete(node);
    return True;
}

/**
 * Handles the root tag in the hierarchy. The context is ignored
 */
XMLTag * DOM_RootCB(void * context, Str tag, const XMLAttr * a)
{
    UNREF(context);
    return DOM_HandleTag2(NULL, tag, a, DOM_EndRoot);
}

/**
 * Handles the root tag in the hierarchy. The context is ignored.
 * This handler ignores the character data and only stores tags
 * and the attributes.
 */
XMLTag * DOM_TagsCB(void * context, Str tag, const XMLAttr * a)
{
    XMLTag * rootTag = DOM_HandleTag2(NULL, tag, a, DOM_EndRoot);
    UNREF(context);
    if (rootTag) rootTag->handleChars = NULL;
    return rootTag;
}

/**
 * Returns the parsed node and deallocates the tag
 */
DOMNode * DOM_FromTag(XMLTag * tag)
{
    DOMNode * node = CAST(tag,DOMTag,tag)->node;
    DOM_EndTag(tag, NULL);
    return node;
}

/**
 * Parses the stream and constructs the DOM tree
 */
DOMNode * DOM_Read(File * in)
{
    XMLTag * tag = XML_Handle(in, NULL, DOM_RootCB, NULL);
    return (tag ? DOM_FromTag(tag) : NULL);
}

/**
 * Loads DOM from from an XML file
 */
DOMNode * DOM_Load(Str file)
{
    DOMNode * root = NULL;
    File * in = FILE_Open(file, READ_TEXT_MODE, NULL);
    if (in) {
        root = DOM_Read(in);
        FILE_Close(in);
    }
    return root;
}

/**
 * Parses the stream and constructs the DOM tree. Only loads the tags
 * and its attributes, ignoring the character data.
 */
DOMNode * DOM_ReadTags(File * in)
{
    XMLTag * tag = XML_Handle(in, NULL, DOM_TagsCB, NULL);
    return (tag ? DOM_FromTag(tag) : NULL);
}

/**
 * Loads DOM from from an XML file. Only loads the tags
 * and its attributes, ignoring the character data.
 */
DOMNode * DOM_LoadTags(Str file)
{
    DOMNode * root = NULL;
    File * in = FILE_Open(file, READ_TEXT_MODE, NULL);
    if (in) {
        root = DOM_ReadTags(in);
        FILE_Close(in);
    }
    return root;
}
