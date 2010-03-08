/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef _INSTREG_H_
#define _INSTREG_H_

/*
 * Installation registry manipulation utility.
 * Version information.
 */

#define _QUOTE(x) #x
#define QUOTE(x) _QUOTE(x) 
#define VERSION(x,y,z) _QUOTE(x.y.z) 

#define COMPANY_NAME            "Nokia Corporation"
#define PRODUCT_COPYRIGHT       "2004-2005 " COMPANY_NAME
#define PRODUCT_NAME            "Installation Registry Manipulation Utility"
#define PRODUCT_VERSION_MAJOR   1
#define PRODUCT_VERSION_MINOR   0
#define PRODUCT_VERSION_MICRO   2

#endif /* _INSTREG_H_ */
