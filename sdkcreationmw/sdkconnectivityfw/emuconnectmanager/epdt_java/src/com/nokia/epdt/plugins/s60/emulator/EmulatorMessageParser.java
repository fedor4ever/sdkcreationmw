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

import java.text.MessageFormat;
import java.text.ParseException;
import java.io.UnsupportedEncodingException;

import org.apache.log4j.Logger;

import com.nokia.epdt.core.messaging.Message;

/**
 * EmulatorMessageParser
 *
 * @version 1.0
 */
public class EmulatorMessageParser extends Object {
    private static Logger log = Logger.getLogger(EmulatorMessageParser.class);
    private static MessageFormat valueMessageFormat = new MessageFormat("JUSTINTIME {0,number,integer} PANICS {1,number,integer} DEBUGINFO {2,number,integer}");
    private static MessageFormat okMessageFormat = new MessageFormat("OK");
    private static MessageFormat errorMessageFormat = new MessageFormat("ERROR {0}");
    private static String charsetName = "UTF8";
    
    /**
     * Creates a new instance of EmulatorMessageParser
     */
    private EmulatorMessageParser() {
        super();
    }
    
    /**
     * Parse a message
     */
    public static EmulatorMessage parseMessage(Message message) {
        EmulatorMessage result = EmulatorMessage.create();
        String body = "";
        String type = "";
        String errorMessage = "";
        // just in time
        boolean justInTime = false; // if false disabled
        // panics
        boolean panics = false; // if false disabled
        // debug info
        boolean debugInfo = false; // if false disabled
        
        try {
            body = new String(message.getBody(), charsetName);
            
            if (log.isDebugEnabled()) {
            	log.debug("Parsing body: " + body);
            }
            
            Object[] o = null;
            
            try {
                o = valueMessageFormat.parse(body);
                type = EmulatorConstants.VALUE_TYPE_EMULATOR_MESSAGE;
                try {
                    justInTime = ((Long)o[0]).intValue() == 0 ? false : true;
                    panics = ((Long)o[1]).intValue() == 0 ? false : true;
                    debugInfo = ((Long)o[2]).intValue() == 0 ? false : true;
                } catch (ClassCastException cce) {
                    // parsing failed
                    throw new ParseException(null, 0);
                }
            } catch (ParseException pe_0) {
                // parsing failed
                try {
                    o = okMessageFormat.parse(body);
                    type = EmulatorConstants.OK_TYPE_EMULATOR_MESSAGE;
                } catch (ParseException pe_1) {
                    // parsing failed
                    try {
                        o = errorMessageFormat.parse(body);
                        type = EmulatorConstants.ERROR_TYPE_EMULATOR_MESSAGE;
                        try {
                            errorMessage = (String)o[0];
                        } catch (ClassCastException cce) {
                            // parsing failed
                            throw new ParseException(null, 0);
                        }
                    } catch (ParseException pe_2) {
                        // parsing failed
                    }
                }
            }
        } catch (UnsupportedEncodingException uee) {
            // parsing failed
        }
        result = EmulatorMessage.create(body, type, errorMessage, justInTime, panics, debugInfo);
        return result;
    }
    
}
