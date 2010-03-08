// Session.java
//
// Copyright (c) Symbian Software Ltd 2003 - 2004.  All rights reserved.
//
package com.symbian.tools.j2me.sei.emulator.session;

import java.util.*;
import java.io.*;
import java.net.*;

import com.symbian.tools.j2me.sei.emulator.commands.*;
import com.symbian.tools.j2me.sei.preferences.*;
import com.symbian.utils.*;
import com.symbian.io.*;
import com.symbian.vm.*;

/**
 * <p>Title: </p>
 * <p>Description: </p>
 * <p>Copyright: Copyright (c) 2004</p>
 * <p>Company: </p>
 * @author not attributable
 * @version 1.0
 */

public abstract class Session {

  //
  // Constants
  //

  /**
   * Mapping of device names to Session classes
   */
  private static final String SESSION_MAP_FILE = "device2session.properties";

  /**
   * Default session class to use, if the IDE did not specify device name.
   */
  private static final String DEFAULT_SESSION = "default";



  //
  // Statics
  //

  /**
   * Device 2 sessions
   */
  private static final Properties sDevices2Sessions = new Properties();


  //
  // Members
  //

  /**
   * Map of suite info -> VM Process
   * This member might be needed for MVM technology where a suites can run
   * isolated in a single VM.
   */
  protected Map iSuiteInfo2Process = new HashMap();

  /**
   * Session name (mapped to device name)
   */
  private String iName;

  /**
   * Session preferences
   */
  protected SessionPreferences iPrefs;



  //
  // Life cycle
  //

  /**
   * Static init
   *
   * Load mapping from device names to session implementation
   * (done here to enable test/customized sessions)
   */
  static
  {
    try
    {
    sDevices2Sessions.load(Session.class.getResourceAsStream(SESSION_MAP_FILE));
    }
    catch(Exception e){
      Debug.printStackTrace(Session.class.getName(), e);
    }
  }

  /**
   * SessionImpl factory method
   *
   * @param aName Session name
   * @return Session concrete SessionImpl
   */
  public static final Session create(String aName) throws Exception{
    Debug.println(Session.class.getName(), "SEI device: " + aName);
    if(aName == null){
      aName = sDevices2Sessions.getProperty(DEFAULT_SESSION);
    }
    final Session session = (Session)
        Class.forName(sDevices2Sessions.getProperty(aName)).newInstance();
    session.iName = aName;
    return session;
  }




  //
  // Operations
  //

  /**
   * Maps KVM (possibly, handset) port number to local port.
   *
   * @param aVmPort the port to map.
   * @return possibly different port number than can be used within this VM
   */
  public int mapVmPort(int aVmPort) {
      return aVmPort; // no mapping by default
  }

  /**
   * Gets the session name (the name of the debug configuration)
   * @return the session name.
   */
  public String getName() {
      return iName;
  }

  /**
   * Initialize the session.
   * The intialization code shouldnt be in the constructor to allow shutdown
   * during initialization, coming from the user.
   *
   * @param aUserPrefs user preferences used for initializing the connection
   *
   * @throws Exception if some error occured
   */
  public synchronized void initialize(UserPreferences aUserPrefs) throws Exception{
    iPrefs = aUserPrefs.getKDPPreferences().getSessionPreferences(iName);
    Debug.println(this, "SEI session: " + getClass());
    Debug.println(this, "session prefs: " + iPrefs.toString());
  }



  /**
   * Get a free port on the device
   *
   * @return int free port
   *
   * @throws Exception if some error occured
   */
  public abstract int getFreePort() throws Exception;



  /**
   * Start the MIDlet
   *
   * @param aMIDletInfo MIDlet to start
   *
   * @return VM process
   *
   * @throws Exception if some error occured
   */
  public abstract VMProcess startMIDlet(VMLaunchArguments aVMLaunchArguments, boolean aDefered) throws Exception;


  /**
   * Start another MIDlet in the same suite VM which must be already running
   *
   * @param aMIDletInfo MIDlet info
   *
   * @throws Exception if some error occured
   */
  public abstract void startMIDlet(MIDletInfo aMIDletInfo) throws Exception;


  /**
   * Install the MIDlet suite
   *
   * @param aJar MIDlet suite Jar file
   * @param aJad MIDlet suite Jad file
   * @param aTrusted trust mode indicator
   *
   * @return MIDletSuiteInfo MIDlet suite information
   *
   * @throws Exception if some error occured
   */
  public abstract MIDletSuiteInfo installMIDletSuite
      (JARFile aJar, JADFile aJad, boolean aTrusted) throws Exception;


  /**
   * Remove the MIDlet suite
   *
   * @param aSuiteInfo removed MIDlet suite info
   *
   * @throws Exception if some error occured
   */
  public abstract void removeMIDletSuite(MIDletSuiteInfo aSuiteInfo) throws Exception;


  /**
   * Remove all MIDlet suites
   *
   * @throws IOException if some error occured
   */
  public synchronized void removeAll() throws Exception {
    //un-install suites
    //for each running VM
    for (Iterator iter = iSuiteInfo2Process.keySet().iterator(); iter.hasNext(); )
    {
      removeMIDletSuite((MIDletSuiteInfo)iter.next());
    }
  }

  /**
   * Get the list of installed MIDlet suites
   *
   * @return list of installed MIDlet suites
   *
   * @throws Exception if some error occured
   */
  public abstract MIDletSuiteInfo[] getInstalledMIDletSuitesInfo() throws Exception;


  /**
   * Get the MIDlet VM process
   *
   * @param aMIDletInfo MIDlet info
   *
   * @return MIDlet VM process
   */
  public synchronized VMProcess getProcess(MIDletInfo aMIDletInfo){
    return (VMProcess)iSuiteInfo2Process.get(aMIDletInfo.getMIDletSuiteInfo());
  }

  /**
   * Brute force terminate the sessions
   */
  public abstract void terminate();

  /**
   * Get SEI-Slave TCP Inet address
   *
   * @return SEI-Slave TCP Inet address
   *
   * @exception UnknownHostException if failed to resolve the host name
   */
  public abstract InetAddress getSlaveInetAddress() throws UnknownHostException;

}









