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

public class GetPortCommand extends SessionCommand{

  //
  // Constants
  //

  /**
   * Get port commmand ID
   */
  public static final int GET_PORT_CMD_ID = 7;


  //
  // Members
  //

  /**
   * Free port
   */
  private int[] iFreePorts;



  //
  // Life cycle
  //

  /**
   * Constructor
   *
   * @aNum number of free ports
   */
  public GetPortCommand(int aNum){
    super(GET_PORT_CMD_ID);
    iFreePorts = new int[aNum];
  }


  //
  // Operations
  //


  /**
   * Get free ports
   *
   * @return array of free ports numbers
   */
  public int[] getFreePorts(){
    return iFreePorts;
  }


  /**
   * Dump the object to the output stream
   *
   * @param aDos Data output stream
   *
   * @throws IOException if some error occured
   */
  public void dump(DataOutputStream aDos) throws IOException{
    super.dump(aDos);
    aDos.writeInt(iFreePorts.length);
  }



  /**
   * Load the object from the stream
   *
   * @param aDis data input stream
   * @throws IOException if some error occured
   */
  public void load(DataInputStream aDis) throws IOException{
    for(int i = 0; i < iFreePorts.length; i++){
      iFreePorts[i] = aDis.readInt();
    }
  }

}
