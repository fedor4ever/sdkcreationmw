/*
* Copyright (c) 2000 - 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  MIDP debugging
 *
*/


package com.nokia.epdt.plugins.s60.seidebug;

interface UIConstants {

    static final int GAP = 6;
    static final String NAME = "MIDP Debugging";

    // Emulator box
    static final String EMULATOR_PORT_LABEL = "Emulator port:";
    static final String EMULATOR_ATTACH_ENABLE = "Keep emulator running";
    static final String EMULATOR_ATTACH_TEXT = "If you choose to keep "
        + "emulator running, then emulator will listen on the TCP port "
        + "specified below so that IDE can re-attach to the emulator when "
        + "you start a new MIDP debug session.";

    // Bluetooth box
    static final String DEVICE_TEXT = "You must run the "
        + "Device Connectivity Tool and establish connection with your "
        + "S60 device before you can use MIDP debugging over USB, Bluetooth or WLAN. "
        + "You also must have Debug Agent installed and running on the "
        + "device. Please refer to the SDK documentation for more details.";

     // Properties
    static final String PROP_ATTACH_TO_EMULATOR = "emulator-attach";
    static final String PROP_EMULATOR_PORT = "emulator-port";

}
