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


package com.nokia.wtk.sdkSimulator.data;

import java.util.Map;
import com.nokia.wtk.device.DeviceMemory;

/**
 * This class represents provides the Memory data type.
 *
 */
public class MemoryData implements DeviceMemory {

  public MemoryData() {
  }


  public MemoryData(int free,
                    int used) {

    this.used = used;
    this.free = free;
  }

  /**
   * Returns the amount of used memory.
   * @return used memory.
   */
  public int getUsed() {
    return used;
  }

  /**
   * Returns the amount of free memory.
   * @return free memory.
   */
  public int getFree() {
    return free;
  }

  /**
   * Returns the total amount of memory.
   * @return total memory.
   */
  public int getTotal() {
    return used + free;
  }

  /**
  * Returns a String representation of the MemoryData object
  */
  public String toString() {

    StringBuffer buf = new StringBuffer();
    buf.append("Used Memory: " + getUsed() + "\n");
    buf.append("Free Memory: " + getFree() + "\n");
    buf.append("Total Memory: " + getTotal() + "\n");
    return buf.toString();
  }


   private int used;
   private int free;
}
