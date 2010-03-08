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
 * Created on Aug 10, 2004
 */
package com.nokia.wtk.sdkSimulator.diagnosticsAdapter;

import com.nokia.wtk.device.DeviceFile;

/**
 * <p>Title: SdkInterface</p>
 * <p>Description: An interface for the communication between an SDK
 * and its diagnostics adapter.</p>
 * <p>Company: Nokia</p>
 * @version 1.0
 */
public interface SdkInterface {

  /**
   * returns the one instance of the SDK
   * @ return the SDK instance
   */
  public SdkInterface getSdkInstance();

  /**
   * registers the specified service name with the specified
   * service object
   * @param serviceName The name of the service
   * @param server The object that implements the service
   */
  public void registerServer(String serviceName, Object serviceObject);

  /**
   * registers the specified listener object with the specified
   * service name.
   * @param serviceName the name of the SDK service
   * @param listener the listener object associated with th eservice
   */
  public void registerPropertyListener(PropertyListener listener);

  // sdk services
  /**
   * Returns the java root directory of the (sample) SDK file system
   * service name.
   * @return a DeviceFileObject
   * @param listener the listener object associated with th eservice
   */
  public DeviceFile getFileSystemRoot();

}
