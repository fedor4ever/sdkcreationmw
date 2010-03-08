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
* Description:  Product information
*
*/


#ifndef _S60_SDK_PRODUCT_INFO_H_
#define _S60_SDK_PRODUCT_INFO_H_

/*
 * There are three preprocessor definitions that affect the default product
 * name (which also becomes part of the product ID, if product registration
 * is supported).
 *
 *   SDK_CPLUSPLUS            - Series60 C++ SDK
 *   SDK_MIDP                 - Series60 MIDP SDK
 *
 * If one (and no more than one) of them is defined, the appropriate suffix
 * is added to the product name. If none of them is defined, the default
 * product name is used. If more than one of them is defined, the build 
 * fails.
 */

/* The product version */
#define SDK_PRODUCT_VERSION "3.2"
#define SDK_PRODUCT_NAME_CPP  "S60 3rd Edition SDK, Feature Pack 2" /* Series60 C++ SDK */
#define SDK_PRODUCT_NAME_MIDP "S60 3rd Edition FP 2 SDK for MIDP" /* Series60 MIDP SDK */
/* Following product names are used by the AboutBox RichEdit control */
#define SDK_FULL_PRODUCT_NAME_CPP "S60 3rd Edition SDK for Symbian OS, Supporting Feature Pack 2, for C++"
#define SDK_FULL_PRODUCT_NAME_MIDP "S60 3rd Edition SDK for Symbian OS, Supporting Feature Pack 2, for MIDP"

#if defined(SDK_CPLUSPLUS) && !defined(SDK_MIDP)
#  define SDK_PRODUCT_NAME      SDK_PRODUCT_NAME_CPP
#elif !defined(SDK_CPLUSPLUS) && defined(SDK_MIDP)
#  define SDK_PRODUCT_NAME      SDK_PRODUCT_NAME_MIDP
#elif !defined(SDK_CPLUSPLUS) && !defined(SDK_MIDP)
#  define SDK_PRODUCT_NAME      SDK_PRODUCT_NAME_CPP
#else
#  error "Only one of SDK_CPLUSPLUS or SDK_MIDP can be defined"
#endif

/* Default product ID used for registration */
#define SDK_PRODUCT_ID      SDK_PRODUCT_NAME_CPP

/* Product ids for specific products */
#define SDK_PRODUCT_ID_CPP  SDK_PRODUCT_NAME_CPP
#define SDK_PRODUCT_ID_MIDP SDK_PRODUCT_NAME_MIDP

#endif /* _S60_SDK_PRODUCT_INFO_H_ */
