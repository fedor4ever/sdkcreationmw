/*
* Copyright (c) 2000 - 2005 Nokia Corporation and/or its subsidiary(-ies).
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



package com.nokia.wtk.device;

/**
 * This interface provides an abstraction for socket events.
 *
 */

public interface DeviceSocketListener {

    /**
     * Called when a the socket is created. 
     * @param socket      a reference to the DeviceSocket.
     */
  public void socketCreated (DeviceSocket socket);

    /**
     * Called when a the socket is in CONNECTED state. This event is an indication 
     * of a successful connection establishment. If for some reason a socket did
     * not reach the CONNECTED state (e.g. the remote host is unreachable), 
     * this event will not be fired.
     * @param socket      a reference to the DeviceSocket.
     */
  public void socketConnected (DeviceSocket socket);

    /**
     * Called when a new connection is accepted by a remote host. 
     * @param listeningSocket      a reference to a socket that has been 
     *                             placed in a listening state.
     * @param acceptedSocket       a reference to the accepted socket.
     */
  public void socketAccepted (DeviceSocket listeningSocket, DeviceSocket acceptedSocket);

    /**
     * Called when a the socket is in LISTENING state. 
     * If for some reason a socket did not reach the LISTENING state 
     * (e.g. the port was already bound by an other application), 
     * this event will not be fired.
     * @param socket      a reference to the DeviceSocket.
     */
  public void socketListening (DeviceSocket socket);

    /**
     * Called after the socket was closed. 
     * @param socket      a reference to the DeviceSocket.
     */
  public void socketClosed (DeviceSocket socket);

    /**
     * Called after a message was sent on a socket.
     * @param socket      a reference to the DeviceSocket.
     * @param payload     payload sent on the socket. 
     */
  public void messageSent (DeviceSocket socket, byte[] payload);

    /**
     * Called after a message was received on a socket.
     * @param socket      a reference to the DeviceSocket.
     * @param payload     payload received on the socket. 
     */
  public void messageReceived (DeviceSocket socket, byte[] payload);
}
