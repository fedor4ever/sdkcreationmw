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



package com.nokia.wtk.util.wbxml;

/* java.io */
import java.io.PrintWriter;

/**
 * An XML Processing Instruction.
 *
 */
public class XMLProcessingInstruction extends WMLElement {

    /**
     * Creates a new XML Processing Instruction.
     * @param target the target of the processing instruction
     * @param data the optional parameterization for the processing instruction
     */
    XMLProcessingInstruction(String target, String data) {
        super(WML.Tag._PI_);
        this.target = target;
        this.data = data;
    }

    /**
     * Returns the target for this PI.
     */
    public String getTarget() {
        return this.target;
    }

    /**
     * Returns the data for this PI.
     */
    public String getData() {
        return this.data;
    }

    /**
     * Returns the string representation of this element for debugging purposes.
     */
    public String toString() {
        StringBuffer sbuf = new StringBuffer();
        sbuf.append("<?");
        if (getTarget() != null) {
            sbuf.append(' ');
            sbuf.append(getTarget());
        }
        if (getData() != null) {
            sbuf.append(' ');
            sbuf.append(getData());
        }
        sbuf.append("?>");
        return sbuf.toString();
    }

    /**
     * Return the string representation of this element.
     */
    public String getAsText() {
        return toString();
    }

    /**
     * Dumps the contents of this element.
     * @param w a text-output stream to use for output
     */
    public void dumpElementTag(PrintWriter w) {
        // Content elements display with no tag syntax:
        w.print(toString());
    }

    private String target;
    private String data;
}
