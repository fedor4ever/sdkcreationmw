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
 * Memory
 *
 * <ram>	= RAM
 * <free-RAM>	= current amount of free RAM
 * <total-RAM>	= total RAM on device
 * <total-ROM>	= total ROM on device
 *
 * @version 1.0
 */
public class Memory extends Object {
    private final int free;
    private final int totalRam;
    private final int totalRom;
    
    /** Creates a new instance of Memory */
    private Memory(int free, int totalRam, int totalRom) {
        super();
        this.free = free;
        this.totalRam = totalRam;
        this.totalRom = totalRom;
    }
    
    /** Creates a new instance of Memory */
    public static Memory create(int free, int totalRam, int totalRom) {
        return new Memory(free, totalRam, totalRom);
    }
    
    /** Getter for property free.
     * @return Value of property free.
     *
     */
    public int getFree() {
        return free;
    }
 
    /** Getter for property totalRam.
     * @return Value of property totalRam.
     *
     */
    public int getTotalRam() {
        return totalRam;
    }  
    
    /** Getter for property totalRom.
     * @return Value of property totalRom.
     *
     */
    public int getTotalRom() {
        return totalRom;
    }
}
