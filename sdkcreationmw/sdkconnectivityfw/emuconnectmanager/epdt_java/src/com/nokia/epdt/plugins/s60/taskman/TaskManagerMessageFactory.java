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




package com.nokia.epdt.plugins.s60.taskman;

import com.nokia.epdt.core.messaging.Message;

/**
 * TaskManagerMessageFactory
 *
 * TaskManager configuration message (from epdt to ecmt)
 * ---------------------------------------------------
 * <update><b><update-val><b><type><type-val>
 *
 * where:
 * <b>		= blank character
 * <update>	= UPDATE
 * <update-val>= Number from 0 to 5 inclusive. 0=pause, 1...5 update speed
 * 			  with 1=lowest 5=highest
 * <type>	= TYPE
 * <type-val>	= 0=applications data, 1=threads data
 *
 * Example TaskManager configuration message (from epdt to ecmt)
 * -----------------------------------------------------------
 * UPDATE 3 TYPE 0
 *
 * @version 1.0
 */
abstract class TaskManagerMessageFactory extends Object {
    private static final String BLANK = " ";
    private static final String UPDATE = "UPDATE";
    private static final String TYPE = "TYPE";

    /**
     * Creates a new instance of Message
     */
    static Message createMessage(int uid, int value, int type) {
        StringBuffer sB = new StringBuffer();
        sB.append(UPDATE);
        sB.append(BLANK);
        sB.append(value);
        sB.append(BLANK);
        sB.append(TYPE);
        sB.append(BLANK);
        sB.append(type);
        return new Message(uid , sB.toString().getBytes());
    }
}
