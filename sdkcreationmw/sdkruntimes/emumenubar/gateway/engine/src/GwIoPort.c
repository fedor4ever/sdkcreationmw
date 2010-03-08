/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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


#include "EcmtGwP.h"

/**
 * Internal data structure for COM port I/O
 */

typedef struct _EcmtPortConnection {
    EcmtConnection connection;          /* common part */
    HANDLE handle;                      /* COM port handle */
    Event cancelEvent;                  /* Unblocks read/write */
    OVERLAPPED roverlap;                /* Overlap structure for read */
    OVERLAPPED woverlap;                /* Overlap structure for write */
} EcmtPortConnection;

/**
 * Attempts to read from COM port without blocking.
 */
STATIC int GWENG_IoPortNonBlockRead(HANDLE h, void* data, DWORD to_read,
                                    OVERLAPPED* over, EcmtConnection* con)
{
    DWORD err = 0;
    COMSTAT comstat;
    memset (&comstat, 0, sizeof (COMSTAT));
    if (!ClearCommError(h, &err, &comstat)) {
        TRACE1("GWENG_IoPortNonBlockRead: ClearCommError error %d\n",
            GetLastError());
        return (-1);
    } else if (comstat.cbInQue > 0) {
        int nbytes = 0;
        ResetEvent(over->hEvent);
        if (ReadFile(h, data, MIN(to_read, comstat.cbInQue), &nbytes, over) ||
            (GetLastError() == ERROR_IO_PENDING &&
             GetOverlappedResult(h, over, &nbytes, True))) {
				
			int dscMsgLength = sizeof(ECMT_USB_DISCONNECT_MSG)-1;			
			// if data length is same as disconnect message length, compare data to 
			// ecmt disconnect message. 
			if(dscMsgLength == nbytes){
				if( strncmp(data, ECMT_USB_DISCONNECT_MSG, (dscMsgLength)) == 0 ){
					TRACE("GWENG_IoPortNonBlockRead: Ecmt disconnect message found!\n");
					/*Ecmt disconnect message found! Ecmt Server/USB bearer wants us to
					close the connection!
					*/
					return -2;
				}
			}

            TRACE1("GWENG_IoPortNonBlockRead: got %d bytes\n", nbytes);
            DEBUG_ONLY(PRINT_Dump(DEBUG_Trace, data, nbytes, 0));
            return nbytes;
        } else {
            TRACE1("GWENG_IoPortNonBlockRead: read error %d\n",GetLastError());
            return (-1);
        }
    } else {
        return 0;
    }
}

/**
 * Waits for COM event ot a stop signal
 */
STATIC Bool GWENG_IoPortWait (EcmtPortConnection* port)
{
    DWORD status = 0;
    HANDLE  h[2];
    h[0] = port->roverlap.hEvent;
    h[1] = port->cancelEvent.handle;
    if (WaitCommEvent(port->handle, &status, &port->roverlap)) {
		TRACE1("GWENG_IoPortWait status: %x",status);
		if(status & EV_BREAK) {
			TRACE("GWENG_IoPortWait: BREAK received");
		}		
        if(status & EV_DSR) {
			TRACE("GWENG_IoPortWait: DSR signal changed state");
		}		
		if (status & EV_ERR) {
            return False;
        }		
		else {
            ASSERT(status & (EV_RXCHAR |EV_RX80FULL));
            return (status & (EV_RXCHAR |EV_RX80FULL) != 0);
        }		
    } else {
        DWORD err = GetLastError(); 
        if (err == ERROR_IO_PENDING) {
            err = WaitForMultipleObjects(2, h, False, INFINITE);
            if (err == (WAIT_OBJECT_0+1)) 
				CancelIo(port->handle);
            return (err == WAIT_OBJECT_0);
			
        } else {
            TRACE1("GWENG_IoPortWait: WaitCommEvent error %d\n", err);
            return False;
        }
    }
}

/**
 * Reads the requested number of bytes from the COM port and puts the data
 * into the Buffer. This function will block if there is not enough data in
 * the COM port.
 */
STATIC int GWENG_IoPortRead(EcmtConnection* con, void* buf, int size)
{
    EcmtPortConnection* port = CAST(con,EcmtPortConnection,connection);
    OVERLAPPED* over = &port->roverlap;
    for (;;) {
        if (!SetCommMask(port->handle, EV_RXCHAR|EV_RX80FULL|EV_ERR|EV_DSR|EV_BREAK)) {
            TRACE1("GWENG_IoPortRead: SetCommMask error %d\n",GetLastError());
            return -1;
        } else {
            int nbytes = GWENG_IoPortNonBlockRead(port->handle,buf,size,over,con);
            if (nbytes == 0) {
                if (!GWENG_IoPortWait(port)) {
                    return -1;
                }				
            } 
			//Disconnect message received from USB bearer.
			else if (nbytes == -2) {
					return -1;
			}
			else {
                return nbytes;
            }
        }
    }
}

/**
 * Writes to the COM port using W32 API. returns number of bytes 
 * written or -1 if any error occures.
 */
STATIC Bool GWENG_IoPortWrite(EcmtConnection* con, const void* data, int size)
{
    int written = 0;
    EcmtPortConnection* port = CAST(con,EcmtPortConnection,connection);
    if (!WriteFile(port->handle, data, size, &written, &port->woverlap)) {
        DWORD err = GetLastError();
        if (err == ERROR_IO_PENDING) {
            HANDLE h[2];
            h[0] = port->woverlap.hEvent;
            h[1] = port->cancelEvent.handle;
            err = WaitForMultipleObjects(2, h, False, INFINITE);
            if (err == WAIT_OBJECT_0) {
                GetOverlappedResult(port->handle,&port->woverlap,&written,True);
            } else if (err == (WAIT_OBJECT_0+1)) {
                TRACE("GWENG_IoPortWrite: stop event\n");
                CancelIo(port->handle);
                return False;
            } else {
                ASSERT(err == WAIT_FAILED);
                TRACE1("GWENG_IoPortWrite: wait error %d\n",err);
                return False;
            }
        } else {
            TRACE1("GWENG_IoPortWrite: write error %d\n",err);
            return False;
        }
    }
    ASSERT(written == size);
    TRACE1("GWENG_IoPortWrite: wrote %d bytes\n", written);
    DEBUG_ONLY(PRINT_Dump(DEBUG_Trace, data, written, 0));
    return True;
}

STATIC void GWENG_IoPortCancel(EcmtConnection* con)
{
    EcmtPortConnection* port = CAST(con,EcmtPortConnection,connection);
    EVENT_Set(&port->cancelEvent);
}

STATIC Bool GWENG_IoPortHandset(EcmtConnection* con)
{
    return True;
}

STATIC void GWENG_IoPortClose(EcmtConnection* con)
{	
    EcmtPortConnection* port = CAST(con,EcmtPortConnection,connection);

	if(con->port->port->type == EcmtGwPortSerial)
		{
		EscapeCommFunction( port->handle, CLRDTR );
		}

	ASSERT(con->ref < 2);
    EVENT_Destroy(&port->cancelEvent);
    CloseHandle(port->roverlap.hEvent);
    CloseHandle(port->woverlap.hEvent);
    CloseHandle(port->handle);
    MEM_Free(port);
}

/**
 * Defines COM port I/O.
 */
STATIC const EcmtIo gwIoComPort = {
    "ComPortIo",               /* name */
    GWENG_IoPortRead,          /* read */
    GWENG_IoPortWrite,         /* write */
    GWENG_IoPortCancel,        /* cancel */
    GWENG_IoPortHandset,       /* handset */
    GWENG_IoPortClose          /* close */
};

/**
 * Creates the COM port transport
 */
EcmtConnection* GWENG_CreateComPortConnection(EcmtGatewayPort* port)
{
	DCB dcb;
	LPDCB lpDcb = &dcb;
    EcmtGateway* gw = port->gw;
    EcmtPortConnection* pc = MEM_New(EcmtPortConnection);
    if (pc) {
        memset(pc, 0, sizeof(*pc));
        pc->handle = CreateFile(port->name, GENERIC_READ | GENERIC_WRITE,
            0, 0, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, 0);
		
        if (pc->handle != INVALID_HANDLE_VALUE) {

			if (GetCommState(pc->handle, lpDcb))
			{
				TRACE3("GWENG_CreateComPortConnection: DtrCtrl %X, DsrSen %X, OutxDsrFlow %X\n", 
					lpDcb->fDtrControl,
					lpDcb->fDsrSensitivity,
					lpDcb->fOutxDsrFlow );
			}
			
			if(port->port->type == EcmtGwPortSerial)
			{
				EscapeCommFunction( pc->handle, SETDTR );
			}

            if (EVENT_Init(&pc->cancelEvent)) {
                pc->roverlap.hEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
                if (pc->roverlap.hEvent) {
                    pc->woverlap.hEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
                    if (pc->woverlap.hEvent) {
                        pc->connection.io = &gwIoComPort;
                        pc->connection.ref = 1;
                        pc->connection.gw = gw;
                        pc->connection.port = port;
                        return &pc->connection;
                    }
                    CloseHandle(pc->roverlap.hEvent);
                }
                EVENT_Destroy(&pc->cancelEvent);
            }
            CloseHandle(pc->handle);
        }
        MEM_Free(pc);
    }
    return NULL;
}
