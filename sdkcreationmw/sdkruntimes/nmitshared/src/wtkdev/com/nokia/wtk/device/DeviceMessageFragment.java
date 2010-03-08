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


/*
 * Created on Mar 27, 2003
 *
 * To change this generated comment go to
 * Window>Preferences>Java>Code Generation>Code and Comments
 */
package com.nokia.wtk.device;

import java.util.Map;
import java.util.Date;

/**
 * This interface provides an abstraction for dealing with message fragments.
 */
public interface DeviceMessageFragment {

  /**
   * Returns the message type of this fragment
   * @return the message type of this fragment.
   */
  public String getMessageType();


 /**
   * Returns the message center address to which this fragment should be sent.
   * @return the message center address to which this fragment should be sent.
   */
  public String getMessageCenterAddress();

  /**
   * Returns the address of the sender of this fragment.
   * @return the address of the sender of this fragment.
   */
  public String getSourceAddress();

  /**
   * Returns the destination of this fragment.
   * @return the destination address of this fragment.
   */
  public String getDestination();

  /**
   * Returns the original message fragment in unparsed byte array form.
   * @return the original message fragment in unparsed byte array form.
   */
  public byte [] getPDU();

  /**
   * Returns the named header's value.
   * @param name the header to get.
   * @return the named header's value.
   */
  public String getHeader( String name );


  /**
   * Returns the header's descriptive name, given the header's key.
   * @param key the header's key.
   * @return the header's name.
   */
  public String getHeaderName( String key );


  /**
   * Returns the headers for this fragment.
   * @return the headers for this fragment.
   */
  public Map getHeaders();

  /**
   * Returns the positional index of this fragment in a complete message.
   * @return the positional index of this fragment in a complete message.
   */
  public int getIndexInMessage();

  /**
   * Returns the number of fragments that compose the entire message this
   * fragment is part of.
   * @return the number of fragments in this message this fragment is in.
   */
  public int getMessageLength();

  /**
   * Returns the timestamp in a Date object
   * @return timestamp in Date object
   */
  public Date getTimestamp();

  /**
   * Returns the timestamp in string format
   * @return timestamp in string format
   */
  public String getFormattedTimestamp();


  /**
   * Returns the data or payload of this fragment.
   * @return the data or payload of this fragment.
   */
  public Object getData();

}
