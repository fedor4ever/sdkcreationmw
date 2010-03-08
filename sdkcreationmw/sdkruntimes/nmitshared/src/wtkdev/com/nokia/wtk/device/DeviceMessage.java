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
 * This interface provides an abstraction for dealing with messages that are
 * composed of X number of fragments.
 */
public interface DeviceMessage {
  
  /**
   * Returns the fragments that make up this message in order.
   * @return the fragments that make up this message.
   */
  public DeviceMessageFragment [] getFragments();
  
  /**
   * Returns the number of fragments that make up this message.
   * @return the number of fragments that make up this message.
   */
  public int numberOfFragments();
}
