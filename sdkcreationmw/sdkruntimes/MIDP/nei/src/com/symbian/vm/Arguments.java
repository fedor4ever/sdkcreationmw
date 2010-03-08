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

package com.symbian.vm;


import java.io.*;
import com.symbian.io.Dumpable;
/**
 * <p>Title: </p>
 * <p>Description: </p>
 * <p>Copyright: Copyright (c) 2004 By Symbian Ltd. All Rights Reserved</p>
 * <p>Company: Symbian Ltd</p>
 * @author Roy Ben Hayun
 * @version 2.0
 */

class Arguments implements Dumpable{


  //
  // Members
  //

  /**
   * Arguments string
   */
  protected StringBuffer iArguments = new StringBuffer();

  //
  // Operations
  //

  /**
   * Get arguments string
   *
   * @return arguments string
   */
  public String getArguments(){
    return iArguments.toString();
  }




  //
  // Dumpable implementation
  //

  /**
   * Dump the object to the stream.
   *
   * @param aDos Data output stream
   *
   * @throws IOException if some error occured
   */
  public void dump(DataOutputStream aDos) throws IOException{
    aDos.writeUTF(iArguments.toString());
    aDos.flush();
  }

}