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

import java.lang.Object;
import java.lang.String;

/**
 * EthernetMessage
 *
 * Emulator message format is following:
 * <JUSTINTIME><blank><justintime-val><blank><PANICS><blank><panics-val><DEBUGINFO><blank><debuginfo-val>
 *
 * where:
 * <blank> = blank character
 * <JUSTINTIME> = JUSTINTIME
 * <justintime-val> = 0 or 1 (0=off 1=on)
 * <PANICS> = PANICS
 * <panics-val> = 0 or 1 (0=off 1=on)
 * <DEBUGINFO> = DEBUGINFO
 * <debuginfo-val> = 0 or 1 (0=off 1=on)
 *
 * example:
 * JUSTINTIME 1 PANICS 1 DEBUGINFO 0
 *
 * @version 1.0
 */
public class EmulatorMessage extends Object {
    private final String body;
    private final String type;
    private final String errorMessage;
    // just in time
    private final boolean justInTime; // if false disabled
    // panics
    private final boolean panics; // if false disabled
    // debug info
    private final boolean debugInfo; // if false disabled
    
    /**
     * Creates a new instance of EmulatorMessage
     */
    private EmulatorMessage() {
        super();
        this.body = "";
        this.type = "";
        this.errorMessage = "";
        this.justInTime = false;
        this.panics = false;
        this.debugInfo = false;
    }
    
    /**
     * Creates a new instance of EmulatorMessage
     */
    private EmulatorMessage(String body, String type, String errorMessage, boolean justInTime, boolean panics, boolean debugInfo) {
        super();
        this.body = body;
        this.type = type;
        this.errorMessage = errorMessage;
        this.justInTime = justInTime;
        this.panics = panics;
        this.debugInfo = debugInfo;
    }
    
    /**
     * Creates a new instance of EmulatorMessage
     */
    public static EmulatorMessage create() {
        return new EmulatorMessage();
    }
    
    /**
     * Creates a new instance of EmulatorMessage
     */
    public static EmulatorMessage create(String body, String type, String errorMessage, boolean justInTime, boolean panics, boolean debugInfo) {
        return new EmulatorMessage(body, type, errorMessage, justInTime, panics, debugInfo);
    }
    
    /** Getter for property body.
     * @return Value of property body.
     *
     */
    public java.lang.String getBody() {
        return body;
    }
    
    /** Getter for property errorMessage.
     * @return Value of property errorMessage.
     *
     */
    public java.lang.String getErrorMessage() {
        return errorMessage;
    }
    
    /** Getter for property type.
     * @return Value of property type.
     *
     */
    public java.lang.String getType() {
        return type;
    }
    
    /** Getter for property justInTime.
     * @return Value of property justInTime.
     *
     */
    public boolean isJustInTime() {
        return justInTime;
    }
    
    /** Getter for property panics.
     * @return Value of property panics.
     *
     */
    public boolean isPanics() {
        return panics;
    }
    
    /** Getter for property debugInfo.
     * @return Value of property debugInfo.
     *
     */
    public boolean isDebugInfo() {
        return debugInfo;
    }
    
    /**
     * Returns a string representation of the object.
     * The toString method returns a string that "textually represents" this object.
     *
     * The toString method for class EthernetMessage returns a string consisting of
     * the body of the message
     *
     * @return a string representation of the object.
     */
    public String toString() {
        return "<ethernet-message body=\"" + this.body + "\"/>";
    }
    
}
