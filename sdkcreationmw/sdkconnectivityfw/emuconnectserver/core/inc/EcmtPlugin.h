/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Interface class for plugins
*
*/


#ifndef MECMTPLUGIN_H
#define MECMTPLUGIN_H

//  INCLUDES
#include <e32std.h>

// FORWARD DECLARATIONS
class MEcmtMessaging;

/**
*  @defgroup EcmtPlugins Plugins
*  The plugins are responsible for implementing the system level
*  use cases.
*/


/**
*  Interface of plugins of Configuration and Monitoring Tool.
*
*  This is the absolute minimum that each plugin must implement.
*  See EcmtMessagingIf.h for the definition of the interface exposed by 
*  messaging component that most Plugins will use to communicate.
*  The messaging services are not included here since there may
*  be plugins that never send or receive any messages.
*
*  Please note also, that each plugin must fulfill the requirements of 
*  the Symbian OS ECom architecture.
*
*  @ingroup EcmtPlugins
*/

class MEcmtPlugin
    {
    public:
    	/**
    	*  Called by core to give to the plugin the interface of the
    	*  messaging services.
    	*  @param aMessaging Interface of messaging services.
    	*/
        virtual void BindMessagingL( MEcmtMessaging* aMessaging ) = 0;
        virtual ~MEcmtPlugin( ) { };
    };

#endif

