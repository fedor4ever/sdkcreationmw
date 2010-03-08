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

/* java.lang.reflect*/
import java.lang.reflect.Array;

/* java.util */
import java.util.List;
import java.util.Random;
import java.util.ArrayList;
import java.util.Comparator;
import java.util.Collection;

/**
 * Collection of utilities for manipulating arrays of objects and primitive
 * types.
 *
 * @see java.util.Arrays
 */
public abstract class ArrayUtils {

    /** Default constructor is suppressed, ensuring non-instantiability. */
    private ArrayUtils() {
        throw new InternalError("ArrayUtils is a container of static methods");
    }

    /**
     * Returns random element of the array.
     *
     * @param a the array to pick element from. Must not be <tt>null</tt>
     *          or empty
     *
     * @return randomly picked element of the array.
     */
    public static Object random(Object [] a) {
        if (a == null)
            throw new IllegalArgumentException("null array");
        if (a.length == 0)
            throw new IllegalArgumentException("empty array");

        if (a.length == 1) return a[0];
        return a[getRandom().nextInt(a.length)];
    }

    private static Random randomNumberGenerator = null;
    private static Random getRandom() {
        if (randomNumberGenerator == null) initRNG();
        return randomNumberGenerator;
    }
    private static synchronized void initRNG() {
        if (randomNumberGenerator == null) {
            randomNumberGenerator = new Random();
        }
    }

    /**
     * Converts array into modifiable List of object. If array
     * if <code>null</code> or empty, this function returns an
     * empty list.<p>
     *
     * @param a  array to be converted to a list.
     * @return the created List.
     */
    public static List toList(Object[] a) {
        List list = new ArrayList((a == null) ? 0 : a.length);
        add(list, a);
        return list;
    }

    /**
     * Fills the list with the elements of the array. When this method
     * returns, the list contains the same elements as the array in the
     * same order.
     *
     * @param list the list to synchronize with the array
     * @param a the array of objects to be styored in the list
     */
    public static void setListValues(List list, Object[] a) {
        if (a == null || a.length == 0) {
            list.clear();
        } else {
            int i = 0;
            int n = a.length;
            int size = list.size();

            // fill existing slots
            while (i < size && i < n) {
                list.set(i, a[i]);
                i++;
            }

            // add the elements that didn't fit
            while (i < n) {
                list.add(a[i++]);
            }

            // remove extra elements if any
            int remove = list.size() - n;
            for (i=0; i<remove; i++) {
                list.remove(n);
            }
        }
    }

    /**
     * Returns <tt>true</tt> if the specified arrays of Objects and a
     * {@link List} contain the same number of elements, and all corresponding
     * pairs of elements are equal. Also, the array and the list are
     * considered equal if both are <tt>null</tt> or empty.<p>
     *
     * @param l the list to be tested for equality.
     * @param a the array to be tested for equality.
     * @return <tt>true</tt> if both collections contain the same elements in
     *         the same order.
     */
    public static boolean equals(List l, Object[] a) {
        int len1 = ((l == null) ? 0 : l.size());
        int len2 = ((a == null) ? 0 : a.length);
        if (len1 != len2) {
            return false;
        }

        for (int i=0; i<len1; i++) {
            if (!Utils.equals(l.get(i),a[i])) {
                return false;
            }
        }

        return true;
    }

    /**
     * Inserts object into the array at specified position.
     * Returns the new array.<p>
     *
     * @param a     array to be updated. This parameter must
     *              not be <code>null</code>.
     * @param obj   object to be inserted into the array
     * @param pos   the position where to insert he new element
     *
     * @return      the resulting array. If both input parameters are
     *              <code>null</code>, the return value is <code>null</code>.
     *
     * @exception   ArrayIndexOutOfBoundsException if <tt>pos</tt> is invalid.
     */
    public static Object[] insert(Object[] a, Object obj, int pos) {

        // validate the parameters
        if (a == null)
            throw new IllegalArgumentException("a");
        if (pos < 0 || pos > a.length)
            throw new ArrayIndexOutOfBoundsException(pos);

        // check if the object can be assigned to the array's component type
        Class type = a.getClass().getComponentType();
        if (obj != null) {
            if (!type.isAssignableFrom(obj.getClass()))
                throw new ArrayStoreException(obj.getClass().getName());
        }

        // allocate new array, copy the old contents and insert new element
        Object [] a1 = (Object[])Array.newInstance(type, a.length + 1);
        if (pos > 0) System.arraycopy(a, 0, a1, 0, pos);
        if (pos < a.length) System.arraycopy(a, pos, a1, pos+1, a.length-pos);
        a1[pos] = obj;
        return a1;
    }

    /**
     * Adds object to the array. Returns the new array.<p>
     *
     * @param a     array to be updated. This parameter must
     *              not be <code>null</code>.
     * @param obj   object to be added to the array
     *
     * @return      the resulting array. If both input parameters are
     *              <code>null</code>, the return value is <code>null</code>.
     */
    public static Object[] add(Object[] a, Object obj) {
        if (obj == null && a == null) return a;
        return insert(a, obj, a.length);
    }

    /**
     * Adds the specified array to the collection of objects. If array
     * if <code>null</code> or empty, this function does nothing.<p>
     *
     * @param c the collection to be updated.
     * @param a  array to be added to the collection.
     */
    public static void add(Collection c, Object[] a) {
        if (a != null && a.length > 0) {
            for (int i=0; i<a.length; i++) {
                c.add(a[i]);
            }
        }
    }

    /**
     * Removes the specified array from the collection of objects. If array
     * if <code>null</code> or empty, this function does nothing. If the
     * collection contains the same object more than once, only one
     * references is removed.<p>
     *
     * @param c the collection to be updated.
     * @param a  array to be added to the collection.
     * @return the number of actually removed objects
     */
    public static int remove(Collection c, Object[] a) {
        int count = 0;
        if (a != null && a.length > 0) {
            for (int i=0; i<a.length; i++) {
                c.remove(a[i]);
            }
        }
        return count;
    }

    /**
     * Removes the specified array from the collection of objects. If array
     * if <code>null</code> or empty, this function does nothing. If the
     * collection contains the same object more than once, all references
     * are removed.<p>
     *
     * @param c the collection to be updated.
     * @param a  array to be added to the collection.
     * @return the number of actually removed objects
     */
    public static int removeAll(Collection c, Object[] a) {
        int count = 0;
        if (a != null && a.length > 0) {
            for (int i=0; i<a.length; i++) {
                while (c.remove(a[i])) {
                    count++;
                }
            }
        }
        return count;
    }

    /**
     * Removes the first occurrence of the given object from the array.
     * Returns the new array.<p>NOTE: this method uses Object.equals method
     * to find the object in the array.
     *
     * @param a     array to be updated. This parameter must
     *              not be <code>null</code>.
     * @param obj   object to be removed from the array
     *
     * @return      the resulting array.
     */
    public static Object[] remove(Object[] a, Object obj) {
        int index = indexOf(a, obj);
        if (index >= 0) {
            return remove(a, index);
        } else {
            return a;
        }
    }

    /**
     * Removes all occurrences of the given object from the array.
     * Returns the new array.<p>NOTE: this method uses Object.equals
     * method to find the object in the array.
     *
     * @param a     array to be updated. This parameter must
     *              not be <code>null</code>.
     * @param obj   object to be removed from the array
     *
     * @return      the resulting array.
     */
    public static Object[] removeAll(Object[] a, Object obj) {
        int index = indexOf(a, obj);
        while (index >= 0) {
            a = remove(a, index);

            // find next index. NOTE: we are searching from the point where
            // we stopped last time, not from the beginning
            int i=index;
            for (index = -1; i<a.length; i++) {
                if (Utils.equals(obj,a[i])) {
                    index = i;
                    break;
                }
            }
        }
        return a;
    }

    /**
     * Removes object from the array. Returns the new array.<p>
     *
     * @param a     array to be updated. This parameter must
     *              not be <code>null</code>.
     * @param index index of the object to be removed from the array.
     *              Negative index is ignored. Non negative index must be
     *              in bounds, otherwise <tt>ArrayIndexOutOfBoundsException</tt>
     *              exception is thrown.
     *
     * @return      the resulting array.
     */
    public static Object[] remove(Object[] a, int index) {
        return ((index < 0) ? a : remove(a, index, index+1));
    }

    /**
     * Removes multiple objects from the array. Returns the new array.<p>
     *
     * @param      a            array to be updated. This parameter must
     *                          not be <code>null</code>.
     * @param      beginIndex   the beginning index, inclusive.
     * @param      endIndex     the ending index, exclusive.
     *
     * @return     the new array.
     * @exception  ArrayIndexOutOfBoundsException  if the
     *             <code>beginIndex</code> is negative, or
     *             <code>endIndex</code> is larger than the length of the
     *             array, or
     *             <code>beginIndex</code> is larger than
     *             <code>endIndex</code>.
     */
    public static Object[] remove(Object[] a, int beginIndex, int endIndex) {

        // validate the parameters
        if (a == null)
            throw new IllegalArgumentException("a");

        if (beginIndex < 0) {
            throw new ArrayIndexOutOfBoundsException(beginIndex);
        }
        if (endIndex > a.length) {
            throw new ArrayIndexOutOfBoundsException(endIndex);
        }
        if (beginIndex > endIndex) {
            throw new ArrayIndexOutOfBoundsException(endIndex - beginIndex);
        }

        if (beginIndex==endIndex) {
            return a;
        }

        // allocate new array, copy the content
        Class type = a.getClass().getComponentType();
        Object a1 = Array.newInstance(type, a.length - endIndex + beginIndex);
        if (beginIndex > 0)
            System.arraycopy(a,0, a1,0, beginIndex);
        if (endIndex < a.length)
            System.arraycopy(a,endIndex, a1,beginIndex, a.length - endIndex);

        return (Object[])a1;
    }

    //======================================================================
    //          H A S H    C O D E
    //======================================================================

    /**
     * Returns a hashcode for the specified byte array. The hashcode for
     * a byte array is computed as
     * <blockquote><pre>
     * b[0]*31^(n-1) + b[1]*31^(n-2) + ... + b[n-1]
     * </pre></blockquote>
     * using <code>int</code> arithmetic, where <code>b[i]</code> is the
     * <i>i</i>th byte of the array, <code>n</code> is the length of
     * the array, and <code>^</code> indicates exponentiation.
     * The hash value of the empty array is zero.
     *
     * @param b the array to calculate hash code for.
     * @return  a hash code value for the specified byte array.
     */
    public static int hashCode(byte [] b) {
        int h = 0;
        if (b != null) {
            int len = b.length;
            for (int i = 0; i < len; i++) {
                h = 31*h + b[i];
            }
        }
        return h;
    }

    /**
     * Returns a hashcode for the specified array of objects. The hashcode for
     * an array of objects is computed as
     * <blockquote><pre>
     * hash(0)*31^(n-1) + hash(0)*31^(n-2) + ... + hash(n-1)
     * </pre></blockquote>
     * using <code>int</code> arithmetic, where <code>hash(i)</code> is the
     * hash code of the <i>i</i>th element of the array, <code>n</code> is
     * the length of the array, and <code>^</code> indicates exponentiation.
     * The hash value of the empty array or <code>null</code> element of
     * the array is zero.
     *
     * @param a the array to calculate hash code for.
     * @return  a hash code value for the specified array of objects.
     */
    public static int hashCode(Object [] a) {
        int h = 0;
        if (a != null) {
            int len = a.length;
            for (int i = 0; i < len; i++) {
                h = 31*h + Utils.hashCode(a[i]);
            }
        }
        return h;
    }

    //======================================================================
    //          A D D    A R R A Y S
    //======================================================================

    /**
     * Combines two arrays into one. Returns the resulting array.<p>
     *
     * @param a1    first array
     * @param a2    second array
     *
     * @return      the resulting array. If both input parameters are
     *              <code>null</code>, the return value is <code>null</code>.
     *              If one of the parameters is <code>null</code>, the second
     *              parameter is returned. If one of the arrays is empty,
     *              the non empty array is returned. If both arrays are
     *              empty, an empty array is returned.
     */
    public static char[] add(char[] a1, char[] a2) {

        if (a1 == null || a1.length == 0) return a2;
        if (a2 == null || a2.length == 0) return a1;

        char [] a = new char[a1.length + a2.length];
        System.arraycopy(a1, 0, a, 0, a1.length);
        System.arraycopy(a2, 0, a, a1.length, a2.length);
        return a;
    }

    /**
     * Combines two arrays into one. Returns the resulting array.<p>
     *
     * @param a1    first array
     * @param a2    second array
     *
     * @return      the resulting array. If both input parameters are
     *              <code>null</code>, the return value is <code>null</code>.
     *              If one of the parameters is <code>null</code>, the second
     *              parameter is returned. If one of the arrays is empty,
     *              the non empty array is returned. If both arrays are
     *              empty, an empty array is returned.
     */
    public static byte[] add(byte[] a1, byte[] a2) {

        if (a1 == null || a1.length == 0) return a2;
        if (a2 == null || a2.length == 0) return a1;

        byte [] a = new byte[a1.length + a2.length];
        System.arraycopy(a1, 0, a, 0, a1.length);
        System.arraycopy(a2, 0, a, a1.length, a2.length);
        return a;
    }

    /**
     * Combines two arrays into one. Returns the resulting array.<p>
     *
     * @param a1    first array
     * @param a2    second array
     *
     * @return      the resulting array. If both input parameters are
     *              <code>null</code>, the return value is <code>null</code>.
     *              If one of the parameters is <code>null</code>, the second
     *              parameter is returned. If one of the arrays is empty,
     *              the non empty array is returned. If both arrays are
     *              empty, an empty array is returned.
     */
    public static short[] add(short[] a1, short[] a2) {

        if (a1 == null || a1.length == 0) return a2;
        if (a2 == null || a2.length == 0) return a1;

        short [] a = new short[a1.length + a2.length];
        System.arraycopy(a1, 0, a, 0, a1.length);
        System.arraycopy(a2, 0, a, a1.length, a2.length);
        return a;
    }

    /**
     * Combines two arrays into one. Returns the resulting array.<p>
     *
     * @param a1    first array
     * @param a2    second array
     *
     * @return      the resulting array. If both input parameters are
     *              <code>null</code>, the return value is <code>null</code>.
     *              If one of the parameters is <code>null</code>, the second
     *              parameter is returned. If one of the arrays is empty,
     *              the non empty array is returned. If both arrays are
     *              empty, an empty array is returned.
     */
    public static int[] add(int[] a1, int[] a2) {

        if (a1 == null || a1.length == 0) return a2;
        if (a2 == null || a2.length == 0) return a1;

        int [] a = new int[a1.length + a2.length];
        System.arraycopy(a1, 0, a, 0, a1.length);
        System.arraycopy(a2, 0, a, a1.length, a2.length);
        return a;
    }

    /**
     * Combines two arrays into one. Returns the resulting array.<p>
     *
     * @param a1    first array
     * @param a2    second array
     *
     * @return      the resulting array. If both input parameters are
     *              <code>null</code>, the return value is <code>null</code>.
     *              If one of the parameters is <code>null</code>, the second
     *              parameter is returned. If one of the arrays is empty,
     *              the non empty array is returned. If both arrays are
     *              empty, an empty array is returned.
     */
    public static long[] add(long[] a1, long[] a2) {

        if (a1 == null || a1.length == 0) return a2;
        if (a2 == null || a2.length == 0) return a1;

        long [] a = new long[a1.length + a2.length];
        System.arraycopy(a1, 0, a, 0, a1.length);
        System.arraycopy(a2, 0, a, a1.length, a2.length);
        return a;
    }

    /**
     * Combines two arrays into one. Returns the resulting array.<p>
     *
     * @param a1    first array
     * @param a2    second array
     *
     * @return      the resulting array. If both input parameters are
     *              <code>null</code>, the return value is <code>null</code>.
     *              If one of the parameters is <code>null</code>, the second
     *              parameter is returned. If one of the arrays is empty,
     *              the non empty array is returned. If both arrays are
     *              empty, an empty array is returned.
     */
    public static float[] add(float[] a1, float[] a2) {

        if (a1 == null || a1.length == 0) return a2;
        if (a2 == null || a2.length == 0) return a1;

        float [] a = new float[a1.length + a2.length];
        System.arraycopy(a1, 0, a, 0, a1.length);
        System.arraycopy(a2, 0, a, a1.length, a2.length);
        return a;
    }

    /**
     * Combines two arrays into one. Returns the resulting array.<p>
     *
     * @param a1    first array
     * @param a2    second array
     *
     * @return      the resulting array. If both input parameters are
     *              <code>null</code>, the return value is <code>null</code>.
     *              If one of the parameters is <code>null</code>, the second
     *              parameter is returned. If one of the arrays is empty,
     *              the non empty array is returned. If both arrays are
     *              empty, an empty array is returned.
     */
    public static double[] add(double[] a1, double[] a2) {

        if (a1 == null || a1.length == 0) return a2;
        if (a2 == null || a2.length == 0) return a1;

        double [] a = new double[a1.length + a2.length];
        System.arraycopy(a1, 0, a, 0, a1.length);
        System.arraycopy(a2, 0, a, a1.length, a2.length);
        return a;
    }

    /**
     * Combines two arrays into one. Returns the resulting array.<p>
     *
     * @param a1    first array
     * @param a2    second array
     *
     * @return      the resulting array. If both input parameters are
     *              <code>null</code>, the return value is <code>null</code>.
     *              If one of the parameters is <code>null</code>, the second
     *              parameter is returned. If one of the arrays is empty,
     *              the non empty array is returned. If both arrays are
     *              empty, an empty array is returned.
     */
    public static Object[] add(Object[] a1, Object[] a2) {

        if (a1 == null) return a2;
        if (a2 == null) return a1;

        Class type = a1.getClass().getComponentType();
        if (!type.isAssignableFrom(a2.getClass().getComponentType()))
            throw new IllegalArgumentException("component type mismatch");

        if (a1.length == 0) return a2;
        if (a2.length == 0) return a1;

        Object [] a = (Object[])Array.newInstance(type, a1.length + a2.length);
        System.arraycopy(a1, 0, a, 0, a1.length);
        System.arraycopy(a2, 0, a, a1.length, a2.length);
        return a;
    }

    //======================================================================
    //          F I N D    E L E M E N T S
    //======================================================================

    /**
     * Returns <tt>true</tt> if the specified array of contains the specified
     * value.<p>
     *
     * @param a array to be tested.
     * @param value the value to look for.
     * @return <tt>true</tt> if array contains the value at least once.
     */
    public static boolean contains(char[] a, char value) {
        return (indexOf(a, value) >= 0);
    }

    /**
     * Returns <tt>true</tt> if the specified array of contains the specified
     * value.<p>
     *
     * @param a array to be tested.
     * @param value the value to look for.
     * @return <tt>true</tt> if array contains the value at least once.
     */
    public static boolean contains(byte[] a, byte value) {
        return (indexOf(a, value) >= 0);
    }

    /**
     * Returns <tt>true</tt> if the specified array of contains the specified
     * value.<p>
     *
     * @param a array to be tested.
     * @param value the value to look for.
     * @return <tt>true</tt> if array contains the value at least once.
     */
    public static boolean contains(short[] a, short value) {
        return (indexOf(a, value) >= 0);
    }

    /**
     * Returns <tt>true</tt> if the specified array of contains the specified
     * value.<p>
     *
     * @param a array to be tested.
     * @param value the value to look for.
     * @return <tt>true</tt> if array contains the value at least once.
     */
    public static boolean contains(int[] a, int value) {
        return (indexOf(a, value) >= 0);
    }

    /**
     * Returns <tt>true</tt> if the specified array of contains the specified
     * value.<p>
     *
     * @param a array to be tested.
     * @param value the value to look for.
     * @return <tt>true</tt> if array contains the value at least once.
     */
    public static boolean contains(long [] a, long value) {
        return (indexOf(a, value) >= 0);
    }

    /**
     * Returns <tt>true</tt> if the specified array of contains the specified
     * value.<p>
     *
     * @param a array to be tested.
     * @param value the value to look for.
     * @return <tt>true</tt> if array contains the value at least once.
     */
    public static boolean contains(float[] a, float value) {
        return (indexOf(a, value) >= 0);
    }

    /**
     * Returns <tt>true</tt> if the specified array of contains the specified
     * value.<p>
     *
     * @param a array to be tested.
     * @param value the value to look for.
     * @return <tt>true</tt> if array contains the value at least once.
     */
    public static boolean contains(double[] a, double value) {
        return (indexOf(a, value) >= 0);
    }

    /**
     * Returns <tt>true</tt> if the specified array of Objects contains
     * the specified Object.<p>
     *
     * @param a array to be tested.
     * @param obj the object to look for.
     * @return <tt>true</tt> if array contains the object at least once.
     */
    public static boolean contains(Object[] a, Object obj) {
        return (indexOf(a, obj) >= 0);
    }

    /**
     * Returns the index of the specified value in the specified array.
     * Returns <tt>-1</tt> if array does not contain the value.<p>
     *
     * @param a array to be tested.
     * @param value the value to look for.
     * @return index of the value in the array, or <tt>-1</tt> if array
     *         does not contain the value.
     */
    public static int indexOf(char[] a, char value) {
        if (a != null) {
            for (int i=0; i<a.length; i++)
                if (a[i] == value)
                    return i;

        }
        return (-1);
    }

    /**
     * Returns the index of the specified value in the specified array.
     * Returns <tt>-1</tt> if array does not contain the value.<p>
     *
     * @param a array to be tested.
     * @param value the value to look for.
     * @return index of the value in the array, or <tt>-1</tt> if array
     *         does not contain the value.
     */
    public static int indexOf(byte[] a, byte value) {
        if (a != null) {
            for (int i=0; i<a.length; i++)
                if (a[i] == value)
                    return i;

        }
        return (-1);
    }

    /**
     * Returns the index of the specified value in the specified array.
     * Returns <tt>-1</tt> if array does not contain the value.<p>
     *
     * @param a array to be tested.
     * @param value the value to look for.
     * @return index of the value in the array, or <tt>-1</tt> if array
     *         does not contain the value.
     */
    public static int indexOf(short[] a, short value) {
        if (a != null) {
            for (int i=0; i<a.length; i++)
                if (a[i] == value)
                    return i;

        }
        return (-1);
    }

    /**
     * Returns the index of the specified value in the specified array.
     * Returns <tt>-1</tt> if array does not contain the value.<p>
     *
     * @param a array to be tested.
     * @param value the value to look for.
     * @return index of the value in the array, or <tt>-1</tt> if array
     *         does not contain the value.
     */
    public static int indexOf(int[] a, int value) {
        if (a != null) {
            for (int i=0; i<a.length; i++)
                if (a[i] == value)
                    return i;

        }
        return (-1);
    }

    /**
     * Returns the index of the specified value in the specified array.
     * Returns <tt>-1</tt> if array does not contain the value.<p>
     *
     * @param a array to be tested.
     * @param value the value to look for.
     * @return index of the value in the array, or <tt>-1</tt> if array
     *         does not contain the value.
     */
    public static int indexOf(long[] a, long value) {
        if (a != null) {
            for (int i=0; i<a.length; i++)
                if (a[i] == value)
                    return i;

        }
        return (-1);
    }

    /**
     * Returns the index of the specified value in the specified array.
     * Returns <tt>-1</tt> if array does not contain the value.<p>
     *
     * @param a array to be tested.
     * @param value the value to look for.
     * @return index of the value in the array, or <tt>-1</tt> if array
     *         does not contain the value.
     */
    public static int indexOf(float[] a, float value) {
        if (a != null) {
            for (int i=0; i<a.length; i++)
                if (a[i] == value)
                    return i;

        }
        return (-1);
    }

    /**
     * Returns the index of the specified value in the specified array.
     * Returns <tt>-1</tt> if array does not contain the value.<p>
     *
     * @param a array to be tested.
     * @param value the value to look for.
     * @return index of the value in the array, or <tt>-1</tt> if array
     *         does not contain the value.
     */
    public static int indexOf(double[] a, double value) {
        if (a != null) {
            for (int i=0; i<a.length; i++)
                if (a[i] == value)
                    return i;

        }
        return (-1);
    }

    /**
     * Returns the index of the specified Object in the specified array
     * of Objects. Returns <tt>-1</tt> if array does not contain the
     * specified object<p>. NOTE: this method uses Object.equals method
     * to find the object in the array.
     *
     * @param a array to be tested.
     * @param obj the object to look for.
     * @return index of the Object in the array, or <tt>-1</tt> if array
     *         does not contain the object.
     */
    public static int indexOf(Object[] a, Object obj) {
        if (a != null) {
            for (int i=0; i<a.length; i++) {
                if (Utils.equals(obj,a[i])) {
                    return i;
                }
            }
        }
        return (-1);
    }

    /**
     * Returns the index of the specified Object in the specified array
     * of Objects. Returns <tt>-1</tt> if array does not contain the
     * specified object<p>. NOTE: this method uses the identity (==)
     * operator to find the object in the array.
     *
     * @param a array to be tested.
     * @param obj the object to look for.
     * @return index of the Object in the array, or <tt>-1</tt> if array
     *         does not contain the object.
     */
    public static int identityIndexOf(Object[] a, Object obj) {
        if (a != null) {
            for (int i=0; i<a.length; i++) {
                if (obj == a[i]) {
                    return i;
                }
            }
        }
        return (-1);
    }

    /**
     * Searches the specified array of ints for the specified value using the
     * binary search algorithm.  The array <strong>must</strong> be sorted (as
     * by the <tt>sort</tt> method from <code>java.util.Arrays</code>) prior
     * to making this call.  If it is not sorted, the results are undefined.
     * If the array contains multiple elements with the specified value,
     * there is no guarantee which one will be found.
     *
     * @param a    the array to be searched.
     * @param from the index of the first element (inclusive) to be searched.
     * @param to   the index of the last element (exclusive) to be searched.
     * @param key  the value to be searched for.
     * @return index of the search key, if it is contained in the list;
     *	       otherwise, <tt>(-(<i>insertion point</i>) - 1)</tt>.  The
     *	       <i>insertion point</i> is defined as the point at which the
     *	       key would be inserted into the list: the index of the first
     *	       element greater than the key, or <tt>list.size()</tt>, if all
     *	       elements in the list are less than the specified key.  Note
     *	       that this guarantees that the return value will be &gt;= 0 if
     *	       and only if the key is found.
     * @see #sort(int[])
     */
    public static int binarySearch(int[] a, int from, int to, int key) {
        int low = from;
        int high = to-1;

        while (low <= high) {
            int mid =(low + high)/2;
            int midVal = a[mid];

            if (midVal < key) {
                low = mid + 1;
            } else if (midVal > key) {
                high = mid - 1;
            } else {
                return mid; // key found
            }
        }
        return -(low + 1);  // key not found.
    }

    //======================================================================
    //          C L O N E    A R R A Y
    //======================================================================

    /**
     * Clones the specified array. If <code>a</code> is <code>null</code>
     * or empty, this function returns <code>a</code>.
     *
     * @param a  array to be cloned.
     * @return copy of the <code>a</code>.
     */
    public static char [] clone(char[] a) {
        if (a == null || a.length == 0) {
            return a;
        } else {
            char [] b = new char[a.length];
            System.arraycopy(a, 0, b, 0, a.length);
            return b;
        }
    }

    /**
     * Clones the specified array. If <code>a</code> is <code>null</code>
     * or empty, this function returns <code>a</code>.
     *
     * @param a  array to be cloned.
     * @return copy of the <code>a</code>.
     */
    public static byte [] clone(byte[] a) {
        if (a == null || a.length == 0) {
            return a;
        } else {
            byte [] b = new byte[a.length];
            System.arraycopy(a, 0, b, 0, a.length);
            return b;
        }
    }

    /**
     * Clones the specified array. If <code>a</code> is <code>null</code>
     * or empty, this function returns <code>a</code>.
     *
     * @param a  array to be cloned.
     * @return copy of the <code>a</code>.
     */
    public static short [] clone(short[] a) {
        if (a == null || a.length == 0) {
            return a;
        } else {
            short [] b = new short[a.length];
            System.arraycopy(a, 0, b, 0, a.length);
            return b;
        }
    }

    /**
     * Clones the specified array. If <code>a</code> is <code>null</code>
     * or empty, this function returns <code>a</code>.
     *
     * @param a  array to be cloned.
     * @return copy of the <code>a</code>.
     */
    public static int [] clone(int[] a) {
        if (a == null || a.length == 0) {
            return a;
        } else {
            int [] b = new int[a.length];
            System.arraycopy(a, 0, b, 0, a.length);
            return b;
        }
    }

    /**
     * Clones the specified array. If <code>a</code> is <code>null</code>
     * or empty, this function returns <code>a</code>.
     *
     * @param a  array to be cloned.
     * @return copy of the <code>a</code>.
     */
    public static long [] clone(long[] a) {
        if (a == null || a.length == 0) {
            return a;
        } else {
            long [] b = new long[a.length];
            System.arraycopy(a, 0, b, 0, a.length);
            return b;
        }
    }

    /**
     * Clones the specified array. If <code>a</code> is <code>null</code>
     * or empty, this function returns <code>a</code>.
     *
     * @param a  array to be cloned.
     * @return copy of the <code>a</code>.
     */
    public static float [] clone(float[] a) {
        if (a == null || a.length == 0) {
            return a;
        } else {
            float [] b = new float[a.length];
            System.arraycopy(a, 0, b, 0, a.length);
            return b;
        }
    }

    /**
     * Clones the specified array. If <code>a</code> is <code>null</code>
     * or empty, this function returns <code>a</code>.
     *
     * @param a  array to be cloned.
     * @return copy of the <code>a</code>.
     */
    public static double [] clone(double[] a) {
        if (a == null || a.length == 0) {
            return a;
        } else {
            double [] b = new double[a.length];
            System.arraycopy(a, 0, b, 0, a.length);
            return b;
        }
    }

    /**
     * Clones the specified array. If <code>a</code> is <code>null</code>
     * or empty, this function returns <code>a</code>.
     * <p>
     * Note that this function only clones the array itself, it does not clone
     * the elements.
     *
     * @param a  array to be cloned.
     * @return copy of the <code>a</code>.
     */
    public static Object [] clone(Object[] a) {
        if (a == null || a.length == 0) {
            return a;
        } else {
            Class type = a.getClass().getComponentType();
            Object [] b = (Object[])Array.newInstance(type, a.length);
            System.arraycopy(a, 0, b, 0, a.length);
            return b;
        }
    }

    //======================================================================
    //          F I N D    M I N I M U M
    //======================================================================

    /**
     * Returns the minimum value stored in the specified array.
     * @param a the array to scan
     * @return the minimum value stored in the array.
     * @exception IllegalArgumentException if array is <tt>null</tt> or empty.
     */
    public static char min(char [] a) {
        if (a == null) throw new IllegalArgumentException("null array");
        if (a.length == 0) throw new IllegalArgumentException("empty array");
        char min = a[0];
        for (int i=1; i<a.length; i++)
            if (min > a[i])
                min = a[i];
        return min;
    }

    /**
     * Returns the minimum value stored in the specified array.
     * @param a the array to scan
     * @return the minimum value stored in the array.
     * @exception IllegalArgumentException if array is <tt>null</tt> or empty.
     */
    public static byte min(byte [] a) {
        if (a == null) throw new IllegalArgumentException("null array");
        if (a.length == 0) throw new IllegalArgumentException("empty array");
        byte min = a[0];
        for (int i=1; i<a.length; i++)
            if (min > a[i])
                min = a[i];
        return min;
    }

    /**
     * Returns the minimum value stored in the specified array.
     * @param a the array to scan
     * @return the minimum value stored in the array.
     * @exception IllegalArgumentException if array is <tt>null</tt> or empty.
     */
    public static short min(short [] a) {
        if (a == null) throw new IllegalArgumentException("null array");
        if (a.length == 0) throw new IllegalArgumentException("empty array");
        short min = a[0];
        for (int i=1; i<a.length; i++)
            if (min > a[i])
                min = a[i];
        return min;
    }

    /**
     * Returns the minimum value stored in the specified array.
     * @param a the array to scan
     * @return the minimum value stored in the array.
     * @exception IllegalArgumentException if array is <tt>null</tt> or empty.
     */
    public static int min(int [] a) {
        if (a == null) throw new IllegalArgumentException("null array");
        if (a.length == 0) throw new IllegalArgumentException("empty array");
        int min = a[0];
        for (int i=1; i<a.length; i++)
            if (min > a[i])
                min = a[i];
        return min;
    }

    /**
     * Returns the minimum value stored in the specified array.
     * @param a the array to scan
     * @return the minimum value stored in the array.
     * @exception IllegalArgumentException if array is <tt>null</tt> or empty.
     */
    public static long min(long [] a) {
        if (a == null) throw new IllegalArgumentException("null array");
        if (a.length == 0) throw new IllegalArgumentException("empty array");
        long min = a[0];
        for (int i=1; i<a.length; i++)
            if (min > a[i])
                min = a[i];
        return min;
    }

    /**
     * Returns the minimum value stored in the specified array.
     * @param a the array to scan
     * @return the minimum value stored in the array.
     * @exception IllegalArgumentException if array is <tt>null</tt> or empty.
     */
    public static float min(float [] a) {
        if (a == null) throw new IllegalArgumentException("null array");
        if (a.length == 0) throw new IllegalArgumentException("empty array");
        float min = a[0];
        for (int i=1; i<a.length; i++)
            if (min > a[i])
                min = a[i];
        return min;
    }

    /**
     * Returns the minimum value stored in the specified array.
     * @param a the array to scan
     * @return the minimum value stored in the array.
     * @exception IllegalArgumentException if array is <tt>null</tt> or empty.
     */
    public static double min(double [] a) {
        if (a == null) throw new IllegalArgumentException("null array");
        if (a.length == 0) throw new IllegalArgumentException("empty array");
        double min = a[0];
        for (int i=1; i<a.length; i++)
            if (min > a[i])
                min = a[i];
        return min;
    }

    /**
     * Returns the minimum value stored in the specified array.
     * @param a the array to scan
     * @param c the comparator to compare the elements
     * @return the minimum value stored in the array, according to the
     *         comparator.
     * @exception IllegalArgumentException if array is <tt>null</tt> or empty
     *            or comparator is <tt>null</tt>
     */
    public static Object min(Object [] a, Comparator c) {
        if (a == null) throw new IllegalArgumentException("null array");
        if (c == null) throw new IllegalArgumentException("null comparator");
        if (a.length == 0) throw new IllegalArgumentException("empty array");
        Object min = a[0];
        for (int i=1; i<a.length; i++)
            if (c.compare(min,a[i]) > 0)
                min = a[i];
        return min;
    }

    //======================================================================
    //          F I N D    M A X I M U M
    //======================================================================

    /**
     * Returns the maximum value stored in the specified array.
     * @param a the array to scan
     * @return the maximum value stored in the array.
     * @exception IllegalArgumentException if array is <tt>null</tt> or empty.
     */
    public static char max(char [] a) {
        if (a == null) throw new IllegalArgumentException("null array");
        if (a.length == 0) throw new IllegalArgumentException("empty array");
        char max = a[0];
        for (int i=1; i<a.length; i++)
            if (max < a[i])
                max = a[i];
        return max;
    }

    /**
     * Returns the maximum value stored in the specified array.
     * @param a the array to scan
     * @return the maximum value stored in the array.
     * @exception IllegalArgumentException if array is <tt>null</tt> or empty.
     */
    public static byte max(byte [] a) {
        if (a == null) throw new IllegalArgumentException("null array");
        if (a.length == 0) throw new IllegalArgumentException("empty array");
        byte max = a[0];
        for (int i=1; i<a.length; i++)
            if (max < a[i])
                max = a[i];
        return max;
    }

    /**
     * Returns the maximum value stored in the specified array.
     * @param a the array to scan
     * @return the maximum value stored in the array.
     * @exception IllegalArgumentException if array is <tt>null</tt> or empty.
     */
    public static short max(short [] a) {
        if (a == null) throw new IllegalArgumentException("null array");
        if (a.length == 0) throw new IllegalArgumentException("empty array");
        short max = a[0];
        for (int i=1; i<a.length; i++)
            if (max < a[i])
                max = a[i];
        return max;
    }

    /**
     * Returns the maximum value stored in the specified array.
     * @param a the array to scan
     * @return the maximum value stored in the array.
     * @exception IllegalArgumentException if array is <tt>null</tt> or empty.
     */
    public static int max(int [] a) {
        if (a == null) throw new IllegalArgumentException("null array");
        if (a.length == 0) throw new IllegalArgumentException("empty array");
        int max = a[0];
        for (int i=1; i<a.length; i++)
            if (max < a[i])
                max = a[i];
        return max;
    }

    /**
     * Returns the maximum value stored in the specified array.
     * @param a the array to scan
     * @return the maximum value stored in the array.
     * @exception IllegalArgumentException if array is <tt>null</tt> or empty.
     */
    public static long max(long [] a) {
        if (a == null) throw new IllegalArgumentException("null array");
        if (a.length == 0) throw new IllegalArgumentException("empty array");
        long max = a[0];
        for (int i=1; i<a.length; i++)
            if (max < a[i])
                max = a[i];
        return max;
    }

    /**
     * Returns the maximum value stored in the specified array.
     * @param a the array to scan
     * @return the maximum value stored in the array.
     * @exception IllegalArgumentException if array is <tt>null</tt> or empty.
     */
    public static float max(float [] a) {
        if (a == null) throw new IllegalArgumentException("null array");
        if (a.length == 0) throw new IllegalArgumentException("empty array");
        float max = a[0];
        for (int i=1; i<a.length; i++)
            if (max < a[i])
                max = a[i];
        return max;
    }

    /**
     * Returns the maximum value stored in the specified array.
     * @param a the array to scan
     * @return the maximum value stored in the array.
     * @exception IllegalArgumentException if array is <tt>null</tt> or empty.
     */
    public static double max(double [] a) {
        if (a == null) throw new IllegalArgumentException("null array");
        if (a.length == 0) throw new IllegalArgumentException("empty array");
        double max = a[0];
        for (int i=1; i<a.length; i++)
            if (max < a[i])
                max = a[i];
        return max;
    }

    /**
     * Returns the maximum value stored in the specified array.
     * @param a the array to scan
     * @param c the comparator to compare the elements
     * @return the maximum value stored in the array according to the
     *         comparator.
     * @exception IllegalArgumentException if array is <tt>null</tt> or empty
     *            or comparator is <tt>null</tt>
     */
    public static Object max(Object [] a, Comparator c) {
        if (a == null) throw new IllegalArgumentException("null array");
        if (c == null) throw new IllegalArgumentException("null comparator");
        if (a.length == 0) throw new IllegalArgumentException("empty array");
        Object max = a[0];
        for (int i=1; i<a.length; i++)
            if (c.compare(max,a[i]) < 0)
                max = a[i];
        return max;
    }

     //======================================================================
    //          SUB - ARRAY
    //======================================================================

    /**
     * Returns the new array that is the sub-array of the specified array .
     * The returned array begins at the specified beginIndex and extends to
     * the byte at index endIndex - 1. Thus the length of the new array is
     * endIndex-beginIndex. If <code>a</code> is <code>null</code> or empty,
     * this function returns <code>null</code>. Also returns <code>null</code>
     * if the beginIndex is negative, or endIndex is larger than the length of
     * the <code>a</code> array, or beginIndex is larger than endIndex.
     *
     * @param a  array data to be copied.
     * @param beginIndex - the beginning index, inclusive.
     * @param endIndex - the ending index, exclusive.
     * @return sub-array of the <code>a</code>. If parameters are not correct,
     * will return new byte[0].
     */
    public static byte [] subArray(byte[] a, int beginIndex, int endIndex) {
        if (a == null || a.length == 0 ||
            beginIndex < 0 || endIndex > a.length ||
            beginIndex > endIndex ) {
            return new byte[0];
        } else {
            byte [] b = new byte[endIndex - beginIndex];
            System.arraycopy(a, beginIndex, b, 0, b.length);
            return b;
        }
    }

}

