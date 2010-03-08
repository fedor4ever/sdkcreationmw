/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Example for developing a plugin
*
*/


#ifndef BITMAPPLUGIN_H
#define BITMAPPLUGIN_H

//  INCLUDES

#include <npupp.h>
#include <PluginAdapterInterface.h>

/**
* The initialization function of the plug-in exchanges function pointers 
* between the browser and the plug-in, allowing calls to and from the plug-in. 
* This function also initializes plug-in library global data. 
* All unimplemented functions should be set to NULL in the function table.
* For example, in the S60 and Series 90 platforms,JRI support is not 
* available. Therefore, aPpf->javaClass should be set to NULL.
* @since 3.0
* @param aPpf - allocated but uninitialized structure containing function 
*               pointers to implemented plug-in functions.
* @return     - an error code, on success returns NPERR_NO_ERROR.
*/
IMPORT_C NPError InitializeFuncs(NPPluginFuncs* aPpf);

/**
* This function has no input parameters and no return value. It is called to 
* allow the plug-in to clean up its data before the plug-in library is unloaded.
* @since 3.0
*/
IMPORT_C void NPP_Shutdown ( void );

IMPORT_C const TDesC* NPP_GetMIMEDescription ( void );

IMPORT_C NPError NPP_GetValue(
                    void* instance, 
                    NPPVariable variable, 
                    void* value );
#endif // BITMAPPLUGIN_H

// End of File

