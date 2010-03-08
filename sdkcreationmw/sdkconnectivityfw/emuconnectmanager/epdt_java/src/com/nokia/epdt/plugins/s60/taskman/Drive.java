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
 * Drive
 *
 * <drive-A>	= A
 * <drive-size>= total capacity of drive
 * <drive-free>= current free capacity on drive
 *
 * @version 1.0
 */
public class Drive extends Object {
    private final String name;
    private final long size;
    private final long free;
    
    /** Creates a new instance of Drive */
    private Drive(String name, long size, long free) {
        super();
        this.name = name;
        this.size = size;
        this.free = free;
    }
    
    /** Creates a new instance of Drive */
    public static Drive create(String name, long size, long free) {
        return new Drive(name, size, free);
    }
    
    /** Getter for property free.
     * @return Value of property free.
     *
     */
    public long getFree() {
        return free;
    }
    
    /** Getter for property name.
     * @return Value of property name.
     *
     */
    public java.lang.String getName() {
        return name;
    }
    
    /** Getter for property size.
     * @return Value of property size.
     *
     */
    public long getSize() {
        return size;
    }
    
    /**
     * Returns a string representation of the object.
     * The toString method returns a string that "textually represents" this object.
     * @return a string representation of the object.
     */
    public String toString() {
        return "<drive name=\"" + this.name + "\" size=\"" + this.size + "\" free=\"" + this.free + "\"/>";
    }
    
}
