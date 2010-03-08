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
* Description:  Menubar utility finctions
*
*/


#ifndef _ECMT_MENUBAR_UTILS_H_
#define _ECMT_MENUBAR_UTILS_H_

#include "EcmtOsUtils.h"
#include "EcmtMenubarConstants.h"

/**
 * Collection of static utility functions. This file if platform-neutral.
 * It doesn't include any Symbian or Windows headers.
 */
class EcmtMenubarUtils: public EcmtOsUtils
{
public:

    /**
     * Converts PC filename into local:// URL. The returned buffer has the
     * requested amount of extra space. Returns NULL on error. The returned
     * buffer must be deallocated by the caller with EcmtOsUtils::Free
     */
    static unsigned short* ConvertFileNameToUrl(const unsigned short* aFile, 
                                                const char* aScheme,
                                                int aExtra = 0);

    /**
     * Same as the above, only the file name does not have to be NULL
     * terminated.
     */
    static unsigned short* ConvertFileNameToUrl(const unsigned short* aFile, 
                                                int aLength, 
                                                const char* aScheme,
                                                int aExtra = 0);

};
    
#endif	// _ECMT_MENUBAR_UTILS_H_

/**
 * Local Variables:
 * mode: c++
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
