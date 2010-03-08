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

package com.symbian.tools.j2me.sei.preferences;

import java.io.*;

/**
 * These command line are for internal use and for licensees only.
 * They should remain immutable to 3rd part developers.
 * NOTE: the SEI test variant includes UI for dynamically setting.
 *
 * <p>Title: </p>
 * <p>Description: </p>
 * <p>Copyright: Copyright (c) 2004 By Symbian Ltd. All Rights Reserved</p>
 * <p>Company: Symbian Ltd</p>
 * @author Roy Ben Hayun
 * @version 2.0
 */
public class CommandLinePreferences implements Serializable
{

  //
  // Members
  //


  /**
   * Bootstrap classpath
   */
  private String iBootStrapClasspath;

  /**
   * KDP log level.
   * NOTE: on some J2SE versions, KDP log into files might cause the VM to hang.
   * also, the 3rd party developers should have no interest in KDWP.
   */
  private int iKDPLogLevel;

  /**
   * Add Symbian logging extensions
   */
  private boolean iKDPExtraLogs = false;

  /**
   * Enable file logging indicator
   */
  private boolean iEnableLogFile;

  /**
   * Mute all logs
   */
  private boolean iMuteAllLogs;

  //
  // Life cycle
  //

  /**
   * Default constructor.
   */
  public CommandLinePreferences()
  {
    setDefaultValues();
  }

  //
  // Operations
  //


  /**
   * Set default values
   */
  protected void setDefaultValues()
  {
    iBootStrapClasspath = null;
    iKDPLogLevel = 0;
    iEnableLogFile = true;
    iMuteAllLogs = false;
    iKDPExtraLogs = false;
  }

  /**
   * Get bootstrap classpath
   *
   * @return bootstrap classpath
   */
  public String getBootStrapClasspath()
  {
    return iBootStrapClasspath;
  }

  /**
   * Get bootstrap classpath
   *
   * @param aPath path to bootstrap classes
   */
  public void setBootStrapClasspath(String aPath)
  {
    iBootStrapClasspath = aPath;
  }

  /**
   * Get log file indicator
   *
   * @return true if to log into a file
   */
  public boolean isLogFileEnabled()
  {
    return iEnableLogFile;
  }

  /**
   * Set log file indicator
   *
   * @param log file indicator
   */
  public void setLogFileEnabled(boolean aEnabled)
  {
    iEnableLogFile = aEnabled;
  }

  /**
   * Get KDP log level
   *
   * @return int KDP log level
   */
  public int getKDPLogLevel()
  {
    return iKDPLogLevel;
  }

  /**
   * Get KDP log level
   *
   * @param aLevel int KDP log level
   */
  public void setKDPLogLevel(int aLevel)
  {
    iKDPLogLevel = aLevel;
  }

  /**
    * Get KDP Extra logs indicator
    *
    * @return true if extra logs enabled
    */
   public boolean isKDPExtraLogs()
   {
     return iKDPExtraLogs;
   }

   /**
     * Set KDP Extra logs indicator
     *
     * @param extra logs indicator
     */
   public void setKDPExtraLogs(boolean aEnabled)
   {
     iKDPExtraLogs = aEnabled;
   }

  /**
   * Get logs indicator
   *
   * @return true if logging enabled
   */
  public boolean isLoggingEnabled()
  {
    return iMuteAllLogs;
  }

  /**
   * Get logs indicator
   *
   * @return true if logging enabled
   */
  public void setLoggingEnabled(boolean aEnabled)
  {
    iMuteAllLogs = aEnabled;
  }

}
