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


package com.nokia.wtk.sdkSimulator.sampleSdk;

// com.nokia.wtk.sdkSimulator.data
import com.nokia.wtk.sdkSimulator.data.FileData;
import com.nokia.wtk.sdkSimulator.data.MemoryData;

// com.nokia.wtk.device
import com.nokia.wtk.device.DeviceFile;
import com.nokia.wtk.device.DeviceMemory;

/**
 * <p>Title: MIDPEventGenerator </p>
 * <p>Description: A test class to generate events for the MIDP view
 * <p>Company: Nokia</p>
 * @version 1.0
 */
public class MIDPEventGenerator  {

  public MIDPEventGenerator() {
    rootDir = new FileData (ROOT_ID,
                            null,
                            "root_directory",
                            DeviceFile.FILE_TYPE_ROOT_DIR,
                            0);
    applDir = new FileData (APPL_ID,
                            rootDir,
                            "Applications",
                            DeviceFile.FILE_TYPE_DIR,
                            0);
    rootDir.addFile (applDir);
  }

  public DeviceFile getRootDir() {
    return rootDir;
  }

  public DeviceFile getApplDir() {
    return applDir;
  }

// file operations (in this test program, we simulate only fileAdded,
// (fileUpdated and fileDeleted are not simulated)
  /**
   * add file
   */
  public DeviceFile addFile() {
    DeviceFile newFile = new FileData (fileId,
                                       applDir,
                                       new String("file " + fileId),
                                       fileType,
                                       fileSize);
    applDir.addFile(newFile);
    fileId ++;

    //update size of parent dirs
    applDir.setSize (applDir.getSize() + fileSize);
    rootDir.setSize (rootDir.getSize() + fileSize);
    if (fileType == DeviceFile.FILE_TYPE_JAD) {
      fileType = DeviceFile.FILE_TYPE_JAR;
      fileSize = 100;
    }
    else {
      fileType = DeviceFile.FILE_TYPE_JAD;
      fileSize = 10;
    }
    return newFile;
  }

// memory operations
  /**
   * changeMemory
   */
  public DeviceMemory changeMemory() {

    MemoryData newMemory = new MemoryData (memoryFree, memoryUsed);
    // change data for next event
    memoryFree -= 10;
    memoryUsed += 10;
    return newMemory;
  }

  FileData rootDir = null;
  FileData applDir = null;
  private final int ROOT_ID = 1;
  private final int APPL_ID = 2;
  private int fileId = 3;
  private int fileSize = 10;
  private static int memoryUsed = 0;
  private static int memoryFree = 1000;

  private int fileType = DeviceFile.FILE_TYPE_JAD;
}

