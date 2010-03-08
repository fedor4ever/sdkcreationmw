/*
* Copyright (c) 1999 - 2004 Nokia Corporation and/or its subsidiary(-ies).
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

import junit.framework.*;
import java.io.*;

public class TestMIDletInfo extends TestCase {
  private int iID = 1;
  private String iName = "Name";
  private MIDletSuiteInfo iSuiteInfo;
  private MIDletInfo iInfo = new MIDletInfo(iName, iID, iSuiteInfo);

  public TestMIDletInfo(String name) throws IOException{
    super(name);
  }



  public void testGetID() {
    assertEquals("Wrong MIDlet ID", iID, iInfo.getID());
  }

  public void testGetName() {
    assertEquals("Wrong MIDlet Name", iName, iInfo.getMIDletName());
  }

  public void testGetMIDletSuiteInfo() {
    assertEquals("Wrong Suite", iSuiteInfo, iInfo.getMIDletSuiteInfo());
  }

  public void testLoadDump() throws IOException {
    throw new UnsupportedOperationException("Load,Dump not implemented yet in MIDletInfo class.");
//    ByteArrayOutputStream bos = new ByteArrayOutputStream();
//    iInfo.dump(new DataOutputStream(bos));
//    ByteArrayInputStream bis = new ByteArrayInputStream(bos.toByteArray());
//    MIDletInfo info2 = new MIDletInfo(new DataInputStream(bis));
//    assertEquals("IDs are not equal", info2.getID(), iInfo.getID());
//    assertEquals("Suite info are not equal", info2.getMIDletSuiteInfo(), iInfo.getMIDletSuiteInfo());
  }

}
