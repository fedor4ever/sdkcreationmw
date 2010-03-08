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
 * Created on Aug 9, 2004
 *
 * To change this generated comment go to
 * Window>Preferences>Java>Code Generation>Code and Comments
 */
package com.nokia.wtk.sdkSimulator.diagnosticsAdapter;

// java.util
import java.util.HashMap;

// java.lang.reflect
import java.lang.reflect.Method;

// com.nokia.wtk.device
import com.nokia.wtk.device.DeviceFileSystemListener;
import com.nokia.wtk.device.DeviceFileSystem;
import com.nokia.wtk.device.DeviceFile;

/**
 * <p>Title: SdkFileSystemService</p>
 * <p>Description: Responsible for handling interaction between remote
 * clients and the native phone emulator for file system services and events.</p>
 * <p>Company: Nokia</p>
 * @version 1.0
 */
public class SdkFileSystemService extends AbstractServiceAdapter
                                  implements FileSystemAdapter,
                                             DeviceFileSystem {


  /**
   * Constructor
   *@param sdk a reference to the SDK object
   *@param name the name of the service adapter object
   */
  public SdkFileSystemService(SdkInterface sdk, String name) {
    super(sdk, name);

    Class clazz = com.nokia.wtk.device.DeviceFileSystemListener.class;
    Method [] methods = clazz.getDeclaredMethods();
    map = new HashMap();
    for ( int i = 0; i < methods.length; i++ ) {
      map.put( methods[i].getName(), methods[i] );
    }
  }

  // --- implementation of FileSystemAdapter---
  /**
   * Called when a new filesystem, a directory, or a file has been created.
   *
   * @param file      a unique reference to a DeviceFile that has been created.
   */
  public void fileAdded (DeviceFile file)  {
    addCallback( FILE_ADDED, new Object [] { file } );
  }

  /**
   * Called when a new filesystem, a directory, or a file has been deleted.
   *
   * @param file      a unique reference to a DeviceFile that has been deleted.
   */
  public void fileDeleted (DeviceFile file) {
    addCallback( FILE_DELETED, new Object [] { file } );
  }

  /**
   * Called when a new filesystem, a directory, or a file has been updateded.
   *
   * @param file      a unique reference to a DeviceFile that has been updated.
   */
  public void fileUpdated (DeviceFile file) {
    addCallback( FILE_UPDATED, new Object [] { file } );
  }

  /**
   * Called when a file has been moved.
   * Reserved for future use.
   *
   * @param file      a unique reference to a DeviceFile that has been moved.
   * @param oldParent a unique reference to a DeviceFile was the old parent
   *                  directory before the move.
   */
  public void fileMoved(DeviceFile file, DeviceFile oldParent) {
    //TBD
  }

  /**
   * Called when a file has been renamed.
   * Reserved for future use.
   *
   * @param file      a unique reference to a DeviceFile that has been renamed.
   * @param oldName   the old name of the file
   */
  public void fileRenamed(DeviceFile file, String oldName) {
    //TBD
  }

  /**
   * Returns with a list of filesystems (root directories) supported by the device.
   * Reserved for future use.
   * @return list of files filesystem supported by the device.
   */
  public DeviceFile[] getRoots() {
    // TBD
    return null;
  }

  // --- implementation of com.nokia.wtk.device.DeviceFileSystem ---
  /**
   * Returns with the root directory, where MIDlets and other MIDLet related data are stored
   * in the device.
   * @return with the J2ME root directory.
   */
  public DeviceFile getJ2MERoot() {
    // TBD - to be implemented by SDK developer
    DeviceFile fileData = null;
    if (sampleSdk != null) {
      fileData = sampleSdk.getFileSystemRoot();
    }
    return fileData;
  }


  /* (non-Javadoc)
   * @see com.nokia.wtk.diagnostics.diagnosticsAdapter.AbstractServiceAdapter#registerWithSdk()
   */
  protected void registerWithSdk() {
    // TBD - to be implemented by SDK developer
    if (sampleSdk != null) {
      sampleSdk.registerServer (serviceName, this);
    }
  }

  /* (non-Javadoc)
   * @see com.nokia.wtk.diagnostics.diagnosticsAdapter.AbstractServiceAdapter#unregisterWithSdk()
   */
  protected void unregisterWithSdk() {
    // TBD - to be implemented by SDK developer
  }


  /* (non-Javadoc)
   * @see com.nokia.wtk.diagnostics.diagnosticsAdapter.AbstractServiceAdapter#methodForName(java.lang.String)
   */
  protected Method methodForName(String name) {
    return (Method)map.get( name );
  }

  private static final String FILE_ADDED = "fileAdded";
  private static final String FILE_DELETED = "fileDeleted";
  private static final String FILE_UPDATED = "fileUpdated";
//private static final String FILE_MOVED = "fileMoved";
//private static final String FILE_RENAMED = "fileRenamed";

  private HashMap map;


}



