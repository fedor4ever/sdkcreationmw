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

package com.nokia.epdt.core;

/* java.io */
import java.io.File;

/**
 * Static utilities
 */
public final class CoreUtils {

    // System properties
    static final String EPOC_ROOT_SYSTEM_PROPERTY = "epoc.root";
    static String epocRoot;
    static File epocRootDir;

    // Prevents instantiation
    private CoreUtils() {
        throw new Error();
    }

    /**
     * Returns Epoc root directory as a {@link File} object.
     * @return Epoc root directory as a {@link File} object.
     */
    public static File EpocRootDir() {
        if (epocRootDir == null) {
            // Get EPOCROOT is via a system variable
            String root = System.getProperty(EPOC_ROOT_SYSTEM_PROPERTY);
            if (root != null) {
                File rootDir = new File(root);
                if (rootDir.isDirectory()) {
                    File epoc32 = new File(rootDir, "epoc32");
                    if (epoc32.isDirectory()) {
                        epocRootDir = rootDir.getAbsoluteFile();
                        return epocRootDir;
                    }
                }
            }

            // Check if we are under EPOCROOT somewhere
            File dir = new File(".").getAbsoluteFile();
            while (dir != null) {
                String name = dir.getName();
                dir = dir.getParentFile();
                if ("epoc32".equals(name)) {
                    epocRootDir = dir.getAbsoluteFile();
                    return epocRootDir;
                }
            }

            // Check if we are next to EPOCROOT
            dir = new File(".").getAbsoluteFile();
            while (dir != null) {
                String name = dir.getName();
                File mayBeEpoc32 = new File(dir, "epoc32");
                if (mayBeEpoc32.isDirectory()) {
                    epocRootDir = dir.getAbsoluteFile();
                    return epocRootDir;
                }
                dir = dir.getParentFile();
            }

            // We are really lost - Unable to locate EPOCROOT.
            throw new RuntimeException("Help! Can't find EPOCROOT");
        }
        return epocRootDir;
    }

    /**
     * Returns Epoc root directory. The name always ends with path separator.
     * @return Epoc root directory.
     */
    public static String EpocRoot() {
        if (epocRoot == null) {
            epocRoot = EpocRootDir().getPath();
            if (!epocRoot.endsWith(File.separator)) {
                epocRoot += File.separator;
            }
        }
        return epocRoot;
    }
}
