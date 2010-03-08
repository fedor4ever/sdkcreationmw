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




package com.nokia.wtk.app;

/* java.beans */
import java.beans.PropertyChangeListener;

/* java.io */
import java.io.File;
import java.io.IOException;

/**
 * This interface provides Saveable routines for different kinds of views.
 */
public interface Saveable {

    /**
     * Name of property to announce change in source file name.
     */
    final public static String SOURCE_FILE_PROPERTY = "source-file";

    /**
     * Name of property to announce change in the "dirty" state.
     */
    final public static String DIRTY_PROPERTY = "dirty";

    /**
     * Adds a property change listener
     */
    public void addPropertyChangeListener(PropertyChangeListener listener);

    /**
     * Removes a property change listener
     */
    public void removePropertyChangeListener(PropertyChangeListener listener);

    /**
     * Sets source file for class. Fires property change event if file name
     * is changed
     */
    public void setSourceFile(File f);

    /**
     * Gets current source file.
     */
    public File getSourceFile();

    /**
     * Saves content to the current file
     *
     * @throws IOException if I/O error occurs or file name is not defined
     */
    public void save() throws IOException;

    /**
     * Saves a template version of the content to the supplied file
     *
     * @Param File to save template to
     * @throws IOException if I/O error occurs or file name is not defined
     */
    public void saveAsTemplate(File f) throws IOException;

    /**
     * Returns true if content has been modified after last save.
     */
    public boolean isDirty();

    /**
     * Returns the content type.
     *
     * #see MimeType
     */
    public String getContentType();

    /**
     * Returns the Character Encoding (for text files).
     *  or null if unknown or not applicable.
     */
    public String getCharEncoding();

    /**
     * Set the Character Encoding (for text files).
     */
    public void setCharEncoding(String enc);
}
