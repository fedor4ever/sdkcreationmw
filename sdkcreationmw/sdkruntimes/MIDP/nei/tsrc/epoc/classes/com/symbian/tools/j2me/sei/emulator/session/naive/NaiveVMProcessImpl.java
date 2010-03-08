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

package com.symbian.tools.j2me.sei.emulator.session.naive;

import com.symbian.vm.*;

class NaiveVMProcessImpl extends VMProcess
{

  //
  // Members
  //

  /**
   * Win32 VM process
   */
  private final Process iWin32Process;

  //
  // Life cycle
  //

  /**
   * Constructor
   *
   * @param aProcess win32 VM process
   * @param aSuiteInfo suite info
   */
  NaiveVMProcessImpl(Process aProcess, MIDletSuiteInfo aSuiteInfo)
  {
    super(aSuiteInfo);
    iWin32Process = aProcess;
  }

  //
  // Operations
  //

  /**
   * Kills the subprocess.
   * The subprocess represented by this object is forcibly terminated.
   */
  public void destroy()
  {
    if (iWin32Process != null)
    {
      iWin32Process.destroy();
    }
  }

  /**
   * Returns the exit value for the subprocess.
   *
   * @return  the exit value of the subprocess
   *
   * @exception  IllegalThreadStateException  if the subprocess represented
   *             by this <code>Process</code> object has not yet terminated.
   */
  public int exitValue()
  {
    if (iWin32Process != null)
    {
      return iWin32Process.exitValue();
    }
    else
    {
      return -1;
    }
  }

  /**
   * Indicates if this process is still alive
   *
   * @return true if this process is alive, false otherwise.
   */
  public boolean isAlive()
  {
    if (iWin32Process != null)
    {
      try
      {
        iWin32Process.exitValue();
        return false;
      }
      catch (IllegalThreadStateException e)
      {
        return true;
      }
    }
    else
    {
      return true;
    }
  }

}
