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
* Description: 
*
*/


package com.nokia.j2me.sei;

/* com.symbian.tools.j2me.sei */
import com.symbian.tools.j2me.sei.preferences.UserPreferences;
import com.symbian.tools.j2me.sei.emulator.session.target.SessionImpl;

/**
 * MIDP debugging session over WLAN
 */
public class WlanSession extends SessionImpl {

    /**
     * Initialize the session.
     * @param prefs user preferences used for initializing the connection
     * @throws Exception if some error occured
     */
    public void initialize(UserPreferences prefs) throws Exception {
        // We only support incoming connections. We provide no UI to configure
        // parameters for establishing outgoing connection to the handset.
        // Force use of incoming connections.
        prefs.getKDPPreferences().getSessionPreferences(getName()).setOutgoing(false);
        super.initialize(prefs);
    }
}
