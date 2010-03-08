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

package com.symbian.tools.j2me.sei.emulator.session;

import com.symbian.vm.*;

class RemoteVMProcessImpl extends VMProcess{

  /**
   * Owner session
   */
  private RemoteSession iSession;

  /**
   * Constructor
   *
   * @param aSession Owner session
   * @param aSuiteInfo suite info
   */
  public RemoteVMProcessImpl(RemoteSession aSession, MIDletSuiteInfo aSuiteInfo){
    super(aSuiteInfo);
    iSession = aSession;
  }



  /**
   * Brute force destroy the remote process
   */
  public void destroy(){
    iSession.terminate();

  }
}
