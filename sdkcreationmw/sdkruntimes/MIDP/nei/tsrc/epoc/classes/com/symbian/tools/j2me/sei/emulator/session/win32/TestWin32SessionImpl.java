/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

package com.symbian.tools.j2me.sei.emulator.session.win32;

import junit.framework.*;
import com.symbian.tools.j2me.sei.emulator.session.*;
import com.symbian.tools.j2me.sei.emulator.session.win32.SessionImpl;
import com.symbian.tools.j2me.sei.*;

public class TestWin32SessionImpl extends TestSessionImpls_BASE
{

  public TestWin32SessionImpl()
  {
    super("com.symbian.tools.j2me.sei.emulator.session.win32.SessionImpl");
    iHost = SDKTestsProperties.WIN32_SESSION_HOST;
    iSessionName = "win32";
  }


}
