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
* Description:  Pan plugin
*
*/



#ifndef ECMTPANPLUGIN_H
#define ECMTPANPLUGIN_H

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
class CEcmtPanPlugin : public CEcmtCommonPluginBase
    {
    public:  // Constructors and destructor

	    /**
	    * Two-phased constructor.
	    */
	    static MEcmtPlugin* NewL();

	    /**
	    * Destructor.
	    */
	    virtual ~CEcmtPanPlugin();

    public: // New functions

    public: // Functions from base classes
		void HandleNotifyL( const CEcmtMessageEvent& aEvent );

    private:

	    /**
	    * C++ default constructor.
	    */
	    CEcmtPanPlugin();

	    /**
	    * By default Symbian 2nd phase constructor is private.
	    */
	    void ConstructL();
	    void SendCurrentValuesL( );
	    void GetBtEskFilename( TDes& aBuff ) const;
	    void GetIrdaEskFilename( TDes& aBuff ) const;

    };

#endif      // CECMTPANPLUGIN_H   
        
// End of File
