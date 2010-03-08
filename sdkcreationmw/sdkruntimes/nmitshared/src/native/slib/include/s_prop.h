/*
 * $Id: s_prop.h,v 1.15 2004/03/16 22:20:50 slava Exp $
 *
 * Copyright (C) 2000-2004 by Slava Monich
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

#ifndef _SLAVA_PROP_H_
#define _SLAVA_PROP_H_

#include "s_def.h"
#include "s_queue.h"
#include "s_hash.h"
#include "s_file.h"
#include "s_itr.h"
#include "s_xml.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

typedef struct _Prop {
    HashTable map;              /* hashtable for fast access */
    Queue contents;             /* ordered contents */
} Prop;

typedef struct _PropData {
    int    index;               /* index, typically matches line number */ 
    Char * key;                 /* key */
    Char * value;               /* value */
    Char * c;                   /* comment */
} PropData;

/* XML attributes */
extern const Str PropRootTag;       /* <props> */
extern const Str PropEntryTag;      /* <p> */
extern const Str PropKeyAttr;       /* k="key" */
extern const Str PropValueAttr;     /* v="value" */
extern const Str PropCommentAttr;   /* c="comment" */

/*
 * PropCB  - callback for use in PROP_Examine(). Returns True
 *           to continue, False to stop. The value returned 
 *           by PROP_Examine() is the last value returned by 
 *           the callback, or True if properties are empty.
 */
typedef Bool (*PropCB)P_((const PropData * entry, void * ctx));

extern Prop * PROP_Create P_((void));
extern Bool PROP_Init P_((Prop * prop));
extern void PROP_Destroy P_((Prop * prop));
extern void PROP_Delete P_((Prop * prop));

extern Bool PROP_Load P_((Prop * prop, Str fname, IODesc io));
extern Bool PROP_Read P_((Prop * prop, File * in));
extern Bool PROP_Save P_((const Prop * prop, Str fname, IODesc io));
extern Bool PROP_Write P_((const Prop * prop, File * out));
extern Bool PROP_LoadXML P_((Prop * prop, Str fname, IODesc io));
extern Bool PROP_SaveXML P_((const Prop * prop, Str fname, IODesc io));
extern Bool PROP_WriteXML P_((const Prop * prop, Str tag, File * out));
extern Bool PROP_WriteDataAsXML P_((const Prop * prop, File * out));
extern XMLTag * PROP_RootCB P_((void * context, Str tag, const XMLAttr * a));
extern Prop * PROP_FromTag P_((XMLTag * tag));

extern Str  PROP_Get P_((const Prop * prop, Str key));
extern Bool PROP_GetInt P_((const Prop * prop, Str key, int * n));
extern Bool PROP_GetUInt P_((const Prop * prop, Str key, unsigned int * n));
extern Bool PROP_GetLong P_((const Prop * prop, Str key, long * n));
extern Bool PROP_GetULong P_((const Prop * prop, Str key, unsigned long * n));
extern Bool PROP_GetBool P_((const Prop * prop, Str key, Bool * b));
extern Bool PROP_GetIntBool P_((const Prop * prop, Str key, int * b));
extern Bool PROP_GetDouble P_((const Prop * prop, Str key, double * d));
extern Str  PROP_GetComment P_((const Prop * prop, Str key));

extern Bool PROP_Set P_((Prop * prop, Str key, Str value));
extern Bool PROP_SetInt P_((Prop * prop, Str key, int value));
extern Bool PROP_SetUInt P_((Prop * prop, Str key, unsigned int value));
extern Bool PROP_SetLong P_((Prop * prop, Str key, long value));
extern Bool PROP_SetULong P_((Prop * prop, Str key, unsigned long value));
extern Bool PROP_SetBool P_((Prop * prop, Str key, Bool value));
extern Bool PROP_SetDouble P_((Prop * prop, Str key, double value));
extern Bool PROP_SetComment P_((Prop * prop, Str key, Str com));

extern Bool PROP_Comment P_((Prop * prop, Str com));

#ifdef __LONG_64__
#  define PROP_GetLong64 PROP_GetLong
#  define PROP_SetLong64 PROP_SetLong
#else  /* !__LONG_64__ */
extern Bool PROP_GetLong64 P_((const Prop * prop, Str key, I64s * n));
extern Bool PROP_SetLong64 P_((Prop * prop, Str key, I64s n));
#endif /* !__LONG_64__ */

extern long PROP_Size P_((const Prop * prop));
extern Bool PROP_Remove P_((Prop * prop, Str key));
extern Bool PROP_Examine P_((const Prop * prop, PropCB cb, void * ctx));
extern Bool PROP_ExamineAll P_((const Prop * prop, PropCB cb, void * ctx));
extern void PROP_Clear P_((Prop * prop));
extern Bool PROP_Copy P_((Prop * dest, const Prop * src));
extern Bool PROP_CopyAll P_((Prop * dest, const Prop * src));
extern void PROP_Merge P_((Prop * dest, Str prefix, const Prop * src));
extern Bool PROP_Extract P_((Prop * dest, Str s1, Str s2, const Prop * src));
extern void PROP_Move P_((Prop * dest, Prop * src));

/* iterators */
extern Iterator * PROP_Keys P_((Prop * prop)); 
extern Iterator * PROP_Values P_((Prop * prop));
extern Iterator * PROP_Data P_((Prop * prop));

#ifdef __cplusplus
} /* end of extern "C" */
#endif  /* __cplusplus */

#endif /* _SLAVA_PROP_H_ */
