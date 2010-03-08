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

/**
 * Cpu
 *
 * <cpu>	= CPU
 * <uptime>	= time since boot in microseconds
 * <usage>	= CPU usage percentage, 0...100
 *
 * @version 1.0
 */
public class Cpu extends Object {
    private final double uptime;
    private final int usage;
    
    /** Creates a new instance of Cpu */
    private Cpu(double uptime, int usage) {
        super();
        this.uptime = uptime;
        this.usage = usage;
    }
    
    /** Creates a new instance of Cpu */
    public static Cpu create(double uptime, int usage) {
        return new Cpu(uptime, usage);
    }
    
    /** Getter for property uptime.
     * @return Value of property uptime.
     *
     */
    public double getUptime() {
        return uptime;
    }
    
    /** Getter for property usage.
     * @return Value of property usage.
     *
     */
    public int getUsage() {
        return usage;
    }
    
    /**
     * Returns a string representation of the object.
     * The toString method returns a string that "textually represents" this object.
     * @return a string representation of the object.
     */
    public String toString() {
        return "<cpu uptime=\"" + this.uptime + "\" usage=\"" + this.usage + "\"/>";
    }
    
}
