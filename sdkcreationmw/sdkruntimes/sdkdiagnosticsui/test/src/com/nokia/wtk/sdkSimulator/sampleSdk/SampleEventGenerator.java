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

// java.util
import java.util.Vector;

// com.nokia.wtk.device
import com.nokia.wtk.device.DeviceSampleData;

// com.nokia.wtk.sdkSimulator.data
import com.nokia.wtk.sdkSimulator.data.SampleData;

// javax.swing
import javax.swing.Icon;
import javax.swing.ImageIcon;

// java.io
import java.io.File;
import java.io.FilenameFilter;

/**
 * <p>Title: SampleEventGenerator </p>
 * <p>Description: A demo class to generate events for the sample view
 * <p>Company: Nokia</p>
 * @version 1.0
 */
public class SampleEventGenerator  {

  /**
  * Constructor:  loads the images from the data folder
  * and puts them in the images Vector
  */
  public SampleEventGenerator() {

    // create the images Vector to hold the data
    images = new Vector();
    count = 0;

    // select the .gif files from the sampledata folder
    String path = System.getProperty(USER_DIR) + DATA_DIR;
    String[] gifFiles = getGifFileNames(path);

    // create an Icon from each file and store it in the Vector
    if (gifFiles != null && gifFiles.length > 0) {
      for (int i=0; i<gifFiles.length; i++) {
        ImageIcon icon = new ImageIcon(path + "\\" + gifFiles[i]);
        if (icon != null) {
          images.add (icon);
        }
      }
    }
  }

  /**
   * returns a list of file names, given a directory, and a name filter
   * @param dir the directory where to search for the files
   * @return an array of file names
   */
  private String [] getGifFileNames(String dir)   {
    String[] fileNames = null;
    File filesDir = new File (dir);
    if (filesDir != null && filesDir.exists() && filesDir.isDirectory()) {
      fileNames = filesDir.list( new FilenameFilter() {
        public boolean accept( File dir, String name) {
          boolean accept = false;
          if ( name.endsWith(GIF_FILTER) ) {
            accept = true;
          }
          return accept;
        }
      } );
    }
    return fileNames;
  }

  /**
   * returns the next Icon object in the Vector. If no more objects,
   * starts again from the first one
   * @return the next icon encapsulated in an object of type DeviceSampleData
   */
  public DeviceSampleData getNextSample() {
    Icon icon = null;
    if (images != null && !(images.isEmpty())) {
      icon = (Icon)images.get(count);
      count ++;
      if (count >= images.size()) {
        count = 0;
      }
    }
    return new SampleData(icon);
  }


  // data
  /**
   * A Vector that holds the list of immages
   */
  private Vector images = null;

    private int count = 0;
  private static final String USER_DIR = "user.dir";
  private static final String DATA_DIR = "\\demoData\\sampleView";
  private static final String GIF_FILTER = ".gif";

}
