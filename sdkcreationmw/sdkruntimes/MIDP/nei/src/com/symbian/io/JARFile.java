/*
* Copyright (c) 2003 - 2004 Nokia Corporation and/or its subsidiary(-ies).
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

package com.symbian.io;

import java.io.*;
import java.util.*;
import java.util.jar.*;
import java.util.zip.ZipEntry;
import java.util.zip.ZipException;
import java.util.zip.ZipOutputStream;

import com.symbian.utils.Debug;

import com.symbian.utils.SEIManifest;


/**
 * Jar file.
 * This class does not extend java.util.JarFile which gives reading capabilities
 * but has no support for writing. Such inheritance would give no benefit and no
 * code reuse.
 *
 * <p>Copyright: Copyright (c) 2003 By Symbian Ltd. All Rights Reserved.</p>
 * <p>Company: Symbian Ltd</p>
 * @author Roy Ben Hayun
 * @version 1.1 (CLDC-HI)
 */
public class JARFile implements FilenameFilter
{
  //
  // Constants
  //

  /**
   * Jar file suffix
   */
  public static final String JAR_SUFFIX = ".jar";

  /**
   * Jar prefix (required for File.createTemp)
   */
  private static final String JAR_PREFIX = "seijar";



  //
  // Members
  //


  /**
   * Table of jar files types
   */
  private Set iJarFileTypes;

  /**
   * Stream piper utility for piping the files into the Jar
   */
  private StreamsPiper iPiper = new StreamsPiper();

  /**
   * File
   */
  private File iFile;





  //
  // Constructor
  //


  /**
   * Creates a temporary Jar file.
   *
   * @param aJarredFilesTypes set of file types
   *
   * @exception IOException if some error occured
   */

  /**
   * Creates a temporary Jar file which is deleted when the VM exits.
   *
   * @param aJarredFilesTypes set of file types to package
   * @param aSrcClassPath array of class path folders or jars
   * @param aJad file to use as Manifest
   *
   * @throws IOException if some error occured
   */
  public JARFile(Set aJarredFilesTypes,
                 String[] aSrcClassPath,
                 File aManifestFile) throws IOException
  {
    iFile = File.createTempFile(JAR_PREFIX, JAR_SUFFIX);
    iFile.deleteOnExit();
    // lowercase the extensions and strip the dots
    iJarFileTypes = new TreeSet();
    for (Iterator i=aJarredFilesTypes.iterator(); i.hasNext();)
    {
      Object entry = i.next();
      if (entry instanceof String)
      {
        String ext = ((String)entry).toLowerCase();
        iJarFileTypes.add(ext.substring(ext.lastIndexOf('.')+1));
      }
    }
    jarClassPath(aSrcClassPath, aManifestFile);
  }


  /**
   * Constructor.
   *
   * @param aJarFile exisiting Jar file
   *
   * @exception IllegalArgumentException if is not an exisiting jar file
   */
  public JARFile(File aJarFile)
  {
    if(isJarFile(aJarFile.getPath()))
    {
      iFile = aJarFile;
    }
    else
    {
      throw new IllegalArgumentException
          (aJarFile.getPath() + " is not a jar file");
    }
  }

  /**
   * Constructor.
   *
   * @param aJarFileName exisiting Jar file name
   *
   * @exception IllegalArgumentException if is not an exisiting jar file
   */
  public JARFile(String aJarFileName)
  {
    this(new File(aJarFileName));
  }

  //
  // Methods
  //






  /**
   * Jar all classpath into a single jar
   *
   * @param aSrcClassPath array of class path folders or jars
   * @param aJar destination jar
   * @param aManifestFile manifest file
   *
   * @throws IOException if some error occured
   */
  public void jarClassPath(String[] aSrcClassPath, File aManifestFile)
      throws IOException
  {
    //SunONE gives duplicate folders in the classpath.
    aSrcClassPath = removeDuplicateFolders(aSrcClassPath);
    JarOutputStream jos = null;
    try
    {
      File tmp;
      jos = new JarOutputStream(new FileOutputStream(iFile));
      iPiper.setOutputStream(jos);
      addManifest(aManifestFile, jos);
      for(int i = 0; i < aSrcClassPath.length; i++)
      {
        tmp = new File(aSrcClassPath[i]);
        if(tmp.isDirectory())
        {
          //recursively add all files to the jar
          jarDirectories("", tmp, jos);
        }
        else if (tmp.exists())
        {
          if(tmp.getName().toLowerCase().endsWith(JAR_SUFFIX))
          {
            byte buffer []= new byte[1024];
            JarFile jar = new JarFile(tmp);
            Enumeration jarEntries = jar.entries();
            while (jarEntries.hasMoreElements())
            {
              //JarEntry entry = (JarEntry)jarEntries.nextElement();
            	JarEntry entry = new JarEntry(((JarEntry)jarEntries.nextElement()).getName());
              // ignore manifest, it's added elsewhere:
              if (!entry.getName().toLowerCase().startsWith("meta-inf"))
              {
                InputStream is = jar.getInputStream(entry);
                try
                {
                  jos.putNextEntry(entry);
                  int read;
                  while((read = is.read(buffer)) > 0)
                  {
                    jos.write(buffer, 0, read);
                    jos.flush();
                  }
                }
                catch (ZipException x)
                {
                  // Ignore "duplicate entry" exceptions
                  Debug.println(this, "JARFile: ignoring "+tmp.getAbsolutePath()+
                    "!"+entry.getName());
                }
              }
            }
          }
        }
      }
    }
    finally
    {
      if(jos != null)
      {
        jos.close();
      }
    }

  }



  /**
   * Recursively Jar all contents of current directory
   * and its sub directories into jar
   *
   * @param aBase base folder name for Zip entries
   * @param aDir current directory
   * @param aJos Jar output stream to write to
   *
   * @exception if some error occured
   */
  private void jarDirectories(String aBase, File aDir, JarOutputStream aJos) throws IOException
  {
    File tmpFile;
    String[] files = aDir.list(this);
    if(files != null)
    {
      for(int i = 0; i < files.length; i++)
      {
        tmpFile = new File(aDir, files[i]);
        if(tmpFile.isDirectory())
        {
          //it's a directory. recurse into sub directories
          //NOTE: dont dare to use File.separator!
          jarDirectories(aBase + tmpFile.getName() + "/", tmpFile, aJos);
        }
        else
        {
          //it's a file which passed the approval of the FileNameFilter.
          FileInputStream fis = null;
          try
          {
            aJos.putNextEntry(new JarEntry(aBase + tmpFile.getName()));
            fis = new FileInputStream(tmpFile);
            iPiper.setInputStream(fis);
            iPiper.pipeAll();
          }
          catch (ZipException x)
          {
            // Ignore "duplicate entry" exceptions
            Debug.println(this,"JARFile: ignoring "+tmpFile.getAbsolutePath());
          }
          finally
          {
            if (fis != null)
            {
              fis.close();
            }
          }

        }
      }
    }
  }


  /**
   * This is a patch method as a workaround to avoid J2SE Manifest class
   */
  private void addManifest(File aManifestFile, JarOutputStream aJos) throws IOException{
    //it's a file which passed the approval of the FileNameFilter.
    JarEntry entry = new JarEntry("meta-inf/Manifest.mf");
    aJos.putNextEntry(entry);
    FileInputStream fis = null;
    try
    {
      fis = new FileInputStream(aManifestFile);
      iPiper.setInputStream(fis);
      iPiper.pipeAll();
    }
    finally
    {
      if (fis != null)
      {
        fis.close();
      }
    }
  }


  /**
   * Utilitity method for removing duplicate folders from the classpath which
   * will cause a ZipException thrown because of duplicate entries.
   *
   * @param aSrcClassPath array of class path folder
   *
   * @return String array of class path folder
   */
  private String[] removeDuplicateFolders(String[] aSrcClassPath)
  {
    // Use a list to preserve the order of entries in the class path
    Collection set = new ArrayList(aSrcClassPath.length);
    for(int i = 0; i < aSrcClassPath.length; i++)
    {
      if (!set.contains(aSrcClassPath[i]))
      {
        set.add(aSrcClassPath[i]);
      }
    }
    String[] res = new String[set.size()];
    set.toArray(res);
    return res;
  }




  /**
   * Get Jar file
   *
   * @return Jar file
   */
  public File getFile(){
    return iFile;
  }


  //
  // FileNameFilter implementation
  //

  /**
   * Tests if a specified file should be included in a file list
   *
   * @param aDir the directory in which the file was found
   * @param aName the name of the file
   *
   * @return true if and only if the name should be included in the file list
   */
  public boolean accept(File aDir, String aName)
  {
    if(iJarFileTypes == null)
    {
      return true;
    }
    if (new File(aDir, aName).isDirectory())
    {
      return true;
    }
    int dot = aName.lastIndexOf('.');
    String ext = ((dot >= 0) ? aName.substring(dot+1).toLowerCase() : "");
    return iJarFileTypes.contains(ext);
  }




  /**
   * Indicates if the given file is an existing jar file
   *
   * @param aJarFile jar file
   *
   * @return true if the file is an existing jar file, false otherwise.
   */
  public static boolean isJarFile(File aJarFile)
  {
    if(!aJarFile.exists() || !aJarFile.isFile())
    {
      return false;
    }
    return true;
  }

  /**
   * Indicates if the given file is an existing jar file
   *
   * @param aFileName jar file name
   *
   * @return true if the file is an existing jar file, false otherwise.
   */
  public static boolean isJarFile(String aFileName)
  {
    return isJarFile(new File(aFileName));
  }


}
