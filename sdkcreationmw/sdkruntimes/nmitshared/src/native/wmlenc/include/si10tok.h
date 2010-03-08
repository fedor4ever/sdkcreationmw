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


/* Service Indication token-set / validation rules */
static char SI10TOK[] = "\
**********************************\n\
***** SECTION 1:  Tag tokens *****\n\
**********************************\n\
\n\
  Name       Code Element handler\n\
---------------------------------\n\
T si          05  default\n\
T indication  06  default\n\
T info        07  default\n\
T item        08  default\n\
\n\
*********************************************\n\
***** SECTION 2: Attribute start tokens *****\n\
*********************************************\n\
\n\
  Name       Prefix        Code    Handler\n\
------------------------------------------\n\
A action     signal-none   05      si_CDATA\n\
A action     signal-low    06      si_CDATA\n\
A action     signal-medium 07      si_CDATA\n\
A action     signal-high   08      si_CDATA\n\
A action     delete        09      si_CDATA\n\
A created    -             0a      si_DateTime\n\
A href       -             0b      si_HREF\n\
A href       http://       0c      si_HREF\n\
A href       http://www.   0d      si_HREF\n\
A href       https://      0e      si_HREF\n\
A href       https://www.  0f      si_HREF\n\
A si-expires -             10      si_DateTime\n\
A si-id      -             11      si_CDATA\n\
A class      -             12      si_CDATA\n\
\n\
*********************************************\n\
***** SECTION 3: Attribute value tokens *****\n\
*********************************************\n\
\n\
  Name  Code \n\
--------------\n\
V .com/  85\n\
V .edu/  86\n\
V .net/  87\n\
V .org/  88\n";
