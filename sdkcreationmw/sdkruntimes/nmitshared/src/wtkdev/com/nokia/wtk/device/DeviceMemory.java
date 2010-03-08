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
 * This interface provides an abstraction for a memory snapshot.
 * The memory snapshot can be used to describe the entire memory usage 
 * of the Device, or certain part of it (e.g. KVM).
 *
 * Invariant: <code> getTotal() = getUsed() + getFree()</code>
 *
 * 
 */
public interface DeviceMemory {

  /**
   * Returns the number of used memory in bytes.
   * @return the number of used memory in bytes
   */
  public int getUsed ();

  /**
   * Returns the number of free memory in bytes.
   * @return the number of free memory in bytes.
   */
  public int getFree ();


  /**
   * Returns the number of total memory in bytes.
   * @return the number of total memory in bytes.
   */
  public int getTotal ();

}
