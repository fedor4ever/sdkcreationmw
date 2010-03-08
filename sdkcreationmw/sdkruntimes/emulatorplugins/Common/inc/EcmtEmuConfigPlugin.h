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
* Description:  Emulator Configuration Plugin
*
*/



#ifndef ECMTEMUCONFIGPLUGIN_H
#define ECMTEMUCONFIGPLUGIN_H

//  INCLUDES
#include <e32base.h>
#include "EcmtPlugin.h"
#include "EcmtMessagingIf.h"
#include "EcmtCommonPluginUids.h"
#include "EcmtCommonPluginBase.h"

// CLASS DECLARATION

/**
*  ?one_line_short_description.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*  @ingroup EcmtPlugins
*/
class CEcmtEmuConfigPlugin : public CEcmtCommonPluginBase
    {
    public:  // Constructors and destructor
    
	    /**
	    * Two-phased constructor.
	    */
	    static MEcmtPlugin* NewL();
    
	    /**
	    * Destructor.
	    */
	    virtual ~CEcmtEmuConfigPlugin();

	public: // New functions
	    
	public: // Functions from base classes

        void HandleNotifyL( const CEcmtMessageEvent& aEvent );
    
    private:

	    /**
	    * C++ default constructor.
	    */
	    CEcmtEmuConfigPlugin();

	    /**
	    * By default Symbian 2nd phase constructor is private.
	    */
	    void ConstructL();
	    void SendCurrentValuesL( );
	    void GetEpocIniFilename( TDes& aBuff ) const;
	    void GetScalableIniFilename( TPtrC8 aName, TDes& aBuff ) const;
	    void GetErrRdFilename( TDes& aBuff ) const;
	    void GetEpocWindFilename( TDes& aBuff ) const;
    };

#endif      // CECMTEMUCONFIGPLUGIN_H   
        
// End of File
