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
 * Thread
 *
 * <name>	= name of thread (contains no blanks, no ';')
 * <id>		= id of thread
 * <mem-used>	= amount of memory used by thread
 * <mem-reserved> = amount of memory reserved for thread
 * <stack>	= stack size of thread
 * <cpu-time>	= thread CPU usage in microseconds
 *
 * @version 1.0
 */
public class Thread extends Object {
    private final String name;
    private final int id;
    private final int memoryUsed;
    private final int memoryReserved;
    private final int stack;
    private final int cpuUsage;
    
    /** Creates a new instance of Thread */
    private Thread(String name, int id, int memoryUsed, int memoryReserved, int stack, int cpuUsage) {
        super();
        this.name = name;
        this.id = id;
        this.memoryUsed = memoryUsed;
        this.memoryReserved = memoryReserved;
        this.stack = stack;
        this.cpuUsage = cpuUsage;
    }
    
    /** Creates a new instance of Thread */
    public static Thread create(String name, int id, int memoryUsed, int memoryReserved, int stack, int cpuUsage) {
        return new Thread(name, id, memoryUsed, memoryReserved, stack, cpuUsage);
    }
    
    /** Getter for property cpuUsage.
     * @return Value of property cpuUsage.
     *
     */
    public int getCpuUsage() {
        return cpuUsage;
    }
    
    /** Getter for property id.
     * @return Value of property id.
     *
     */
    public int getId() {
        return id;
    }
    
    /** Getter for property memoryReserved.
     * @return Value of property memoryReserved.
     *
     */
    public int getMemoryReserved() {
        return memoryReserved;
    }
    
    /** Getter for property memoryUsed.
     * @return Value of property memoryUsed.
     *
     */
    public int getMemoryUsed() {
        return memoryUsed;
    }
    
    /** Getter for property name.
     * @return Value of property name.
     *
     */
    public java.lang.String getName() {
        return name;
    }
    
    /** Getter for property stack.
     * @return Value of property stack.
     *
     */
    public int getStack() {
        return stack;
    }
    
    /**
     * Returns a string representation of the object.
     * The toString method returns a string that "textually represents" this object.
     * @return a string representation of the object.
     */
    public String toString() {
        return "<thread name=\"" + this.name + "\" id=\"" + this.id + "\" memoryUsed=\"" + this.memoryUsed + "\" memoryReserved=\"" + this.memoryReserved + "\" stack=\"" + this.stack + "\" cpuUsage=\"" + this.cpuUsage + "\"/>";
    }
    
}
