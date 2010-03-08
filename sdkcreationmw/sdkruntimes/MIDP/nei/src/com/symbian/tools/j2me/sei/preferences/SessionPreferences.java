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
package com.symbian.tools.j2me.sei.preferences;

import java.io.*;

public class SessionPreferences implements Serializable
{

  //
  // Members
  //

  private String iHost = "host";
  private int iPort = 11011;
  private int iListeningPort = 11022;
  private boolean iOutgoing = true;


  //
  // Operations
  //

  public SessionPreferences(){

  }

  public String getHost()
  {
    return iHost;
  }

  public int getPort()
  {
    return iPort;
  }

  public int getLocalPort()
  {
    return iListeningPort;
  }

  public boolean isOutgoing()
  {
    return iOutgoing;
  }

  public void setHost(String aHost)
  {
    iHost = aHost;
  }

  public void setPort(int aPort)
  {
    iPort = aPort;
  }

  public void setLocalPort(int aPort)
  {
    iListeningPort = aPort;
  }

  public void setOutgoing(boolean aOut)
  {
    iOutgoing = aOut;
  }

  public String toString()
  {
    return "host: " + iHost + " port: " + iPort +
        " localport: " + iListeningPort + " outgoing: " + iOutgoing;
  }
}
