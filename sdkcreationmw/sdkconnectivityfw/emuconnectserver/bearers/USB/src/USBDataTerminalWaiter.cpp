/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class is for waiting serial line signal DSR and for 
*                completing active objects from Ecmt Server for improving the 
*                Ecmt Server offered socket interface for USB usage. 
*
*/


#include <C32COMM.H> 
#include "USBDataTerminalWaiter.h"


// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// C++ default constructor.
// -----------------------------------------------------------------------------
//    
CUSBDataTerminalWaiter::CUSBDataTerminalWaiter( CUSBSocket& aUSBSocket,
                                                RComm& aCommServer ) : 
        CActive( EPriorityNormal ),   
        iUSBSocket( aUSBSocket ),
        iComm( aCommServer )
    {      
    }

// -----------------------------------------------------------------------------
// Destructor.
// -----------------------------------------------------------------------------
//    
CUSBDataTerminalWaiter::~CUSBDataTerminalWaiter()
    {   
    USBRDEBUG("CUSBDataTerminalWaiter::~CUSBDataTerminalWaiter");
    USBWAITERLOG( 
        _L("CUSBDataTerminalWaiter::~CUSBDataTerminalWaiter()"), 
        _L8("Goodbye!") ); 
           
    if(IsActive())
   	    {
   	    USBRDEBUG("CUSBDataTerminalWaiter::~CUSBDataTerminalWaiter - Cancelling");
   	    USBRDEBUG2("iSocketEngineRequestStatus:%d", iSocketEngineRequestStatus->Int());
   	    USBRDEBUG2("iSocketReaderRequestStatus:%d", iSocketReaderRequestStatus->Int());
   	    
   	    Cancel();           
   	    }
    }

// -----------------------------------------------------------------------------
// Two-phased constructor.
// -----------------------------------------------------------------------------
//        
CUSBDataTerminalWaiter* CUSBDataTerminalWaiter::NewL( CUSBSocket& aUSBSocket,
                                                      RComm& aCommServer )
    {
    USBRDEBUG("CUSBDataTerminalWaiter* CUSBDataTerminalWaiter::NewL");
    CUSBDataTerminalWaiter* self = 
        new ( ELeave ) CUSBDataTerminalWaiter( aUSBSocket, aCommServer );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// 2nd phase constructor.
// -----------------------------------------------------------------------------
//    
void CUSBDataTerminalWaiter::ConstructL()
    {
    USBRDEBUG("CUSBDataTerminalWaiter::ConstructL");
    USBWAITERLOG(  _L("CUSBDataTerminalWaiter::ConstructL()"), 
                            _L8(" ") );
                            
    CActiveScheduler::Add( this );
    iWaiterState = ESleep;
    }

// -----------------------------------------------------------------------------
// Returns the state of DSR signal.
// -----------------------------------------------------------------------------
//    
TBool CUSBDataTerminalWaiter::IsDSR()
    {    
    USBRDEBUG("CUSBDataTerminalWaiter::IsDSR");
    USBWAITERLOG( _L("CUSBDataTerminalWaiter::IsDSR()"), _L8(" ") );
    
    TUint signals = iComm.Signals( KSignalDTEInputs );
    
    if( signals & KSignalDSR ) 
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

// -----------------------------------------------------------------------------
// Setter for iSocketEngineRequestStatus.
// -----------------------------------------------------------------------------
//    
void CUSBDataTerminalWaiter::SetEngineTRS( TRequestStatus& aSocketEngineReqStat )
    {
    USBRDEBUG("CUSBDataTerminalWaiter::SetEngineTRS");
    USBWAITERLOG(  _L("CUSBDataTerminalWaiter::SetEngineTRS()"), 
                            _L8(" ") );
                            
    iSocketEngineRequestStatus = &aSocketEngineReqStat;
    USBRDEBUG2("iSocketEngineRequestStatus:%d", iSocketEngineRequestStatus->Int());
    USBRDEBUG2("aSocketEngineReqStat:%d", aSocketEngineReqStat.Int());
    }

// -----------------------------------------------------------------------------
// Setter for iSocketReaderRequestStatus.
// -----------------------------------------------------------------------------
//    
void CUSBDataTerminalWaiter::SetReaderTRS( TRequestStatus& aSocketReaderReqStat )
    {
    USBRDEBUG("CUSBDataTerminalWaiter::SetReaderTRS");
    USBWAITERLOG(  _L("CUSBDataTerminalWaiter::SetReaderTRS()"), 
                            _L8(" ") );
                            
    iSocketReaderRequestStatus = &aSocketReaderReqStat;
    USBRDEBUG2("iSocketReaderRequestStatus:%d", iSocketReaderRequestStatus->Int());
    USBRDEBUG2("aSocketReaderReqStat:%d", aSocketReaderReqStat.Int());
    }   

// -----------------------------------------------------------------------------
// Waits for DSR signal to change.
// -----------------------------------------------------------------------------
//    
void CUSBDataTerminalWaiter::WaitDSRChange()
    {
    USBRDEBUG("CUSBDataTerminalWaiter::WaitDSRChange()");
    USBWAITERLOG(  _L("CUSBDataTerminalWaiter::WaitDSRChange()"), 
                            _L8(" ") );
                            
    TUint signals;
    iComm.NotifySignalChange( iStatus, signals, KSignalDSR ); 
    iWaiterState = EWaitDSRChange;
    
    SetActive();      
    }
    
// -----------------------------------------------------------------------------
// Waits CEcmtSocketsEngine AO to be activated for 1 second.
// -----------------------------------------------------------------------------
//        
void CUSBDataTerminalWaiter::WaitEngineActivation()
    {
    USBRDEBUG("CUSBDataTerminalWaiter::WaitEngineActivation()");
    USBWAITERLOG( _L("CUSBDataTerminalWaiter::WaitEngineActivation()"), _L8(" ") );
            
    RTimer timer;
    timer.CreateLocal();
    //Wait for a while, so EcmtSocketEngine can activate its active object
    timer.After( iStatus, 1000000 );
    iWaiterState = EWaitEngineActivation;
    SetActive();
    }

// -----------------------------------------------------------------------------
// Waits CEcmtSocketsReader AO to be activated for 1 second.
// -----------------------------------------------------------------------------
//    
void CUSBDataTerminalWaiter::WaitReaderActivation()
	{
	USBRDEBUG("CUSBDataTerminalWaiter::WaitReaderActivation()");
	USBWAITERLOG( _L("CUSBDataTerminalWaiter::WaitReaderActivation()"), _L8(" ") );
	        
    RTimer timer;
    timer.CreateLocal();
    //Wait for a while, so EcmtSocketReader can activate its active object
    timer.After( iStatus, 1000000 );
    iWaiterState = EWaitReaderActivation;
    SetActive();
    
	}

// -----------------------------------------------------------------------------
// Completes both CEcmtSocketsEngine and CEcmtSocketsReader AOs.
// -----------------------------------------------------------------------------
//    	
void CUSBDataTerminalWaiter::CompleteAllAO()
    {
    USBRDEBUG("CUSBDataTerminalWaiter::CompleteAllAO()");
    USBWAITERLOG(  _L("CUSBDataTerminalWaiter::CompleteAllAO()"), 
                            _L8(" ") );
                            
    if( iSocketEngineRequestStatus )
        {
        User::RequestComplete( iSocketEngineRequestStatus, KErrNone );
        }
    if( iSocketReaderRequestStatus )
        {
        User::RequestComplete( iSocketReaderRequestStatus, KErrNone );
        }
   
    }

// -----------------------------------------------------------------------------
// Completes either CEcmtSocketsEngine or CEcmtSocketsReader active object. 
// Which one?, depends of CUSBDataTerminalWaiter AO state (iWaiterState).
// -----------------------------------------------------------------------------
//    
void CUSBDataTerminalWaiter::CompleteAO()
    {    
    USBRDEBUG("CUSBDataTerminalWaiter::CompleteAO()");
    USBWAITERLOG(  _L("CUSBDataTerminalWaiter::CompleteAO()"), 
                            _L8(" ") );
                            
    if( iWaiterState == ESleep )
        {

        USBWAITERLOG(  _L("CUSBDataTerminalWaiter::CompleteAO()"), 
                                _L8("iWaiterState==ESleep") );
        return;
        }
        
    TRequestStatus* status;
    TInt completeError( KErrNone );
    
    switch( iWaiterState )
        {
        case EWaitEngineActivation:
            USBRDEBUG("CUSBDataTerminalWaiter::CompleteAO() - EWaitEngineActivation");
            USBWAITERLOG( _L("CUSBDataTerminalWaiter::CompleteAO()"), 
                                    _L8("EWaitEngineActivation") );
      
            status = iSocketEngineRequestStatus;
            break;
        case EWaitReaderActivation:
            USBRDEBUG("CUSBDataTerminalWaiter::CompleteAO() - EWaitReaderActivation");
            USBWAITERLOG( _L("CUSBDataTerminalWaiter::CompleteAO()"), 
                                    _L8("EWaitReaderActivation") );
      
            status = iSocketReaderRequestStatus;
            break;
        case EWaitDSRChange:
            if( IsDSR() )
                {
                USBRDEBUG("CUSBDataTerminalWaiter::CompleteAO() - Complete SocketEngine");
                USBWAITERLOG( 
                    _L("CUSBDataTerminalWaiter::CompleteAO()"), 
                    _L8("Complete SocketEngine") );
      
                status = iSocketEngineRequestStatus;
                }
            else
                {
                USBRDEBUG("CUSBDataTerminalWaiter::CompleteAO() - Complete SocketReader");
                USBWAITERLOG( 
                    _L("CUSBDataTerminalWaiter::CompleteAO()"), 
                    _L8("Complete SocketReader") );
      
                status = iSocketReaderRequestStatus;
                completeError = KErrGeneral;
                
                }
            break;
        }
        
    if( iWaiterState == EWaitReaderActivation ||
        completeError == KErrGeneral )
        {
        iComm.ReadCancel(); 
        }
    else
        {
        //Complete AO
        User::RequestComplete( status, completeError ); 
        }
    }

// -----------------------------------------------------------------------------
// From class CActive
// Calls CompleteAO if no error.
// -----------------------------------------------------------------------------
//    
void CUSBDataTerminalWaiter::RunL()
    {
    if( iStatus == KErrNone )
        { 
        USBRDEBUG("CUSBDataTerminalWaiter::RunL() - KErrNone");
        USBWAITERLOG( _L("CUSBDataTerminalWaiter::RunL()"), 
                                _L8("KErrNone") );   
      
        switch( iWaiterState )
            {
            case EWaitEngineActivation:
                //Complete EcmtSocketEngine active object
                CompleteAO();
                iWaiterState = ESleep;
                break;
            case EWaitReaderActivation:
                CompleteAO();
                iWaiterState = ESleep;
                break;
            case EWaitDSRChange:
                CompleteAO();
                iWaiterState = ESleep;
                break;
            default:
                break;                
            }
        }
    else
        {
        USBRDEBUG("CUSBDataTerminalWaiter::RunL() - Error");
        USBWAITERLOG( _L("CUSBDataTerminalWaiter::RunL()"), 
                                _L8("Error") );
      
        //TODO: Handle error
         }   
    }

// -----------------------------------------------------------------------------
// From class CActive
// -----------------------------------------------------------------------------
//    
TInt CUSBDataTerminalWaiter::RunError()
    {
    USBRDEBUG("CUSBDataTerminalWaiter::RunError()");
    USBWAITERLOG( _L("CUSBDataTerminalWaiter::RunError()"), _L8(" ") );

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// From class CActive
// -----------------------------------------------------------------------------
//        
void CUSBDataTerminalWaiter::DoCancel()
    {
    USBRDEBUG("CUSBDataTerminalWaiter::DoCancel()");
    USBWAITERLOG( _L("CUSBDataTerminalWaiter::DoCancel()"), _L8(" ") );
    }
