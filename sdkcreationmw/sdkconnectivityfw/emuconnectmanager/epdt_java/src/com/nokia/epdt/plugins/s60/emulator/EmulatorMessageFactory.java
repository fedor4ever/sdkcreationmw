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




package com.nokia.epdt.plugins.s60.emulator;

import com.nokia.epdt.core.messaging.Message;

/**
 * EmulatorMessageFactory
 *
 * Emulator message format is following:
 * <MEMORY><blank><mem-val><blank><JUSTINTIME><blank><justintime-val><blank><PANICS><blank><panics-val><DEBUGINFO><blank><debuginfo-val>
 *
 * where:
 * <blank> = blank character
 * <MEMORY> = MEMORY
 * <mem-val> = 0-4096
 * <JUSTINTIME> = JUSTINTIME
 * <justintime-val> = 0 or 1 (0=off 1=on)
 * <PANICS> = PANICS
 * <panics-val> = 0 or 1 (0=off 1=on)
 * <DEBUGINFO> = DEBUGINFO
 * <debuginfo-val> = 0 or 1 (0=off 1=on)
 *
 * example:
 * MEMORY 32 JUSTINTIME 1 PANICS 1 DEBUGINFO 0
 *
 * @version 1.0
 */
public abstract class EmulatorMessageFactory {
    private static final String BLANK = " ";
    private static final String JUSTINTIME = "JUSTINTIME";
    private static final String PANICS = "PANICS";
    private static final String DEBUGINFO = "DEBUGINFO";

    /**
     * Disables instantiation
     */
    private EmulatorMessageFactory() {
        throw new Error("EmulatorMessageFactory");
    }

    /**
     * Creates a new instance of Message
     * @throws ValidateException if validation fails
     */
    public static Message createMessage(int uid) throws ValidateException {
        EmulatorModel eM = EmulatorModel.getInstance();
        try {
            eM.validate();
        } catch (ValidateException ve) {
            throw ve;
        }
        StringBuffer sB = new StringBuffer();
        sB.append(JUSTINTIME);
        sB.append(BLANK);
        sB.append(eM.isJustInTime() ? "1" : "0");
        sB.append(BLANK);
        sB.append(PANICS);
        sB.append(BLANK);
        sB.append(eM.isPanics() ? "1" : "0");
        sB.append(BLANK);
        sB.append(DEBUGINFO);
        sB.append(BLANK);
        sB.append(eM.isDebugInfo() ? "1" : "0");
        return new Message(uid , sB.toString().getBytes());
    }

}
