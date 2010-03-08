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
package com.nokia.wtk.sdkSimulator.data;

import java.util.Iterator;
import java.util.Vector;

import com.nokia.wtk.device.DeviceMessage;
import com.nokia.wtk.device.DeviceMessageFragment;

/**
 *
 * To change this generated comment go to
 * Window>Preferences>Java>Code Generation>Code and Comments
 */
public class SMSMessage implements DeviceMessage, Cloneable {
  public SMSMessage() {
  }

  public void addFragment( DeviceMessageFragment fragment ) {
    fragments.add( fragment );
  }

  public int numberOfFragments() {
    return fragments.size();
  }

  /**
   * Returns a clone of itself
   * @return a clone of myself.
   */
  public SMSMessage getClone() {
    try {
      SMSMessage myClone = (SMSMessage)this.clone();
      return myClone;
    } catch (CloneNotSupportedException e) {
      return null;
    }
  }

  /* (non-Javadoc)
   * @see com.nokia.wtk.device.DeviceMessage#getFragments()
   */
  public DeviceMessageFragment [] getFragments() {
    if ( array == null ) {
      array = new DeviceMessageFragment[ fragments.size() ];
      Iterator iterator = fragments.iterator();
      SMSMessageFragment fragment;
      int index = 0;
      while ( iterator.hasNext() ) {
        fragment = (SMSMessageFragment)iterator.next();
        index = fragment.getIndexInMessage();
        if (index > 0) {
          index--;
        }
        array[index] = fragment;
      }
    }
    return array;
  }

  protected Vector fragments = new Vector();
  private transient DeviceMessageFragment [] array = null;
}
