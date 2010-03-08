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


#include <winsock2.h>
#include <stdio.h>

#include "EcmtGwP.h"
#include "..\..\..\..\NmitShared\src\native\slib\include\s_strbuf.h"
#include "..\..\..\..\NmitShared\src\native\wlib\w_lib.h"

static const char ecmtLanPortName [] = "WLAN";
static const char settingsFileName [] = "gwSettings.dat";
static const char defaultHandsetIp [] = "127.0.0.1";
static const int defaultEcmtPort = 49359;


typedef struct _EcmtGatewayLanSocketPort{
	EcmtGatewayPort common;
	SOCKADDR_IN lanSockAddr;
} EcmtGatewayLanSocketPort;


STATIC Bool GWENG_LanSockCanOpen(const EcmtGatewayPort* port)
{	
	return True;
}


int  lanEcmtIoRead(EcmtConnection* con, void* buf, int size){
	buf= NULL;
	return 0;
}
Bool lanEcmtIoWrite(EcmtConnection* con, const void* data, int size) {return True;}
void lanEcmtIoCancel(EcmtConnection* con) {}
Bool lanEcmtIoHandset(EcmtConnection* con){return True;}
void lanEcmtIoClose(EcmtConnection* con){}


STATIC EcmtConnection* GWENG_LanSockConnect(EcmtGatewayPort* port)
{
	
	EcmtGatewayLanSocketPort* lanPort = CAST(port,EcmtGatewayLanSocketPort,common);
	/*
	EcmtConnection* c = MEM_New(EcmtConnection);
	EcmtIo* io = MEM_New(EcmtIo);
	io->read = lanEcmtIoRead;
	io->write=lanEcmtIoWrite;
	io->cancel=lanEcmtIoCancel;
	io->handset=lanEcmtIoHandset;
	io->close=lanEcmtIoClose;

	c->io=io;
	c->port = port;
	c->ref=1;
	*/
	SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(s!= INVALID_SOCKET){
		//try to connect:
		if( connect(s, (SOCKADDR*) &(lanPort->lanSockAddr), sizeof(lanPort->lanSockAddr) ) != SOCKET_ERROR )
		{	
			EcmtConnection* c = GWENG_CreateHandsetSocketConnection(port, s);
			TRACE1("GW: connected to device %s over WLAN  ", inet_ntoa(lanPort->lanSockAddr.sin_addr));
			if (c) {
				return c;
			}
			TRACE("GWLanPort:LanSockConnect:Error creating EcmtConnection");
		}
		TRACE1("GW: connect error %d\n",SOCKET_GetLastError());
        SOCKET_Close(s);	
	}
	TRACE1("GW: socket creation error %d\n",SOCKET_GetLastError());
	SOCKET_Close(s);
	return NULL;
}
STATIC void GWENG_LanSockFree(EcmtGatewayPort* port)
{//no reason to remove Lan socket
}

STATIC EcmtPort gwLanSocket = {
    "LanSocket",                     /* name */
    EcmtGwPortSocket,	             /* type */
    GWENG_LanSockCanOpen,            /* canOpen */
    GWENG_LanSockConnect,            /* connect */
    GWENG_LanSockFree                /* free */
};

STATIC void GWENG_LanTransportRefresh(EcmtGatewayTransport* t)
{
	EcmtGateway* gw = t->gw;
	EcmtGatewayLanSocketPort* p;
	StrBuf ipPortBuf;
	char* ip = (char*)malloc(IP_ADDR_LEN*sizeof(char));
	int* port = malloc(sizeof(int));
	ECMTGW_GetWlanIpPort(ip, port);
	//we keep just one WLAN connection (should be ehough)
	//therefore, if connection already exists,
	//we update it
	//otherwise - add new one
	if(QUEUE_IsEmpty(&t->ports) )
	{
		p = MEM_New(EcmtGatewayLanSocketPort);
		//inintialize SOCKADDR_IN:
		//read the address from file:
		p->common.port = &gwLanSocket;
		p->common.name = (Char*)ecmtLanPortName;
		p->common.gw = t->gw;
		p->lanSockAddr.sin_family = AF_INET;
	}else
	{
		p = (EcmtGatewayLanSocketPort*)QCAST( QUEUE_First(&t->ports) ,EcmtGatewayPort,entry);	
	}	
		p->lanSockAddr.sin_port = htons( (u_short)*port );
		p->lanSockAddr.sin_addr.s_addr = inet_addr( ip );

		STRBUF_Init(&ipPortBuf);
		STRBUF_Copy(&ipPortBuf, inet_ntoa(p->lanSockAddr.sin_addr));
		STRBUF_AppendChar(&ipPortBuf, ':');
		STRBUF_AppendInt(&ipPortBuf, ntohs(p->lanSockAddr.sin_port));
		p->common.comment=(Char*)STRBUF_GetString(&ipPortBuf);

	if(QUEUE_IsEmpty(&t->ports) )
	{
		QUEUE_InsertTail(&t->ports, &p->common.entry);
		if (gw->cb.portFound) {
			gw->cb.portFound(gw, &(p->common), gw->ctx);
		}
	}
	else
	{
		if (gw->cb.portModified)
		{
			gw->cb.portModified(gw, &(p->common), gw->ctx);
		}
	}
}

STATIC void GWENG_LanTransportFree(EcmtGatewayTransport* t)
{
    GWENG_TransportDestroy(t);
    MEM_Free(t);
}

STATIC EcmtTransport gwLanTransport = {
    "LanPortTransport",                /* name */
    GWENG_LanTransportRefresh,         /* refresh */
    GWENG_LanTransportFree             /* free */
};

/**
 * Writes wlan IP & port into setings file
 */

EcmtBool ECMTGW_WriteToFileWlanIpPort(const char* ip, int port)
{ 
	FILE* gwSettingsS;
	int err;
	char* portString = (char *)malloc(15*sizeof(char));
	
	if(ip != NULL)
	{		
		//File Writing
		if( (gwSettingsS=fopen(settingsFileName, "w")) != NULL)
		{	
			//try writing
			fseek(gwSettingsS, 0L, SEEK_SET);
			err = fwrite(ip, strlen(ip), 1, gwSettingsS);
			err = fwrite(" ", strlen(" "), 1, gwSettingsS);
			_itoa(port, portString, 10);
			err = fwrite(portString, strlen(portString), 1, gwSettingsS);

			free(portString);
			fflush(gwSettingsS);
			fclose(gwSettingsS);
			return EcmtTrue;
		}
	}
	free(portString);
	return EcmtFalse;
}

/**
 * Creates LAN port family
 *defined	 in EcmtGWP.h
 */
EcmtGatewayTransport* GWENG_CreateLanSocketTransport(EcmtGateway* gw)
{
    EcmtGatewayTransport* t = MEM_New(EcmtGatewayTransport);
    if (t) {
        memset(t, 0, sizeof(*t));
        if (MUTEX_Init(&t->mutex)) {
            t->transport = &gwLanTransport;
            t->gw = gw;
            QUEUE_Init(&t->ports);
            return t;
        }
        MEM_Free(t);
    }
    return NULL;
}

/**
 * Gets wlan IP & port read from setings file
 */
EcmtBool ECMTGW_GetWlanIpPort(char* aIp, int* aPort)
{
	FILE* gwSettingsS;
	char* ipPort = malloc(IP_ADDR_LEN*sizeof(char));
	if((gwSettingsS=fopen(settingsFileName, "r")) != NULL)
	{	
		//try reading
		fseek(gwSettingsS, 0L, SEEK_SET);
		//read limited number of bytes, to be safe (instead of fscanf)
		fread(ipPort, IP_ADDR_LEN, 1, gwSettingsS);
		sscanf(ipPort, "%s %d", aIp, aPort);

	}
	else
	{
		//Problems with opening, 
		//is the settings file present? No?
		//Create it and write default values (call SetWlanIpPort)
		strcpy(aIp, defaultHandsetIp);
		*aPort=defaultEcmtPort;
		free(ipPort);
		
		if( ECMTGW_WriteToFileWlanIpPort(aIp, *aPort) != EcmtTrue)
		{	
			return EcmtFalse;
		}
		return EcmtTrue;

	}
	free(ipPort);
	fflush(gwSettingsS);
	fclose(gwSettingsS);
	return EcmtTrue;
}

/**
 * Writes wlan IP & port into setings file
 */

EcmtBool ECMTGW_SetWlanIpPort(const char* ip, int pn, EcmtGateway* gw )
{ 
	if(ip != NULL)
	{
		//we have EcmtTransport gw->lanSockets;
		//EcmtTransport has Queue ports, where we have to get the port to update

		/* Get WLAN port: */
		QEntry* lanEntry;
		EcmtGatewayLanSocketPort* port;
		StrBuf ipPortBuf;
		Queue* lanPorts = &gw->lanSockets->ports;

		if( !QUEUE_IsEmpty(lanPorts))
		{	
			lanEntry = QUEUE_First(lanPorts);
			port = (EcmtGatewayLanSocketPort*)QCAST(lanEntry,EcmtGatewayPort,entry);
			
			/*p->common.port = &gwLanSocket;
			p->common.name = ecmtLanPortName;
			p->common.gw = gw;
			p->lanSockAddr.sin_family = AF_INET;
			//inintialize SOCKADDR_IN:
			//read the address from file:
			*/
			port->lanSockAddr.sin_port = htons( (u_short)pn );
			port->lanSockAddr.sin_addr.s_addr = inet_addr( ip );

			STRBUF_Init(&ipPortBuf);
			STRBUF_Copy(&ipPortBuf, inet_ntoa(port->lanSockAddr.sin_addr));
			STRBUF_AppendChar(&ipPortBuf, ':');
			STRBUF_AppendInt(&ipPortBuf, ntohs(port->lanSockAddr.sin_port));
			port->common.comment=(Char*)STRBUF_GetString(&ipPortBuf);

			if (gw->cb.portModified)
			{
				gw->cb.portModified(gw, &(port->common), gw->ctx);
			}
		}
		if(ECMTGW_WriteToFileWlanIpPort(ip, pn))
		{
				return EcmtTrue;
		}
	}
	return EcmtFalse;
}


