/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Provides access to virtual USB serial port.
*
*/


#ifndef USBSOCKET_H
#define USBSOCKET_H


#include <e32base.h>
#include <c32comm.h>
#include <e32debug.h>
#include "EcmtBiSocket.h"

/* 
Define this if you want to enable loggings from CUSBSocket and 
CUSBDataTerminalWaiter. Enables TRACE, USBWAITERLOG, USBSOCKETLOG.
*/
//#define _ENABLE_USB_LOGGINGS_ 1
//#define _ENABLE_USB_RDEBUG_ 1


#ifdef _ENABLE_USB_LOGGINGS_
// This define is for USBDataTerminalWaiter. Do NOT use it from elsewhere! 
#define USBWAITERLOG(X,Y) iUSBSocket.WriteToLog( X, Y );
// This define is for USBSocket. Do NOT use it from elsewhere!
#define USBSOCKETLOG(X,Y) WriteToLog( X, Y );
#else
#define USBWAITERLOG(X,Y) ((void)0)
#define USBSOCKETLOG(X,Y) ((void)0)
#endif //_ENABLE_USB_LOGGINGS_

#ifdef _ENABLE_USB_RDEBUG_
#define USBRDEBUG(X)   RDebug::Print(_L(X));
#define USBRDEBUG2(X,A)   RDebug::Print(_L(X),A);
#else
#define USBRDEBUG(X)   ((void)0)
#define USBRDEBUG2(X,A) ((void)0)
#endif // _ENABLE_USB_RDEBUG_


class CUSBSocket;
class CUSBDataTerminalWaiter;

/**
 *  A class for creating new sockets on Ecmt Server request.
 *
 *  @lib EcmtUSBBearer.dll
 *  @since S60 v3.1
 */
class CUSBSocketFactory : public CEcmtBiSocketFactory
    {
    
public:

    /**
     * 2nd phase constructor.
     */
    void ConstructL();
    
    /**
     * Destructor.
     */
    ~CUSBSocketFactory( );

public: // From CEcmtBiSocketFactory
    
    /**
     * With this function Ecmt Server requests a new socket.
     */
    CEcmtBiSocket* NewSocketL( );

private: // Data

    /**
     * Boolean representing is CUSBSocket created with connected RCommServ 
     * handle.
     */
    TBool		    iInitialized;
    
    /**
     * RCommServ handle. Given to CUSBSocket.
     * Own.
     */
    RCommServ	    iCommServ;
    
    /**
     * CUSBSocket which initializes USB connection with Ecmt Gateway
     * Own.
     */
    CUSBSocket*     iSocket;
    };



/**
 *  Provides access to USB sockets. Uses CUSBDataTerminalWaiter for making 
 *  the Ecmt Server offered socket interface suitable for USB usage. 
 *
 *  @lib EcmtUSBBearer.dll
 *  @since S60 v3.1
 */ 
class CUSBSocket : public CEcmtBiSocket
{
public:  // Constructors and destructor
    
    /**
     * Two-phased constructor.
     */
    static CUSBSocket* NewL( RCommServ& aCommServ );
    
    /**
     * Destructor.
     */
    virtual ~CUSBSocket();

public: // From CEcmtBiSocket
    
    /**
     * From CEcmtBiSocket.
     * Opens USB serial connection.
     *
     * @since S60 v3.1
     * @param aBlank If ETrue, serial connection is not opened. 
     * @return TInt Error from RComm::Open
     */
    TInt Open( TBool aBlank );

    /**
     * From CEcmtBiSocket.
     * Writes to the connection.
     *
     * @since S60 v3.1
     * @param aDesc Data for writing.
     * @param aStatus iStatus from CEcmtSocketsWriter AO, which waits for 
     *                RComm::Write to complete.
     */
    void Write( const TDesC8& aDesc, TRequestStatus& aStatus );
    
    /**
     * From CEcmtBiSocket.
     * Cancels write requests from C32.
     *
     * @since S60 v3.1
     */
    void CancelWrite();
    
    /**
     * From CEcmtBiSocket.
     * Reads from connection. Sets CUSBDataTerminalWaiter to wait DSR change. 
     * If DSR changes, iWaiter completes CEcmtSocketsReader AO with error. By 
     * this way Ecmt Server closes the socket.
     *
     * @since S60 v3.1
     * @param aDesc A buffer for reading.
     * @param aStatus CEcmtSocketsReader AO iStatus.
     */    
    void Read( TDes8& aDesc, TRequestStatus& aStatus );
    
    /**
     * From CEcmtBiSocket.
     * Cancels read requests fro C32.
     *
     * @since S60 v3.1
     */
    void CancelRead();        
    
    /**
     * From CEcmtBiSocket.
     * Does nothing. 
     *
     * @since S60 v3.1
     * @return TInt Always KErrNone.
     */    
    TInt Bind( );
    
    /**
     * From CEcmtBiSocket.
     * Does nothing.
     *
     * @since S60 v3.1
     * @param aSize Not used
     * @return TInt Always KErrNone.
     */
    TInt Listen( TUint aSize );
    
    /**
     * From CEcmtBiSocket.
     * Accepts connection. If data terminal is not ready (DSR==0) it waits for
     * it using CEcmtDataTerminalWaiter. 
     *
     * @since S60 v3.1
     * @param aBlankSocket A Socket from Ecmt Server. This socket does not have
     *                     initialized connection.
     * @param aStatus iStatus from CEcmtSocketsEngine AO.
     */    
    void Accept( CEcmtBiSocket& aBlankSocket, TRequestStatus& aStatus );
    
    /**
     * From CEcmtBiSocket.
     * Cancels accept by cancelling read/write requests from C32.
     *
     * @since S60 v3.1
     */
    void CancelAccept();
    
    /**
     * From CEcmtBiSocket.
     * Closes USB socket. If DSR is down (Ecmt Gateway in disconnected state), 
     * it only closes iComm handle, otherwise it sends Ecmt Disconnect message 
     * to the GW for informing it that server is going down. If the disconnect 
     * message is sent when GW is in disconnected state, we end up to error 
     * situation where Ecmt Gateway receives the message from some queue in 
     * next connection immediately after that closes the connection.
     *
     * @since S60 v3.1
     */    
    void Close();
    
    /**
     * From CEcmtBiSocket.
     * Returns pointer to iParameterString. Used for Ecmt Server notification 
     * messages which are shown on the GuiLauncher log view.
     *
     * @since S60 v3.1
     * @return TPtrC pointer to iParameterString. Empty in USB bearer impl.
     */    
    const TPtrC ParameterString() const;
    
    /**
     * From CEcmtBiSocket.
     * Returns pointer descriptor to string defined in literal KUSBAf. Used 
     * for Ecmt Server notification messages which are shown on the GuiLauncher 
     * log view.
     *
     * @since S60 v3.1
     * @return TPtrC A Pointer descriptor literal KUSBAf.
     */
    const TPtrC AddressFamily() const; 
    
public: // New functions
    
    /**
     * Writes to log file C:\Logs\EcmtServer\EcmtServer.log. 
     *
     * @since S60 v3.1
     * @param aFName Name for the function from where this function is called.
     * @param aString The log string.
     */
#ifdef _ENABLE_USB_LOGGINGS_
    void WriteToLog( const TDesC& aFName, const TDesC8& aString );
#endif


private:

    /**
     * C++ default constructor.
     */
    CUSBSocket( RCommServ& aCommServ );

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

    /**
     * Setter for iRealSocket.
     *
     * @since S60 v3.1
     * @param aSocket A pointer to CUSBSocket object. This instance should have
     *                the opened USB serial connection, not the blank one.
     */
    void SetRealSocket( CUSBSocket* aSocket );
    	    
private:    // Data

    /**
     * A reference to the connected RCommServ handle, which is connected in 
     * CEcmtBiSocketFactory::NewSocketL.
     * Not own.
     */
    RCommServ&		        iCommServ;
    
    /**
     * RComm handle.
     * Own.  
     */
    RComm		            iComm;
    
    /**
     * Buffer for parameters which are used as Ecmt Server notification
     * messages and shown in GuiLauncher log view.
     * Own.  
     */
    HBufC*		            iParameterString;
    
    /**
     * ?description_of_pointer_member
     * Own.  *** Write "Own" if this class owns the object pointed to; in
                 other words, if this class is responsible for deleting it.
     */
    CUSBSocket*             iRealSocket;
    
    /**
     * A Pointer to CUSBDataTerminalWaiter object. Used for making 
     * the Ecmt Server offered socket interface suitable for USB usage. See 
     * USBDataTerminalWaiter.h for more info.  
     * Own.  
     */
    CUSBDataTerminalWaiter* iWaiter;
        
    };

#endif // USBSOCKET_H
        
// End of File
