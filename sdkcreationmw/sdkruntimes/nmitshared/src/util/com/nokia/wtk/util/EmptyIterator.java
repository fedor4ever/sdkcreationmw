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



package com.nokia.wtk.util;

/* java.util */
import java.util.Iterator;
import java.util.Enumeration;
import java.util.NoSuchElementException;

/**
 * Empty iterator.
 *
 */
public class EmptyIterator implements Iterator, Enumeration {

    /**
     * Shared instance of <code>EmptyIterator</code>.
     */
    private static EmptyIterator instance = null;

    /**
     * Returns shared instance of <code>EmptyIterator</code>.
     */
    public static EmptyIterator getInstance() {
        if (instance == null) instance = new EmptyIterator();
        return instance;
    }

    /**
     * Creates <code>EmptyIterator</code>.
     */
    private EmptyIterator() {}

    //=======================================================================
    //      I T E R A T O R
    //=======================================================================

    /**
     * Returns <tt>true</tt> if the iteration has more elements.
     * Empty iterator always returns <tt>false</tt>.
     *
     * @return always <tt>false</tt>
     */
    public boolean hasNext() {
        return false;
    }

    /**
     * Returns the next element in the interation.
     *
     * @returns nothing. Always throws {@link NoSuchElementException}.
     * @exception NoSuchElementException iteration has no more elements.
     */
    public Object next() {
        throw new NoSuchElementException();
    }

    /**
     * Removes from the underlying collection the last element returned
     * by the iterator (optional operation). This implementation always
     * throws <code>UnsupportedOperationException</code>.
     *
     * @exception UnsupportedOperationException because this operation is not
     *            supported by this class.
     */
    public void remove() {
        throw new UnsupportedOperationException();
    }

    //=======================================================================
    //      E N U M E R A T I O N
    //=======================================================================

    /**
     * Tests if this enumeration contains more elements.
     * Empty iterator always returns <tt>false</tt>.
     *
     * @return always <tt>false</tt>
     */
    public boolean hasMoreElements() {
        return false;
    }

    /**
     * Returns the next element of this enumeration if this enumeration
     * object has at least one more element to provide.
     *
     * @returns nothing. Always throws {@link NoSuchElementException}.
     * @exception NoSuchElementException iteration has no more elements.
     */
    public Object nextElement() {
        throw new NoSuchElementException();
    }
}
