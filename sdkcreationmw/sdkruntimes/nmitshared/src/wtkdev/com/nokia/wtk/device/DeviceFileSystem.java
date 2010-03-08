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
 * This interface provides an abstraction for handling file system calls
 *
 */
public interface DeviceFileSystem {

  /**
   * Returns with a list of filesystems (root directories) supported by the device. 
   * Reserved for future use.
   * @return list of files filesystem supported by the device.
   */
  public DeviceFile[] getRoots();

  /**
   * Returns with the root directory, where MIDlets and other MIDLet related data are stored
   * in the device. 
   * @return with the J2ME rot directory.
   */
  public DeviceFile getJ2MERoot();

}


