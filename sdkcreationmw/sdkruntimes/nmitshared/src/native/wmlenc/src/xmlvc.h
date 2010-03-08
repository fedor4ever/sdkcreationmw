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
/* File    : U:\wpsvob\wmlenc\src\include\xmlvc.h
   Version : \main\6
   Date    : 17-Oct-00.12:52:04
*/

/*
 * ========================================================================
 *  Name        : xmlvc.h
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
-----------------------------------------------------------------------------

    DESCRIPTION

    This header file contains function declarations used by the 
    XML parser for validation.

-----------------------------------------------------------------------------
*/

#ifndef XMLVC_H
#define XMLVC_H

#include "xmlpars.h"
#include "xmlsymt.h"
#include "xmlmsg.h"

/*  CONSTANTS  */

/*  MACROS  */

/*  DATA TYPES  */

/*  EXTERNAL DATA STRUCTURES  */

/*  FUNCTION PROTOTYPES  */

SElement * vcAuxElement
                    (PARDATA d,
                     PString name);

SElement * vcUniqueElement
                    (PARDATA d,
                     PString name);

SElement * vcElementForAttList
                    (PARDATA d,
                     PString name);

void       vcCharData
                    (PARDATA d,
                     PString text);

void       vcAttribute
                    (PARDATA d,
                     PString name,
                     PString value);

void       vcUsedAttributes
                    (PARDATA d);


#endif  /* XMLVC_H */

/*  End of File  */
