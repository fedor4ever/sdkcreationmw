/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef HEADER_SOCKET_WRITER
#define HEADER_SOCKET_WRITER

#include <e32base.h>

#include <es_sock.h>
#include <utf.h>
	
class CSocketWriter : public CActive
	{
public:

	static CSocketWriter* NewL(RSocket* aSocket);
	~CSocketWriter();
	
	// all following methods are blocking

	/**
	* Write a byte to the socket.
	*
	* @param aByte The byte value
    *
	* @leave  If write error occurs leaves with one of the Symbian OS 
	*         wide error codes.
	*/
	void WriteL(TUint8 aByte);
	
	/**
	* Write a 32bit int to the socket.
	*
	* @param aInt The int value
    *
	* @leave  If write error occurs leaves with one of the Symbian OS 
	*         wide error codes.
	*/
	void WriteIntL(TInt aInt);

	/**
	* Write a 16 bit int to the socket.
	*
	* @param aInt The int value
    *
	* @leave  If write error occurs leaves with one of the Symbian OS 
	*         wide error codes.
	*/
	void WriteShortL(TInt16 aInt);

	/**
	* Write bytes from aBuff. Will write all bytes from position 0 to 
    * aBuff.Length().
	*
	* @param  aBuff A buffer to write from
    *
	* @leave  If read error occurs leaves with one of the Symbian OS 
	*         wide error codes.
	*/
	void Write8L(TDes8& aBuff);

	/**
	* Write aLen unicode (16 bit) characters from aBuff.
	*
	* @param  aBuff A buffer to write from
    *
	* @leave  If read error occurs leaves with one of the Symbian OS 
	*         wide error codes.
	*/
	void WriteL(TDes& aBuff);

	/**
	* Write UTF formatted 8 bit data. 
	* This method calls WriteIntL(aBuff.Length()) and then Write8L(aBuff).
	*
	* @param  aBuff A buffer containing data to write.
	*				               
	* @leave  If read error occurs leaves with one of the Symbian OS 
	*         wide error codes.
	*/
	void WriteUtf8L(TDes8& aBuff);

	/**
	* Write UTF formatted unicode data. 
	* This method calls WriteIntL(aBuff.Length()) and then WriteL(aBuff).
	*
	* @param  aBuff A buffer containing data to write.
	*				               
	* @leave  If read error occurs leaves with one of the Symbian OS 
	*         wide error codes.
	*/
	void WriteUtfL(TDes& aBuff);

	/**
	* Write bytes from aBuff. Will write all bytes from position 0 to 
    * aBuff.Length().
	*
	* @param  aBuff A buffer to write from
    *
	* @leave  If read error occurs leaves with one of the Symbian OS 
	*         wide error codes.
	*/
	void Write8L(TDesC8& aBuff);

	/**
	* Write aLen unicode (16 bit) characters from aBuff.
	*
	* @param  aBuff A buffer to write from
    *
	* @leave  If read error occurs leaves with one of the Symbian OS 
	*         wide error codes.
	*/
	void WriteL(TDesC& aBuff);

	/**
	* Write UTF formatted 8 bit data. 
	* This method calls WriteIntL(aBuff.Length()) and then Write8L(aBuff).
	*
	* @param  aBuff A buffer containing data to write.
	*				               
	* @leave  If read error occurs leaves with one of the Symbian OS 
	*         wide error codes.
	*/
	void WriteUtf8L(TDesC8& aBuff);

	/**
	* Write UTF formatted unicode data. 
	* This method calls WriteIntL(aBuff.Length()) and then WriteL(aBuff).
	*
	* @param  aBuff A buffer containing data to write.
	*				               
	* @leave  If read error occurs leaves with one of the Symbian OS 
	*         wide error codes.
	*/
	void WriteUtfL(TDesC& aBuff);

protected:
	// CActive
	virtual void DoCancel();
	virtual void RunL();

private:
	CSocketWriter(RSocket* aSocket);
	void ConstructL();
	void WaitForRequest();

    RSocket* iSocket;
	CActiveSchedulerWait iWait;
	};

#endif
