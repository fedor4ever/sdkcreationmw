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
import com.symbian.vm.*;

public class HandshakeCommand extends SessionCommand{


  //
  // Constants
  //


  /**
   * Handshake request identifier
   */
  private static final int HANDSHAKE_CMD_ID = 8;


  //
  // Members
  //

  /**
   * Challenge is set by the user and sent at the beginning of the session
   */
  private String iChallenge;


  //
  // Life cycle
  //

  /**
   * Default constructor.
   *
   * @param aChallenge challenge string used to authenticate the session
   */
  public HandshakeCommand(String aChallenge){
    super(HANDSHAKE_CMD_ID);
    iChallenge = aChallenge;
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
    aDos.writeUTF(iChallenge);
    aDos.flush();
  }

}
