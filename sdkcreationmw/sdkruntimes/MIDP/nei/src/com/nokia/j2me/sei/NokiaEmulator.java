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
* Description:  Implements Emulator interface, required by Sun's WTK
*
*/


package com.nokia.j2me.sei;

import java.util.Properties;
import com.sun.kvem.environment.Emulator;

import com.symbian.tools.j2me.sei.emulator.Main;
import com.symbian.utils.SystemExitHandler;

/**
 * Implements Emulator interface for Sun's WTK.
 */
public class NokiaEmulator implements com.sun.kvem.environment.Emulator {

    /**
     * Implements Emulator interface for Sun's WTK.
     * @param props some properties
     * @param argv command line arguments for the emulator
     * @return zero
     */
    public int run(Properties props, String[] argv) {
        //Setting ExitHandler not to stop WTK's JVM on exit:
        SystemExitHandler.setExitOperation(false);
        Main.main(argv);
        return 0;
    }
}
