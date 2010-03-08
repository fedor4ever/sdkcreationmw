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




package com.nokia.epdt.core.connection;

/**
 * ConnectionStatus
 *
 * Connection status for indicate a status of the connection.
 *
 * @version 1.0
 */
public final class ConnectionStatus {
    public final static ConnectionStatus IDLE = new ConnectionStatus(0, "IDLE");
    public final static ConnectionStatus CONNECTING = new ConnectionStatus(1, "CONNECTING");
    public final static ConnectionStatus ESTABLISHED = new ConnectionStatus(2, "ESTABLISHED");
    public final static ConnectionStatus DISCONNECTING = new ConnectionStatus(4, "DISCONNECTING");
    public final static ConnectionStatus FAILED = new ConnectionStatus(8, "FAILED");

    private int value;
    private String description;

    /**
     * Creates a new instance of ConnectionStatus
     * @param value indicates a status of the connection
     * @param description textually indicates a status of the connection
     */
    private ConnectionStatus(int value, String description) {
        this.value = value;
        this.description = description;
    }

    /**
     * Getter for property description.
     * @return Value of property description.
     */
    public String getDescription() {
        return description;
    }

    /**
     * Getter for property value.
     * @return Value of property value.
     */
    public int getValue() {
        return value;
    }

    /**
     * Returns a string representation of the object.
     * The toString method returns a string that "textually represents"
     * this object.
     * @return a string representation of the object.
     */
    public String toString() {
        return description;
    }
}
