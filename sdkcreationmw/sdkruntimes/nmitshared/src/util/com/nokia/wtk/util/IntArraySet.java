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
import java.util.Arrays;

/**
 * An interface similar to <code>java.util.Set</code> which contains
 * primitive integers rather than objects.
 *
 */
public class IntArraySet implements IntSet {

    private static int [] emptyArray = null;

    private int [] data;
    private int size;

    /**
     * Creates <code>IntArraySet</code>
     */
    public IntArraySet() {
    }

    /**
     * Constructs an empty set with the specified initial capacity.
     * @param   initialCapacity   the initial capacity of the set.
     * @exception IllegalArgumentException if the specified initial capacity
     *            is negative
     */
    public IntArraySet(int initialCapacity) {
        if (initialCapacity < 0) {
            throw new IllegalArgumentException("Illegal Capacity: "+
                                               initialCapacity);
        }
        data = new int[initialCapacity];
    }

    /**
     * Creates <code>IntArraySet</code> and fills it with values from
     * another set
     */
    public IntArraySet(IntSet that) {
        data = that.toArray();
        size = data.length;
        Arrays.sort(data);
    }

    /**
     * Returns the number of elements in this set.
     * @return the number of elements in this set
     */
    public int size() {
        return size;
    }

    /**
     * Returns <code>true</code> if this set contains no elements.
     * @return <code>true</code> if this set contains no elements.
     */
    public boolean isEmpty() {
        return (size == 0);
    }

    /**
     * Removes all of the elements from this set (optional operation).
     * This set will be empty after this call returns (unless it throws an
     * exception).
     *
     * @throws UnsupportedOperationException if the <tt>clear</tt> method
     * 		  is not supported by this set.
     */
    public void clear() {
        size = 0;
    }

    /**
     * Returns <code>true</code> if this set contains the specified element.
     * @param value element whose presence in this set is to be tested.
     * @return <code>true</code> if this set contains the specified element.
     */
    public boolean contains(int value) {
        return (ArrayUtils.binarySearch(data,0,size,value) >= 0);
    }

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
    public boolean add(int value) {
        int pos = ArrayUtils.binarySearch(data,0,size,value);
        if (pos >= 0) {
            return false;
        } else {
            ensureCapacity(size + 1);
            pos = -(pos+1);
            for (int i=size; i>pos; i--) data[i] = data[i-1];
            data[pos] = value;
            size++;
            return true;
        }
    }

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
    public boolean remove(int value) {
        int pos = ArrayUtils.binarySearch(data,0,size,value);
        if (pos >= 0) {
            int n = (--size);
            for (int i=pos; i<n; i++) data[i] = data[i+1];
            return true;
        }
        return false;
    }

    /**
     * Returns an array containing all of the elements in this set.
     * If this set is empty, returns an empty array
     * @return an array containing all of the elements in this set.
     */
    public int [] toArray() {
        if (size == 0) {
            if (emptyArray == null) emptyArray = new int[0];
            return emptyArray;
        } else {
            int [] values = new int[size];
            System.arraycopy(data, 0, values, 0, size);
            return values;
        }
    }

    //=======================================================================
    //          I M P L E M E N T A T I O N
    //=======================================================================

    /**
     * Increases the capacity of this <tt>IntArraySet</tt> instance, if
     * necessary, to ensure  that it can hold at least the number of elements
     * specified by the minimum capacity argument.
     *
     * @param minCapacity the desired minimum capacity.
     */
    public void ensureCapacity(int minCapacity) {
        int oldCapacity = ((data == null) ? 0 : data.length);
        if (minCapacity > oldCapacity) {
            int [] oldData = data;
            int newCapacity = (oldCapacity * 3)/2 + 1;
            if (newCapacity < minCapacity) {
                newCapacity = minCapacity;
            }
            data = new int[newCapacity];
            if (size > 0) System.arraycopy(oldData, 0, data, 0, size);
        }
    }

}
