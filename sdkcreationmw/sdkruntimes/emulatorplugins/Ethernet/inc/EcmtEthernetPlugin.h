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
* Description:  Ethernet plugin
*
*/



#ifndef ECMTETHERNETPLUGIN_H
#define ECMTETHERNETPLUGIN_H

//  INCLUDES
#include <e32base.h>
#include "EcmtPlugin.h"
#include "EcmtMessagingIf.h"
#include "EcmtCommonPluginUids.h"
#include "EcmtCommonPluginBase.h"

// FORWARD DECLARATION
struct TDbItem;
class CCommsDatabase;
class CCommsDbTableView;

// CLASS DECLARATION

/**
*  ?one_line_short_description.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*  @ingroup EcmtPlugins
*/
class CEcmtEthernetPlugin : public CEcmtCommonPluginBase
    {
    public:  // Constructors and destructor
    
	    /**
	    * Two-phased constructor.
	    */
	    static MEcmtPlugin* NewL();
	    
	    /**
	    * Destructor.
	    */
	    virtual ~CEcmtEthernetPlugin();

    public: // New functions
    
    public: // Functions from base classes
		void HandleNotifyL( const CEcmtMessageEvent& aEvent );

    private:

	    /**
	    * C++ default constructor.
	    */
	    CEcmtEthernetPlugin();

	    /**
	    * By default Symbian 2nd phase constructor is private.
	    */
	    void ConstructL();
	    void ReadTableL( CCommsDatabase* aDb, const TText* aTableName, 
										  const TDbItem* aItems,
										  TInt aItemCount,
										  TDes8& aMsg  );
	    void WriteTableL( CCommsDatabase* aDb, const TText* aTableName, 
										  const TDbItem* aItems,
										  TInt aItemCount,
										  TLex8& aLexer );
		void WriteAllL( TLex8& aLexer );
		void RunAdapters( TBool aPromiscuous, TBool aPromptAlways );
	  void SendCurrentValuesL( );
	  void GetPromiscuousIniFileName( TDes& aBuff );
	  void GetEtherMacFileName( TDes& aBuff );
		void SetPromiscuous( TBool aVal );
		void GetPromiscuous( TBool& aVal );
		void FixDhcpDaemonL( CCommsDbTableView* aTable, TBool aDhcpInUse );
    
    };

#endif      // CECMTETHERNETPLUGIN_H   
        
// End of File
