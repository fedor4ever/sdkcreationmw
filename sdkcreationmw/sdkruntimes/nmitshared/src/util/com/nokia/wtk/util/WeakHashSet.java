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

/* java.io */
import java.io.IOException;
import java.io.Serializable;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;

/* java.util */
import java.util.Set;
import java.util.Map;
import java.util.Iterator;
import java.util.Collection;
import java.util.AbstractSet;
import java.util.WeakHashMap;

/**
 * A {@link WeakHashMap}-based {@link Set} implementation that holds
 * <em>weak</em> references to the objects it contains. An entry in a
 * <code>WeakHashSet</code> will automatically be removed when it is
 * no longer in ordinary use.  More precisely, the presence of a given
 * object in <code>WeakHashSet</code> will not prevent the object from
 * being discarded by the garbage collector, that is, made finalizable,
 * finalized, and then reclaimed. When an object has been discarded it
 * is effectively removed from the set, so this class behaves somewhat
 * differently than other {@link Set} implementations.
 *
 */
public
class       WeakHashSet
extends     AbstractSet
implements  Set, Serializable
{
    /** The default initial capacity, which is <tt>11</tt> */
    public static int DEFAULT_INITIAL_CAPACITY = 11;

    /** The default load factor, which is <tt>0.75</tt> */
    public static float DEFAULT_LOAD_FACTOR = 0.75f;

    /** The actual storage for elements of this Set. */
    private transient Map map;

    /** The initial capacity of the hash map. */
    private int initialCapacity;

    /** The load factor of the hash map. */
    private float loadFactor;

    /** Dummy value to associate with an Object in the backing Map */
    private static final Object PRESENT = new Object();

    /**
     * Constructs a new, empty set; the backing <tt>WeakHashMap</tt> instance
     * has default capacity and load factor, which is <tt>0.75</tt>.
     */
    public WeakHashSet() {
        this(DEFAULT_INITIAL_CAPACITY, DEFAULT_LOAD_FACTOR);
    }

    /**
     * Constructs a new, empty set; the backing <tt>WeakHashMap</tt> instance has
     * the specified initial capacity and default load factor, which is
     * <tt>0.75</tt>.
     *
     * @param      initialCapacity   the initial capacity of the hash table.
     * @throws     IllegalArgumentException if the initial capacity is less
     *             than zero.
     */
    public WeakHashSet(int initialCapacity) {
        this(initialCapacity, DEFAULT_LOAD_FACTOR);
    }

    /**
     * Constructs a new, empty set; the backing <tt>WeakHashMap</tt> instance has
     * the specified initial capacity and the specified load factor.
     *
     * @param      initialCapacity   the initial capacity of the hash map.
     * @param      loadFactor        the load factor of the hash map.
     * @throws     IllegalArgumentException if the initial capacity is less
     *             than zero, or if the load factor is nonpositive.
     */
    public WeakHashSet(int initialCapacity, float loadFactor) {
        map = new WeakHashMap(initialCapacity, loadFactor);
        this.initialCapacity = initialCapacity;
        this.loadFactor = loadFactor;
    }

    /**
     * Constructs a new set containing the elements in the specified
     * collection.  The capacity of the backing <tt>WeakHashMap</tt> instance is
     * twice the size of the specified collection or eleven (whichever is
     * greater), and the default load factor (which is <tt>0.75</tt>) is used.
     */
    public WeakHashSet(Collection c) {
        this(Math.max(2*c.size(), DEFAULT_INITIAL_CAPACITY));
        addAll(c);
    }

    /**
     * Returns an iterator over the elements in this set.  The elements
     * are returned in no particular order.
     *
     * @return an Iterator over the elements in this set.
     */
    public Iterator iterator() {
        return map.keySet().iterator();
    }

    /**
     * Returns the number of elements in this set (its cardinality).
     *
     * @return the number of elements in this set (its cardinality).
     */
    public int size() {
        return map.size();
    }

    /**
     * Returns <tt>true</tt> if this set contains no elements.
     *
     * @return <tt>true</tt> if this set contains no elements.
     */
    public boolean isEmpty() {
        return map.isEmpty();
    }

    /**
     * Returns <tt>true</tt> if this set contains the specified element.
     *
     * @return <tt>true</tt> if this set contains the specified element.
     */
    public boolean contains(Object o) {
        return map.containsKey(o);
    }

    /**
     * Adds the specified element to this set if it is not already
     * present.
     *
     * @param o element to be added to this set.
     * @return <tt>true</tt> if the set did not already contain the specified
     * element.
     */
    public boolean add(Object o) {
        return (map.put(o, PRESENT)==null);
    }

    /**
     * Removes the given element from this set if it is present.
     *
     * @param o object to be removed from this set, if present.
     * @return <tt>true</tt> if the set contained the specified element.
     */
    public boolean remove(Object o) {
        return (map.remove(o)==PRESENT);
    }

    /**
     * Removes all of the elements from this set.
     */
    public void clear() {
        map.clear();
    }

    /**
     * Save the state of this <tt>WeakHashSet</tt> instance to a stream
     * (that is, serialize this set).
     *
     * @serialData The capacity of the backing <tt>WeakHashMap</tt> instance
     *		   (int), and its load factor (float) are emitted, followed by
     *		   the size of the set (the number of elements it contains)
     *		   (int), followed by all of its elements (each an Object) in
     *             no particular order.
     */
    private synchronized void writeObject(ObjectOutputStream s)
    throws IOException {
        // Write out any hidden serialization magic
        s.defaultWriteObject();

        // Write out HashMap capacity and load factor
        s.writeInt(initialCapacity);
        s.writeFloat(loadFactor);

        // Write out size
        s.writeInt(map.size());

        // Write out all elements in the proper order.
        Iterator itr=map.keySet().iterator();
        while (itr.hasNext()) s.writeObject(itr.next());
    }

    /**
     * Reconstitute the <tt>HashSet</tt> instance from a stream (that is,
     * deserialize it).
     */
    private synchronized void readObject(ObjectInputStream s)
    throws IOException, ClassNotFoundException {

        // Read in any hidden serialization magic
        s.defaultReadObject();

        // Read in HashMap capacity and load factor and create backing HashMap
        int capacity = s.readInt();
        float loadFactor = s.readFloat();
        map = new WeakHashMap(capacity, loadFactor);

        // Read in size
        int size = s.readInt();

        // Read in all elements in the proper order.
        for (int i=0; i<size; i++) {
            Object e = s.readObject();
            map.put(e, PRESENT);
        }
    }
}

