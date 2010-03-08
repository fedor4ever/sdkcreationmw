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
 * Application
 *
 * <name>	= name of application (contains no blanks, no ';')
 * <id>		= id of primary thread of application
 * <mem-used>	= amount of memory used by application
 * <mem-reserved> = amount of memory reserved for application
 * <stack>	= stack size of application
 * <cpu-time>	= application CPU usage in microseconds
 *
 * @version 1.0
 */
class Application {
    private String name;
    private int id;
    private int memoryUsed;
    private int memoryReserved;
    private int stack;
    private long cpuTime;
    private int cpuUsage;

    /** Creates a new instance of Application */
    Application(String name, int id, int memoryUsed, int memoryReserved, int stack, long cpuTime) {
        this.name = name;
        this.id = id;
        this.memoryUsed = memoryUsed;
        this.memoryReserved = memoryReserved;
        this.stack = stack;
        this.cpuTime = cpuTime;
    }

    /**
     * Gets CPU time in microseconds.
     * @return CPU time in microseconds.
     */
    long getCpuTime() {
        return cpuTime;
    }

    /**
     * Gets CPU usage percentage.
     * @return CPU usage percentage.
     */
    int getCpuUsage() {
        return cpuUsage;
    }

    /**
     * Sets CPU usage percentage.
     * @param usage CPU usage percentage
     */
    void setCpuUsage(int usage) {
        cpuUsage = usage;
    }

    /** Getter for property id.
     * @return Value of property id.
     *
     */
    int getId() {
        return id;
    }

    /** Getter for property memoryReserved.
     * @return Value of property memoryReserved.
     *
     */
    int getMemoryReserved() {
        return memoryReserved;
    }

    /** Getter for property memoryUsed.
     * @return Value of property memoryUsed.
     *
     */
    int getMemoryUsed() {
        return memoryUsed;
    }

    /** Getter for property name.
     * @return Value of property name.
     *
     */
    String getName() {
        return name;
    }

    /** Getter for property stack.
     * @return Value of property stack.
     *
     */
    int getStack() {
        return stack;
    }

    /**
     * Returns a string representation of the object.
     * The toString method returns a string that "textually represents"
     * this object.
     *
     * @return a string representation of the object.
     */
    public String toString() {
        return "<application name=\"" + name + "\" id=\"" + id +
            "\" memoryUsed=\"" + memoryUsed + "\" memoryReserved=\"" +
            memoryReserved + "\" stack=\"" + stack + "\" cpuTime=\"" +
            cpuTime + "\"/>";
    }
}
