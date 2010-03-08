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



/*
 * Java bearer adapter.
 *
 */
#ifndef JAVA_BEARER_ADAPTER_H
#define JAVA_BEARER_ADAPTER_H

#ifdef __cplusplus
extern "C" { 
#endif

#include "wdp_bearer_interface.h"	/* WDP interface structure */
/*
 * java udp bearer adapter factory
 */
bearer_adapter_info *java_bearer_adapter_factory0( void );
bearer_adapter_info *java_bearer_adapter_factory1( void );
bearer_adapter_info *java_bearer_adapter_factory2( void );

#ifdef __cplusplus
}
#endif
  
#endif
