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




package com.nokia.wtk.device;


/**
 * A device-related exception.
 *
 * @deprecated This class is no longer needed and should not be used in new code.
 *             Instead, just use <code>RuntimeException</code>.
 */
public class DeviceException extends RuntimeException {

    /**
     * Creates a new <code>DeviceException</code>.
     *
     * @param msg       the detail message
     */
    public DeviceException(String msg) {
        super(msg);
    }

    /**
     * Creates a new <code>DeviceException</code> from an existing exception.
     *
     * @param ex        the nested exception
     */
    public DeviceException(Throwable ex) {
        super();
        this.exception = ex;
    }

    /**
     * Creates a new <code>DeviceException</code> from an existing exception.
     *
     * @param msg       the detail message
     * @param ex        the nested exception
     */
    public DeviceException(String msg, Throwable ex) {
        super(msg);
        this.exception = ex;
    }

    /**
     * Returns a detail message for this exception.
     *
     * If there is an embedded exception, and if this exception
     * has no detail message of its own, this method will return
     * the detail message from the embedded exception.
     *
     * @return          The detail message for this exception.
     */
    public String getMessage() {
        String message = super.getMessage();

        if ((message == null) && (this.exception != null)) {
            return this.exception.getMessage();
        } else {
            return message;
        }
    }

    /**
     * Returns the embedded exception, if any.
     *
     * @return          The embedded exception, or <code>null</code> if non-existent.
     */
    public Throwable getException () {
        return exception;
    }

    /**
     * Overrides <code>toString()</code> to pick up any embedded exception.
     *
     * @return          A string representation of this exception.
     */
    public String toString() {
        if (this.exception != null) {
            return this.exception.toString();
        } else {
            return super.toString();
        }
    }


    /**
     * The embedded exception.
     */
    private Throwable exception = null;
}
