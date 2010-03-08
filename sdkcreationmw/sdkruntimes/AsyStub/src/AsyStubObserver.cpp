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
#include "TfAccessoryTestControlTypes.h"
#include "AsyStubObserver.h" 

// ----------------------------------------------------------------------------------
// CASYStubObserver* CASYStubObserver::NewL()
// ----------------------------------------------------------------------------------	

CASYStubObserver* CASYStubObserver::NewL(CASYStubService& aService)
    {
    CASYStubObserver* result = new (ELeave) CASYStubObserver(aService);
    CleanupStack::PushL(result);	
    CleanupStack::Pop();
    return result;	
    }

// ----------------------------------------------------------------------------------
// CASYStubObserver::CASYStubObserver() 
// ----------------------------------------------------------------------------------	
CASYStubObserver::CASYStubObserver(CASYStubService& aService) 
: CActive( CActive::EPriorityStandard ),iService(&aService)
	{						
	TInt retval(KErrNone);
	retval = RProperty::Define(KTFAccessoryTestProperty, KTFAccessoryMethod, RProperty::EByteArray);    
    if(retval != KErrAlreadyExists)
    	User::LeaveIfError(retval);       
    
    retval =  iSubscribe.Attach(KTFAccessoryTestProperty, KTFAccessoryMethod);
    User::LeaveIfError(retval);       
    
	CActiveScheduler::Add( this );		    		     	
	}
//
// ----------------------------------------------------------------------------------
// CASYStubObserver::~CASYStubObserver()
// ----------------------------------------------------------------------------------	
CASYStubObserver::~CASYStubObserver()
	{				
	}
//
// ----------------------------------------------------------------------------------
// CASYStubObserver::RunL()
// ----------------------------------------------------------------------------------
void CASYStubObserver::RunL()
    {                      
	TInt retval(KErrNone);	
    TPckgBuf< TTFAccessoryPublishAndSubscribe > buf;
	retval = iSubscribe.Get( KTFAccessoryTestProperty, KTFAccessoryMethod, buf );	
	User::LeaveIfError(retval);			
	
	iService->Service( buf().iMethod,
	                   buf().iParam1 );	
	if(!IsActive())
		{
		iStatus = KRequestPending;
		iSubscribe.Subscribe(iStatus);		
		SetActive();		
		}	
    }        
//
// ----------------------------------------------------------------------------------
// CASYStubObserver::DoCancel()
// ----------------------------------------------------------------------------------
void CASYStubObserver::DoCancel()
    {
    }    

//
// ----------------------------------------------------------------------------------
// CASYStubObserver::DoCancel()
// ----------------------------------------------------------------------------------    
TInt CASYStubObserver::RunError( TInt aError )
    {
    TInt errorCode(KErrNone);
    errorCode=aError;
    return errorCode;
    }
//
// ----------------------------------------------------------------------------------
// CASYStubObserver::IssueRequest
// ----------------------------------------------------------------------------------  
void CASYStubObserver::IssueRequest() 			  
    {
    if(!IsActive())
        {
        iStatus = KRequestPending;
        iSubscribe.Subscribe(iStatus);		
        SetActive();		
        }
    }

