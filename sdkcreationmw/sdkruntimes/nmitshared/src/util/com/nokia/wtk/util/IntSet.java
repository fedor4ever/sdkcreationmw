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

/**
 * An interface similar to <code>java.util.Set</code> which contains
 * primitive integers rather than objects.
 *
 */
public interface IntSet {

    /**
     * Returns the number of elements in this set.
     * @return the number of elements in this set
     */
    public int size();

    /**
     * Returns <code>true</code> if this set contains no elements.
     * @return <code>true</code> if this set contains no elements.
     */
    public boolean isEmpty();

    /**
     * Removes all of the elements from this set (optional operation).
     * This set will be empty after this call returns (unless it throws an
     * exception).
     *
     * @throws UnsupportedOperationException if the <tt>clear</tt> method
     * 		  is not supported by this set.
     */
    public void clear();

    /**
     * Returns <code>true</code> if this set contains the specified element.
     * @param value element whose presence in this set is to be tested.
     * @return <code>true</code> if this set contains the specified element.
     */
    public boolean contains(int value);

    /**
     * Adds the specified element to this set if it is not already present
     * (optional operation).  If this set already contains the specified
     * element, the call leaves this set unchanged and returns <tt>false</tt>.
     *
     * @param value element to be added to this set.
     * @return <code>true</code> if this set did not already contain the
     *          specified element.
     *
     * @throws UnsupportedOperationException if the <tt>add</tt> method is not
     * 	       supported by this set.
     */
    public boolean add(int value);

    /**
     * Removes the specified element from this set if it is present
     * (optional operation).  Returns <tt>true</tt> if the set contained
     * the specified element (or equivalently, if the set changed as a
     * result of the call).
     *
     * @param value object to be removed from this set, if present.
     * @return <code>true</code> if the set contained the specified element.
     * @throws UnsupportedOperationException if the <tt>remove</tt> method is
     *         not supported by this set.
     */
    public boolean remove(int value);

    /**
     * Returns an array containing all of the elements in this set.
     * If this set is empty, returns an empty array. The returned
     * array is a copy of the internal data structure, i.e. the caller
     * is allowed to modify it.
     *
     * @return an array containing all of the elements in this set.
     */
    public int [] toArray();
}
