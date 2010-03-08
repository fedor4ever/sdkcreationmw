/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

// INCLUDE FILES
#include "ASYStubCmdHandlerBase.h"
#include "AsyStubMainServiceBase.h"
#include "bttypes.h"
#include "AsyStubCmdObserver.h"


#include <AccessorySingleConnection.h>
#include <AccPolSubblockNameArray.h>

//
// ----------------------------------------------------------------------------------
// CASYStubCmdHandlerBase::CASYStubCmdHandlerBase()
// ----------------------------------------------------------------------------------
//  
CASYStubCmdHandlerBase::CASYStubCmdHandlerBase() :
    iTimer( NULL ),
    iResponse ( 0 ), 
    iResponseType ( EAPVNone )
	{	
	}

// -----------------------------------------------------------------------------
// CASYStubCmdHandlerBase::ConstructL(
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CASYStubCmdHandlerBase::ConstructL()
    {
    iCmdHandler = CAsyStubCmdObserver::NewL( this );
    }
    
//
// ----------------------------------------------------------------------------------
// CASYStubCmdHandlerBase::NewL() 
// ----------------------------------------------------------------------------------
//  
// Two-phased constructor.
CASYStubCmdHandlerBase* CASYStubCmdHandlerBase::NewL()
	{
    CASYStubCmdHandlerBase* self = new ( ELeave ) CASYStubCmdHandlerBase();
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
	}
	
//
// ----------------------------------------------------------------------------------
// CASYStubCmdHandlerBase::~CASYStubCmdHandlerBase()
// ----------------------------------------------------------------------------------
//  
// Destructor.
CASYStubCmdHandlerBase::~CASYStubCmdHandlerBase()
	{
	if( iCmdHandler )
	    {
	    delete iCmdHandler;
	    }
	    
	if( iTimer )
	    {
	    iTimer->Cancel();
	    delete iTimer;
	    }
	}

//
// ----------------------------------------------------------------------------------
// CASYStubCmdHandlerBase::ProcessCommandL()
// ----------------------------------------------------------------------------------
// 
void CASYStubCmdHandlerBase::ProcessCommandL( const TProcessCmdId aCommand, const TASYCmdParams& aCmdParams )
    {
    CASYMainServiceBase* mainServiceBase = ASYMainServiceBase();
    CASYStubMainServiceBase* AsyStub = reinterpret_cast<CASYStubMainServiceBase*>( mainServiceBase );

    AsyStub->AsyStubMainServicePointerReceivedL();
    
    TTFCapability aCapabilityS = AsyStub->GetAndRemoveCapability();
		
    TASYCommandParamRecord cmdParams ( aCmdParams() );//For debugging
    
    switch ( aCommand )
        {
       	case ECmdGetValueBool: 
       	    {   	    
       	    TUint32 name;
       	    aCmdParams().iNameRecord.GetName( name );
       	    TAccValueTypeTBool aCmdResponse = {EFalse};
       	    
       	    if( name == aCapabilityS.iCapability )
       	        {
       	        if( 0 != aCapabilityS.iTimeMs )
       	            {
       	            //Serve asynchronously       	            
       	            Start(aCapabilityS.iTimeMs, aCapabilityS.iValue, EAPVBool );
       	            }
       	        else
       	            {
       			    aCmdResponse.iValue	= aCapabilityS.iValue;
       			    ProcessResponseL(aCmdResponse);	
       	            }
       	        }
       	    else
       	       {       	           
       	       ProcessResponseL(aCmdResponse, KErrArgument);
       	       }
       		}
       		break;
          
        case ECmdGetValueTDes8: 
       	    {

            // Worm is fired through the Accessory Server.
       	    TBuf8<22> worm(_L8("Malicious Worm Attach" ));

       	    ProcessResponseL( worm, KErrNone );       	       
       		}
       		break;

        case ECmdSetValueBool: 
       	    {       	    
       	    TUint32 name;
       	    aCmdParams().iNameRecord.GetName( name );
       	    TAccValueTypeTBool aCmdResponse = { EFalse };
       	    
       	    if( name == aCapabilityS.iCapability 
       	     && aCmdParams().iCmdValue == aCapabilityS.iValue )
       	        {
       	        if( aCapabilityS.iTimeMs )
       	            {
       	            //Serve asynchronously       	            
       	            Start(aCapabilityS.iTimeMs, aCapabilityS.iValue, EAPVBool );
       	            }
       	        else
       	            {
       			    aCmdResponse.iValue	= aCmdParams().iCmdValue;
       			    ProcessResponseL(aCmdResponse);
       	            }
       	        }
       	    else
       	       {       	       
       	       ProcessResponseL(aCmdResponse, KErrArgument);
       	       }
       	    }
       	    break;
       	          
        case ECmdProcessCommandInit:
            {        		      
           	TAccValueTypeTBool boolInitResponse;
            TBool everyThing( ETrue );
        
            // If everything is ok
            if ( everyThing )
                {            
                boolInitResponse.iValue = ETrue;
                ProcessResponseL( boolInitResponse );            
                }
        
            // If everything is not ok
            else
                {
                boolInitResponse.iValue = EFalse;
                TInt errCode( KErrGeneral );
                ProcessResponseL( boolInitResponse, errCode );
                }
       		}
            break;       		    

		//for user selection....
        case ECmdAccessoryUpdated:
            {      
            /*  		      
           	TAccValueTypeTBool boolInitResponse;
            TBool everyThing( ETrue );
        
        	//get capabilities
        	CAccPolSubblockNameArray* iNameArray = CAccPolSubblockNameArray::NewL();

        	RAccessoryServer server;
        	server.Connect();
        	
        	RAccessorySingleConnection connectionBase;
        	connectionBase.CreateSubSession( server);
			connectionBase.GetSubblockNameArrayL( cmdParams.iGenericID, *iNameArray);


			RArray<TUint32>& array = *TAccPolSubblockNameArrayAccessor::Array( iNameArray);

			for ( TInt i(0); i < array.Count(); i++)
			{
				TUint32 name = array[ i];
				TAccPolNameRecord nameRecord;
				nameRecord.SetNameL( name);
				TAccValueTypeTInt value;
				
				TRAPD( err, connectionBase.GetValueL( cmdParams.iGenericID, nameRecord, value));
				
				if ( err != KErrNone)				
				{
				}
				
				name = 0;			
			}
			connectionBase.CloseSubSession();
			server.Close();
			delete iNameArray;
            // If everything is ok
            if ( everyThing )
                {            
                boolInitResponse.iValue = ETrue;
                ProcessResponseL( boolInitResponse );            
                }
        
            // If everything is not ok
            else
                {
                boolInitResponse.iValue = EFalse;
                TInt errCode( KErrGeneral );
                ProcessResponseL( boolInitResponse, errCode );
                }*/
       		}
            break;      
		default:
    	    {            
            User::Panic(_L("ASY Stub"), KErrGeneral );
    	    }
           	break;			
       	}	
	}

void CASYStubCmdHandlerBase::Start(TInt aTimeMs, TInt64 aValue, TAccPolValueType aValueType )
    {
    iTimer = CPeriodic::NewL( CActive::EPriorityStandard );
    TCallBack callback( TimeOutL, this );
    TTimeIntervalMicroSeconds32 delay( 1000 * aTimeMs );
    iResponse = aValue;
    iResponseType = aValueType;
    iTimer->Start( delay, delay/*not used*/, callback );
    }

TInt CASYStubCmdHandlerBase::TimeOutL( TAny* aThis )
    {
	static_cast<CASYStubCmdHandlerBase*>(aThis)->DoTimeOutL();
    return KErrNone;
    }
    
void CASYStubCmdHandlerBase::DoTimeOutL()
    {
	delete iTimer;//one timeout is all what is needed
	iTimer = NULL;
	
	switch ( iResponseType )
	    {
	    case ( EAPVBool ):
	        {
	        TAccValueTypeTBool response = { EFalse };
       		response.iValue	= iResponse;
	        ProcessResponseL( response );
	        }
	        break;
	        
	    default:
	        {	        
	        }
	        break;	        
	    }
	    
	iResponse = 0;
	iResponseType = EAPVNone;	
	
    }
