/*
* Copyright (c) 2000 - 2005 Nokia Corporation and/or its subsidiary(-ies).
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


package com.nokia.j2me.sei;

import java.io.File;
import com.symbian.tools.j2me.sei.SDKRuntimeProperties;

public class EmulatorSession extends DebugSession {

    /**
     * Creates instance of EmulatorSession
     */
    public EmulatorSession() {
        super(getExe());
    }

    /**
     * Returns the name of the SEI-Slave win32 executable
     * @return the name of the executable to start
     */
    private static String getExe() {
        return new File(SDKRuntimeProperties.EPOC_HOME + File.separator+
            ".." + File.separator + "epoc32" + File.separator +
            "release" + File.separator +
            SDKRuntimeProperties.PLATFORM + File.separator +
            SDKRuntimeProperties.VARIANT + File.separator +
            "DebugAgent.exe").getAbsolutePath();
    }
}
