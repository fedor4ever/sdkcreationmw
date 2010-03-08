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
#include "ASYStubObserver.h"
#include "TfAccessoryTestControlTypes.h"
#include <AccConGenericID.h>
#include <AccPolProprietaryNameValuePairs.h>


// ----------------------------------------------------------------------------------
// CASYStubService::CASYStubService() 
// ----------------------------------------------------------------------------------	
CASYStubService::CASYStubService() 
	{			
	}
	
// ----------------------------------------------------------------------------------
// CASYStubService::ConstructL() 
// ----------------------------------------------------------------------------------	
void CASYStubService::ConstructL(void) 
	{					
	TInt err(KErrNone);	
	err = iAccessoryServer.Connect();                
    
	if ( err != KErrNone )
        {
        User::Leave( err );
        }
    
	err = iAccessoryControl.CreateSubSession( iAccessoryServer ); 
	
	if ( err != KErrNone )
        {
        User::Leave( err );
        }    
	}	
//
// ----------------------------------------------------------------------------------
// CASYStubService::~CASYStubService()
// ----------------------------------------------------------------------------------	
CASYStubService::~CASYStubService()
	{				  
   	    iAccessoryControl.CloseSubSession();
        iAccessoryServer.Close();   
	}
//
// ----------------------------------------------------------------------------------
// CASYStubService::Service()
// ----------------------------------------------------------------------------------
void CASYStubService::Service(  TTFASYReference aMethod, TInt64 aParam1 )
    {

		switch ( aMethod )
       	{
       	case ETFAsyConn:
       		{

       		TRequestStatus status;		   			    
       		CAccConfigFileParser* accConfigFileParser = CAccConfigFileParser::NewL( _L("Configuration file"));	
       		
       		TBuf<12> HWModelID;
   			HWModelID.Num(aParam1);

        	CAccConGenericID* genericID = CAccConGenericID::NewL();   
    		CleanupStack::PushL( genericID);	
    
       		accConfigFileParser->FindL( genericID, aParam1, HWModelID );			
			
  			iAccessoryControl.ConnectAccessory( status, genericID, EFalse);
    		
    		User::WaitForRequest(status);	
    		TInt retval = status.Int();	
			
			iGenericId = genericID->GenericID();	
			CleanupStack::PopAndDestroy( genericID);

			delete accConfigFileParser;		
		    		    	
       		}
       	break;
       	
       	case ETFAsyDisc:
       		{
       		TRequestStatus status;		   			    
			iAccessoryControl.DisconnectAccessory(status, iGenericId);
			User::WaitForRequest(status);	
    		TInt retval = status.Int();						
       		}
       	    break;
        
		default:		 
            break;			
   	    }           
    }    

//
// ----------------------------------------------------------------------------------
// CASYStubService::GetTestCaseID()
// ----------------------------------------------------------------------------------
 TTFCapability CASYStubService::GetAndRemoveCapability()
{

    TTFCapability aCapabilityS; // Jihuu

    if( iCapabilityArray.Count() != 0 )
        {
        aCapabilityS.iCapability = iCapabilityArray[0].iCapability;
        aCapabilityS.iValue  = iCapabilityArray[0].iValue;
        aCapabilityS.iTimeMs = iCapabilityArray[0].iTimeMs;
        iCapabilityArray.Remove(0);    
        }
    else
        {
        aCapabilityS.iCapability = 0;
        aCapabilityS.iValue = 0;
        aCapabilityS.iTimeMs = 0;
        }
        
   return aCapabilityS;     
}




