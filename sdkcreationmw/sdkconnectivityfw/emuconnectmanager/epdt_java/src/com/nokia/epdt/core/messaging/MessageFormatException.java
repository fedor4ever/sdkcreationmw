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




package com.nokia.epdt.core.messaging;

import java.lang.Exception;

/**
 * MessageFormatException
 *
 * @version 1.0
 */
public class MessageFormatException extends Exception {
    
    /**
     * Creates a new instance of MessageFormatException
     */
    public MessageFormatException() {
        super();
    }
    
    /**
     * Creates a new instance of MessageFormatException
     */
    public MessageFormatException(String message) {
        super(message);
    }
    
}
