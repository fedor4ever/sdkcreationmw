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

import java.util.Vector;
import java.util.Map;
import com.nokia.wtk.device.DeviceFile;

/**
 * This class provides the File data type, which represents a file or directory
 * in the persistent storage of the Device.
 *
 */
public class FileData implements DeviceFile {

  public FileData() {
  }

  public FileData(int id,
                  DeviceFile parent,
                  String name,
                  int type,
                  int size) {

    this.id = id;
    this.parent = parent;
    this.name = name;
    this.type = type;
    this.size = size;

    // for test only
    fileList = new Vector();
  }


  /**
   * Returns the file Id.
   * @return the file Id.
   */
  public int getId () {
    return id;
  }


  /**
   * Returns the name of the file.
   * For the root directory, this is the name of the filesystem (e.g. "JAM Storage").
   * For the subdirectories, this is the name of the directory.
   * For files, this function returns the name of the file.
   * @return the name of the file.
   */
  public String getName () {
    return name;
  }

  /**
   * Returns the size of the file, after getting it from the native code.
   * For the root directory, this is the size of the entire filesystem.
   * For subdirectories, this is the size of the directory.
   * For files, this function returns the size of the file.
   * @return the size of the file.
   */
  public int getSize () {
    //TBD in real system: invoke native method to get the latest file size
//    size = getFileSize(id, type);
    return size;
  }


  /**
   * Returns the type of the file.
   * For the root directory, the function must return {@link #FILE_TYPE_ROOT_DIR}.
   * For subdirectories, the function must return {@link #FILE_TYPE_DIR}.
   * For files, this function returns the type of the file.
   * @return the type of the file.
   */
  public int getType () {
    return type;
  }


  /**
   * Returns the parent of the the file.
   * For the root directory, the function must return null.
   * For subdirectories, the function must return the reference to the root directory.
   * For files, this function returns the reference to the directory, where the file resides.
   * @return the parent of the the file.
   */
  public DeviceFile getParent () {
    return parent;
  }

  /**
   * FOR TEST ONLY!!!!
   * Adds the specified DeviceFile to the list of files,
   * assuming this is a directory.
   * @param the new file to add.
   */
  public void addFile (DeviceFile file) {
    fileList.add(file);
  }

  /**
   * FOR TEST ONLY!!!!
   * Updates the size of the file
   * @param the new size.
   */
  public void setSize (int newSize) {
    size = newSize;;
  }

  /**
  * Returns a String representation of the FileData object
  */
  public String toString() {

    StringBuffer buf = new StringBuffer();
    buf.append("File ID: " + this.id + "\n");
    buf.append("File Name: " + getName() + "\n");
    if (getParent() != null) {
      buf.append("Parent File Name " + getParent().getName() + "\n");
    }
    else {
      buf.append ("Parent File: none\n");
    }
    String typeStr;
    if (getType() == DeviceFile.FILE_TYPE_ROOT_DIR) {
      typeStr = "Root directory";
    }
    else if (getType() == DeviceFile.FILE_TYPE_DIR) {
      typeStr = "Directory";
    }
    else if (getType() == DeviceFile.FILE_TYPE_GENERIC) {
      typeStr = "Generic file";
    }
    else if (getType() == DeviceFile.FILE_TYPE_JAD) {
      typeStr = "JAD file";
    }
    else if (getType() == DeviceFile.FILE_TYPE_JAR) {
      typeStr = "JAR file";
    }
    else if (getType() == DeviceFile.FILE_TYPE_RMS) {
      typeStr = "RMS file";
    }
    else {
      typeStr = "unknown";
    }
    buf.append("File Type: " + typeStr + "\n");
    buf.append("File Size: " + getSize() + "\n");

    return buf.toString();
  }

  /**
   * For directories, the function returns with the list of files in the directory.
   * For files, this function returns a null reference.
   * @return list of files in the directory.
   */
  public DeviceFile [] list() {
    DeviceFile [] fileArray = null;
    if (fileList != null && !fileList.isEmpty()) {
      fileArray = new DeviceFile[fileList.size()];
      for (int i=0; i< fileList.size(); i++) {
        fileArray[i] = (DeviceFile)fileList.get(i);
      }
    }
    return fileArray;
  }

   private int id;
   private DeviceFile parent;
   private String name;
   private int type;
   private int size;

   // for test only, SDK should keep count of the this:
   private Vector fileList;


}
