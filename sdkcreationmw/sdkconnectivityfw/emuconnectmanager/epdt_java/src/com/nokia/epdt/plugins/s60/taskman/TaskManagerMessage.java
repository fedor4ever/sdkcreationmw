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

import java.util.ArrayList;

/**
 * TaskManagerMessage
 *
 * @version 1.0
 */
public class TaskManagerMessage extends Object {
    private final Cpu cpu;
    private final ArrayList driveList;
    private final Memory memory;
    private final ArrayList applicationList;
    private final ArrayList threadList;
    
    /** Creates a new instance of TaskManagerMessage */
    private TaskManagerMessage(Cpu cpu, ArrayList driveList, Memory memory, ArrayList applicationList, ArrayList threadList) {
        super();
        this.cpu = cpu;
        this.driveList = driveList;
        this.memory = memory;
        this.applicationList = applicationList;
        this.threadList = threadList;
    }
    
    /** Creates a new instance of TaskManagerMessage */
    public static TaskManagerMessage create(Cpu cpu, ArrayList driveList, Memory memory, ArrayList applicationList, ArrayList threadList) {
        return new TaskManagerMessage(cpu, driveList, memory, applicationList, threadList);
    }
    
    /** Getter for property applicationList.
     * @return Value of property applicationList.
     *
     */
    public java.util.ArrayList getApplicationList() {
        return applicationList;
    }    

    /** Getter for property cpu.
     * @return Value of property cpu.
     *
     */
    public com.nokia.epdt.plugins.s60.taskman.Cpu getCpu() {
        return cpu;
    }

    /** Getter for property driveList.
     * @return Value of property driveList.
     *
     */
    public java.util.ArrayList getDriveList() {
        return driveList;
    }

    /** Getter for property memory.
     * @return Value of property memory.
     *
     */
    public com.nokia.epdt.plugins.s60.taskman.Memory getMemory() {
        return memory;
    }

    /** Getter for property threadList.
     * @return Value of property threadList.
     *
     */
    public java.util.ArrayList getThreadList() {
        return threadList;
    }
    
}
