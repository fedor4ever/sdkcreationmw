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

/**
 *
 * To change this generated comment go to 
 * Window>Preferences>Java>Code Generation>Code and Comments
 */
public interface DeviceMessageListener {
  
  /**
   * Notifies the listener that a fragment has been sent.
   * @param fragment the fragment that was sent.
   */
  public void fragmentSent( DeviceMessageFragment fragment );
  
  /**
   * Notifies the listener that a fragment has been received.
   * @param fragment the fragment that was received.
   */
  public void fragmentReceived( DeviceMessageFragment fragment );
  
  /**
   * Notifies the listener that a full message has been sent. This will always
   * occur after all fragments of the message have been sent.
   * @param message the message that was sent.
   */
  public void messageSent( DeviceMessage message );
  
  /**
   * Notifies the listener that a message has been received. This will always
   * occur after all fragments of the message have been received.
   * @param message the message that was received.
   */
  public void messageReceived( DeviceMessage message );
}
