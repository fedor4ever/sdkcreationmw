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
import java.io.Serializable;

/**
 * Base class for typesafe enumerations. Typesafe enumeration classes
 * provide better compile time and runtime checking than other alternatives.
 * The basic idea is simple: define a class representing a single element
 * of the enumerated type, and don't provide any public constructors for it.
 * Just provide public static final fields, one for each constant in the
 * enumerated type. Because there is no way for clients to create objects
 * of the type, there will never be any objects of the type besides those
 * exported via the public static final fields. Here's how it looks in
 * practice:
 *
 *<blockquote><pre>
 *<font color="#0000FF">public final class</font> KeyType <font color="#0000FF">extends</font> Enum {
 *    <font color="#0000FF">private</font> KeyType(String id) { <font color="#0000FF">super</font>(id); }
 *    <font color="#0000FF">public static final</font> KeyType NAVIGATION = <font color="#0000FF">new</font> KeyType(<font color="#808080">&quot;nav&quot;</font>);
 *    <font color="#0000FF">public static final</font> KeyType SOFTKEY = <font color="#0000FF">new</font> KeyType(<font color="#808080">&quot;soft&quot;</font>);
 *    <font color="#0000FF">public static final</font> KeyType ENTRY = <font color="#0000FF">new</font> KeyType(<font color="#808080">&quot;entry&quot;</font>);
 *    <font color="#0000FF">public static final</font> KeyType CONTROL = <font color="#0000FF">new</font> KeyType(<font color="#808080">&quot;control&quot;</font>);
 *    <font color="#0000FF">public static final</font> KeyType OTHER = <font color="#0000FF">new</font> KeyType(<font color="#808080">&quot;other&quot;</font>);
 *}
 *
 *<font color="#0000FF">public final class</font> DisplayType <font color="#0000FF">extends</font> Enum {
 *    <font color="#0000FF">private</font> DisplayType(String id) { <font color="#0000FF">super</font>(id); }
 *    <font color="#0000FF">public static final</font> DisplayType MONOCHROME = <font color="#0000FF">new</font> DisplayType(<font color="#808080">&quot;bw&quot;</font>);
 *    <font color="#0000FF">public static final</font> DisplayType COLOR = <font color="#0000FF">new</font> DisplayType(<font color="#808080">&quot;color&quot;</font>);
 *}
 *</pre></blockquote>
 *
 * Compare the above examples with something like
 *
 *<blockquote><pre>
 *<font color="#0000FF">public final class</font> KeyType {
 *    <font color="#0000FF">private</font> KeyType() {}
 *    <font color="#0000FF">public static final int</font> NAVIGATION = <font color="#FF00FF">1</font>;
 *    <font color="#0000FF">public static final int</font> SOFTKEY = <font color="#FF00FF">2</font>;
 *    <font color="#0000FF">public static final int</font> ENTRY = <font color="#FF00FF">3</font>;
 *    <font color="#0000FF">public static final int</font> CONTROL = <font color="#FF00FF">4</font>;
 *    <font color="#0000FF">public static final int</font> OTHER = <font color="#FF00FF">5</font>;
 *}
 *
 *<font color="#0000FF">public final class</font> DisplayType {
 *    <font color="#0000FF">private</font> DisplayType() {}
 *    <font color="#0000FF">public static final int</font> MONOCHROME = <font color="#FF00FF">1</font>;
 *    <font color="#0000FF">public static final int</font> COLOR = <font color="#FF00FF">2</font>;
 *}
 *</pre></blockquote>
 *
 * Both methods have their own advantages and disadvantages. The obvious
 * advantage of the second method (the one with integers) is the use of
 * primitive numbers. Manipulating primitive integers (especially static
 * final numbers which can be inlined by the compiler) is a lot more
 * efficient then using objects and especially calling virtial methods,
 * such as <code>equals()</code>. The downside of integer enums is that
 * they provide no type checking, neither at compile time nor at run time.
 * Consider these two statements:
 *
 *<blockquote><pre>
 *setDisplayType(DisplayType.MONOCHROME);
 *setDisplayType(KeyType.SOFTKEY);
 *</pre></blockquote>
 *
 * Both are equivalent, both compile and work identically (as long as
 * <code>DisplayType.MONOCHROME</code> and <code>KeyType.SOFTKEY</code>
 * are defined as the same number) even though one of them looks like
 * a total nonsense. I would appreciate some sort of compile time warning
 * if a enumeration constant is used in a place where it's not supposed
 * to be used. This can be archived by using enumeration classes. If
 * <code>DisplayType</code> is a typesafe enumaration class, then
 * <code>setDisplayType()</code> function can specify exactly what
 * kind of enumeration it expects as a parameter:
 *
 *<blockquote><pre>
 *public void setDisplayType(DisplayType type);
 *</pre></blockquote>
 *
 * and this statement
 *
 *<blockquote><pre>
 *setDisplayType(KeyType.SOFTKEY);
 *</pre></blockquote>
 *
 * will produce a compile time error. And here is a good news for those
 * who care about runtime performance. Since <code>DisplayType</code>
 * constructor is private and there will never be any objects of this
 * type besides those exported via the public static final fields, you
 * can compare the enumeration values using the <em>identity</em> operator
 *
 *<blockquote><pre>
 *if (type == DisplayType.COLOR) {...}
 *</pre></blockquote>
 *
 * rather than <code>equals()</code> method:
 *
 *<blockquote><pre>
 *if (DisplayType.COLOR.equals(type)) {...}
 *</pre></blockquote>
 *
 * The <em>identity</em> operator (<code>==</code>) is a lot more efficient
 * then invoking the object's virtual <code>equals()</code> method, perhaps
 * even (almost) as efficient as comparing primitive numbers.
 *
 * <p>
 *
 * In addition to the type checking, typesafe enumeration classes have
 * other nice features. Since they are objects, they can be used as
 * keys in a <code>Map</code> or stored in a <code>List</code>. Their
 * <code>toString()</code> method may return something useful for debugging
 * or tracing purposes. Why not use them?
 *
 * <p>
 *
 * QED (<em>quod erat demonstrandum</em>)
 *
 */
public abstract class Enum implements Serializable {

    /** The string that identifies the enum value */
    private String name;

    /**
     * The hashcode. Since the hashcode never changes, there's no need
     * to calculate it every time. Assumption is here that normally strings
     * don't have zero hashcode. Note that the String object itself caches
     * its hashcode since JDK 1.3
     */
    private int hash = 0;

    /**
     * Default constructor is disabled.
     */
    private Enum() {
        throw new InternalError("Default constructor is disabled");
    }

    /**
     * Creates a new enum value.
     * @param id Identifier for the enum value.
     */
    protected Enum(String id) {
        if (id == null) throw new NullPointerException("id");
        name = id;
    }

    /**
     * Returns the ID string for this Enum value.
     * @return The ID string for this Enum value.
     */
    public final String getID() {
        return name;
    }

    /**
     * Compares this object to the specifed object.
     * The result is <code>true</code> if and only if the argument is
     * the same Enum object.
     * @param     obj   The object to compare this object with.
     * @return    <code>true</code> if the objects are equal;
     *            <code>false</code> otherwise.
     */
    public final boolean equals(Object obj) {
        return (obj == this);
    }

    /**
     * Returns the hashcode for this object.
     * @return The hashcode for this object.
     */
    public final int hashCode() {
        if (hash == 0) {
            // calculate the hash code if we haven't done it yet. Note that
            // the String object itself caches its hashcode since JDK 1.3
            hash = Utils.hashCode(getID());
        }
        return hash;
    }

    /**
     * Returns the string representation of this Enum value.
     * This implementation returns the ID string.
     * @return The string representation of this Enum value.
     */
    public String toString() {
        return getID();
    }
}
