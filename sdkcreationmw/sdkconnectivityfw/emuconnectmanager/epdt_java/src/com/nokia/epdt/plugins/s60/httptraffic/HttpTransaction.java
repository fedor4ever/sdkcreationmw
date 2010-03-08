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

package com.nokia.epdt.plugins.s60.httptraffic;

import java.util.*;

class HttpTransaction
{

  public HttpTransaction ()
  {
    _history  = new Vector ();
  }

  public HttpTransaction (int id, int procId)
  {
    _tid = id;
    _pid = procId;
    _history = new Vector ();
  }

  public HttpTransaction (int id, int procId, String reqURL)
  {
    _tid = id;
    _pid = procId;
    _history = new Vector ();
    _history.add (new RequestEntry (reqURL));
  }


  public void addRequest (String reqURL)
  {
    _history.add (new RequestEntry (reqURL));
  }


  public void removeRequest ()
  {
    if (_history.size () > 0) {
      _history.remove (0);
    }
  }

  public String getCurrentReqURL ()
  {
    RequestEntry req = _history.size() > 0 ? (RequestEntry)_history.get (0) : null;
    String url = (req != null) ? req.getURL () : "NO REQUEST";
    return url;
  }


  public long getCurrentTransId ()
  {
    RequestEntry req = _history.size () > 0 ? (RequestEntry)_history.get (0) : null;
    long id = (req != null) ? req.getId () : -1;
    return id;
  }

  public boolean isProcessingResponse ()
  {
    return _processingResponse;
  }

  public void setProcessingResponse (boolean value)
  {
     _processingResponse = value;;
  }


  private class RequestEntry
  {

    RequestEntry (String url)
    {
      _url = (url != null) ? url : "NO REQUEST URL!";
      /*
       * Makes uniq ID from 3 low bytes of _id, 3 low bytes of _pid and reqCounter
       */
      _id = (((long)_tid) << 40) | (((long)(_pid & 0xFFFFFF)) << 16) |
                (((long)(++reqCounter)) & 0xFFFF);
    }

    public long getId ()
    {
      return _id;
    }

    public String getURL ()
    {
      return _url;
    }

    private String _url;
    private long _id;
  }


  private Vector _history = null;
  private int _tid = -1;
  private int _pid = -1;
  private boolean  _processingResponse = false;;
  private static short reqCounter = 0;
}