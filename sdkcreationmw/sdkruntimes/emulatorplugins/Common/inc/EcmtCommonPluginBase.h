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
* Description:  Abstract base for common plugins
*
*/



#ifndef ECMTCOMMONPLUGINBASE_H
#define ECMTCOMMONPLUGINBASE_H

//  INCLUDES
#include "EcmtPlugin.h"
#include "EcmtMessagingIf.h"
#include <e32std.h>
#include <EcmtCommonPlugin.rsg>
#include <f32file.h>

/**
*  Utility class, mainly for resource reading
*  @ingroup EcmtPlugins
*/
class CEcmtCommonPluginBase : public CBase, public MEcmtPlugin, public MEcmtMessageEventObserver 
    {
    public:
	    /**
	    * Destructor.
	    */
	    virtual ~CEcmtCommonPluginBase();

    public: // Functions from base classes

		/**
		* From MEcmtPlugin
		*/
		virtual void BindMessagingL( MEcmtMessaging* aMessaging );
		
		/**
		* From MecmtMessageEventObserver
		*/ 
		virtual void HandleNotifyL( const CEcmtMessageEvent& aEvent ) = 0;
    
    protected:
		virtual void SendCurrentValuesL( ) = 0;

	    /**
	    * C++ default constructor.
	    */
	    CEcmtCommonPluginBase( TUid aUid, TUid aTargetUid );
	    void ErrorReply( TInt aResourceId, const TDesC& aInfo );
	    void SuccessReply( );

    private:
	    /**
	    * By default Symbian 2nd phase constructor is private.
	    */
	    void ConstructL();
	    HBufC8* ReadResourceStringL( RFs& aFs, TInt aResourceId );

    protected:    // Data
		MEcmtMessaging* 	iMessaging;
		MEcmtMessageSender*	iMessageSender;
		TUid				iUid;
		TUid				iTargetUid;
    };

#endif      // ECMTCOMMONPLUGINBASE_H  
            
// End of File
