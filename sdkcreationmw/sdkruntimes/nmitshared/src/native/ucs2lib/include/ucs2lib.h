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


#ifndef _UCS2_LIB_H_
#define _UCS2_LIB_H_ 1

typedef unsigned short Ucs2;            /* UCS2 character */
typedef const Ucs2 * UStr;              /* NULL-terminated UCS2 string */

extern int    UCS2_Len(const Ucs2 * str);
extern int    UCS2_Cmp(const Ucs2 * src, const Ucs2 * dst);
extern int    UCS2_CmpASCII(const Ucs2 * src, const char * dst);
extern int    UCS2_Hash(const Ucs2 * str);
extern Ucs2 * UCS2_Cpy(Ucs2 * dest, const Ucs2 * src);
extern Ucs2 * UCS2_Ncpy(Ucs2 * dest, const Ucs2 * src, int len);
extern Ucs2 * UCS2_SubStr(const Ucs2 * src, int off, int count);
extern Ucs2 * UCS2_New(int len);
extern Ucs2 * UCS2_Dup(const Ucs2 * src);   /* crashes if argument is NULL */
extern Ucs2 * UCS2_Clone(const Ucs2 * src); /* allows NULL argument */
extern void   UCS2_Free(Ucs2 * str);        /* ignores NULL argument */
extern void   UCS2_FreeASCII(char * str);   /* ignores NULL argument */
extern int    UCS2_IsSpace(Ucs2 ch);
extern Ucs2 * UCS2_URLDecode(const Ucs2 * src);
extern Ucs2 * UCS2_FromASCII(const char * src);
extern char * UCS2_ToASCII(const Ucs2 * src);
extern int    UCS2_CpyFromASCII(Ucs2 * dest, const char * src);
extern int    UCS2_CpyToASCII(char * dest, const Ucs2 * src);
extern int    UCS2_Hex(Ucs2 ch);
extern int    UCS2_Equal(const Ucs2 * str1, const Ucs2 * str2);
extern int    UCS2_EqASCII(const Ucs2 * str1, const char * str2);
extern UStr   UCS2_Chr(const Ucs2 * str, Ucs2 ch);
extern UStr   UCS2_Rchr(const Ucs2 * str, Ucs2 ch);
extern int    UCS2_IndexOf(const Ucs2 * str, Ucs2 ch);
extern int    UCS2_LastIndexOf(const Ucs2 * str, Ucs2 ch);
extern int    UCS2_StartsWith(const Ucs2 * str, const Ucs2 * substr);
extern int    UCS2_EndsWith(const Ucs2 * str, const Ucs2 * substr);

#endif /* _UCS2_LIB_H_ */

/*
 * Local Variables:
 * mode:C
 * c-basic-offset:4
 * indent-tabs-mode: nil
 * End:
 */
