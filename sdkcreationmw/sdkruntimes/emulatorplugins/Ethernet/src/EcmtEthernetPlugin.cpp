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



// INCLUDE FILES
#include "EcmtMessageEvent.h"
#include "EcmtEthernetPlugin.h"
#include "EcmtEthernetPluginUids.h"
#include "EcmtFile.h"
#include "EcmtOsUtils.h"

#include <ImplementationProxy.h>
#include <commdb.h>
#include <cdbcols.h>

// Exported proxy for instantiation method resolution
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aCount)
{
    static const TImplementationProxy ImplementationTable[] = {
        {{KETHERNETPLUGIN_IMPL_UID}, CEcmtEthernetPlugin::NewL}
    };

    aCount = sizeof(ImplementationTable)/sizeof(ImplementationTable[0]);
    return ImplementationTable;
}

/**
 * EthernetMessage
 * 
 * <ada><blank>ada-value><pro><blank><pro-value><blank> ...
 * <ip><blank><ip-value><blank><add><blank><add-value><blank><mask><blank><mask-value><blank><gw><blank><gw-value><blank> ...
 * <dns><blank><dns-value><blank><pre><blank><pre-value><blank><alt><blank><alt-value><blank> ...
 * <proxy><blank><proxy-value><blank><name><blank><name-value><blank><port><blank><port-value><blank><exc><blank><exc-value>
 * where
 * <blank> = blank character
 * <ada> = ADA
 * <ada-value> = 0 or 1 (0=no adapter run, 1=run adapter command>
 * <pro> = PRO
 * <pro-value> = 0 or 1 (0=off 1=on)
 * <ip> = IP
 * <ip-value> = 0 or 1 (1=auto 0=manual)
 * <add> = ADD
 * <add-value> = x.x.x.x (x=0-255)
 * <mask> = MASK
 * <maskvalue> = x.x.x.x (x=0-255)
 * <gw> = GW
 * <gw-value> = x.x.x.x (x=0-255)
 * <dns> = DNS
 * <dns-value> = 0 or 1 (1=auto 0=manual)
 * <pre> = PRE
 * <pre-value> = x.x.x.x (x=0-255)
 * <alt> = ALT
 * <alt-value> = x.x.x.x (x=0-255)
 * <proxy> = PROXY
 * <proxy-value> = 0 or 1 (0=off 1=on)
 * <name> = NAME
 * <name-value> = www.nokia.proxy.com (blanks are not allowed)
 * <port> = PORT
 * <port-value> = 0-65535
 * <exc> = EXC
 * <exc-value> = www.nokia.proxy.com/exception (blanks are not allowed)
 *
*/

struct TDbItem
    {
    public:
    enum Type {	EBoolean, EUint, EText, ELongText };
    const TText* iName;
    const TText8* iMsgIdent;
    const Type iType;
    inline TDbItem( const TText* aName, const TText8* aMsgIdent, const Type aType ) 
    : iName( aName ), iMsgIdent( aMsgIdent ), iType( aType ) {};
    };

const TInt KMaxMsgIdentLen = 16;
const TDbItem LanDbItems[] =
    { 
    TDbItem( LAN_IP_ADDR_FROM_SERVER,	_S8("IP"),	TDbItem::EBoolean ),
    TDbItem( LAN_IP_ADDR,				_S8("ADD"),	TDbItem::EText ),
    TDbItem( LAN_IP_NETMASK,			_S8("MASK"),TDbItem::EText ),
    TDbItem( LAN_IP_GATEWAY,			_S8("GW"),	TDbItem::EText ),
    TDbItem( LAN_IP_DNS_ADDR_FROM_SERVER,_S8("DNS"),TDbItem::EBoolean ),
    TDbItem( LAN_IP_NAME_SERVER1,		_S8("PRE"),	TDbItem::EText ),
    TDbItem( LAN_IP_NAME_SERVER2,		_S8("ALT"),	TDbItem::EText )
    };

const TDbItem ProxyDbItems[] =
    { 
    TDbItem( PROXY_USE_PROXY_SERVER,	_S8("PROXY"),TDbItem::EBoolean ),
    TDbItem( PROXY_SERVER_NAME,			_S8("NAME"), TDbItem::ELongText ),
    TDbItem( PROXY_PORT_NUMBER,			_S8("PORT"), TDbItem::EUint ),
    TDbItem( PROXY_EXCEPTIONS,			_S8("EXC"),	 TDbItem::ELongText )
    };

const TInt KMaxEthernetMsgLen = 1024;	
_LIT8( KPro, "PRO" );
_LIT8( KAda, "ADA" );
_LIT8( KPromiscuousMode, "promiscuous_mode=" );
_LIT8( KTrue, "true" );
_LIT8( KFalse, "false" );

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CEcmtEthernetPlugin::CEcmtEthernetPlugin
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CEcmtEthernetPlugin::CEcmtEthernetPlugin() :
    CEcmtCommonPluginBase( TUid::Uid( KETHERNETPLUGIN_IMPL_UID ), TUid::Uid( KJAVA_ETHERNETPLUGIN_IMPL_UID ) )
    {
    }

// -----------------------------------------------------------------------------
// CEcmtEthernetPlugin::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CEcmtEthernetPlugin::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CEcmtEthernetPlugin::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
MEcmtPlugin* CEcmtEthernetPlugin::NewL()
    {
    CEcmtEthernetPlugin* self = new( ELeave ) CEcmtEthernetPlugin;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

    
// Destructor
CEcmtEthernetPlugin::~CEcmtEthernetPlugin()
    {
    }

// -----------------------------------------------------------------------------
// CEcmtEthernetPlugin::ReadTableL
// 
// -----------------------------------------------------------------------------
//
void CEcmtEthernetPlugin::ReadTableL( CCommsDatabase* aDb, const TText* aTableName, 
                                      const TDbItem* aItems,
                                      TInt aItemCount,
                                      TDes8& aMsg  )
    {
    TBuf<KCommsDbSvrMaxColumnNameLength> name;
    name.Copy( aTableName );

    CCommsDbTableView* table = aDb->OpenTableLC( name );
    User::LeaveIfError( table->GotoFirstRecord() );
    
    for ( TInt i=0; i < aItemCount; i++ )
        {
        aMsg.Append( ' ' );
        TBuf8<KMaxMsgIdentLen> ident( aItems[i].iMsgIdent );
        aMsg.Append( ident );
        aMsg.Append( ' ' );
        
        name.Copy( aItems[i].iName );
        switch ( aItems[i].iType )
            {
            case TDbItem::EBoolean:
                {
                TBool yes;
                table->ReadBoolL( name, yes );
                aMsg.Append( yes ? '1' : '0' );
                break;
                }
            case TDbItem::EUint:
                {
                TUint32 intVal;
                table->ReadUintL( name, intVal );
                aMsg.AppendNum( intVal, EDecimal );
                break;
                }
            case TDbItem::EText:
                {
                TBuf8<KCommsDbSvrMaxFieldLength> txt;
                table->ReadTextL( name, txt );
                aMsg.Append( txt );
                break;
                }
            case TDbItem::ELongText:
                {
                HBufC* longTxt = table->ReadLongTextLC( name );
                aMsg.Append( *longTxt );
                CleanupStack::PopAndDestroy( longTxt );
                break;
                }
            }
        }
    CleanupStack::PopAndDestroy();
    }

// -----------------------------------------------------------------------------
// CEcmtEthernetPlugin::FixDhcpDaemonL
// 
// -----------------------------------------------------------------------------
void CEcmtEthernetPlugin::FixDhcpDaemonL( CCommsDbTableView* aTable, TBool aDhcpInUse )
	{
    // Ugly fix: must set ConfigDaemonName to null if fixed IP. Otherwise
    // Symbian TCP/IP stack still tries to find DHCP server
  _LIT( KLanIpAddrFromServer, "LAN_IP_ADDR_FROM_SERVER" );
	_LIT( KKeyConfigDaemonName, "ConfigDaemonName" );
	_LIT( KValConfigDaemonName, "!DhcpServ" );
	_LIT( KKeyConfigDaemonManagerName, "ConfigDaemonManagerName" );
	_LIT( KValConfigDaemonManagerName, "NetCfgExtnDhcp" );
	_LIT( KValNone, "" );
	
	if ( aDhcpInUse )
		{
		aTable->WriteTextL( KKeyConfigDaemonName, KValConfigDaemonName );
		aTable->WriteTextL( KKeyConfigDaemonManagerName, KValConfigDaemonManagerName );
		}
	else
		{
		aTable->WriteTextL( KKeyConfigDaemonName, KValNone );
		aTable->WriteTextL( KKeyConfigDaemonManagerName, KValNone );
		}
	}


// -----------------------------------------------------------------------------
// CEcmtEthernetPlugin::WriteTableL
// 
// -----------------------------------------------------------------------------
//
void CEcmtEthernetPlugin::WriteTableL( CCommsDatabase* aDb, const TText* aTableName, 
                                      const TDbItem* aItems,
                                      TInt aItemCount,
                                      TLex8& aLexer  )
    {
    TBuf<KCommsDbSvrMaxColumnNameLength> name;
    name.Copy( aTableName );
    
    CCommsDbTableView* table = aDb->OpenTableLC( name );
    TInt err = table->GotoFirstRecord();
    if ( err == KErrNotFound )
        {
        TUint32 id;
        User::LeaveIfError( table->InsertRecord( id ) );
        if ( aTableName == PROXIES )
            {
            const TPtrC KIsp = PROXY_ISP;
            const TPtrC KProtocol = PROXY_PROTOCOL_NAME;
            table->WriteUintL( KIsp, 1 );
            table->WriteTextL( KProtocol, _L("http" ) );
            }
        }
    else if ( err != KErrNone )
        {
        User::Leave( err );
        }
    else
        {    
        User::LeaveIfError( table->UpdateRecord() );
        }

    for ( TInt i=0; i < aItemCount; i++ )
        {
        TBuf8<KMaxMsgIdentLen> ident8( aItems[i].iMsgIdent );
        if ( ident8.Compare( aLexer.NextToken() ) || aLexer.Get() != ' ' )
            {
            User::Leave( 1 );
            }

        name.Copy( aItems[i].iName );
        TBuf16<KMaxEthernetMsgLen> textVal;

        if ( aLexer.Peek() != ' ' )
            {
            textVal.Copy( aLexer.NextToken() );
            }
        else
            {
            textVal.Copy( _L("0.0.0.0") );
            }
        
        switch ( aItems[i].iType )
            {
            case TDbItem::EBoolean:
                {
                table->WriteBoolL( name, textVal[0] == '1' ? ETrue : EFalse );
                // Need to fix the ConfigDaemon according to DHCP setting
				// This is only possible from SOS 8.0 onwards, so need to ifdef the code
				// for it to work on v7.0 
				#ifdef SERVICE_CONFIG_DAEMON_MANAGER_NAME
              if ( name.Compare( TPtrC( LAN_IP_ADDR_FROM_SERVER ) ) == 0 )
              	{
              	FixDhcpDaemonL( table, textVal[0] == '1' ? ETrue : EFalse );
              	}
        #endif
              	break;
                }
            case TDbItem::EUint:
                {
                TUint32 intVal;
                TLex converter( textVal );
                User::LeaveIfError( converter.Val( intVal, EDecimal ));
                table->WriteUintL( name, intVal );
                break;
                }
            case TDbItem::EText:
                {
                table->WriteTextL( name, textVal );
                break;
                }
            case TDbItem::ELongText:
                {
                table->WriteLongTextL( name, textVal );
                break;
                }
            }
        }
    User::LeaveIfError( table->PutRecordChanges() );
    CleanupStack::PopAndDestroy();
    }

// -----------------------------------------------------------------------------
// CEcmtEthernetPlugin::SendCurrentValuesL
// 
// -----------------------------------------------------------------------------
//
void CEcmtEthernetPlugin::SendCurrentValuesL( )
    {
#ifdef ECMT_RDEBUG
	RDebug::Print( _L( "EcmtEthernetPlugin::SendCurrentValuesL" ) );
#endif
    CCommsDatabase* db = CCommsDatabase::NewL( EDatabaseTypeIAP );
    CleanupStack::PushL( db );

    TBuf8<KMaxEthernetMsgLen> msg;
    
    msg.Append( KAda );
    msg.Append( ' ' );
    msg.Append( '0' );
    msg.Append( ' ' );
    msg.Append( KPro );
    msg.Append( ' ' );
    TBool val;
    GetPromiscuous( val );
    msg.Append( val ? '1' : '0' );
    
#ifdef ECMT_RDEBUG
	RDebug::Print( _L( "EcmtEthernetPlugin::SendCurrentValuesL: Reading LAN services..." ) );
#endif
    ReadTableL( db, LAN_SERVICE, LanDbItems, 
                sizeof(LanDbItems)/sizeof(LanDbItems[0]),
                msg );

#ifdef ECMT_RDEBUG
	RDebug::Print( _L( "EcmtEthernetPlugin::SendCurrentValuesL: Reading proxies..." ) );
#endif
    ReadTableL( db, PROXIES, ProxyDbItems, 
                sizeof(ProxyDbItems)/sizeof(ProxyDbItems[0]),
                msg );

    CEcmtMessageEvent* m = iMessaging->NewMessageEvent( iTargetUid, msg );
#ifdef ECMT_RDEBUG
	RDebug::Print( _L( "EcmtEthernetPlugin::SendCurrentValuesL: Sending a message" ) );
#endif
    iMessageSender->SendMessage( m );
    CleanupStack::PopAndDestroy( db );
    }

// -----------------------------------------------------------------------------
// CEcmtEthernetPlugin::WriteAllL
// 
// -----------------------------------------------------------------------------
//
void CEcmtEthernetPlugin::WriteAllL( TLex8& aLexer )
    {
    CCommsDatabase* db = CCommsDatabase::NewL( EDatabaseTypeIAP );
    CleanupStack::PushL( db );
    TInt err = db->BeginTransaction();
    if( err == KErrLocked )
    {
    	//waits 100 ms and tries again
    	User::After(100);
    	err = db->BeginTransaction();
    }
    User::LeaveIfError(err);
                    
    WriteTableL( db, LAN_SERVICE, LanDbItems, 
                sizeof(LanDbItems)/sizeof(LanDbItems[0]),
                aLexer );

	WriteTableL( db, PROXIES, ProxyDbItems, 
                sizeof(ProxyDbItems)/sizeof(ProxyDbItems[0]),
                aLexer );    
    
    User::LeaveIfError( db->CommitTransaction() );
    
    CleanupStack::PopAndDestroy( db );
    }

// -----------------------------------------------------------------------------
// CEcmtEthernetPlugin::GetPromiscuousIniFileName
// 
// -----------------------------------------------------------------------------
//
void CEcmtEthernetPlugin::GetPromiscuousIniFileName( TDes& aBuff )
    {
    aBuff.Copy( EcmtOsUtils::Epocroot() );
    _LIT( KPromiscuousIni, "epoc32\\data\\promiscuous.ini" );
    aBuff.Append( KPromiscuousIni );
    aBuff.ZeroTerminate();
    }

// -----------------------------------------------------------------------------
// CEcmtEthernetPlugin::GetEtherMacFileName
// 
// -----------------------------------------------------------------------------
//
void CEcmtEthernetPlugin::GetEtherMacFileName( TDes& aBuff )
    {
    aBuff.Copy( EcmtOsUtils::Epocroot() );
    _LIT( KPromiscuousIni, "epoc32\\data\\ethermac.dat" );
    aBuff.Append( KPromiscuousIni );
    aBuff.ZeroTerminate();
    }

// -----------------------------------------------------------------------------
// CEcmtEthernetPlugin::SetPromiscuous
// 
// -----------------------------------------------------------------------------
//
void CEcmtEthernetPlugin::SetPromiscuous( TBool aVal )
    {
    TBuf<KMaxWin32Path> buff;
    GetPromiscuousIniFileName( buff );        
    REcmtFile file( buff );
    file.Read();
    if ( !file.IsGood() )
        {
        ErrorReply( R_FILE_NOT_FOUND, buff );
        return;
        }
    
    TPtrC8 line( file.FindLine( KPromiscuousMode ) );

    if ( line.Length() == 0 )
        {
        ErrorReply( R_FILE_CORRUPTED, buff );
        return;
        }

    TBuf8< 64 > newLine;
    newLine.Append( KPromiscuousMode );
    newLine.Append( aVal ? KTrue() : KFalse() );
    newLine.Append( _L("\r\n") );
    
    if ( !file.ReplaceLine( line, newLine ) )
        {
        ErrorReply( R_INTERNAL_ERROR, KNullDesC );
        return;
        }
    if ( !file.Write() )
        {
        ErrorReply( R_FILE_WRITE_ERROR, buff );
        return;
        }
    }

// -----------------------------------------------------------------------------
// CEcmtEthernetPlugin::GetPromiscuous
// 
// -----------------------------------------------------------------------------
//
void CEcmtEthernetPlugin::GetPromiscuous( TBool& aVal )
    {
    TBuf<KMaxWin32Path> buff;
    GetPromiscuousIniFileName( buff );        
    REcmtFile file( buff );
    file.Read();
    if ( !file.IsGood() )
        {
        return;
        }
    
    TPtrC8 line( file.FindLine( KPromiscuousMode ) );

    if ( line.Length() == 0 )
        {
        return;
        }

    TLex8 lexer( line );
    while( lexer.Get() != '=' )
        {
        }

    TPtrC8 token( lexer.NextToken() );
    if ( token.CompareF( KTrue() ) == 0 )
        {
        aVal = ETrue;
        }
    else if ( token.CompareF( KFalse() ) == 0 )
        {
        aVal = EFalse;
        }
    else 
        {
        return;
        }
    }

// -----------------------------------------------------------------------------
// CEcmtEthernetPlugin::HandleNotifyL
// 
// -----------------------------------------------------------------------------
//
void CEcmtEthernetPlugin::HandleNotifyL( const CEcmtMessageEvent& aEvent )
    {
    const TPtrC8 m = iMessaging->Message( aEvent );
    TLex8 msgLexer( m );

    TInt err( 1 );
    
    TPtrC8 ada( msgLexer.NextToken() );
    TPtrC8 adaStr( msgLexer.NextToken() );
    TPtrC8 pro( msgLexer.NextToken() );
    TPtrC8 proStr( msgLexer.NextToken() );

    if ( pro != KPro || ada != KAda )
        {
		ErrorReply( R_INTERNAL_ERROR, KNullDesC );
        return;
        }

    TBool promiscuous = proStr[0] == '1' ? ETrue : EFalse;
//    SetPromiscuous( promiscuous );

     if ( adaStr[0] == '1' )
        {
        }
    else
        {
        TRAP( err, WriteAllL( msgLexer ) );
        
        if ( err != KErrNone )
        	{
       	    TBuf<16> errcode;
	        errcode.AppendNum( err );
    	    ErrorReply( R_COMMSDB_ERROR, errcode );
    	    return;
        	}
        else
        	{
            TBuf<KMaxWin32Path> buff;
	        GetEtherMacFileName( buff );
    	    REcmtFile etherMacFile( buff );
        	}
        }
    SuccessReply();
    }

//  End of File  
