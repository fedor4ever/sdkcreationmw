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

package com.symbian.tools.j2me.sei.emulator.session.commands;

import java.io.*;

import com.symbian.tools.j2me.sei.emulator.session.EpocErrorCodes;
import com.symbian.io.Dumpable;
import com.symbian.io.Loadable;
import com.symbian.utils.Debug;

public abstract class SessionCommand implements Loadable, Dumpable{

  //
  // Members
  //

  /**
   * Request identifier for serialization.
   */
  private final int iType;

  /**
   * Error code recieved from slave
   */
  private int iErrorCode;

  //
  // Life cycle
  //


  /**
   * Constructor.
   *
   * @param aType Request identifier for serialization
   */
  public SessionCommand(int aType) {
    iType = aType;
  }

  //
  // Operations
  //

  /**
   * Dump the object to the output stream
   *
   * @param aDos Data output stream
   *
   * @throws IOException if some error occured
   */
  public void dump(DataOutputStream aDos) throws IOException{
    aDos.writeInt(iType);
  }


  /**
   * Load the object from the stream
   *
   * @param aDis data input stream
   * @throws IOException if some error occured
   */
  public void load(DataInputStream aDis) throws IOException{
    iErrorCode = aDis.readInt();
    if(iErrorCode != EpocErrorCodes.KErrNone){
      Debug.println(this, "command failed with Symbian error code: " + iErrorCode);
      throw new RuntimeException("command failed with Symbian error code: " + iErrorCode);
    }
  }

  /**
   * Dump and Load
   *
   * @param aIn input stream
   * @param aOs output stream
   *
   * @throws IOException if some error occured
   */
  public void execute(DataInputStream aIn, DataOutputStream aOs)
      throws IOException{
    dump(aOs);
    aOs.flush();
    load(aIn);
  }
}
