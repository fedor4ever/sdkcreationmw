/*
* Copyright (c) 2003 - 2004 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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

package com.symbian.vm;


import com.symbian.tools.j2me.sei.emulator.commands.*;

/**
 * <p>Title: </p>
 * <p>Description: </p>
 * <p>Copyright: Copyright (c) 2004 By Symbian Ltd. All Rights Reserved</p>
 * <p>Company: Symbian Ltd</p>
 * @author Roy Ben Hayun
 * @version 2.0
 */

public class VMArguments extends Arguments{

  //
  // Constants
  //

  /**
   * Debugger argument
   */
  private static final String DEBUG_ARG = "-debugger";

  /**
   * Debugger port argument
   */
  private static final String DEBUG_PORT_ARG = "-port";

  /**
   * Suspended
   */
  private static final String UNSUSPENDED = "-nosuspend";

  /**
   * Console port
   */
  //private static final String CONSOLE_PORT_ARG = "-vp";
  private static final String CONSOLE_PORT_ARG = "-ide_io_port";

  /**
   * Bootstrap classpath
   */
  private static final String BOOTSTRAP = "-bcp";

  //
  // Members
  //

  /**
   * VM debug port
   */
  private int iDebugPort;

  /**
   * VM verbose port
   */
  private int iConsolePort;



  //
  // Life cycle
  //

  /**
   * Default constructor.
   */
  public VMArguments() {
  }

  //
  // Operations
  //

	/**
	 * Method has been updated to set the -Xdebug and -Xrunjdwp as part of VM argument for MIDlet debugging
	 *
	 * @param aDebugPort int debug port
	 * @param peramXdebug String -Xdebug parameter
	 * @param peramXrunjdwp String -Xrunjdwp parameter
	 */
	public void setVMDebugPort(int aDebugPort, String peramXdebug,
			String peramXrunjdwp) {
    iDebugPort = aDebugPort;
    
		// Previous args: Ignoring it....
		//iArguments.append(" -ide_server_mode "+ (isVmInServerMode?"y":"n"));
		//iArguments.append(" -ide_port ").append(iDebugPort);

		iArguments.append(peramXdebug + " ");
		iArguments.append(peramXrunjdwp);

  }

  /**
   * Get VM debug port
   *
   * @return VM debug port
   */
  public int getVMDebugPort(){
    return iDebugPort;
  }

  /**
   * Set VM console port.
   *
   * @param VM console port
   */
	// Pranav: Method is no longer useful - Keep it for time being and will remove in future.
  public void setVMConsolePort(int aConsolePort) {
    iConsolePort = aConsolePort;
    iArguments.append(" -ide_addr 127.0.0.1");
    iArguments.append(" ").append(CONSOLE_PORT_ARG)
        .append(" ").append(iConsolePort);
  }

  /**
   * Get VM console port
   *
   * @return VM console port
   */
	// Pranav: Method is no longer useful - Keep it for time being and will remove in future.
  public int getVMConsolePort(){
    return iConsolePort;
  }


  /**
   * Add verbose type argument
   *
   * @param aVerboseType new verbose type
   */
  public void setVerboseType(String aVerboseType){
    iArguments.append(aVerboseType);
  }

  /**
   * Set the VM to be in debug/non-suspended mode
   */
  public void setUnsuspendedMode() {
    iArguments.append(" ").append(UNSUSPENDED);
  }

  /**
   * Use non-romized bootstrap classes
   *
   * @param aBootstrapClassaPath bootstrap classes classpath
   */
  public void setBootstrapClassPath(String aBootstrapClassaPath){
    iArguments.append(" ").append(BOOTSTRAP)
        .append(" ").append(aBootstrapClassaPath);
  }

}
