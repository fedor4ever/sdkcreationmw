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


/* Cache Operation token-set / validation rules */
static char CO10TOK[] = "\
-----------------------------------\n\
  Name                Code  Handler\n\
-----------------------------------\n\
T co                  05    default\n\
T invalidate-object   06    default\n\
T invalidate-service  07    default\n\
-----------------------------------\n\
  Name  Prefix        Code  Handler\n\
-----------------------------------\n\
A uri   -             05    si_HREF\n\
A uri   http://       06    si_HREF\n\
A uri   http://www.   07    si_HREF\n\
A uri   https://      08    si_HREF\n\
A uri   https://www.  09    si_HREF\n\
--------------\n\
  Name  Code  \n\
--------------\n\
V .com/  85\n\
V .edu/  86\n\
V .net/  87\n\
V .org/  88\n";
