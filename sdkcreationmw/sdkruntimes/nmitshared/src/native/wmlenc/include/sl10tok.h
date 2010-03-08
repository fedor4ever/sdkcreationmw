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


/* Service Loading token-set / validation rules */
static char SL10TOK[] = "\
-------------------------------------\n\
  Name                  Code  Handler\n\
-------------------------------------\n\
T sl                    05    default\n\
-------------------------------------\n\
  Name    Prefix        Code  Handler\n\
-------------------------------------\n\
A action  execute-low   05    si_CDATA\n\
A action  execute-high  06    si_CDATA\n\
A action  cache         07    si_CDATA\n\
A href    -             08    si_HREF\n\
A href    http://       09    si_HREF\n\
A href    http://www.   0a    si_HREF\n\
A href    https://      0b    si_HREF\n\
A href    https://www.  0c    si_HREF\n\
--------------\n\
  Name  Code  \n\
--------------\n\
V .com/  85\n\
V .edu/  86\n\
V .net/  87\n\
V .org/  88\n";
