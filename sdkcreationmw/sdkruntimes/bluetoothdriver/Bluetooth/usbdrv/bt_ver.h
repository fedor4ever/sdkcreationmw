/*
* Copyright (c) 2000 - 2006 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef _S60BT_V2U_VER_H_
#define _S60BT_V2U_VER_H_ 1

/* 
 * odd micro version - development build
 * even micro version - stable build
 */
#define PRODUCT_VERSION_MAJOR   3
#define PRODUCT_VERSION_MINOR   1
#define PRODUCT_VERSION_MICRO   1
#define PRODUCT_VERSION_NANO    0

#define _QUOTE(x) #x
#define QUOTE(x) _QUOTE(x) 
#define VERSION(w,x,y,z) _QUOTE(w.x.y.z) 

#define COMPANY_NAME            "Nokia"
#define PRODUCT_COPYRIGHT       "2004-2006 " COMPANY_NAME
#define FILE_DESCRIPTION        "S60 SDK Bluetooth driver"
#define PRODUCT_NAME            "S60 SDK"
#define PRODUCT_VERSION         VERSION(PRODUCT_VERSION_MAJOR,\
PRODUCT_VERSION_MINOR,PRODUCT_VERSION_MICRO,PRODUCT_VERSION_NANO)

#endif /* _S60BT_V2U_VER_H_ */
