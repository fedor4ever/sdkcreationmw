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

/* java.lang.ref */
import java.lang.ref.Reference;
import java.lang.ref.SoftReference;

/* java.io */
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.FileInputStream;
import java.io.BufferedInputStream;

/* java.util */
import java.util.Map;
import java.util.HashMap;

/* java.util.zip */
import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;

/**
 * JarResources maps all resources included in a Zip or Jar file.
 * Additionaly, it provides a method to extract one. It may be
 * configured to store soft references to large objects, so that
 * they can be garbage collected in response to memory demand.
 * The downside of using soft references is that we would have
 * to re-read the Jar in case if entry we need happened to be
 * picked up by the garbage collector.
 *
 */
public class JarResources {

    /**
     * An entry of size greater or equal this constant is considered to
     * be "big". It means that this class would store a {@link SoftReference}
     * to this entry in the hashtable rather than entry itself. This size
     * cannot be too small because allocation of a {@link SoftReference}
     * takes 416 bytes from the Java heap by itself, as measured with JDK 1.3
     * under Windows. Allocating {@link SoftReference} for objects of
     * smaller or even comparable size would be a waste or memory.
     */
    public static final int BIG_ENTRY = 10000;

    /** Internal debug flag */
    private final boolean debugOn = Trace.isVerbose() &&
        Utils.getSystemProperty("nokia.wtk.JarResources.debug",false);

    /** Jar resource mapping tables */
    private Map contents = new HashMap(12);

    /**
     * If this is <code>true</code>, then this class will keep
     * {@likn SoftReference} to the big entries so that can be
     * picked up by the garbage collector in response to memory
     * demand.
     */
    private boolean softRef;

    /**
     * Number of softly referenced objects in the hashtable. Used for
     * optimization purposes.
     */
    private int softRefCount;

    /**
     * The jar file from which resources have been loaded
     */
    private File jarFile;

    /**
     * Creates a JarResources. It extracts all resources from a Jar
     * into an internal hashtable, keyed by resource names.
     *
     * @param jarFile a jar or zip file
     */
    public JarResources(File jarFile) throws IOException {
        this(jarFile, true);
    }

    /**
     * Creates a JarResources. It extracts all resources from a Jar
     * into an internal hashtable, keyed by resource names.
     *
     * @param jarFile a jar or zip file
     * @param softRef to use or not to use soft references
     */
    public JarResources(File jarFile, boolean softRef) throws IOException {
        this.jarFile = jarFile;
        this.softRef = softRef;
        refresh(null);
    }

    /**
     * Returns the file from which resources have been loaded.
     */
    public File getFile() {
        return jarFile;
    }

    /**
     * Extracts a jar resource or returns <code>null</code> if such entry
     * does not exist in the Jar file. The caller must never modify the
     * contents of the returned byte array.
     *
     * @param name a resource name.
     */
    public byte[] getResource(String name) {
        // if we are not using soft references, then we don't need
        // synchronization because we are never going to modify the
        // contents of the hashtable.
        if (softRef && softRefCount > 0) {
            synchronized (this) {
                return getBytes(name);
            }
        } else {
            return getBytes(name);
        }
    }

    /**
     * Extracts a jar resource or returns <code>null</code> if such entry
     * does not exist in the Jar file. The caller must never modify the
     * contents of the returned byte array. This method is not synchronized.
     * If synchronization is required, it must be provided by the caller.
     *
     * @param name a resource name.
     */
    private byte[] getBytes(String name) {
        Object object = contents.get(name);
        byte[] bytes;
        if (object instanceof Reference) {
            bytes = (byte[])((Reference)object).get();
            if (bytes == null) {

                // entry has been garbage-collected, re-read the Jar
                if (debugOn) {
                    debugOut("JarResources: entry \"" + name +
                                        "\" needs refresh");
                }
                try { bytes = refresh(name); }
                catch (IOException x) {
                    Trace.Debug.printStackTrace(x);
                    bytes = null;
                }
            }
        } else {
            bytes = (byte[])object;
        }
        return bytes;
    }

    /**
     * Re-reads the jar file, refreshing the hashtable.
     *
     * @param name the particular entry the caller is looking for.
     *             If this parameter is not <code>null</code>, then this
     *             method guarantees that this entry won't get garbage
     *             collected while we are reading the Jar.
     *
     * @return the entry requested by the caller, or <code>null</code>
     *         if entry wasn't found or input parameter was <code>null</code>.
     */
    private byte[] refresh(String name) throws IOException {
        byte[] entry = null;

        // reset the hashtable
        contents.clear();
        softRefCount = 0;

        // read the jar file
        if (debugOn) debugOut("JarResources: reading " + jarFile);
        InputStream fis = new FileInputStream(jarFile);
        ZipInputStream zis = new ZipInputStream(new BufferedInputStream(fis));
        ZipEntry ze = null;
        while ((ze = zis.getNextEntry()) != null) {

            // skip directories
            if (!ze.isDirectory()) {

                String zname = ze.getName();
                byte[] bytes = IOUtils.readStream(zis);

                // store SoftReference to the data if the data is big enough
                if (softRef && bytes.length >= BIG_ENTRY) {
                    contents.put(zname, new SoftReference(bytes));
                    softRefCount++;
                } else {
                    contents.put(zname, bytes);
                }

                // store strong reference to the requested entry to prevent
                // it from being garbage collected
                if (name != null && name.equals(zname)) {
                    entry = bytes;
                }

                if (debugOn && name == null) {
                    debugOut(jarFile +
                            ": " + zname +
                            ",size=" + bytes.length +
                            ",csize=" + ze.getCompressedSize() );
                }
            }
        }

        return entry;
    }

    /**
     * Dumps the string to message log and/or console window.
     */
    private void debugOut(String s) {
        Trace.Debug.println(s);
    }

    /**
     * Dumps a zip entry into a string.
     * @param ze a ZipEntry
     */
    public static String dumpZipEntry(ZipEntry ze) {
        StringBuffer sb = new StringBuffer();
        sb.append(ze.isDirectory() ? "d " : "f ");
        if (ze.getMethod() == ZipEntry.STORED) {
            sb.append("stored   ");
        } else {
            sb.append("deflated ");
        }

        sb.append(ze.getName());
        sb.append("\t");
        sb.append(""+ze.getSize());
        if (ze.getMethod() == ZipEntry.DEFLATED) {
            sb.append("/"+ze.getCompressedSize());
        }

        return (sb.toString());
    }

    /**
     * Is a test driver. Given a jar file and a resource name, it trys to
     * extract the resource and then tells us whether it could or not.
     */
    public static void main(String[] args) throws IOException {
        String outFile = null;
        if (args.length < 2 || args.length > 3) {
            System.out.println("Usage: java " + JarResources.class.getName() +
                               " <jar> <name> [output file]");
            System.exit(1);
        }
        JarResources jr = new JarResources(new File(args[0]));
        byte[] bytes = jr.getResource(args[1]);
        if (bytes == null) {
            System.out.println("Could not find "+args[1]+".");
        } else {
            System.out.println("Found "+args[1]+", "+bytes.length+" bytes");
            if (args.length > 2) {
                IOUtils.writeFile(bytes, args[2]);
                System.out.println("Written " + args[2]);
            } else {
                System.out.println("Data: " + StringUtils.toString(bytes));
            }
        }
    }
}
