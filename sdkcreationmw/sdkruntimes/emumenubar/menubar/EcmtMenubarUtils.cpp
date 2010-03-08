/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Ecmt menubar utilities
*
*/


// INCLUDE FILES

#include "EcmtWin32.h"
#include "EcmtMenubarUtils.h"


// --------------------------------------------------------------------------
// CEcmtMenubarUtils::ConvertFileNameToUrl
// Converts PC filename into local:// URL. The returned buffer has the
// requested amount of extra space. Returns NULL on error. The returned
// buffer must be deallocated by the caller with the Free function.
// --------------------------------------------------------------------------
unsigned short* 
EcmtMenubarUtils::ConvertFileNameToUrl(const unsigned short* aFileName,
                                       const char* aScheme, 
                                       int aExtraSpace)
{
    return ConvertFileNameToUrl(aFileName, -1, aScheme, aExtraSpace);
}

// --------------------------------------------------------------------------
// CEcmtMenubarUtils::ConvertFileNameToUrl
// Converts PC filename into local:// URL. The returned buffer has the
// requested amount of extra space. Returns NULL on error. The returned
// buffer must be deallocated by the caller with the Free function.
// --------------------------------------------------------------------------
unsigned short* 
EcmtMenubarUtils::ConvertFileNameToUrl(const unsigned short* aFileName,
                                       int aLength,
                                       const char* aScheme,
                                       int aExtraSpace)
{
    if (!aScheme) aScheme = LOCAL_FILE_SCHEME;
    int schemeLen = strlen(aScheme);
    TCHAR* fileUrl;
    int len = ((aLength >= 0) ? aLength : wcslen(aFileName));
    int i, size = schemeLen + 1 + len + aExtraSpace;

    // Calculate the amount of space we need for the URL
    for (i=0; i<len; i++)
    {
        TCHAR c = aFileName[i];
        if (c == '%' || isspace(c)) size += 2;
    }

    // Allocate the memory and fill the buffer
    fileUrl = (TCHAR*)Alloc(size*sizeof(TCHAR));
    if (fileUrl)
    {
        TCHAR* p = fileUrl;
        const char* s = aScheme;
        while (*s) *p++ = *s++;
        for (i=0; i<len; i++)
        {
            TCHAR c = aFileName[i];
            if (c == '\\')
            {
                *p++ = '/';
            }
            else if (c == '%' || isspace(c))
            {
                static const TCHAR hex[] = {'0','1','2','3','4','5','6','7',
                                            '8','9','A','B','C','D','E','F' };
                *p++ = '%';
                *p++ = hex[(c >> 4) & 0x0f];
                *p++ = hex[c & 0x0f];
            }
            else
            {
                *p++ = c;
            }
        }
        *p++ = 0;
    }

    return fileUrl;
}

/*
 * Local Variables:
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
