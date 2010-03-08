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

#ifndef CUSBDATATERMINALWAITER_H
#define CUSBDATATERMINALWAITER_H

#include <e32base.h>
#include "USBSocket.h"

/**
 *  This class is for waiting serial line signal DSR and for 
 *  completing active objects from Ecmt Server for improving the 
 *  Ecmt Server offered socket interface for USB usage. 
 *
 *  When DSR signal is up, we assume that Ecmt Gateway has 
 *  connected to COM port where S60 device is connected and set 
 *  the DTR signal up from Windows OS side. In this case this 
 *  class completes AO from CEcmtSocketsEngine. If DSR 
 *  goes down, we assume that Ecmt Gateway has disconnected the 
 *  connection and set the DTR signal down from Windows OS side.
 *  In this case this class completes CEcmtSocketsReader AO with
 *  error code. All of this AO completion is done because Ecmt 
 *  Server offered socket interface is not fully suitable for 
 *  USB connectivity, where socket connection is not possible. 
 *  So this class helps CUSBSocket to fit to the Ecmt Server 
 *  offered socket interface, and does needed triks for making
 *  Ecmt Server to notify Ecmt Gateway connections and 
 *  disconnections.
 *
 *  @lib EcmtUSBBearer.dll
 *  @since S60 v3.1
 */
class CUSBDataTerminalWaiter : public CActive
    {
public:

    /** Represents the state of the AO */
    enum TWaiterState 
        { 
        /** Sleeping */
        ESleep,
        /** Waiting for CEcmtSocketsEngine AO to be activated */
        EWaitEngineActivation,
        /** Waiting for CEcmtSocketsReader AO to be activated */
        EWaitReaderActivation,
        /** Waiting for DSR signal change */
        EWaitDSRChange
        };
            
    /**
     * Two-phased constructor.
     * @param aUSBSocket A reference to CUSBSocket
     * @param aCommServer A reference to RComm
     */
    static CUSBDataTerminalWaiter* NewL( CUSBSocket& aUSBSocket,
                                         RComm& aCommServer );
    
    /**
    * Destructor.
    */
    ~CUSBDataTerminalWaiter();
    
    
public: // New functions

    /**
     * Waits for DSR signal to change
     *
     * @since S60 v3.1
     */
    void WaitDSRChange();
    
    /**
     * Setter for iSocketEngineRequestStatus. 
     *      
     * @since S60 v3.1
     * @param aSocketEngineReqStat iStatus from CEcmtSocketsEngine AO.
     */
    void SetEngineTRS( TRequestStatus& aSocketEngineReqStat );
    
    /**
     * Setter for iSocketReaderRequestStatus.
     *
     * @since S60 v3.1
     * @param aSocketReaderReqStat iStatus from CEcmtSocketsReader AO.
     */
    void SetReaderTRS( TRequestStatus& aSocketReaderReqStat );        
    
    /**
     * Waits CEcmtSocketsEngine AO to be activated for 1 second. Sets 
     * CUSBDataTerminalWaiter AO to EWaitEngineActivation state.
     *
     * @since S60 v3.1
     */
    void WaitEngineActivation();
    
    /**
     * Waits CEcmtSocketsReader AO to be activated for 1 second. Sets 
     * CUSBDataTerminalWaiter AO to EWaitReaderActivation state.
     *
     * @since S60 v3.1
     */
    void WaitReaderActivation();
    
    /**
     * Completes either CEcmtSocketsEngine or CEcmtSocketsReader active object. 
     * Which one?, depends of CUSBDataTerminalWaiter AO state (iWaiterState). 
     *
     * @since S60 v3.1
     */
    void CompleteAO();
    
    /**
     * Completes both CEcmtSocketsEngine and CEcmtSocketsReader AOs.
     *
     * @since S60 v3.1
     */
    void CompleteAllAO();
    
    /**
     * Returns the state of DSR signal. Requests it from RComm.
     *
     * @since S60 v3.1
     * @return TBool ETrue if DSR is 1, otherwise EFalse.
     */
    TBool IsDSR(); 
    
protected: // From CActive

    void RunL();
    TInt RunError();
    void DoCancel();
    
private:

    /**
     * C++ default constructor.
     */
    CUSBDataTerminalWaiter( CUSBSocket& aUSBSocket,
                            RComm& aCommServer );

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();
        
private: // Data
    
    /**
     * Points iStatus of CEcmtSocketsEngine.
     * Not own.
     */
    TRequestStatus* iSocketEngineRequestStatus;
    
    /**
     * Points iStatus of CEcmtSocketsReader.
     * Not own.
     */
    TRequestStatus* iSocketReaderRequestStatus;
    
    /**
     * A Reference to CUSBSocket. This is needed only for accessing to 
     * CUsbSocket::WriteToLog function.
     */
    CUSBSocket& iUSBSocket;
    
    /**
     * A reference to CUSBSocket member iComm. Used for getting DSR state, 
     * cancelling pending read requests and for requesting DSR signal state 
     * notify from C32 server.
     */
    RComm& iComm;
       
    /**
     * The state of this active object. Available values are listed in 
     * TWaiterState enumeration.
     */ 
    TWaiterState iWaiterState;    
    
    };
 
#endif // CUSBDATATERMINALWAITER_H
