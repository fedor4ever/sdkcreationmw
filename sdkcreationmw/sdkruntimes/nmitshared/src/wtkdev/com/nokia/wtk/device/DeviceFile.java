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
 * This interface provides an abstraction for the files and directories
 * stored in the persistent storage of the Device. With the help of this interface,
 * is is possible to represent entire filesystems in a hiararchical fashion.
 *
 */

public interface DeviceFile {

  /**
   * Returns the name of the file.
   * For the root directory, this is the name of the filesystem (e.g. "JAM Storage").
   * For the subdirectories, this is the name of the directory.
   * For files, this function returns the name of the file.
   * @return the name of the file.
   */
  public String getName ();

  /**
   * Returns the file Id.
   * @return the file Id.
   */
  public int getId ();

  /**
   * Returns the size of the file.
   * For the root directory, this is the size of the entire filesystem.
   * For subdirectories, this is the size of the directory.
   * For files, this function returns the size of the file.
   * @return the size of the file.
   */
  public int getSize ();


  /**
   * Returns the type of the file.
   * For the root directory, the function must return {@link #FILE_TYPE_ROOT_DIR}.
   * For subdirectories, the function must return {@link #FILE_TYPE_DIR}.
   * For files, this function returns the type of the file.
   * @return the type of the file.
   */
  public int getType ();


  /**
   * Returns the parent of the the file.
   * For the root directory, the function must return null.
   * For subdirectories, the function must return the reference to the root directory.
   * For files, this function returns the reference to the directory, where the file resides.
   * @return the parent of the the file.
   */
  public DeviceFile getParent ();


  /**
   * For directories, the function returns with the list of files in the directory.
   * For files, this function returns a null reference.
   * @return list of files in the directory.
   */
  public DeviceFile [] list();

  /* File types */

  /**
   * File type for the root directory. There should be only one Object
   * (the root directory) with the <code>FILE_TYPE_ROOT_DIR</code> type in one the
   * filesystem. The first level subdirectories and files in the root
   * directory must be a child of the root directory Object.
   * @see #getParent()
   */

   public static final int FILE_TYPE_ROOT_DIR = 0;

  /**
   * File type for directories.
   */

   public static final int FILE_TYPE_DIR = 1;

  /**
   * Generic file type. If the file system does not stores
   * any "meta-information" about the file type, this type should
   * be used.
   * Since the ISA handsets always store the type information together
   * with the content of the file, this type should not be used for
   * ISA handsets.
   */

   public static final int FILE_TYPE_GENERIC = 2;

  /**
   * File type for JAD (Java Application Descriptor) files.
   */

   public static final int FILE_TYPE_JAD = 3;

  /**
   * File type for JAR (Java Archive) files.
   */

   public static final int FILE_TYPE_JAR = 4;

  /**
   * File type for RMS (Record Management System) files.
   */

   public static final int FILE_TYPE_RMS = 5;
}
