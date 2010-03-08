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
import java.io.File;

/* java.util */
import java.util.Map;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Properties;

/**
 * Collection of very basic static utilities.
 *
 */
public abstract class Utils {

    private static boolean is1dot3;
    private static boolean is1dot4;
    private static boolean is1dot3orBetter;
    private static boolean is1dot4orBetter;
    private static boolean parsedJavaVersion = false;
    private static String javaPath = null;
    private static String osName = null;
    private static boolean windozeFound = false;
    private static Map primitiveTypes;

    static {
        primitiveTypes = new HashMap(12);
        primitiveTypes.put( "byte"   , Byte      .TYPE );
        primitiveTypes.put( "char"   , Character .TYPE );
        primitiveTypes.put( "double" , Double    .TYPE );
        primitiveTypes.put( "float"  , Float     .TYPE );
        primitiveTypes.put( "int"    , Integer   .TYPE );
        primitiveTypes.put( "long"   , Long      .TYPE );
        primitiveTypes.put( "short"  , Short     .TYPE );
        primitiveTypes.put( "boolean", Boolean   .TYPE );
    }

    /** Default constructor is suppressed, ensuring non-instantiability. */
    private Utils() {
        throw new InternalError("Utils is a collection of static methods");
    }

    /**
     * Returns a system property interpreted as a boolean value.
     */
    public static boolean getSystemProperty(String key, boolean def) {
        try {
            String s = System.getProperty(key);
            if (s != null) {
                if (s.equalsIgnoreCase(Boolean.TRUE.toString())) return true;
                if (s.equalsIgnoreCase(Boolean.FALSE.toString())) return false;
                if (s.equalsIgnoreCase("yes")) return true;
                if (s.equalsIgnoreCase("no")) return false;
            }
        } catch (Throwable x) { /* ignore */ }
        return def;
    }

    /**
     * Returns a system property catching all exceptions such as
     * {@link SecurityException}
     */
    public static String getSystemProperty(String key, String def) {
        try { return System.getProperty(key, def); }
        catch (Throwable x) { /* ignore */ }
        return def;
    }

    /**
     * Returns the value of the system property interpreted as an integer
     * value
     *
     * @param key the name of the system property
     * @param def the default value
     *
     * @return  the parsed value of the specified system property,
     *          or <code>def</code> if the property is undefined or
     *          if its value cannot be parsed.
     */
    public static int getSystemProperty(String key, int def) {
        try {
            String value = System.getProperty(key);
            if (value != null) return Integer.parseInt(value);
        } catch (Throwable x) { /* ignore */ }
        return def;
    }

    /**
     * Tests if two objects "equal to" each other. The objects are considered
     * to be equal if either of the following conditions holds true:
     * <ul>
     * <li>both objects are <code>null</code>.
     * <li>the objects are identical to each other (<code>o1 == o2</code>)
     * <li><code>o1</code> is not <code>null</code> and
     *     <code>o1.equals(o2)</code> returns <code>true</code>.
     * <li><code>o2</code> is not <code>null</code> and
     *     <code>o2.equals(o1)</code> returns <code>true</code>.
     * </ul>
     * Otherwise, this function returns <code>false</code>.
     */
    public static boolean equals(Object o1, Object o2) {
        boolean equal = false;
        if (o1 == o2) {
            equal = true;
        } else if (o1 != null) {
            equal = o1.equals(o2);
        } else if (o2 != null) {
            equal = o2.equals(o1);
        }
        return equal;
    }

    /**
     * Returns hash code for a <code>boolean</code> value.
     *
     * @return  the integer <tt>1231</tt> if this value is
     * <tt>true</tt>; returns the integer <tt>1237</tt> if
     * value is <tt>false</tt>. These numbers are taken from
     * <code>Boolean.hashCode()</code> implementation.
     */
    public static int hashCode(boolean value) {
        return (value ? 1231 : 1237);
    }

    /**
     * Returns hash code for the specified object or zero of the object
     * is <code>null</code>.
     *
     * @return  zero if <code>value</code> is <code>null</code>, otherwise
     *          the value returned by <code>value.hashCode()</code>.
     */
    public static int hashCode(Object obj) {
        return ((obj == null) ? 0 : obj.hashCode());
    }

    /**
     * Returns {@link Boolean} object for specified primitive
     * <code>boolean</code>.
     */
    public static Boolean getBoolean(boolean b) {
        return (b ? Boolean.TRUE : Boolean.FALSE);
    }

    /**
     * Waits forever until thread terminates, ignoring InterruptedException
     */
    public static void join(Thread thread) {
        while (thread.isAlive()) {
            try { thread.join(); }
            catch (InterruptedException x) { /* ignore */ }
        }
    }

    /**
     * Waits until thread terminates or timeout expires
     * @return <code>true</code> if thread has terminated,
     *         <code>false</code> if timeout expired.
     */
    public static boolean join(Thread thread, long millis) {
        long deadline = System.currentTimeMillis() + millis;
        long timeLeft = millis;
        while (thread.isAlive() && timeLeft > 0) {
            try {
                thread.join(timeLeft);
                break;
            } catch (InterruptedException x) {
                timeLeft = deadline - System.currentTimeMillis();
            }
        }
        return (!thread.isAlive());
    }

    /**
     * Waits forever until object gets notified, ignoring InterruptedException
     * @exception  IllegalMonitorStateException  if the current thread is not
     *               the owner of this object's monitor.
     */
    public static void waitFor(Object object) {
        while (true) {
            try {
                object.wait();
                break;
            } catch (InterruptedException x) {
                // ignore
            }
        }
    }

    /**
     * Waits object gets notified or timeout expires. Zero timeout is
     * interpreted as indefinite
     *
     * @exception  IllegalMonitorStateException  if the current thread is not
     *               the owner of this object's monitor.
     */
    public static void waitFor(Object object, long millis) {
        if (millis > 0) {
            long deadline = System.currentTimeMillis() + millis;
            long timeLeft = millis;
            while (timeLeft > 0) {
                try {
                    object.wait(timeLeft);
                    break;
                } catch (InterruptedException x) {
                    timeLeft = deadline - System.currentTimeMillis();
                }
            }
        } else {
            waitFor(object);  // forever
        }
    }

    /**
     * Returns <code>true</code> if this code is running in JVM 1 dot 3.
     * @return <code>true</code> if this code is running in JVM version 1.3
     */
    public static boolean is1dot3() {
        if (!parsedJavaVersion) parseJavaVersion();
        return is1dot3;
    }

    /**
     * Returns <code>true</code> if this code is running in JVM 1 dot 3.
     * ot better
     * @return <code>true</code> if this code is running in JVM version
     *         &gt;= 1.3
     */
    public static boolean is1dot3orBetter() {
        if (!parsedJavaVersion) parseJavaVersion();
        return is1dot3orBetter;
    }

    /**
     * Returns <code>true</code> if this code is running in JVM 1 dot 4.
     * @return <code>true</code> if this code is running in JVM version 1.4
     */
    public static boolean is1dot4() {
        if (!parsedJavaVersion) parseJavaVersion();
        return is1dot4;
    }

    /**
     * Returns <code>true</code> if this code is running in JVM 1 dot 4.
     * ot better
     * @return <code>true</code> if this code is running in JVM version
     *         &gt;= 1.4
     */
    public static boolean is1dot4orBetter() {
        if (!parsedJavaVersion) parseJavaVersion();
        return is1dot4orBetter;
    }

    /**
     * Parses Java version
     */
    private static void parseJavaVersion() {
        if (!parsedJavaVersion) {
            try {
                String ver = System.getProperties().getProperty("java.version");
                int dot = ver.indexOf('.');
                String one = ver.substring(0,dot);
                String two = ver.substring(dot+1);
                for (int i=0; i<two.length(); i++) {
                    if (!Character.isDigit(two.charAt(i))) {
                        two = two.substring(0,i);
                    }
                }
                int n1 = Integer.parseInt(one);
                int n2 = Integer.parseInt(two);
                is1dot3 = (n1 == 1 && n2 == 3);
                is1dot4 = (n1 == 1 && n2 == 4);
                is1dot3orBetter = ((n1 == 1 && n2 >= 3) || (n1 > 1));
                is1dot4orBetter = ((n1 == 1 && n2 >= 4) || (n1 > 1));
            } catch (Throwable x) {
                Trace.printStackTrace(x);
            }
            parsedJavaVersion = true;
        }
    }

    /**
     * Returns <code>true</code> if we are running under Windows,
     * <code>false</code> otherwise.
     */
    public static boolean isWindoze() {
        if (osName == null) {
            osName = getSystemProperty("os.name", "unknown");
            windozeFound = (osName.toLowerCase().indexOf("windows") >= 0);
        }
        return windozeFound;
    }

    /**
     * Returns full path to the java executable that can be used to start
     * a new java process.
     */
    public static String getJavaPath() {
        if (javaPath == null) {
            synchronized (Utils.class) {
                if (javaPath == null) {
                    StringBuffer sb = new StringBuffer();
                    String javaHome = System.getProperty("java.home");
                    if (javaHome != null && javaHome.length() > 0) {
                        sb.append(javaHome);
                        sb.append(File.separatorChar);
                        sb.append("bin");
                        sb.append(File.separatorChar);
                    }
                    String javaExe = (isWindoze() ? "javaw.exe" : "java");
                    sb.append(javaExe);
                    String path = sb.toString();
                    if ((new File(path)).exists()) {
                        javaPath = path;
                    } else {
                        System.err.println("WARNING! file not found: " + path);
                        javaPath = javaExe;
                    }
                }
            }
        }
        return javaPath;
    }

    /**
     * Loads class for name. Unlike <code>Class.forName()</code> handles
     * primitive types such as <code>int</code>, <code>boolean</code>, etc.
     * Does not support <code>void</code> although <code>Void.TYPE</code>
     * exists.
     *
     * @return the Class object for the class with the specified name.
     */
    public static Class classForName(String name) throws ClassNotFoundException {
        Class c = (Class)primitiveTypes.get(name);
        if (c == null) c = Class.forName(name);
        return c;
    }


    /**
     * Returns another {@link Properties} object only containing the keys
     * from the this object that start with the specified prefix. The keys
     * in the returned properties object are subkeys of the keys in the
     * original object with the prefix stripped off.
     *
     * @param p the properties to extract the subset from.
     * @param prefix the prefix of the keys to extract
     * @return the new object; or null if no keys with such prefix were found;
     *         returns the input object if prefix if null or empty
     */
    public static Properties extract(Properties p, String prefix) {

        Properties subset = null;

        // return self if prefix is null or empty
        if ( prefix == null || prefix.length() == 0) {
            if ( p.size() > 0) subset = p;

        // extract the subset
        } else if (!p.isEmpty()) {
            Iterator itr = p.entrySet().iterator();
            int length = prefix.length();

            /* enumerate the keys and find the matching entries */
            while (itr.hasNext() ) {
                Map.Entry entry = (Map.Entry)itr.next();
                String key = (String)entry.getKey();
                if (key.startsWith(prefix)) {

                    // create the subset when we first time get here
                    if ( subset == null ) {
                        subset = new Properties();
                    }

                    // add key-value pair to the subset
                    String value = (String)entry.getValue();
                    subset.put(key.substring(length), value);
                }
            }
        }

        return subset;
    }
}
